# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainPortOPCUAStatus
{
  local portOutputText="OPC UA"
  local portParameterText="set opcua enabled"
  local portState=$1
  MainPortGeneric "${portOutputText}" "${portParameterText}" "${portState}"
}

function MainPortOPCUA
{
    ShowEvaluateDataWindow "OPC UA Port Configuration"

    local quit=$FALSE
    local selection

    while [ "$quit" = "$FALSE" ]; do

        local state=$(./config_opcua state)

        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "OPC UA Port Configuration" \
                  "0. Back to Ports and Services Menu" \
                  "1. State .............. $state" \
                  "2. Firewall status"

        case "$selection" in

          0)
              ShowEvaluateDataWindow "Ports and Services"
              quit=$TRUE;;

          1)
              MainPortOPCUAStatus ${state}
              ;;

          2)
              FirewallServiceInterfaces opcua OPCUA ${state}
              ;;

          *)
              errorText="Error in wdialog"
              quit=TRUE;;

        esac
    done
}

MainPortOPCUA

