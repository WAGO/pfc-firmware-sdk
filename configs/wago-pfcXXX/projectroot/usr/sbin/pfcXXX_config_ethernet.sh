#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

# Translate port label to port number

options=

while getopts ":p:i:Ws:" opt; do
  
  case $opt in
    p) options="$options -p $(LabelToPort $OPTARG)";;
    W) options="$options -W";;
    *) options="$options -$opt $OPTARG";;
  esac

done

# NOTE: pfc200_ethtool is tolerant of empty suboptions/multiple commas (sth. like -s ,speed=10M, )

/etc/config-tools/pfc200_ethtool $options
status=$?
sync

exit $status
