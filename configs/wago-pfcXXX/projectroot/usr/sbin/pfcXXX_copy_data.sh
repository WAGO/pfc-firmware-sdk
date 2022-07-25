#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of PTXdist package wago-custom-install.
#
# Copyright (c) 2014-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   pfcXXX_copy_data.sh
#
# Brief:    Script to copy system data from/to devices (eg. NAND/EMMC/SD card).
#
# Author:   HT:  WAGO GmbH & Co. KG
# Author:   AGa: WAGO GmbH & Co. KG
# Author:   PEn: WAGO GmbH & Co. KG
# Author:   MLa: WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

# Load general definitions and functions.
CWD=$(cd -P "$(dirname "$0")" && pwd)
if [ ! -f "/etc/config-tools/board_specific_defines" ]; then
    echo "Missing dependency board_specific_defines"
    exit 64
fi
source "/etc/config-tools/board_specific_defines"

if [ ! -f "${CWD}/${BOARD_ID}_commons" ]; then
    echo "Missing dependency ${BOARD_ID}_commons"
    exit 64
fi
source "${CWD}/${BOARD_ID}_commons"

if [ ! -f "${CWD}/${BOARD_ID}_copy_data_emmc_extensions.sh" ]; then
    echo "Missing dependency ${BOARD_ID}_copy_data_emmc_extensions.sh"
    exit 64
fi
source "${CWD}/${BOARD_ID}_copy_data_emmc_extensions.sh"


MOUNTPOINTS=""

BACKUP_CMD="cp -a"

CODESYS_PAUSED="no"

function do_mount
{
    local fstype=${1}
    local dev=${2}
    local mntpoint="${3}"

    print_dbg "mounting $dev to $mntpoint (fstype \"${fstype}\")"

    mount -t ${fstype} ${dev} "${mntpoint}" || exit $?
    # we only continue if mount succeeds
    MOUNTPOINTS="${mntpoint}:${MOUNTPOINTS}"
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
        sync
        umount "${mntpoint}"
        print_dbg $?
    done
    IFS=$save_ifs

    set -e

    rm -rf "/tmp/sd" "/tmp/nand" "/tmp/emmc"

    # when system is run from NAND, ubidetach aborts with an error which can be ignored
    ubidetach &> /dev/null /dev/ubi_ctrl -m ${G_UBIFS_MTD_DEV_NR} || true

    # reenable sd card automount
    udevadm control --env __SD_AUTOMOUNT="1"
}

function cleanup_abnormal
{
    local lastline=$1
    local lasterr=$2

    local errcmd=$BASH_COMMAND

    shopt -u dotglob

    if [[ "$lasterr" == "0" ]]; then
        lasterr=254
    fi

    # Filter "expected" errors
    case $lasterr in
        $SUCCESS)
            ;;
        $MOUNT_ERROR)
            SetLastError "Error: target device is in use."
            ;;
        *)
            WriteTraceMessage "Error $lasterr in $0 near line $lastline (command $errcmd)"
            ;;
    esac

    if [[ "$CODESYS_PAUSED" == "yes" ]]; then
        /etc/init.d/runtime start || true
        CODESYS_PAUSED="no"
    fi

    cleanup

    exit $lasterr
}

function mount_target_sd
{
    mkdir -p "/tmp/sd/boot"
    mkdir -p "/tmp/sd/root"

    do_mount vfat ${G_SD_BOOT_DEV} "/tmp/sd/boot"
    do_mount ext3 ${G_SD_ROOT_DEV} "/tmp/sd/root"
}

function mount_target_nand
{
    mkdir -p "/tmp/nand/home"
    mkdir -p "/tmp/nand/boot"
    mkdir -p "/tmp/nand/backup_root"

    local root_vol

    case $1 in
        ${G_UBIFS_ROOT1_LABEL})
            root_vol=${G_UBIFS_ROOT1_VOL}
            ;;
        ${G_UBIFS_ROOT2_LABEL})
            root_vol=${G_UBIFS_ROOT2_VOL}
            ;;
        *)
            exit ${INTERNAL_ERROR}
            ;;
    esac

    do_mount ubifs /dev/${root_vol} "/tmp/nand/backup_root"
    do_mount ubifs /dev/${G_UBIFS_HOME_VOL} "/tmp/nand/home"
}

function create_static_nodes
{ 
    print_dbg "Creating basic device nodes..."
    local prefix="$1"
    mknod ${prefix}/dev/null c 1 3
    mknod ${prefix}/dev/console c 5 1
    mknod ${prefix}/dev/zero c 1 5
}

