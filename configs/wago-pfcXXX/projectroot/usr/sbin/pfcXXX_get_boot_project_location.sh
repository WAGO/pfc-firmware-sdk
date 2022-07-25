#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

codesysConfigBootProjectLocation=$1
currentBootMedium=$2

if [[ "$currentBootMedium" == "$INTERNAL_FLASH_NAND" ]]; then

    case $codesysConfigBootProjectLocation in
      CARD*)
        echo $SD_CARD
        ;;
      HOME*)
        echo $INTERNAL_FLASH_NAND
        ;;
      *)
        echo "unknown"
        ;;
    esac

elif [[ "$currentBootMedium" == "$SD_CARD" ]]; then

    case $codesysConfigBootProjectLocation in
      CARD*)
        echo $SD_CARD
        ;;
      HOME*)
        echo $SD_CARD
        ;;
      *)
        echo "unknown"
        ;;
    esac

fi

