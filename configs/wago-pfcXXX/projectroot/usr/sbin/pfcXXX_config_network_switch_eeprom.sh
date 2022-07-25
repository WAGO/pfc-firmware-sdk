#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project ct-board-specific-extensions.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   pfcXXX_config_network_switch_eeprom.sh
#
# Brief:    Setup EEPROM of network switch.
#
# Author:   MOe: WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

# Set error on use of undefined shell variables
set -u

if [ ! -f "/etc/config-tools/board_specific_defines" ]; then
    echo "Missing dependency board_specific_defines"
    exit 64
fi
source "/etc/config-tools/board_specific_defines"

IMAGE_SIZE=256
IMAGE_TMP="/tmp/network_switch_eeprom.bin"
ETHTOOL="/sbin/ethtool"
NETWORK_DEVICE="ethX1"
MAGIC=0xc3ec4951
LOG_TAG="${BOARD_ID}_config_network_switch_eeprom"
LOG="/usr/bin/logger -t $LOG_TAG"

# Error codes
FILE_NOT_FOUND=64
FILE_SIZE_INVALID=65
EEPROM_READ_ERROR=67
EEPROM_WRITE_ERROR=68


function print_usage
{
  echo "Usage: ${BOARD_ID}_config_network_switch_eeprom.sh eeprom-image"
}

function is_equal_content
{
  local status=0
  local is_equal="true"

  # Dump current EEPROM content to file.
  $ETHTOOL -e "$NETWORK_DEVICE" raw on > "$IMAGE_TMP" 2> /dev/null
  status=$?

  if [ $status == 0 ]; then
    diff --brief "$IMAGE" "$IMAGE_TMP" &> /dev/null
    [ $? != 0 ] && is_equal="false"
  fi

  [ -e "$IMAGE_TMP" ] && rm "$IMAGE_TMP"

  echo $is_equal
  return $status
}

function write_eeprom
{
  local status=0 
  local is_equal="false"
  local write_protection_state=0 # 0 => write protection enabled; 1 => write protection disabled

  [ ! -e /sys/class/gpio/gpio509 ] && echo 509 > /sys/class/gpio/export
  echo out > /sys/class/gpio/gpio509/direction

  # Store current write protection state. 
  write_protection_state=$(cat /sys/class/gpio/gpio509/value)
  # Disable EEPROM write protection.
  [ "$write_protection_state" = 0 ] && echo 1 > /sys/class/gpio/gpio509/value

  # Write EEPROM
  $ETHTOOL -E "$NETWORK_DEVICE" magic "$MAGIC" < "$IMAGE" &> /dev/null
  status=$?

  # Restore write protection state
  echo "$write_protection_state" > /sys/class/gpio/gpio509/value

  # Check if EEPROM was written successful.
  if [ $status == 0 ]; then
    is_equal=$(is_equal_content)
    status=$?
    if [ $status == 0 ]; then
      if [ "$is_equal" == "false" ]; then
        $LOG "Error: Found unexpected EEPROM content."
        status=$EEPROM_WRITE_ERROR
      fi
    else
      $LOG "Failed to determine EEPROM content."
    fi
  fi
 
  return $status
}

function has_netdev_eeprom
{
  $($ETHTOOL -e "$NETWORK_DEVICE" length 4 1>/dev/null 2>&1)
}

#### MAIN ####
function main
{
  local status=0
  local is_equal="true"

  # Only execute if network device support eeprom
  if ! has_netdev_eeprom; then
    exit 0
  fi
  
  if [ $# = 1 ]; then
    if [ "$1" = -h ] || [ "$1" = --help ]; then
      print_usage 
      exit 0
    else
      IMAGE="$1"
    fi
  else
    print_usage
    exit 1
  fi
  
  if [ ! -e "$IMAGE" ]; then
    status=$FILE_NOT_FOUND
    $LOG "EEPROM image does not exist: $IMAGE"
  fi

  # Check size of image
  if [ $status = 0 ]; then
    local image_size=$(stat -t "$IMAGE" | cut -f2 -d' ')
    if [ "$image_size" != $IMAGE_SIZE ]; then
      status=$FILE_SIZE_INVALID
      $LOG "EEPROM file image has wrong size: $image_size; expexted size: $IMAGE_SIZE"
    fi
  fi

  if [ $status == 0 ]; then
    is_equal=$(is_equal_content)
    status=$?
    if [ $status != 0 ]; then
      $LOG "Failed to determine EEPROM content."
    fi
  fi
  
  if [ $status == 0 ]; then
    if [ "$is_equal" == "false" ]; then
      write_eeprom
      status=$?
    fi
    if [ $status != 0 ]; then
      $LOG "Failed writing EEPROM."
    fi
  fi
 
  exit $status
}

main "$@"