function update_partition_info
{
    local device="$1"
    local part_update=0
    local attempts=10
    while [ "$attempts" -gt "0" ]; do
        echo "Ask kernel to re-read partition table..."
        /sbin/partprobe "${device}"
        if [ "$?" -eq "0" ]; then
            echo "done"
            part_update=1
            break
        fi
        sleep 2
        attempts=$((attempts-1))
    done

    if [ "$part_update" -eq "0" ]; then
        return 112
    else
        udevadm settle
        return $?
    fi
}

function set_boot_partition_to_first
{
    print_dbg "updating barebox bootchooser data..."
    /usr/sbin/${BOARD_ID}_switch_boot_partition.sh 1
    /usr/sbin/barebox-state_failsafe.sh -n bootstate -s rootfs.2.priority=0
}

function copy_rootfs
{
    local src_root="$1"
    local dst_root="$2"
    print_dbg "copying rootfs files from $src_root to $dst_root..."
    shopt -s dotglob

    for dir in ${src_root}/*; do
        print_dbg "    processing $dir..."
        case ${dir} in
            *dev|*sys|*proc|*tmp|*home|*mnt|*media|*run|*boot)
                mkdir -p "${dst_root}/${dir}"
                ;;
            *var)
                ${BACKUP_CMD} --one-file-system "${dir}" "${dst_root}/"
                ;;
            *lost+found|'.vol'|'.Trashes'|'System Volume Information'|'$Recycle.Bin'|'RECYCLER')
                ;; #skip fs-specific stuff
            *)
                ${BACKUP_CMD} "${dir}" "${dst_root}/"
                ;;
        esac
    done
}

function copy_source_to_sd
{
    local source="${1}"

    case $source in
        ${INTERNAL_FLASH_NAND})
            copy_nand_to_sd
            ;;
        ${INTERNAL_FLASH_EMMC})
            copy_emmc_to_sd
            ;;
        *)
            false
            ;;
    esac
}

function copy_nand_to_sd
{
    local DEST_ROOT="/tmp/sd/root"
    local DEST_HOME="/tmp/sd/root/home"
    local DEST_BOOT="/tmp/sd/boot/"

    copy_rootfs "/" "$DEST_ROOT"

    print_dbg "    processing /boot..."
    if ls "/boot/loader/"* &>/dev/null; then
        ${BACKUP_CMD} "/boot/loader/"* "${DEST_BOOT}"
    fi
    ${BACKUP_CMD} "/boot/"* "${DEST_ROOT}/boot"
    rm -rf "${DEST_ROOT}/boot/loader"
    rm -rf "${DEST_ROOT}/boot/loader-backup"

    print_dbg "    processing /home..."
    ${BACKUP_CMD} "/home/"* "${DEST_HOME}/"

    create_static_nodes "/tmp/sd/root"
}

function copy_emmc_to_sd
{
    copy_nand_to_sd

    # copy EMMC-only partitions
    [[ -d "/log" ]] && ${BACKUP_CMD} --one-file-system "/log/"* "/tmp/sd/root/log/" || true
}

function copy_sd_to_nand
{
    # Ignore cmdline: it is assembled in bootloader according to
    # boot mode id in eeprom

    # cp --one-file-system won't suffice here: contents of /home and
    # /lost+found might not fit into NAND root partition. So we skip
    # them.
    # We also need special handling for /var which is a mix of persistent
    # and temporary files.

    local SRC_BOOT="/boot"
    local SRC_HOME="/home"
    local DEST_ROOT="/tmp/nand/backup_root"
    local DEST_BOOT="/tmp/nand/backup_root/boot"
    local DEST_HOME="/tmp/nand/home"

    copy_rootfs "/" "$DEST_ROOT"

    # Include hidden files (i.e. starting with dot)
    print_dbg "processing /home..."
    (shopt -s dotglob; ${BACKUP_CMD} "${SRC_HOME}/"* "${DEST_HOME}/")

    print_dbg "processing /boot..."
    ${BACKUP_CMD} "${SRC_BOOT}/"* "${DEST_BOOT}/"
    rm -rf "${DEST_BOOT}/loader/"
}

#
# detaches /dev/mtdX, reformats it as UBI and creates ubifs volumes as defined
# in ${BOARD_ID}_commons
#
# assumption: no ubifs volumes mounted

function reformat_nand
{
    [[ -c /dev/${G_UBIFS_ROOT_ID} ]] && \
        ubidetach /dev/ubi_ctrl -m ${G_UBIFS_MTD_DEV_NR}

    ubiformat -y -q /dev/mtd${G_UBIFS_MTD_DEV_NR}

    ubiattach /dev/ubi_ctrl -m ${G_UBIFS_MTD_DEV_NR} 

    for vol_nr in ${ubifs_volume_numbers[@]}; do

        # last volume => use all available space
        if [[ "$(( ${vol_nr} + 1))" == "${#ubifs_volume_labels[@]}" ]]; then
            ubimkvol /dev/${G_UBIFS_ROOT_ID} \
                --name ${ubifs_volume_labels[${vol_nr}]} \
                --maxavsize
        else
            ubimkvol /dev/${G_UBIFS_ROOT_ID} \
                --name ${ubifs_volume_labels[${vol_nr}]} \
                --lebs ${ubifs_volume_sizes[${vol_nr}]} 
        fi
    done

    # let the udev daemon create all nodes in /dev
    udevadm settle --quiet

}

function reformat_sd
{
    local sd_card_size=$(( $(($1 + ${G_BOOT_PART_SIZE_MB} + ${G_PART_ALIGNMENT_MB})) * 1024 * 1024 )) # rootfs size + /boot size + (boot_fs offset)
    local sd_card_device=/dev/$(/etc/config-tools/get_device_data name "sd-card")

    if [[ -e ${sd_card_device} ]]; then

       if is_in_use "$sd_card_device"; then
            return "$MOUNT_ERROR"
        fi

        # Disable udev sd card automount.
        #
        # Otherwise, if the sd card contained a vfat partition, right after the 
        # new partitions are created, udev will recognize the rests of the 
        # old vfat metadata on the new partition and mount it :(

        udevadm control --env __SD_AUTOMOUNT="0"

        # Delete MBR (win7 stores its FAT there :( )
        dd if=/dev/zero of=${sd_card_device} bs=446 count=1

        # Create 2 $G_PART_ALIGNMENT_MB aligned partitions: 1st one is $G_BOOT_PART_SIZE_MB large.
        # 2nd one is ${image_size} - $G_BOOT_PART_SIZE_MB large, aligned at $G_PART_ALIGNMENT_MB 

        if [[ "${sd_card_size}" != "" ]]; then

            local root_part_offset=$(/etc/config-tools/get_min_sd_card_size rootfs-offset)

            local root_part_size=$((${image_size} - ${root_part_offset}))

            max_image_size=$[$(/etc/config-tools/get_device_data size $(/etc/config-tools/get_device_data name sd-card)) / 1024]
            if [[ ${image_size} -ge ${max_image_size} ]]; then
                # use all available space
                root_part_size=""
            fi

            # sfdisk doesn't accept leading spaces in the heredocument
            # -EOF only allows to ignore tabs => no indentation
            local logical_block_size=$(cat "/sys/block/mmcblk0/queue/logical_block_size")
            local block_factor=$((1024 * 1024 / logical_block_size))
            local alignment_blocks_boot=$((G_PART_ALIGNMENT_MB * block_factor))
            local alignment_blocks_root=$(((G_PART_ALIGNMENT_MB - (G_BOOT_PART_SIZE_MB % G_PART_ALIGNMENT_MB)) % G_PART_ALIGNMENT_MB * block_factor))
            local boot_blocks=$((G_BOOT_PART_SIZE_MB * block_factor))
            local boot_offset_blocks=$alignment_blocks_boot
            local root_offset_blocks=$((boot_offset_blocks + boot_blocks + alignment_blocks_root))
            local boot_part_size_string=${G_BOOT_PART_SIZE_MB}M
            local root_part_size_string=${root_part_size}
            if [[ "$root_part_size_string" =~ ^[0-9]+$ ]]; then
                root_part_size_string=${root_part_size_string}M
            fi
            flock ${sd_card_device} /sbin/sfdisk ${sd_card_device} <<EOF
${boot_offset_blocks},${boot_part_size_string},b,*
${root_offset_blocks},${root_part_size_string},L
EOF
            update_partition_info "${sd_card_device}" || return "$?"

            print_dbg "create file systems..."
            /sbin/mkfs.vfat "${sd_card_device}p1" -n "BOOT" -i 7761676F
            do_mkfs "${sd_card_device}p2" "ROOT" "ext3"
            return $SUCCESS
        fi
    fi
    return $INVALID_PARAMETER
}

function umount_all_sd
{
    local sd_card_dev=/dev/$(/etc/config-tools/get_device_data name "sd-card")

    # Nothing to do if sd_card_dev is not mounted
    grep -q ${sd_card_dev} /proc/mounts || return $SUCCESS

    local mountpoints=$(awk -v device=${sd_card_dev} \
        '$0 ~ device {mountpoints=mountpoints $2 " " } END { print mountpoints }' /proc/mounts)

    # would we kill ourselves (or one of our parents) if we continued?
    # i.e. ipcconfig has been started after a 'cd /media/sd'
    for mntpoint in $mountpoints; do

        local sd_in_use_by_myself="no"
        local pid=$(cut -f4 -d' ' "/proc/self/stat")
        while [[ "$pid" != "1" ]] && [[ "$sd_in_use_by_myself" == "no" ]]; do
            pid=$(cut -f4 -d' ' "/proc/$pid/stat") # pid=ppid
            lsof -p $pid | grep -q $mntpoint && sd_in_use_by_myself="yes"
        done

        if [[ "$sd_in_use_by_myself" == "yes" ]]; then
            return $MOUNT_ERROR
        fi

    done

    # does codesys use one of the mountpoints?
    lsof $mountpoints | grep -E -q '^(plclinux_|codesys3)' && \
    { 
        CODESYS_PAUSED=yes
        /etc/init.d/runtime stop || true
    }

    # handle the rest
    for mntpoint in $mountpoints; do
        # Create a temp file on the partition in question; 
        # then find processes that use the same filesystem and kill them
        # (fuser -mk)
        #    # FIXME: is SIGKILL not an overkill here?
        tmpfile=$(mktemp "$mntpoint/dummy.XXXXXX") && \
            fuser -mk "$tmpfile" && \
            rm "$tmpfile"
        umount "$mntpoint"
    done

    if [[ "$CODESYS_PAUSED" == "yes" ]]; then
        /etc/init.d/runtime start || true
    fi

}

function check_size
{
    local size_arg=$1
    local sd_card_dev_node=$2
    local size_in_use_by_rootfs=$(/etc/config-tools/get_min_sd_card_size)
    local sd_card_size=$(/etc/config-tools/get_device_data size ${sd_card_dev_node}) #size in KB
    sd_card_size=$((sd_card_size/1024))

    local space_needed=${size_in_use_by_rootfs} 

    if [[ ${size_arg} -lt ${space_needed} ]] || [[ ${size_arg} -gt ${sd_card_size} ]]; then
        false
    else
        true
    fi

}

function print_usage
{ 
    echo "Usage: copy_firmware_image <source-device=value | source-medium=value > <destination-device=value | destination-medium=value> <action=check-in-use|nand-check-size|stop-codesys> <size=[size in MB]>"
    echo
    echo "Devices can be given as names or medium, for example:"
    echo "device: mmcblk0, mtd2"
    echo "medium: sd-card, internal-flash-nand"
    echo
    echo "actions:"
    echo "check-in-use      - returns $TRUE when any files on the SD card are in use by CoDeSys"
    echo "nand-check-size   - returns $TRUE when NAND is larger than the data on SD card by more than 5%"
    echo "                    returns $NARROW_SPACE_WARNING when NAND is larger than the data on SD card by less than 5%"
    echo "                    returns $NOT_ENOUGH_SPACE_ERROR when the data on SD card will not fit into NAND"
    echo "stop-codesys      - when copying NAND contents to SD card and CoDeSys uses files from SD card, stop CoDeSys"
    echo
    echo " [when destination is SD card]:"
    echo "size: - SD partition size in MB"
}

#######################################
# main
#######################################

if [[ $# == 0 ]]; then
    print_usage
    exit 0
fi

status="${SUCCESS}"

# arguments:
# $1: source device (sd-card/internal-flash-nand/internal-flash-emmc)
# $2: destination device (sd-card/internal-flash-nand/internal-flash-emmc)
# $3: image size (in MB) (only for sd-card)

# environment:
# FORCE_UMOUNT=yes => if target is SD card, kill all processes
# that use it and unmount all shares

trap 'cleanup_abnormal ${LINENO} ${?}' EXIT

source_device=$1
destination_device=$2
image_size=${3:-0}

source=$(/etc/config-tools/get_device_data medium "${source_device}")
target=$(/etc/config-tools/get_device_data medium "${destination_device}")

case ${target} in
    ${INTERNAL_FLASH_NAND})
        reformat_nand
        # Always use 1st root partition
        mount_target_nand ${G_UBIFS_ROOT1_LABEL}
        copy_sd_to_nand 
        set_boot_partition_to_first
        ;;
    ${INTERNAL_FLASH_EMMC})
        reformat_emmc && \
        mount_target_emmc "EMMC_PARTITION_LABELS[@]" "EMMC_PARTITION_FS_TYPES[@]" && \
        copy_sd_to_emmc && \
        set_boot_partition_to_first
        status=$?
        ;;
        
    ${SD_CARD})
        if ! check_size ${image_size} ${destination_device}; then
            SetLastError "Cannot create SD card image of ${image_size} MB: illegal size."
            exit $LAST_ERROR_SET
        fi

        force_umount=${FORCE_UMOUNT:-"no"} 
        if [[ "$force_umount" == "yes" ]]; then
            umount_all_sd 
        fi

        reformat_sd ${image_size}
        mount_target_sd
        copy_source_to_sd "${source}"
        ;;
    *)
        exit $INVALID_PARAMETER
        ;;
esac

trap - EXIT

cleanup

exit ${status}
