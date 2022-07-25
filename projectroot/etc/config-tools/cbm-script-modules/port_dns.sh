#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#-----------------------------------------------------------------------------#
# Script-name: port_dns.sh
#
# CBM - Console based management - Menu implementation for DNS service config.
#
# Makes use of common functions of script /etc/config-tools/cbm.
# Author: WAGO GmbH
#-----------------------------------------------------------------------------#

. ./cbm-script-modules/port_interfaces.sh

function MenuListChange
# Change an entry in varMenuValueList.
# Input: $1 = pattern to search for.
#        $2 = new value, empty removes the array entry.
{
    local pattern=$1
    local newval=$2
    local -a newArray=(${varMenuValueList[@]/$pattern/$newval})
    unset varMenuValueList
    varMenuValueList=(${newArray[@]})
    unset newArray
}

function MainDnsConfig
{
    # Set text variables for editing variable menu entries.
    # Used in function MenuItemEdit.
    editSubmenuHeading="Change static host entry"
    editHeading="Edit Static Host Entry"
    editField="Terminate Edit With Return Key"
    editFieldLength="66"

    ShowEvaluateDataWindow "DNS Service Configuration"

    # Get the values for the several parameters.
    local state=$(./get_dns_service_config dns-state)
    local mode=$(./get_dns_service_config dns-mode)
    local fixhosts=$(./get_dns_service_config dns-fix-host)

    if [ -z $state ]; then
        lasterror=$(cat $LAST_ERROR_FILENAME)
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "DNS Configuration on error" \
            "0. Back to Ports and Services Menu" \
            "" \
            "ERROR: $lasterror"
        return 0
    fi

    # Variables for menu presentation loop.
    local quit=$FALSE
    local selection
    local retUnused

    # Loop until the user wants to quit. fixhosts holds one line with all
    # configured static host entries delimited by commas. This variable is changed during menu actions.
    while [ "$quit" = "$FALSE" ]; do
        # Build array of static host values.
        varMenuValueList=($(echo $fixhosts | awk -F, '{for(i = 1; i <= NF; i++) { print $i }}') )

        # Build array of variable menu lines.
        local -a varmenu
        local idx=0
        local baseItem=5
        unset varmenu
        for value in "${varMenuValueList[@]}"; do
            varmenu[$idx]=$(printf "%d. Static Host %d..........%s" $(($idx + $baseItem)) $idx $value)
            idx=$(($idx + 1))
        done
        local numhostentries=$idx

        # Show selection-menu with the DNS-parameters.
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "DNS Service Configuration" \
            "0. Back to Ports and Services Menu" \
            "1. State..................$state" \
            "2. Mode...................$mode" \
            "3. Firewall Status" \
            "4. Add static hostname" \
            "${varmenu[@]}"
    
        # Analyse user selection and do to the according processing.
        if [ $selection -lt 128 ]; then
            case "$selection" in
                0)  # Quit was selected -> end loop and get back to superior menu.
                    quit=$TRUE;;
    
                1)  # State was selected -> show selection-menu with the possible states.
                    WdialogWrapper "--menu" selection \
                        "$TITLE" \
                        "Change DNS State ($state)" \
                        "0. Back to DNS Configuration Menu" \
                        "1. Enable" \
                        "2. Disable"
    
                    # Assign new state according to the selected number.
                    case "$selection" in
                        1) local newState="enabled";;
                        2) local newState="disabled";;
                        *) local newState=""
                    esac
    
                    # If a new state was selected - change it, get actual value again and show possible errors.
                    if [ -n "$newState" ] && [ "$state" != "$newState" ]; then
                        ShowProcessingDataWindow "Process DNS Configuration"
                        ./config_dns_service dns-state="$newState" >& /dev/null
                        state=`./get_dns_service_config dns-state`
                        ShowLastError
                    fi
                    ;;
    
                2)  # Mode was selected -> show input-window to get new mode.
                    WdialogWrapper "--menu" selection \
                        "$TITLE" \
                        "Change DNS Mode ($mode)" \
                        "0. Back to DNS Configuration Menu" \
                        "1. Proxy (Default, caches name resolutions)" \
                        "2. Relay (Forwards requests immediately)"
    
                    case "$selection" in
                        1) local newMode="proxy";;
                        2) local newMode="relay";;
                        *) local newMode=
                    esac
    
                  # If new mode was given - change it, get the actual value again and show possible errors.
                  if [ -n "$newMode" ] && [ "$newMode" != "$mode" ]; then
                        ShowProcessingDataWindow "Process DNS Configuration"
                        ./config_dns_service dns-mode=$newMode >& /dev/null
                        mode=$(./get_dns_service_config dns-mode)
                        ShowLastError
                  fi
                  ;;
 
                3)  
                    running=`/etc/config-tools/get_dns_service_config dns-state`
                    FirewallServiceInterfaces dns DNS ${running}
                    ;;
    
                4)  # Add static host was selected -> show input-window to get new value.
                    WdialogWrapper "--inputbox" retUnused "$TITLE" "Add Static Host Name" \
                        "Enter ip-addr:hostname[_hostname]:" 66 "" 2> temp
                    local newFixhosts=`cat temp`
                    rm temp
                    if [ -z $fixhosts ]; then
                        fixhosts=$newFixhosts
                    else
                        fixhosts="$fixhosts,$newFixhosts"
                    fi

                    # If new hosts were given - change it, get the actual value again and show possible errors.
                    if [ -n "$newFixhosts" ]; then
                        ShowProcessingDataWindow "Process DNS Configuration"
                        ./config_dns_service dns-fix-host=$fixhosts >& /dev/null
                        fixhosts=`./get_dns_service_config dns-fix-host`
                        ShowLastError
                    fi
                    ;;
    
                *)  # Variable menu item selected. Call menu to edit or delete the item.
                    MenuItemEdit $(($selection - $baseItem))
                    ShowProcessingDataWindow "Process DNS Configuration"
                    fixhosts=$(ct_array_join varMenuValueList ",")
                    ./config_dns_service dns-fix-host=$fixhosts >& /dev/null
                    fixhosts=`./get_dns_service_config dns-fix-host`
                    ShowLastError
                    ;;
            esac
        else
            echo "CBM wdialog error $(($selection - 256))" > $LAST_ERROR_FILENAME
            echo "Abort dialog." >> $LAST_ERROR_FILENAME
            ShowLastError
            quit=$true
        fi
    done
} # End of MainDnsConfig.

MainDnsConfig

