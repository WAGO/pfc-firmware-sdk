#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2021 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   webvisu_filewatch.sh
#
# Brief:    replace webvisu.js file 
#
# Author:   elrest Automationssysteme GmbH
#
#-----------------------------------------------------------------------------#

TRG="/home/codesys/PlcLogic/visu/webvisu.js"
SRC="/etc/specific/webvisu.js"
NOTIFY="/usr/bin/inotifywait"

function Replace {
  if [ -e "$SRC" ]; then
    diff "$SRC" "$TRG" 2>&1 > /dev/null
    if [ ! $? == 0 ]; then    
      echo "replace"
      cp -f "$SRC" "$TRG" 2>&1 > /dev/null
      chmod 664 "$TRG"
      sync
    else
      echo "equal"
    fi
  fi
}

if [ -e "$NOTIFY" ]; then
  while true ; do
    if [ -e "$TRG" ]; then
      Replace
      while inotifywait -e modify $TRG; do
        Replace
      done
    fi
    sleep 5
    #echo "sleep"
  done
else
  Replace
fi
