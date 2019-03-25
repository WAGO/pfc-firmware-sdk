#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#

# Add a MAC address to the Marvell switch address translation unit (ATU) 
# The entry will be static hence not removed by ATU aging process
# Frames having the destination MAC will be forwarded to the ports specified.
# Usage:
# pfcXXX_mv88e6321_write_atu.sh <MAC address> [ports..]
#   MAC address - The destination MAC address in the format like 00:30:DE:01:02:03
#   ports..     - The ports to forward the frame with the given destination MAC address as separated list, e.g. 3 4 6
#                 When no port is specified, frames with this destination get dropped by the switch. 


GLOBAL_REG=$((0x1B))
GLOBAL_REG_ATU_FID=$((0x01))
GLOBAL_REG_OP=$((0x0B))
GLOBAL_REG_OP_ATU_BUSY=$((1<<15))
GLOBAL_REG_OP_MASK=$((7<<12))
GLOBAL_REG_OP_GET_NEXT=$((4<<12))
GLOBAL_REG_OP_LOADPURGE=$((3<<12))
GLOBAL_REG_DATA=$((0x0C))
GLOBAL_REG_MAC_0=$((0x0D))
GLOBAL_REG_MAC_1=$((0x0E))
GLOBAL_REG_MAC_2=$((0x0F))

function read_reg()
{
  swconfig dev switch0 set mdio_phy $1
  swconfig dev switch0 set mdio_reg $2
  REG_VAL=$(swconfig dev switch0 get mdio_rw_mdio_generic)
  #printf " READ ADDR: 0x%02X REG: 0x%02X VAL: 0x%04X\n" $1 $2 $REG_VAL
}

function write_reg()
{
  swconfig dev switch0 set mdio_phy $1
  swconfig dev switch0 set mdio_reg $2
  swconfig dev switch0 set mdio_rw_mdio_generic $3
  #printf "WRITE ADDR: 0x%02X REG: 0x%02X VAL: 0x%04X\n" ${1} ${2} ${3}
}

function wait_atu_ready()
{
  read_reg $GLOBAL_REG $GLOBAL_REG_OP
  while [ $(($REG_VAL & $GLOBAL_REG_OP_ATU_BUSY)) != 0 ]; do
    read_reg $GLOBAL_REG $GLOBAL_REG_OP
  done
}

function set_fid()
{
  write_reg $GLOBAL_REG $GLOBAL_REG_ATU_FID ${1}
}


# Pass port numbers plain e.g. 'generate_dpv_value 1 2 3'
function generate_dpv_value()
{
  # Create value for register 12 bits 4:14
  local dpv=0;
  for port in $@; do
    dpv=$(($dpv | 1<<($port+4) ))
  done
  echo $dpv
}

#  Assume a mac string like "00:30:DE:01:02:03" and split it into register values
#+ that are put into variables MAC0 MAC1 MAC2 
function split_mac_to_regvalues()
{
  local mac=${1//:/} 
  MAC0=$(( 0x${mac:0:4} ))
  MAC1=$(( 0x${mac:4:4} ))
  MAC2=$(( 0x${mac:8:4} ))
}


function write_entry()
{
  read_reg $GLOBAL_REG $GLOBAL_REG_OP
  VAL=$(($REG_VAL & ~GLOBAL_REG_OP_MASK))
  VAL=$(($VAL | $GLOBAL_REG_OP_LOADPURGE))
  VAL=$(($VAL | $GLOBAL_REG_OP_ATU_BUSY))
  write_reg $GLOBAL_REG $GLOBAL_REG_OP $VAL
}

split_mac_to_regvalues $1
#printf "MAC0=%04X MAC1=%04X MAC2=%04X\n" $MAC0 $MAC1 $MAC2
shift
wait_atu_ready
write_reg $GLOBAL_REG $GLOBAL_REG_MAC_0 $MAC0
write_reg $GLOBAL_REG $GLOBAL_REG_MAC_1 $MAC1
write_reg $GLOBAL_REG $GLOBAL_REG_MAC_2 $MAC2
set_fid 0

if (( $MAC0 & 1 ))
then
 ENTRY_STATE=$((0x7)) # static multicast entry
else
 ENTRY_STATE=$((0xE)) #static unicast entry
fi

write_reg $GLOBAL_REG $GLOBAL_REG_DATA $(( $ENTRY_STATE | $(generate_dpv_value $@) ))

write_entry

