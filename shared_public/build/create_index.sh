#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

FILE="$1"

PLATFORM="$2"
FIRMWARE_MAJOR="$3"
FIRMWARE_MINOR="$4"
FIRMWARE_BUGFIX="$5"
FIRMWARE_INDEX="$6"
SVN_REVISION="$7"
SD_IMAGE="$(basename "$8")"
RAUC_UPDATEFILE="$(basename "$9")"
WUP_FILE="$(basename "${10}")"
OUT_DIR="$(dirname "${10}")"

cat <<EOF > $FILE
{
	"platform": "$PLATFORM",
	"svn_revision": "$SVN_REVISION",
	"firmware": {
		"major" : "$FIRMWARE_MAJOR",
		"minor" : "$FIRMWARE_MINOR",
		"bugfix": "$FIRMWARE_BUGFIX",
		"index" : "$FIRMWARE_INDEX"
	},
	"out_dir": "$OUT_DIR",
	"images": {
		"sd" : "$SD_IMAGE",
		"wup": "$WUP_FILE",
		"RAUC": "$RAUC_UPDATEFILE"
	}
}
EOF