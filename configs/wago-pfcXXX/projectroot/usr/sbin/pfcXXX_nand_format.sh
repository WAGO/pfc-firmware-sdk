#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

MTD_NR=${G_UBIFS_MTD_DEV_NR}
NAND_DEV=${G_UBIFS_MTD_DEV}

test -e ${NAND_DEV} || exit 1

# Check if device is in use
grep -q /dev/${G_UBIFS_HOME_VOL} /proc/mounts && exit 2


# TODO: check which root partition is backup one and set BACKUP_ROOT
BACKUP_ROOT_NR=2
BACKUP_ROOT=/dev/${G_UBIFS_ROOT_ID}_${BACKUP_ROOT_NR}

# ubirmvol --name "root2" or sth?
ubirmvol /dev/${G_UBIFS_ROOT_ID} -n $(BACKUP_ROOT_NR) 
ubimkvol /dev/${G_UBIFS_ROOT_ID} -N "root"${BACKUP_ROOT_NR} -s 56MiB
