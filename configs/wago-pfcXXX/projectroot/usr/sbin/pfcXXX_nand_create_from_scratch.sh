#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

NAND_DEV_NR=${G_UBIFS_MTD_DEV_NR}
NAND_DEV=${G_UBIFS_MTD_DEV}

test -e ${NAND_DEV} || exit 1

# TODO: warnings and questions are suppressed. Is it ok?

ubiformat -y -q ${NAND_DEV}

ubiattach /dev/ubi_ctrl -m ${MTD_NR}

ubimkvol /dev/${G_UBIFS_ROOT_ID} -N "$G_ROOT1_LABEL" --lebs 556 #logical erase blocks, ~68MiB 
ubimkvol /dev/${G_UBIFS_ROOT_ID} -N "$G_ROOT2_LABEL" --lebs 556 #68MiB
ubimkvol /dev/${G_UBIFS_ROOT_ID} -N "$G_HOME_LABEL"  --lebs 522 #64MiB


