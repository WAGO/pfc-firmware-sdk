#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of PTXdist package wago-custom-install.
#
# Copyright (c) 2017-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   rauc_failsafe
#
# Brief:    "rauc" wrapper with retries in case of blocked resources.
#
# Author:   MLa: WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

declare readonly RAUC="/usr/bin/rauc"
declare readonly RAUC_BAREBOX_LOCK="/var/lock/rauc_barebox.lock" 

function rauc_status_fail_safe
{ 
    flock "${RAUC_BAREBOX_LOCK}" $RAUC "$@"
}

function main
{ 
    if [[ ! -e "$RAUC_BAREBOX_LOCK" ]]; then
        touch "$RAUC_BAREBOX_LOCK"
    fi
    rauc_status_fail_safe $@
}

main $@
