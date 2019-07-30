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

DEFAULT_CONF="/etc/firewall/firewall"

if [[ ! -s $DEFAULT_CONF ]]; then
    echo "err - $DEFAULT_CONF is missing - can't execute."
    exit 64
fi

source "$DEFAULT_CONF"


validate()
{
    if [[ -s ${FW_NETIF} ]]; then
        netconf="exists"
    else
        netconf="missing"
    fi


    other_if=$(${FW_XST} sel -T -t -m "/interfaces/iface" --if "ip_settings/port_name and not(device_name='br0' and ip_settings/port_name='X1') and not(device_name='br1' and ip_settings/port_name='X2')" -o "invalid" ${FW_NETIF})

    if [[ _$other_if = "_" ]]; then
        other_if="valid"
    else
        other_if="invalid"
    fi


    br0X1=$(${FW_XST} sel -T -t -m "/interfaces/iface" --if "device_name='br0' and ip_settings/port_name='X1'" -o "valid" ${FW_NETIF})

    if [[ _$br0X1 = "_valid" ]]; then
        br0X1="valid"
    else
        br0X1="invalid"
    fi


    br1X2=$(${FW_XST} sel -T -t -m "/interfaces/iface" --if "device_name='br1' and ip_settings/port_name='X2'" -o "valid" ${FW_NETIF})

    if [[ _$br1X2 = "_valid" ]]; then
        br1X2="valid"
    else
        br1X2="invalid"
    fi


    if [[ $netconf = "exists" ]] && [[ $other_if = "valid" ]] && [[ $br0X1 = "valid" ]] && [[ $br1X2 = "valid" ]] ; then
        eblog "notice" "Firewall - interfaces configuration is valid."
    else
        eblog "alert" "Firewall - interfaces configuration is NOT valid. Firewall is most likely not working properly!"
    fi
}


#
# Main body of the script.
#


cd "$FW_DIR"

eblog "notice" "Firewall - validating interfaces configuration..."
validate

exit 0
