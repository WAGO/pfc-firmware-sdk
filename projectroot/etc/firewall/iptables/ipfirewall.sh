#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#
# Set network layer firewall rules.
#
# For detailed description of firewalling policies please refer firewall
# specification for PFC200.
#

umask 0177

DEFAULT_CONF=/etc/firewall/firewall

if [[ ! -s $DEFAULT_CONF ]]; then
    echo "err - $DEFAULT_CONF is missing - can't execute."
    exit 64
fi

source "$DEFAULT_CONF"

# Please note that the paths below are hard-coded into the config-tools.
FW_IP_RULES_BASE="$FW_IP/ipbase.rls"
FW_IP_RULES_AA="$FW_IP/ipcmn.aa.rls"
FW_IP_RULES_NAT_TEMP="/tmp/ipnat.tmp"
FW_IP_RULES_NAT="$FW_IP/ipnat.rls"
FW_IP_RULES_TEMP="/tmp/ipcmn.tmp"
FW_IP_RULES="$FW_IP/ipcmn.rls"
FW_IP_CONF_XML="$FW_IP/ipcmn.xml"
FW_IP_CONF_XSD="$FW_IP/ipcmn.xsd"
FW_IP_CONF_XSL="$FW_IP/ipcmn.xsl"
FW_IP_NAT_XSL="$FW_IP/ipnat.xsl"
FW_DYN_DIR="/var/run/firewall/iptables"
FW_DYN_DIR_DEFAULT="$FW_DYN_DIR/default"
FW_DYN_DIR_ENABLED="$FW_DYN_DIR/enabled"
FW_DYN_SCRIPTS="/etc/config-tools/events/firewall/iptables"

print_help()
{
    echo "Set network layer firewall rules."
    echo "Usage: $0 [--apply]"
    echo "  --apply                 apply only filtering rules, don't apply"
    echo "                          basic framework rules."
}

#
# Checks if required tools and files are present.
# If not then the script should exit cleanly.
#
check_prerequisites()
{
    which "$FW_IPR" 2>&1 > /dev/null
    if [[ $? -ne 0 ]]; then
        eblog "err" "$FW_IPR tool isn't available - can't execute."
        exit 1
    fi

    if [[ ! -s $FW_XST ]]; then
        eblog "err" "$FW_XST is missing - can't execute."
        exit 1
    fi

    if [[ ! -s $FW_PATTERNS_XSD ]]; then
        eblog "err" "$FW_PATTERNS_XSD is missing - can't execute."
        exit 1
    fi

    if [[ ! -s $FW_PARAMS_XML ]]; then
        eblog "err" "$FW_PARAMS_XML is missing - can't execute."
        exit 1
    fi

    if [[ ! -s $FW_PARAMS_XSD ]]; then
        eblog "err" "$FW_PARAMS_XSD is missing - can't execute."
        exit 1
    fi

    if [[ ! -s $FW_IP_RULES_BASE ]]; then
        eblog "err" "$FW_IP_RULES_BASE is missing - can't execute."
        exit 1
    fi
    
    if [[ ! -s $FW_IP_CONF_XML ]]; then
        eblog "err" "$FW_IP_CONF_XML is missing - can't execute."
        exit 1
    fi

    if [[ ! -s $FW_IP_CONF_XSD ]]; then
        eblog "err" "$FW_IP_CONF_XSD is missing - can't execute."
        exit 1
    fi

    if [[ ! -s $FW_IP_CONF_XSL ]]; then
        eblog "err" "$FW_IP_CONF_XSL is missing - can't execute."
        exit 1
    fi
}

set_nat()
{
    if [[ ! -s $FW_IP_CONF_XML ]]; then
        eblog "err" "$FW_IP_CONF_XML is missing - can't execute."
        exit 1
    fi
    
    if [[ ! -s $FW_IP_NAT_XSL ]]; then
        eblog "err" "$FW_IP_NAT_XSL is missing - can't execute."
        exit 1
    fi
    
    if [[ ${FW_IP_TRANSFORM:-true} != "false" ]]; then
        $FW_XST val --xsd "$FW_IP_CONF_XSD" "$FW_IP_CONF_XML" 2>&1 > /dev/null
        result1=$?
        $FW_XST val --xsd "$FW_PARAMS_XSD" "$FW_PARAMS_XML" 2>&1 > /dev/null
        result2=$?
        if [[ $result1 -eq 0 ]] && [[ $result2 -eq 0 ]] ; then
            $FW_XST tr "$FW_IP_NAT_XSL" "$FW_IP_CONF_XML" > "$FW_IP_RULES_NAT_TEMP"
            cmp "-s" "$FW_IP_RULES_NAT_TEMP" "$FW_IP_RULES_NAT" 2>&1 > /dev/null
            if [[ $? -ne 0 ]]; then
                mv "$FW_IP_RULES_NAT_TEMP" "$FW_IP_RULES_NAT"
                sync
            else
                rm "$FW_IP_RULES_NAT_TEMP"
            fi      
        else
            eblog "err" "$FW_IP_CONF_XML or $FW_PARAMS_XML failed to validate. Keeping old setup."
        fi
    fi

    $FW_IPR -n > /dev/null 2>&1 < "$FW_IP_RULES_NAT"
    if [[ $? -ne 0 ]]; then
        eblog "err" "Failed do set-up network-layer firewall NAT!"
    fi
}

