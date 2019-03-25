#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#
# Set firewall rules related to a given service.
#
# For detailed description of firewalling policies please refer firewall
# specification for PFC200.
#

umask 0177

if [ -n "$2" ]; then
    SRV_NAME=$2
else
    SRV_NAME="service"
fi

DEFAULT_CONF=/etc/firewall/firewall

if [ ! -s $DEFAULT_CONF ]; then
    echo "err - $DEFAULT_CONF is missing - can't execute."
    exit 1
fi

. $DEFAULT_CONF
FW_SV_RULES_UP_TEMP="./${SRV_NAME}_up.tmp"
FW_SV_RULES_DOWN_TEMP="./${SRV_NAME}_down.tmp"
FW_SV_RULES_UP="./${SRV_NAME}_up.rls"
FW_SV_RULES_DOWN="./${SRV_NAME}_down.rls"
FW_SV_CONF_XML="./${SRV_NAME}.xml"
FW_SV_CONF_XSD="$FW_SV/service.xsd"
FW_SV_CONF_XSL_UP="$FW_SV/service_up.xsl"
FW_SV_CONF_XSL_DOWN="$FW_SV/service_down.xsl"


print_help()
{
    echo "Set firewall rules related to a given service."
    echo "Usage: $0 <up|down> [service_name]"
}

#
# Checks if required tools and files are present.
# If not then the script should exit cleanly.
#
check_prerequisites()
{
    which $FW_IPR 2>&1 > /dev/null
    if [ $? -ne 0 ]; then
        eblog "err" "$FW_IPR tool isn't available - can't execute."
        exit 1
    fi

    if [ ! -s $FW_XST ]; then
        eblog "err" "$FW_XST is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_PATTERNS_XSD ]; then
        eblog "err" "$FW_PATTERNS_XSD is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_PARAMS_XML ]; then
        eblog "err" "$FW_PARAMS_XML is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_PARAMS_XSD ]; then
        eblog "err" "$FW_PARAMS_XSD is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_TRANSFORM_XSLT ]; then
        eblog "err" "$FW_TRANSFORM_XSLT is missing - can't execute."
        exit 1
    fi

    if [ "$FW_PATTERNS_XSD" != "/etc/firewall/patterns.xsd" ]; then
        eblog "err" "$FW_PATTERNS_XSD not in its hardcoded location (service.xsd) - can't execute."
        exit 1
    fi

    if [ "$FW_TRANSFORM_XSLT" != "/etc/firewall/transform.xsl" ]; then
        eblog "err" "$FW_TRANSFORM_XSLT not in its hardcoded location (service_up.xsl service_down.xsl) - can't execute."
        exit 1
    fi

    if [ ! -s $FW_SV_CONF_XML ]; then
        eblog "err" "$FW_SV_CONF_XML is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_SV_CONF_XSD ]; then
        eblog "err" "$FW_SV_CONF_XSD is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_SV_CONF_XSL_UP ]; then
        eblog "err" "$FW_SV_CONF_XSL_UP is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_SV_CONF_XSL_DOWN ]; then
        eblog "err" "$FW_SV_CONF_XSL_DOWN is missing - can't execute."
        exit 1
    fi
}

transform_rules()
{
    if [ ! -s $FW_SV_RULES_UP ] || [ $FW_SV_CONF_XML -nt $FW_SV_RULES_UP ] || 
       [ $FW_PARAMS_XML -nt $FW_SV_RULES_UP ] || 
       [ ! -s $FW_SV_RULES_DOWN ] || [ $FW_SV_CONF_XML -nt $FW_SV_RULES_DOWN ] || 
       [ $FW_PARAMS_XML -nt $FW_SV_RULES_DOWN ] ; then

        $FW_XST val --xsd $FW_SV_CONF_XSD $FW_SV_CONF_XML 2>&1 > /dev/null
        result1=$?
        $FW_XST val --xsd $FW_PARAMS_XSD $FW_PARAMS_XML 2>&1 > /dev/null
        result2=$?
        if [ ! "$result1" -eq "0" ] || [ ! "$result2" -eq "0" ] ; then
            eblog "err" "$FW_PARAMS_XML failed to validate. Can't transform service's firewall rules."
            return -1
        fi

        if [ ! -s $FW_SV_RULES_UP ] || [ $FW_SV_CONF_XML -nt $FW_SV_RULES_UP ] || [ $FW_PARAMS_XML -nt $FW_SV_RULES_UP ] ; then
            $FW_XST tr $FW_SV_CONF_XSL_UP $FW_SV_CONF_XML > $FW_SV_RULES_UP_TEMP
            sync
            mv $FW_SV_RULES_UP_TEMP $FW_SV_RULES_UP
            sync
            if [ ! "$?" -eq "0" ]; then
                eblog "err" "$FW_SV_CONF_XML failed to transform into 'up' rules file."
            fi
        fi

        if [ ! -s $FW_SV_RULES_DOWN ] || [ $FW_SV_CONF_XML -nt $FW_SV_RULES_DOWN ] || [ $FW_PARAMS_XML -nt $FW_SV_RULES_DOWN ] ; then
            $FW_XST tr $FW_SV_CONF_XSL_DOWN $FW_SV_CONF_XML > $FW_SV_RULES_DOWN_TEMP
            sync
            mv $FW_SV_RULES_DOWN_TEMP $FW_SV_RULES_DOWN
            sync
            if [ ! "$?" -eq "0" ]; then
                eblog "err" "$FW_SV_CONF_XML failed to transform into 'down' rules file."
            fi
        fi
    fi

    return 0
}

set_firewall_up()
{
# First 'down' call may fail if the rules weren't set-up previously. That's OK.
    $FW_IPR -n > /dev/null 2>&1 < $FW_SV_RULES_DOWN
    $FW_IPR -n > /dev/null 2>&1 < $FW_SV_RULES_UP
    if [ "$?" -ne "0" ]; then
        eblog "err" "Failed to set-up service's network-layer firewall!"
    fi
}

set_firewall_down()
{
    $FW_IPR -n > /dev/null 2>&1 < $FW_SV_RULES_DOWN
    if [ "$?" -ne "0" ]; then
        eblog "err" "Failed to remove service's firewall rules!"
    fi
}


#
# Main body of the script.
#


if [ -n "$3" ]; then
    eblog "warning" "Too many parameters."
    print_help
    exit 1
fi

if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    print_help
    exit 0
fi

if [ "$1" != "up" ] && [ "$1" != "down" ] ; then
    print_help
    exit 0
fi

cd $FW_SV
check_prerequisites

transform_rules

if [ -z "$1" ] || [ "$1" = "up" ] ; then
    eblog "notice" "[${SRV_NAME}] Setting service's firewall rules up..."
    set_firewall_up
    eblog "notice" "[${SRV_NAME}] Service's firewall rules are up."
else
    eblog "notice" "[${SRV_NAME}] Removing service's firewall rules..."
    set_firewall_down
    eblog "notice" "[${SRV_NAME}] Service's firewall rules are removed."
fi

exit 0
