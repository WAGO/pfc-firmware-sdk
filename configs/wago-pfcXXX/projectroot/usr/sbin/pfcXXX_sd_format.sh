#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

rm -f $LAST_ERROR_FILENAME

SDCARD_DEV=$1

test -e ${SDCARD_DEV} || { echo "${SDCARD_DEV} does not exist!" > $LAST_ERROR_FILENAME; exit 1; }

# -j: add journal
# -i: set time intervall before auto-fsck
# - c: set mount count before auto-fsck
#
# Setting them to 0 disables auto-fsck altogether

EXT3_OPTIONS='-j -i 0 -c 0' 

# Abort if device is in use
grep -q $SDCARD_DEV /proc/mounts && { echo "Device $SDCARD_DEV is in use (see mount table)!" > $LAST_ERROR_FILENAME; exit 2; }

sd_card_size=$(fdisk -l ${SDCARD_DEV} | grep MB | awk '{print $5}')

# Delete MBR (win7 stores its FAT there :( )
dd if=/dev/zero of=${SDCARD_DEV} bs=446 count=1

# Create 2 partitions: 1st one 16MB large, 2nd one on the rest of
# space.
if [[ "${sd_card_size}" != "" ]]; then
    flock ${SDCARD_DEV} sfdisk ${SDCARD_DEV} <<EOF
,16M,b,*
,,L
EOF

# Wait for udev to update the device nodes
udevadm settle

# Create file systems.

mkfs.vfat ${SDCARD_DEV}p1 -n "BOOT" -i 7761676F

mke2fs ${SDCARD_DEV}p2 -L "ROOT"
tune2fs ${EXT3_OPTIONS} ${SDCARD_DEV}p2 

fi
