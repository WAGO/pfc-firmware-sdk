#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2019 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   firewall_apply.sh
#
# Brief:    Config tool to apply firewall rules for known services.
#
# Author:   MP:  WAGO Kontakttechnik GmbH & Co. KG
# Author:   MOe: WAGO Kontakttechnik GmbH & Co. KG
# Author:   OG:  WAGO Kontakttechnik GmbH & Co. KG
# Author:   PEn: WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#


firewall="/etc/config-tools/firewall"

usage="Wrapper for firewall config-tool with automatic service state detection (up|down).
Usage is identical as in the case of the firewall config-tool, with the few changes:
- it works only for services! (firewall, iptables and ebtables are not allowed)
- --apply up|down option is not allowed
- it automatically detects state of the service (enabled|disabled)
- if the service is disabled it simply apply all given options as the are
- if the service is enabled it applys all given options plus --apply up at the end"


print_help()
{
    echo "$usage"
}


firewall_get_service_state()
{
    local active=0
    local running=" "

    case "$1" in
    codesysr)
        running=$(/etc/config-tools/get_plc_config)
        if [[ "2" == "$running" ]] ; then
            local stater=$(/etc/config-tools/get_rts3scfg_value PLC DisableTcpIpProgramming)
            [[ "NO" == "$stater" ]] && active=1
        elif [[ "3" == "$running" ]] ; then
            local active=1
        fi
        ;;

    codesysw)
        local statew=" "
        running=$(/etc/config-tools/get_plc_config)
        if [[ "2" == "$running" ]] ; then
            statew=$(/etc/config-tools/get_port_state codesys-webserver)
            [[ "enabled" == "$statew" ]] && active=1
        elif [[ "3" == "$running" ]] ; then
            statew=$(/etc/config-tools/get_port_state codesys3-webserver)
            [[ "enabled" == "$statew" ]] && active=1
        fi
        ;;

    dhcpd)
        running=$(/etc/config-tools/get_dhcpd_config --json | tr -d " '\"\t\n\r\v" | grep -e 'dhcpdState:enabled')
        if [[ "x" != "x${running}" ]] ; then
            active=1
        fi
        ;;

    dnp3)
        running=$(/etc/config-tools/get_telecontrol_state service=dnp3)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    iec60870_5_104)
        running=$(/etc/config-tools/get_telecontrol_state service=iec60870_5_104)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    iec61850_mms)
        running=$(/etc/config-tools/get_telecontrol_state service=iec61850_mms)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    dns)
        running=$(/etc/config-tools/get_dns_service_config -g dns-state)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    ftp)
        running=$(/etc/config-tools/config_ssl ftp-status)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    ftps)
        running=$(/etc/config-tools/config_ssl ftps-status)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    http)
        running_http=$(/etc/config-tools/config_ssl http-status)
        running_https=$(/etc/config-tools/config_ssl https-status)
        if [[ "enabled" == "$running_http" ]] || [[ "enabled" == "$running_https" ]] ; then
            active=1
        fi
        ;;

    https)
        running=$(/etc/config-tools/config_ssl https-status)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    iocheck)
        running=$(/etc/config-tools/get_iocheckport_config state)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    modbus_tcp)
        #we need sudo priviliges here, otherwise modbus_config will fail
        running=$(sudo /etc/config-tools/modbus_config get tcp enabled)
        if [[ "true" == "$running" ]] ; then
            active=1
        fi
        ;;

    modbus_udp)
        #we need sudo priviliges here, otherwise modbus_config will fail
        running=$(sudo /etc/config-tools/modbus_config get udp enabled)
        if [[ "true" == "$running" ]] ; then
            active=1
        fi
        ;;

    snmp)
        running=$(/etc/config-tools/get_port_state snmp)
        if [[ "enabled" == "$running" ]] ; then
            v1v2state=$(/etc/config-tools/get_snmp_data v1-v2c-state)
            if [[ "enabled" == "$v1v2state" ]] ; then
                active=1
            fi
        fi
        ;;

    snmps)
        running=$(/etc/config-tools/get_port_state snmp)
        if [[ "enabled" == "$running" ]] ; then
            v1v2state=$(/etc/config-tools/get_snmp_data v1-v2c-state)
            if [[ "enabled" != "$v1v2state" ]] ; then
                active=1
            fi
        fi
        ;;

    ssh)
        running=$(/etc/config-tools/get_ssh_config state)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    telnet)
        running=$(/etc/config-tools/get_port_state telnet)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    tftp)
        running=$(/etc/config-tools/get_tftp_config state)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    opcua)
        running=$(/etc/config-tools/config_opcua state)
        if [[ "enabled" == "$running" ]] ; then
            active=1
        fi
        ;;

    *)
        ;;
    esac

    return $active
}


firewall_apply()
{
    logger -s -t firewall_apply.sh -- "$@"

    firewall_get_service_state "$1"

    if [[ 1 -eq $? ]]; then
        local apply=" --apply up"
    else
        local apply=""
    fi

    if [[ -n "$2" ]] || [[ -n "$apply" ]]; then
        sudo $firewall "$@" $apply
    fi
}

date +"%d.%m.%Y %H:%M $1" >> "/tmp/firewall.log"

if [[ -n "$1" ]]; then
    firewall_apply "$@"
else
    print_help
fi

