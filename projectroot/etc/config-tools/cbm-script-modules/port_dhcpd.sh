#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2020 WAGO Kontakttechnik GmbH & Co. KG

#-----------------------------------------------------------------------------#
# Script-name: port_dhcpd.sh
#
# CBM - Console based management - Menu implementation for DHCP-Server config.
#
# Makes use of common functions of script /etc/config-tools/cbm.
# Author: WAGO Kontakttechnik GmbH
#-----------------------------------------------------------------------------#

. ./cbm-script-modules/port_interfaces.sh

GETCONFIG="./get_dhcpd_config"
SETCONFIG="./config_dhcpd"

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

function DhcpdConfig {
    local bridge=$1

    # Set text variables for editing variable menu entries.
    # Used in function MenuItemEdit.
    editSubmenuHeading="Change static host entry"
    editHeading="Edit Static Host Entry"
    editField="Terminate Edit With Return Key"
    editFieldLength="66"

    ShowEvaluateDataWindow "DHCPD Configuration"

    # Get the values for the several parameters.
    local state=$($GETCONFIG -p $bridge -g dhcpd-state)
    local range=$($GETCONFIG -p $bridge -g dhcpd-range)
    local lease_time=$($GETCONFIG -p $bridge -g dhcpd-lease-time)
    local fixhosts=$($GETCONFIG -p $bridge -g dhcpd-fix-host)

    if [ -z $state ]; then
        lasterror=$(cat $LAST_ERROR_FILENAME)
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "DHCPD Configuration on error" \
            "0. Back to Bridge Selection Menu" \
            "" \
            "ERROR: $lasterror"
        return 0
    fi

    # Variables for menu presentation loop.
    local quit=$FALSE
    local selection
    local retUnused

    # Loop until the user wants to quit. fixhosts holds one line with all
    # configured static host entries delimited by commas. This variable is
    # changed during menu actions.
    local processMsg="Process DHCPD Configuration"
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

        # Show parameter selection-menu.
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "DHCPD Configuration Bridge $bridge" \
            "0. Back to Bridge Selection Menu" \
            "1. State..................$state" \
            "2. Range..................$range" \
            "3. Lease Time (min).......$lease_time" \
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
                        "Change DHCPD-State ($state) Bridge $port" \
                        "0. Back to DHCPD Port Selection Menu" \
                        "1. Enable" \
                        "2. Disable"
    
                    # Assign the selected number to the according parameter value.
                    case "$selection" in
                        1) local newState="enabled";;
                        2) local newState="disabled";;
                        *) local newState=""
                    esac
    
                    # If state changed, set it, get actual value, and show possible errors.
                    if [ -n "$newState" ] && [ "$state" != "$newState" ]; then
                        ShowProcessingDataWindow "$processMsg"
                        $SETCONFIG -p $bridge dhcpd-state="$newState"
                        state=$($GETCONFIG -p $bridge -g dhcpd-state)
                        ShowLastError
                    fi
                    ;;
    
                2)  # Range was selected -> show input-window to get new range.
                    WdialogWrapper "--inputbox" retUnused "$TITLE" "Change DHCPD Range Bridge $bridge" \
                        "Enter Range:" 60 $range 2> temp
                    local newRange=$(cat temp)
                    rm temp
    
                    # If new range were given - change it, get the actual value again and show possible errors.
                    if [ -n "$newRange" ] && [ "$newRange" != "$range" ]; then
                        ShowProcessingDataWindow "$processMsg"
                        $SETCONFIG -p $bridge dhcpd-range=$newRange  
                        range=$($GETCONFIG -p $bridge -g dhcpd-range)
                        ShowLastError
                    fi
                    ;;
    
                3)  # DHCPD lease time is selected -> show input-window to get new value.
                    WdialogWrapper "--inputbox" retUnused "$TITLE" "Change DHCPD Lease Time Bridge $bridge" \
                        "Enter a lease time in minutes greater or equal to 2." 8 $lease_time 2> temp
                    local newLeaseTime=$(cat temp)
                    rm temp
    
                    # If new lease time was given - change it, get the actual value again and show possible errors.
                    if [ -n "$newLeaseTime" ] && [ "$newLeaseTime" != "$lease_time" ]; then
                        ShowProcessingDataWindow "$processMsg"
                        $SETCONFIG -p $bridge dhcpd-lease-time=$newLeaseTime
                        lease_time=$($GETCONFIG -p $bridge -g dhcpd-lease-time)
                        ShowLastError
                    fi
                    ;;
    
                4)  # Add static host was selected -> show input-window to get new value.
                    WdialogWrapper "--inputbox" retUnused "$TITLE" "Add Static Host Name" \
                        "Enter mac-addr_ip-addr or hostname_ip-addr" 66 "" 2> temp
                    local newFixhosts=$(cat temp)
                    rm temp
                    if [ -z $fixhosts ]; then
                        fixhosts=$newFixhosts
                    else
                        fixhosts="$fixhosts,$newFixhosts"
                    fi
    
                    # If new host was given - change it, get the actual value again and show possible errors.
                    if [ -n "$newFixhosts" ]; then
                        ShowProcessingDataWindow "$processMsg"
                        $SETCONFIG -p $bridge dhcpd-fix-host=$fixhosts
                        fixhosts=$($GETCONFIG -p $bridge -g dhcpd-fix-host)
                        ShowLastError
                    fi
                    ;;
    
                *)  # Variable menu item selected. Call menu to edit or delete the item.
                    MenuItemEdit $(($selection - $baseItem))
                    ShowProcessingDataWindow "$processMsg"
                    fixhosts=$(ct_array_join varMenuValueList ",")
                    $SETCONFIG -p $bridge dhcpd-fix-host=$fixhosts
                    fixhosts=$($GETCONFIG -p $bridge -g dhcpd-fix-host)
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
} # end of DhcpdConfig

function MainDhcpd {
    declare -a bridges
    declare -a menu_parameters
    local nr=2

    # Build menu entries for port selection.
    for bridge in $(GetBridges); do
        bridges=("${bridges[@]}" "$bridge")
        menu_parameters=("${menu_parameters[@]}" "$nr. $bridge")
        nr=$[$nr + 1]
    done
 
    local quit=$FALSE
    local selection
    declare -a menu_parameters
  
    while [ "$quit" = "$FALSE" ]; do
        # Show menu.
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "DHCPD Port Selection" \
            "0. Back to Ports and Services Menu" \
            "1. DHCPD firewalling" \
            "${menu_parameters[@]}"

        case "$selection" in
        
            0)  # Quit was selected -> end loop and get back to superior menu.
                quit=$TRUE
            ;;

            1)  
                running=$(/etc/config-tools/get_dhcpd_config --json | tr -d " '\"\t\n\r\v" | grep -e 'DhcpdState:enabled')

                [[ "x" != "x${running}" ]] && running="enabled" || running="disabled"

                FirewallServiceInterfaces dhcpd DHCPD ${running}
                ;;

            *)
                if [[ $selection -le $((${#menu_parameters[@]} + 1)) ]]; then
                    DhcpdConfig "${bridges[$(($selection - 2))]}"
                else
                    errorText="Error in wdialog"
                    quit=$TRUE 
                fi
             ;;
        esac
    done
} # End of MainDhcpd.

MainDhcpd

