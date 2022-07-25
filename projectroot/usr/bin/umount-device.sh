#!/bin/sh

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

DEVICE="$1"

MOUNTPOINT=`mount | grep "$DEVICE[ ]" | cut -d " " -f 3`

if [ -d "$MOUNTPOINT" ]
then
	umount "$DEVICE"
	sleep 1
	rm -rf "$MOUNTPOINT"
fi