set_dynamic_default()
{
    [[ -d "$FW_DYN_DIR_DEFAULT" ]] && find "${FW_DYN_DIR_DEFAULT}" -type f | sort -u | xargs cat | "${FW_IPR}" -n
}

set_dynamic_enabled()
{
    [[ -d "$FW_DYN_DIR_ENABLED" ]] && find "${FW_DYN_DIR_ENABLED}" -type f | sort -u | xargs cat | "${FW_IPR}" -n
}

set_firewall()
{
    if [[ "--apply" != $1 ]] ; then
        $FW_IPR < "$FW_IP_RULES_BASE"
    fi

    if [[ "${FW_IP_TRANSFORM:-true}" != "false" ]]; then        
        $FW_XST val --xsd "$FW_IP_CONF_XSD" "$FW_IP_CONF_XML" 2>&1 > /dev/null
        result1=$?
        $FW_XST val --xsd "$FW_PARAMS_XSD" "$FW_PARAMS_XML" 2>&1 > /dev/null
        result2=$?
        if [[ $result1 -eq 0 ]] && [[ $result2 -eq 0 ]] ; then
            $FW_XST tr "$FW_IP_CONF_XSL" "$FW_IP_CONF_XML" > "$FW_IP_RULES_TEMP"
            cmp "-s" "$FW_IP_RULES_TEMP" "$FW_IP_RULES" 2>&1 > /dev/null
            if [[ $? -ne 0 ]]; then
                mv "$FW_IP_RULES_TEMP" "$FW_IP_RULES"
                sync
            else
                rm "$FW_IP_RULES_TEMP"
            fi      
        else
            eblog "err" "$FW_IP_CONF_XML or $FW_PARAMS_XML failed to validate. Keeping old setup."
        fi
    fi

    $FW_IPR -n > /dev/null 2>&1 < "$FW_IP_RULES"
    if [[ $? -ne 0 ]]; then
        eblog "err" "Failed do set-up network-layer firewall!"
    fi

    set_nat
    set_dynamic_enabled
    set_dynamic_default

    # Ping broadcast protection should be disabled _only_ if explicitly requested. By default it is on.
    broadcast_protection=$(${FW_XST} sel -N f=http://www.wago.com/security/firewall -T -t --if "/f:firewall/f:ipv4/f:echo[@broadcast_protection='off']" -o "off" --else -o "on" "${FW_IP_CONF_XML}")
    if [[ $broadcast_protection = "on" ]] ; then
        echo "1" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
    else
        echo "0" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
    fi
}

set_firewall_services()
{
    RESULT=$(find /etc/config-tools/events/*/firewall -type f -exec {} start \;)
    if [[ "x" != "x$RESULT" ]]; then
        eblog "err" "Failed to set-up network-layer firewall rules for services"
    fi
}

clean_firewall()
{
    if [[ ! -s $FW_IP_RULES_AA ]]; then
        eblog "err" "$FW_IP_RULES_AA is missing - can't execute."
        exit 1
    fi

    $FW_IPR < "$FW_IP_RULES_AA"

    # TODO: execute NAT transformations
    # Setup dnat, snat, masquerading
    set_nat
    set_dynamic_default
}

#
# Main body of the script.
#

if [[ -n $2 ]]; then
    eblog "warning" "Too many parameters."
    print_help
    exit 1
fi


if [[ $1 == "-h" ]] || [[ $1 == "--help" ]]; then
    print_help
    exit 0
fi

if [[ -n $1 ]] && [[ "--apply" != $1 ]] && [[ "--disable" != $1 ]] && [[ "--services" != $1 ]]; then
    eblog "warning" "Bad parameters."
    print_help
    exit 1
fi

cd "$FW_IP"

[[ -d $FW_DYN_SCRIPTS ]] && run-parts -a "$1" "$FW_DYN_SCRIPTS"

case "$1" in
--disable)
    eblog "notice" "Firewall - cleaning network layer firewall..."
    clean_firewall
    eblog "notice" "Firewall - ...finished."
    ;;

--services)
    eblog "notice" "Firewall - setting network layer firewall for services up..."
    set_firewall_services
    eblog "notice" "Firewall - ...finished."
    ;;

*)
    check_prerequisites

    eblog "notice" "Firewall - setting network layer firewall up..."
    set_firewall "$1"
    eblog "notice" "Firewall - ...finished."
    ;;
esac

exit 0
