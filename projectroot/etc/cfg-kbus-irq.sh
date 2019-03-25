#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

IRQ=`cat /proc/interrupts | grep KBUS | sed -e "s/:.*//" -e "s/ //g"`
TID=`ps aux | grep -m 1 "\-$IRQ" | tr -s " " "|" | cut -d '|' -f 2`
#TID=`ps | grep "IRQ-"$IRQ | head -n1 | sed -e "s/ //g" | grep -o "^[[:digit:]]\+"`

echo "Set IRQ-"$IRQ " on TID="$TID " to highest RT-Priority !!!"

[ ! -z $TID ] && chrt -f -p 98 $TID


