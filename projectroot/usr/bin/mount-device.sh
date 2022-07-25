#!/bin/sh

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

DEVICE="$1"

if [ "$DEVICE" = "" ] ; then exit 1 ; fi

#do not mount intern and extern flash memory
CHECKHDDEV=$(echo $DEVICE | grep -m1 -o "/dev/[[:alpha:]][[:alpha:]]")
if [ "$CHECKHDDEV" = "/dev/hd" ] ; then exit 0 ; fi

LABEL=`/sbin/dosfslabel $DEVICE 2>/dev/null`

if [ "$LABEL" = "" ]
then
	LABEL=`/sbin/e2label $DEVICE 2>/dev/null`
fi

if [ "$LABEL" = "" ]
then
	LABEL=${DEVICE##/dev/}
fi

MOUNTPOINT=/media/$LABEL

mkdir -p $MOUNTPOINT
mount $DEVICE $MOUNTPOINT
if [ "$?" != "0" ]
then
	rm -r -f $MOUNTPOINT
fi

