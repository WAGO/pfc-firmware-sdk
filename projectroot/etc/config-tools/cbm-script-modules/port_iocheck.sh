#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#-----------------------------------------------------------------------------#
# Script-name: port_iocheck.sh
#
# CBM - Menu to enable /disable port of iocheck demon.
#
# Author: WAGO GmbH
#-----------------------------------------------------------------------------#

. ./cbm-script-modules/port_interfaces.sh

function MainPortIocheckStatus
{
    local portOutputText="IO-Check"
    local portParameterText=""
    local state=`./get_iocheckport_config state`

    if [ -f /etc/config-tools/events/iocheckport/firewall ];then

        # loop until user wants to get back to main menu
        local selection
        local quit=$FALSE

        while [[ "$quit" == "$FALSE" ]]; do

            ShowEvaluateDataWindow "Port Configuration"

            # if the user selected a port, show a selection menu with the possible port-states
            WdialogWrapper "--menu" selection \
                "$TITLE" \
                "Configuration of $portOutputText port ($state)" \
                "0. Back to Ports and Services Menu" \
                "1. Enable" \
                "2. Disable"

            # assign the port-state to the number returned by the selection-menu
            case "$selection" in
                1) local newState="enabled";;
                2) local newState="disabled";;
                *) local newState="";
                   quit="${TRUE}"
                   ShowEvaluateDataWindow "Ports and Services"
                   ;;
            esac

            # if the user selected a new state -> configure the selected port with the selected state
            if [ -n "$newState" ] && [ "$newState" != "$state" ]; then
                ShowProcessingDataWindow "Port Configuration"
                ./config_iocheckport state=$newState
                if [[ "$?" == "0" ]]; then
                    state=$newState
                fi
                ShowLastError
            fi
        done # main loop
    else
        SetLastError "Firewall is missing. Firewall needs to be installed \nfor blocking / unblocking iocheck demon port."
        ShowLastError
    fi
}

function MainPortIocheck
{
    ShowEvaluateDataWindow "IOCheck Port Configuration"

    local quit=$FALSE
    local selection

    while [ "$quit" = "$FALSE" ]; do

        local state=$(./get_iocheckport_config state)

        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "IOCheck Port Configuration" \
                  "0. Back to Ports and Services Menu" \
                  "1. State .............. $state" \
                  "2. Firewall status"

        case "$selection" in

          0)
              ShowEvaluateDataWindow "Ports and Services"
              quit=$TRUE;;

          1)
              MainPortIocheckStatus
              ;;

          2)
              FirewallServiceInterfaces iocheck IOCheck ${state}
              ;;

          *)
              errorText="Error in wdialog"
              quit=TRUE;;

        esac
    done
}

MainPortIocheck

