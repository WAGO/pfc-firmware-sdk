#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#
#
#

umask 0177

DEFAULT_CONF=/etc/firewall/firewall
GENERAL_CONF=/etc/firewall/firewall.conf

if [ ! -s $DEFAULT_CONF ]; then
    echo "err - $DEFAULT_CONF is missing - can't execute."
    exit 1
fi

. $DEFAULT_CONF

if [ ! -s $GENERAL_CONF ]; then
    echo "err - $GENERAL_CONF is missing - can't execute."
    exit 1
fi

. $GENERAL_CONF

FIREWALL_INIT="/etc/init.d/firewall"

do_is_enabled()
{
    if [ "enabled" = ${FIREWALL_GENERAL_STATE:-disabled} ]; then
        echo "enabled"
    else
        echo "disabled"
    fi
}

do_enable()
{
    ENABLED=`do_is_enabled`

    if [ "enabled" != "$ENABLED" ]; then
        echo "FIREWALL_GENERAL_STATE=enabled" > "$GENERAL_CONF"
        sync
        $FIREWALL_INIT restart
    fi
}

do_disable()
{
    echo "FIREWALL_GENERAL_STATE=disabled" > "$GENERAL_CONF"
    sync
    $FIREWALL_INIT stop
}


#
# Main body of the script.
#


cd $FW_DIR

case "$1" in
--is-enabled)
    do_is_enabled
    ;;

--enable)
    do_enable
    ;;

--disable)
    do_disable
    ;;

*)
    echo "Usage: $0 {--is-enabled|--enable|--disable}" >&2
    exit 1
    ;;
esac

exit 0
