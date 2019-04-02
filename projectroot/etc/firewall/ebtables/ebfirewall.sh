#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#
# Set link layer firewall rules.
#
# For detailed description of firewalling policies please refer firewall
# specification for PFC200.
#

umask 0177

DEFAULT_CONF=/etc/firewall/firewall

if [ ! -s $DEFAULT_CONF ]; then
    echo "err - $DEFAULT_CONF is missing - can't execute."
    exit 1
fi

. $DEFAULT_CONF

# Please note that the paths below are hard-coded into the config-tools.
FW_EB_WLIST_XML="$FW_EB/ebwlist.xml"   # path to link-layer configuration file
FW_EB_WLIST_XSD="$FW_EB/ebwlist.xsd"   # path to XML Schema file describing the ebwlist.xml configuration file.
FW_EB_WLIST_XSL="$FW_EB/ebwlist.xsl"   # path to xslt style sheet used to transform ebwlist.xml into end ebwlist.rls form.
FW_EB_RULES_AA="$FW_EB/ebwlist.aa.rls" # path to link-layer all-allow rules file
FW_EB_RULES="$FW_EB/ebwlist.rls"       # path to link-layer rules file
FW_EB_RULES_TEMP="$FW_EB/ebwlist.tmp"  # path to link-layer rules temporary file


print_help()
{
    echo "Set link layer firewall rules."
    echo "Usage: $0"
}

#
# Checks if required tools and files are present.
# If not then the script should exit cleanly.
#
check_prerequisites()
{
    which $FW_EBR 2>&1 > /dev/null
    if [ $? -ne 0 ]; then
        eblog "err" "$FW_EBR tool isn't available - can't execute."
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

    if [ ! -s $FW_EB_WLIST_XML ]; then
        eblog "err" "$FW_EB_WLIST_XML is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_EB_WLIST_XSD ]; then
        eblog "err" "$FW_EB_WLIST_XSD is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_EB_WLIST_XSL ]; then
        eblog "err" "$FW_EB_WLIST_XSL is missing - can't execute."
        exit 1
    fi

    if [ ! -s $FW_EB_RULES_AA ]; then
        eblog "err" "$FW_EB_RULES_AA is missing - can't execute."
        exit 1
    fi
}

set_firewall()
{
    if [ "${FW_EB_TRANSFORM:-true}" != "false" ]; then
        if [ ! -s $FW_EB_RULES ] || [ $FW_EB_WLIST_XML -nt $FW_EB_RULES ] || [ $FW_PARAMS_XML -nt $FW_EB_RULES ] ; then
            $FW_XST val --xsd $FW_EB_WLIST_XSD $FW_EB_WLIST_XML 2>&1 > /dev/null
            result1=$?
            $FW_XST val --xsd $FW_PARAMS_XSD $FW_PARAMS_XML 2>&1 > /dev/null
            result2=$?
            if [ "$result1" -eq "0" ] && [ "$result2" -eq "0" ] ; then
                $FW_XST tr $FW_EB_WLIST_XSL $FW_EB_WLIST_XML > $FW_EB_RULES_TEMP
                sync
                mv $FW_EB_RULES_TEMP $FW_EB_RULES
                sync
            else
                eblog "err" "$FW_EB_WLIST_XML or $FW_PARAMS_XML failed to validate. Setting all-allow mode."
                cp $FW_EB_RULES_AA $FW_EB_RULES
                sync
            fi
        fi
    fi

    $FW_EBR  > /dev/null 2>&1 < $FW_EB_RULES
    if [ "$?" -ne "0" ]; then
        eblog "err" "Failed do set-up link-layer firewall!"
    fi
}

clean_firewall()
{
    if [ ! -s $FW_EB_RULES_AA ]; then
        eblog "err" "$FW_EB_RULES_AA is missing - can't execute."
        exit 1
    fi

    $FW_EBR  > /dev/null 2>&1 < $FW_EB_RULES_AA
}


#
# Main body of the script.
#


if [ -n "$2" ]; then
    eblog "warning" "Too many parameters."
    print_help
    exit 1
fi

if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    print_help
    exit 0
fi

cd $FW_EB


case "$1" in
--disable)
    eblog "notice" "Firewall - cleaning link layer firewall..."
    clean_firewall
    eblog "notice" "Firewall - ...finished."
    ;;

*)
    check_prerequisites

    eblog "notice" "Firewall - setting link layer firewall up..."
    set_firewall
    eblog "notice" "Firewall - ...finished."
    ;;
esac


exit 0
