#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

# destinationPartition is not used when codesys backup is done: in this case no
# partition is mounted, instead the current /home which may (or may not) be a
# mount point for the home partition device is overwritten.
destinationPartition=$1

sourceFilename=$2
settingsFilename=${3:-""}

destinationDir=/mnt/restore_target

MOUNTPOINTS=""
UBIATTACH=""

function do_mount
{
  local fstypeArg=${1}
  local dev=${2}
  local mntpoint=${3}

  print_dbg "mounting $dev to $mntpoint"
 
  # Case SD-Card: mtd device is not attached by default
  # ${dev} is "ubi0_0"/"ubi0_1", without "/dev/" prefix!
  if [[ ! -e /dev/${dev} && -n "$fstypeArg" ]]; then
    /usr/sbin/ubiattach -m ${G_UBIFS_MTD_DEV_NR} && UBIATTACH="yes"
  fi
  
  mount ${fstypeArg} ${dev} ${mntpoint} || exit $MOUNT_ERROR 
  # because of 'set -o errexit' in ${BOARD_ID}_commons
  # we only continue if mount succeeds
  MOUNTPOINTS=${mntpoint}:${MOUNTPOINTS}
}

function cleanup
{
  # disable "errexit" to continue cleaning up even after
  # errors

  set +e

  local save_ifs=$IFS
  IFS=':'
  print_dbg "cleaning up mountpoints: $MOUNTPOINTS"
  for mntpoint in ${MOUNTPOINTS}; do
    print_dbg "$mntpoint: "
    umount ${mntpoint}
    print_dbg $?

    # dirty fix to preserve /boot directory in target partition
    if [[ "$mntpoint" != "${destinationDir}/boot/loader" ]]; then
        rmdir $mntpoint
    fi

  done
  IFS=$save_ifs
 
  if [[ "${UBIATTACH}" == "yes" ]]; then
    /usr/sbin/ubidetach -m ${G_UBIFS_MTD_DEV_NR}
  fi

  set -e
}

function system_backup_is_compatible
{
    local status=$EXECUTABLE_ERROR
    local backup_file=$1
    local backup_fw_version
    local current_fw_version
    tar -xz -C /tmp -f ${backup_file} ./etc/REVISIONS
    backup_fw_version=$(cat  /tmp/etc/REVISIONS | grep "FIRMWARE=" | cut -d= -f2 | sed -e 's/\...(..)//g')
    current_fw_version=$(cat /etc/REVISIONS     | grep "FIRMWARE=" | cut -d= -f2 | sed -e 's/\...(..)//g')
    rm -r /tmp/etc
    print_dbg "[system_backup_is_compatible] backup version : ${backup_fw_version}"
    print_dbg "[system_backup_is_compatible] current version: ${current_fw_version}"
    if [ ${backup_fw_version} = ${current_fw_version} ]; then
      result=$SUCCESS
    fi
    return $result
}


############### main #################

trap cleanup EXIT

pv_label=""

FSTYPE_ARG=''

flash_type=$(/etc/config-tools/get_device_data medium "${destinationPartition}")

case $destinationPartition in
  *ubi*)
    FSTYPE_ARG='-t ubifs';; # mount needs an explicit fstype in this case
esac 

case "$sourceFilename" in
  *${SYSTEM_FILENAME}*)
    print_dbg mount partitions...
    if ! system_backup_is_compatible ${sourceFilename}; then
        SetLastError "Backup version doesn't match corrent system version."
        exit $LAST_ERROR_SET
    fi

    pv_label="system"
    mkdir -p $destinationDir
    do_mount "$FSTYPE_ARG" "$destinationPartition" "$destinationDir"

    
    # Be careful! Wiping $destinationPartition!
    # rm .* causes rm to return 1 (because of . and ..). Thus the 'true'
    rm -rf $destinationDir/* $destinationDir/.* 2>&- || true

    #
    # Backup of the system partition contains contents of /boot/loader (mounted boot-partition).
    # Mount boot-partition to $destinationDir/boot/loader to restore 
    #
    mkdir -p ${destinationDir}/boot/loader
    case $flash_type in
        ${INTERNAL_FLASH_NAND})
            #has no boot partition
            ;;
        ${INTERNAL_FLASH_EMMC})
            do_mount "$FSTYPE_ARG" "${G_EMMC_BOOT_DEV}" "${destinationDir}/boot/loader"
            ;;
    esac

    ;;
  *${CODESYS_FILENAME}*)
      
    pv_label="codesys"
    # Be careful! Wiping /home!
    rm -rf /home/*
    destinationDir="/home/"
    ;;
esac

print_dbg extract rootfs to ${destinationDir}...
if [[ "$(which pv)" != "" ]]; then
  # show progress bar
  totalSize=$(ls -l ${sourceFilename} | awk '{print $5}') # busybox' stat cannot handle -c
  { pv -N $pv_label -tps $totalSize -w 70 ${sourceFilename} | \
  tar 2>/dev/null xzp -C ${destinationDir} -f -; } || exit $SHELL_ERROR
else
  tar 2>/dev/null xzpf ${sourceFilename} -C ${destinationDir} || exit $SHELL_ERROR
fi

case "$sourceFilename" in
  *${SYSTEM_FILENAME}*)
     print_dbg copy settings...     
     # The system partition we created  will restore its settings when it boots
     # for the first time. It will look for the settings file in its
     # $DEFAULT_SETTINGS_DIR, so it is copied there. If no settings file is
     # provided by the caller, the current settings have to be backed up and
     # copied over to the new partition.

     mkdir -p ${destinationDir}/${DEFAULT_SETTINGS_DIR}
     if [[ -f "${settingsFilename}" ]]; then
       print_dbg "cp ${settingsFilename} ${destinationDir}/${DEFAULT_SETTINGS_DIR}/${completeSettingsFileName}"
       cp ${settingsFilename} ${destinationDir}/${DEFAULT_SETTINGS_DIR}/${completeSettingsFileName}
     fi

     # Trigger settings restore in auto_firmware_restore script on next bootup
     printf $UPDATE_STATE_COMPLETE > ${destinationDir}/${DEFAULT_SETTINGS_DIR}/${autoRestoreStatusFile}

    ;;
esac

trap - EXIT

cleanup

exit $SUCCESS
