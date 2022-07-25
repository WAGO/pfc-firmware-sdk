#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project fwupdate (PTXdist package rauc_bundle_conf).
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   hooks.sh
#
# Brief:    WAGO firmware update hook script for RAUC.
#
# Author:   PEn: WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

# Workaround rauc install lock, so lock is released while hooks called
mv /var/lock/rauc_barebox.lock /var/lock/rauc_barebox.lock.install 
touch /var/lock/rauc_barebox.lock

# Load common definitions and functions
readonly HOOKS_DIR=$(dirname ${0})/hooks
if [ ! -f "$HOOKS_DIR/hooks_common" ]; then
    echo "Missing dependency hooks_common"
    exit 64
fi
source "$HOOKS_DIR/hooks_common"


# Load hook routine for current slot and state
fwupdate_report_info "Call for hook \"${1}\" of slot \"${RAUC_SLOT_NAME}\" (slot class \"${RAUC_SLOT_CLASS}\")"
case ${1} in

    # Pre-Install is not called if install hook is defined
    slot-pre-install)
        [ -e ${HOOKS_DIR} ] && source ${HOOKS_DIR}/slot-${RAUC_SLOT_CLASS}-pre-install
    ;;

    # Post-Install is not called if install hook is defined
    slot-post-install)
        [ -e ${HOOKS_DIR} ] && source ${HOOKS_DIR}/slot-${RAUC_SLOT_CLASS}-post-install
    ;;

    slot-install)
        [ -e ${HOOKS_DIR} ] && source ${HOOKS_DIR}/slot-${RAUC_SLOT_CLASS}-install
    ;;
esac


# Call hook routine
hook_routine $(< /sys/class/wago/system/board_variant)
RESULT=$?

# End of workaround, take lock again
rm /var/lock/rauc_barebox.lock
mv /var/lock/rauc_barebox.lock.install /var/lock/rauc_barebox.lock

if [ "$RESULT" -ne "0" ]; then
    fwupdate_report_info "Hook \"${1}\" of slot \"${RAUC_SLOT_NAME}\" (slot class \"${RAUC_SLOT_CLASS}\") exited with error code $RESULT"
    exit 112
else
    fwupdate_report_info "Hook \"${1}\" of slot \"${RAUC_SLOT_NAME}\" (slot class \"${RAUC_SLOT_CLASS}\") has finished"
    exit 0
fi

