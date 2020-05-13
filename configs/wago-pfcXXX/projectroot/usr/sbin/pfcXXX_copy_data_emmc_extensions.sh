#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of PTXdist package wago-custom-install.
#
# Copyright (c) 2017-2018 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   pfcXXX_copy_data_emmc_extensions.sh
#
# Brief:    EMMC extension functions for pfcXXX_copy_data.sh.
#
# Author:   AGa: WAGO Kontakttechnik GmbH & Co. KG
# Author:   PEn: WAGO Kontakttechnik GmbH & Co. KG
# Author:   MLa: WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#

function reformat_emmc
{
    local emmc_card_device
    local status=$SHELL_ERROR

    emmc_card_device=/dev/$(/etc/config-tools/get_device_data name "internal-flash-emmc")

    if [[ -e $emmc_card_device ]]; then
        
        if is_in_use "$emmc_card_device"; then
            return "$MOUNT_ERROR"
        fi

        disable_automount && \
        do_format "${emmc_card_device}" "$(generate_partition_config_sfdisk "EMMC_PARTITION_SIZES[@]" \
                                                                            "EMMC_PARTITION_TYPES[@]")" && \

        update_partition_info "${emmc_card_device}" && \
        create_filesystems_emmc && status="${SUCCESS}"
    fi

    return "$status"
}

function is_in_use
{
    local dev="$1"
    grep -q "$dev" /proc/cmdline /proc/mounts
}

function disable_automount
{
    udevadm control --env __SD_AUTOMOUNT="0"
}

function do_format
{
    local dev="$1"
    local partition_layout="$2"
    # SC2059: Don't use variables in the printf format string. Use printf "..%s.." "$foo".
    # partition_layout string contains escape sequences (\n) which are not interpreted otherwise
    # shellcheck disable=SC2059
    printf "${partition_layout}" | flock "${dev}" /sbin/sfdisk "${dev}"
}

function generate_partition_config_sfdisk
{
    # We create a string in sfdisk input format, i.e.
    #
    # start_offset,size,type,bootable
    #
    # 8,64,b,*
    # 72,256,L
    # 328,256,L
    # ...
    # 
    # with offset being i.e. 72=8+64, 328=72+256 etc.
    
    local -a partition_sizes=("${!1}")
    local -a partition_types=("${!2}")

    local index=0
    local result
    local emmc_card_device=$(/etc/config-tools/get_device_data name "internal-flash-emmc")
    local alignment=$(cat "/sys/block/$emmc_card_device/device/preferred_erase_size")
    local logical_block_size=$(cat "/sys/block/$emmc_card_device/queue/logical_block_size")
    local offset=$((alignment / logical_block_size))
    local extended=false

    for part_size in "${partition_sizes[@]}"; do
        result+="$(generate_partition_config_line \
                    "${offset}" \
                    "${part_size}" \
                    "${partition_types[${index}]}"\
                    "${index}")"

        if [[ "${partition_types[${index}]}" == "E" ]]; then
            extended=true
        fi
        if [[ ${part_size} =~ ^[0-9]+$ ]]; then
            # Adjust alignment that partition starts fit to flash erase block size (assumed from preferred_erase_size)
            local alignment_offset=$(((alignment - (part_size * 1024 * 1024 % alignment)) % alignment))
            # After an extended partition in front of all logical partitions at least one logical block is needed
            # for extended partition information.
            if $extended && [[ $alignment_offset -lt $logical_block_size ]]; then
                alignment_offset=$((alignment_offset + alignment))
            fi
            offset=$((offset + (part_size * 1024 * 1024 / logical_block_size) + (alignment_offset / logical_block_size)))
        else      
            offset=$((offset + (alignment / logical_block_size)))
        fi 
        index=$((index+1))
    done

    echo "${result}"
}

