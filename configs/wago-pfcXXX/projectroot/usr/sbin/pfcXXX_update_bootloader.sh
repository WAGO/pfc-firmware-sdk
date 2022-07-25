#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of PTXdist package wago-custom-install.
#
# Copyright (c) 2015-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   pfcXXX_update_bootloader.sh
#
# Brief:    Update script for PFC bootloaders.
#
# Author:   AGa: WAGO GmbH & Co. KG
# Author:   PEn: WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

#
# PFCXXXv1/v2 has 4 copies of each the MLO and the 2nd stage Barebox bootloader.
# 
# We use the following strategy to prevent bricking the device when updating the bootolader:
# - a copy of MLO and Barebox are written to the flash but purposely left invalid.
# The invalidation method depends on the CRC algorithm used:
# MLO on PFC200 uses 1 bit Hamming - we inject a CRC error to the first page.
# Other bootloader parts use 8 bit BCH - we skip 1st 2048 bytes long block first and write
# it in a separate "validation" step if everything else went well.
# The CPU skips the invalid copies and use the old ones.
#
# This approach is repeated for each of the 4 copies.

#
# PFCXXXv3 has a central boot partition for MLO bootloader. Barebox 2nd stage bootloader
# is stored in /boot-Directory on ROOT FS.
#
PATH=/sbin:/usr/sbin:/bin:/usr/bin

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

FLASH_XLOADER=flash-xloader
NANDWRITE=nandwrite
#
set -o nounset
set -o errexit

function write_invalid_hamming_mlo
{
    local mlo_img="$1"
    local mlo_dev="$2"
    ${FLASH_XLOADER} -i "${mlo_img}" -d "${mlo_dev}" --invalidate
}

function set_valid_hamming_mlo
{
    local mlo_img="$1"
    local mlo_dev="$2"
    ${FLASH_XLOADER} -i "${mlo_img}" -d "${mlo_dev}" --set-valid 0
}

