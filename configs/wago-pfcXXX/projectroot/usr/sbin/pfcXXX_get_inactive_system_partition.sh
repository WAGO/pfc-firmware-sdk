#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines 
. $(dirname $0)/${BOARD_ID}_commons

active_medium=$(/etc/config-tools/get_filesystem_data active-partition-medium)

case $active_medium in
  ${INTERNAL_FLASH_EMMC})
    case $(/etc/config-tools/get_systeminfo active-system-number) in
      1) printf "${G_EMMC_ROOT2_DEV}";;
      2) printf "${G_EMMC_ROOT1_DEV}";;
    esac
      ;;
  ${INTERNAL_FLASH_NAND})
    case $(/etc/config-tools/get_systeminfo active-system-number) in
      1) printf "${G_UBIFS_ROOT2_VOL}";;
      2) printf "${G_UBIFS_ROOT1_VOL}";;
    esac
    ;;
esac