function generate_partition_config_line
{
    local start="${1}"
    local size="${2}"
    local type="${3}"
    local index="${4}"

    local bootable=""
    if [[ $index == 0 ]]; then
        bootable="*"
    fi

    local size_string=$size
    if [[ "$size_string" =~ ^[0-9]+$ ]]; then
        size_string=${size_string}M
    fi
    # Don't expand \n yet, do it above
    # SC2028: echo won't expand escape sequences
    # shellcheck disable=SC2028
    echo "${start},${size_string},${type},${bootable}\n"
}

function create_filesystems_emmc
{
    local index=0
    local pnum=1
    local device
    local typefs
    device="/dev/$(/etc/config-tools/get_device_data name "internal-flash-emmc")"

    for label in "${EMMC_PARTITION_LABELS[@]}"; do
        ptype=${EMMC_PARTITION_TYPES[${index}]}

        do_mkfs "$(generate_partition_dev "${device}" "${pnum}" "mmc")" \
                "${label}" \
                "${EMMC_PARTITION_FS_TYPES[${index}]}"
        if [[ $ptype == "E" ]]; then
            pnum=5 # 5 is the start index of logical partitions
        else
            pnum=$((pnum+1))
        fi
        index=$((index+1))
    done
}

function generate_partition_dev
{
    local dev="${1}"
    local nr="${2}"
    local type="${3}"

    case $type in
        mmc) echo "${dev}p$((nr))" ;;
    esac
}

function do_mkfs
{
    local device="${1}"
    local label="${2}"
    local type="${3}"

    case $type in
        vfat)
            # Follow sfdisk's advice and delete 1st sector of a dos partition
            dd if=/dev/zero of="${device}" bs=512 count=1 && \
            /sbin/mkfs.vfat -n "${label}" "${VFAT_OPTIONS[@]}" "${device}"
            ;;
        ext3)
            /sbin/mke2fs "${device}" -L "${label}" -F && \
            /sbin/tune2fs "${EXT3_OPTIONS[@]}" "${device}" || true   #do not trap on "journal already exists"
            ;;
        ext4)
            /sbin/mkfs.ext4 -L "${label}" -F "${device}" && \
            /sbin/tune2fs "${EXT4_OPTIONS[@]}" "${device}"
            ;;
        *)
            false
            ;;
    esac
}

function mount_target_emmc
{
    local -a partition_mount_points=("${!1}") #partition labels
    local -a partition_fs_types=("${!2}")     #vfat, ext4, " " , ...

    local device
    device="${3:-"/dev/$(/etc/config-tools/get_device_data name "internal-flash-emmc")"}"

    local index=0
    local pnum=1

    for mount_point in "${partition_mount_points[@]}"; do
        ptype=${EMMC_PARTITION_TYPES[${index}]}
        if [[ -n "${mount_point}" && "${partition_fs_types[$index]}" != " " ]]; then
            
            mkdir -p "/tmp/emmc/${mount_point}"

            do_mount "${partition_fs_types[${index}]}" \
                     "$(generate_partition_dev "${device}" "${pnum}" "mmc")" \
                     "/tmp/emmc/${mount_point}"
        fi
        
        if [[ $ptype == "E" ]]; then
            pnum=5 # 5 is the start index of logical partitions
        else
            pnum=$((pnum+1))
        fi        
        index=$((index+1))
    done
}

function copy_sd_to_emmc
{
    local DEST_BOOT=/tmp/emmc/boot
    local DEST_ROOT=/tmp/emmc/rootfs.1
    local DEST_HOME=/tmp/emmc/home1

    copy_rootfs / "$DEST_ROOT"
    
    print_dbg "    processing /boot..."
    ${BACKUP_CMD} /boot/*        ${DEST_ROOT}/boot
    rm -rf ${DEST_ROOT}/boot/loader

    print_dbg "    processing /home..."
    ${BACKUP_CMD} /home/*               ${DEST_HOME}
    
    create_static_nodes $DEST_ROOT
}