function write_invalid_bch
{
    local img="$1"
    local dev="$2"

    local img_rest="/tmp/$(basename ${img}).rest"

    dd if="${img}" of="${img_rest}" bs=2048 skip=1  && \
    ${NANDWRITE} -pm -s 2048 "${dev}" "${img_rest}" && \
    rm -f /tmp/*.rest
}

function set_valid_bch
{
    local img="$1"
    local dev="$2"

    local img_first2k="/tmp/$(basename ${img}).first2k"
    dd if="${img}" of="${img_first2k}" bs=2048 count=1 && \
    ${NANDWRITE} -pm "${dev}" "${img_first2k}" && \
    rm -f /tmp/*first2k
}

function update_copy
{
    local mlo_img="$1"
    local mlo_dev="$2"
    local bb_img="$3"
    local bb_dev="$4"

    local dev="${5:-}"

    case "${dev}" in
        pfc100|pfc200v2|src)
            flash_erase "${mlo_dev}"  0 0 && \
            flash_erase "${bb_dev}"   0 0 && \
            
            write_invalid_bch "${mlo_img}" "${mlo_dev}" && \
            write_invalid_bch "${bb_img}" "${bb_dev}" && \
            set_valid_bch "${mlo_img}" "${mlo_dev}" && \
            set_valid_bch "${bb_img}" "${bb_dev}"
            ;;
            
        pfc200)
            flash_erase "${mlo_dev}"  0 0 && \
            flash_erase "${bb_dev}"   0 0 && \

            write_invalid_hamming_mlo   "${mlo_img}" "${mlo_dev}" && \
            write_invalid_bch           "${bb_img}" "${bb_dev}"   && \
            set_valid_hamming_mlo    "${mlo_img}" "${mlo_dev}" && \
            set_valid_bch            "${bb_img}" "${bb_dev}"
            ;;
            
        *)
            false
            ;;
    esac
}

function update_nand
{
    local mlo_path="$1"
    local bb_path="$2"
    local dev_type="$3"
    local mlo_img="${mlo_path}.${dev_type}"
    local bb_img="${bb_path}.${dev_type}"

    if ! which ${FLASH_XLOADER}; then
        ReportError "Error: flash-xloader not found."
        exit $SHELL_ERROR
    fi

    if ! which ${NANDWRITE}; then
        ReportError "Error: nandwrite not found."
        exit $SHELL_ERROR
    fi

    local bb_dev_index=0
    for mlo_dev in ${mlo_device_list[@]}; do
        local bb_dev="${barebox_device_list[${bb_dev_index}]}"

        if ! update_copy "${mlo_img}" "${mlo_dev}" "${bb_img}" "${bb_dev}" "${dev_type}"; then
            ReportError "Error updating bootloader (copy nr. ${bb_dev_index})"
            exit $SHELL_ERROR
        fi

        bb_dev_index=$[${bb_dev_index}+1]
    done

    return 0
}

function update_emmc
{
    local mlo_path="$1"
    local bb_path="$2"
    local dev_type="$3"
    local mlo_img="${mlo_path}.${dev_type}"

    local loader_mount_point="/tmp/bootloader"
    local update_folder="$loader_mount_point/update"
    mkdir -p "$loader_mount_point"
    mount "/dev/mmcblk1p1" "$loader_mount_point"
    mkdir -p "$update_folder"
    cp "$mlo_img" "$loader_mount_point/mlo.update"
    cp "$mlo_path"* "$update_folder/"
    cp "$bb_path"* "$update_folder/"
    mv "$update_folder/"* "$loader_mount_point/"
    sync
    rmdir "$update_folder"
    mv "$loader_mount_point/mlo.update" "$loader_mount_point/mlo"
    sync
    umount "$loader_mount_point"
    rmdir  "$loader_mount_point"
    
    return 0
}

function backup_bootloader
{
    local system_index=$1
    local barebox_compatible_versions="$2"
    local mlo_path="$3"
    local bb_path="$4"
    local dev_type="$5"
    local result=0

    # Determine target ROOT FS device
    local backup_root=""
    local mount_options=""
    local ubi=false
    
    case "${dev_type}" in
        pfc200v3|pac100)
            case $system_index in
                "1")
                    backup_root="$G_EMMC_ROOT1_DEV"
                    ;;
                "2")
                    backup_root="$G_EMMC_ROOT2_DEV"
                    ;;
                *)
                    result=$INTERNAL_ERROR
                    ;;
            esac
            ;;
            
        pfc100|pfc200|pfc200v2)
            case $system_index in
                "1")
                    backup_root="$G_UBIFS_ROOT1_DEV"
                    ;;
                "2")
                    backup_root="$G_UBIFS_ROOT2_DEV"
                    ;;
                *)
                    result=$INTERNAL_ERROR
                    ;;
            esac
            mount_options="-t ubifs"
            if [[ ! -c "$backup_root" ]]; then
                ubi=true
            fi
            ;;
            
        *)
            report_unknown_device
            ;;
    esac

    # Mount target ROOT FS and copy bootloaders
    local mount_point="/tmp/bootloader-update/backup_root"
    local loader_backup_destination="/boot/loader-backup"
    if [[ "$result" -eq "0" ]]; then
        mkdir -p "$mount_point"
        result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
    fi
    if $ubi && [[ "$result" -eq "0" ]]; then
        ubiattach -m 8
        result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
    fi
    if [[ "$result" -eq "0" ]]; then
        mount $mount_options "$backup_root" "$mount_point"
        result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
        if [[ "$result" -eq "0" ]]; then
            rm -rf "$mount_point$loader_backup_destination"
            mkdir "$mount_point$loader_backup_destination"
            result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
            if [[ "$result" -eq "0" ]]; then
                cp "$mlo_path"* "$mount_point$loader_backup_destination"
                result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
            fi
            if [[ "$result" -eq "0" ]]; then
                cp "$bb_path"* "$mount_point$loader_backup_destination"
                result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
            fi
            if [[ "$result" -eq "0" ]]; then
                cp "$barebox_compatible_versions" "$mount_point$loader_backup_destination"
                result=$(( $? != 0 ? $INTERNAL_ERROR : 0 ))
            fi
            sync
            umount "$mount_point"
            rm -rf "/tmp/bootloader-update"
        fi
        if $ubi; then
            ubidetach -m 8
        fi
    fi

    return $result
}

#
# If the installed version is whitelisted, return false, else return true
# If no version is stored in typelabel, the version IS too old
#
function current_bootloader_version_too_old
{
    local too_old="yes"

    local installed_bl_version="$(/etc/config-tools/get_typelabel_value -n BLIDX)"
    installed_bl_version="${installed_bl_version##BLIDX=}" # cut 'BLIDX='

    local whitelist="${BAREBOX_COMPATIBLE_VERSIONS}"

    for version in ${whitelist}; do
        if [[ "${installed_bl_version}" == "${version}" && "${version}" != "99" ]]; then
            too_old="no"
            break
        fi
    done

    if [[ "${too_old}" == "yes" ]]; then
        true
    else
        false
    fi
}

#Reports unknown device and exits with $SHELL_ERROR
function report_unknown_device
{
    ReportError "Error: unknown device (pfc100, pfc200{,v2,v3} or pac100 expected)."
    exit $SHELL_ERROR
}

#
# Parameters: $1 - platform type ('pfc100', 'pfc200', 'pfc200v2' ...)
#             $2 - bootloader source path (optional)
#             $3 - system index to store bootloader backup (optional, default: active system, 9: restore backuped bootloader)
#
function __main
{
    local dev_type="${1:-unknown}"
    local base_path="${2:-}"
    local barebox_compatible_versions="${2:-/etc}"
    local system_index=${3:-$(/etc/config-tools/get_systeminfo active-system-number)}

    if [[ -z "$system_index" ]]; then
        ReportError "Error: undefined system index."
        exit $SHELL_ERROR
    fi

    barebox_compatible_versions="${barebox_compatible_versions}/barebox-compatible-versions"
    if [[ -f "$barebox_compatible_versions" ]]; then
        source "$barebox_compatible_versions"
    else
        ReportError "Error: \"$barebox_compatible_versions\" not found."
        exit $SHELL_ERROR
    fi

    if [[ "${dev_type}" == "unknown" ]]; then
        report_unknown_device
    fi

    # Build source file names
    local mlo_base="$(basename ${G_MLO_FILE_BASE})"
    local bb_base="$(basename ${G_BAREBOX_FILE_BASE})"
    local mlo_path=""
    local bb_path=""
    if [[ -n "$base_path" ]]; then
        mlo_path="${base_path}/${mlo_base}"
        bb_path="${base_path}/${bb_base}"
    else
        mlo_path="${G_MLO_FILE_BASE}"
        bb_path="${G_BAREBOX_FILE_BASE}"
    fi

    # Store bootloader backup
    if [[ -z "$system_index" ]]; then
        echo "Note: Skipping bootloader backup in ROOT FS."
    elif [[ "$system_index" -eq "9" ]]; then
        echo "Restore bootloaders for type $dev_type from backup at \"$base_path\""
    else
        # Backup bootloader to ROOT FS
        backup_bootloader $system_index "$barebox_compatible_versions" "${mlo_path}" "${bb_path}" "${dev_type}"
        if [[ "$?" -ne "0" ]]; then
            ReportError "Error: Unable to write bootloaders backup to system $system_index"
            exit $INTERNAL_ERROR
        fi
    fi

    # Update bootloader if necessary
    if ! current_bootloader_version_too_old; then
        echo "Note: No bootloader update needed on central place."
        exit $SUCCESS
    fi
    
    case "${dev_type}" in
        pfc200v3|pac100)
            update_emmc "${mlo_path}" "${bb_path}" "${dev_type}"
            ;;

        pfc100|pfc200|pfc200v2)
            update_nand "${mlo_path}" "${bb_path}" "${dev_type}"
            ;;

        *)
            report_unknown_device
            ;;
    esac
    
    # Update bootloader version stored in eeprom
    local new_version="${BAREBOX_COMPATIBLE_VERSIONS}"

    # Replace version by 2 if ${BAREBOX_COMPATIBLE_VERSIONS} is empty
    new_version=${new_version:-2}

    echo "Setting bootloader version to ${new_version}..."
    setfwnr ${new_version}

    return $?
}

__main "$@"
