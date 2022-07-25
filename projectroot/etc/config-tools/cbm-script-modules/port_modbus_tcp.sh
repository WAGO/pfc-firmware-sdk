# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainPortModbusTcpStatus
{
  local portOutputText="Modbus TCP"
  local portParameterText="set tcp enabled"
  local portState=$1
  MainPortGeneric "${portOutputText}" "${portParameterText}" "${portState}"
}

function MainPortModbusTcp
{
    ShowEvaluateDataWindow "Modbus TCP Port Configuration"

    local quit=$FALSE
    local selection

    while [ "$quit" = "$FALSE" ]; do

        local state=$(./modbus_config get tcp enabled)
        if [ "$state" = "true" ]; then
            state="enabled"
        else
            state="disabled"
        fi
        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "Modbus TCP Port Configuration" \
                  "0. Back to Ports and Services Menu" \
                  "1. State .............. $state" \
                  "2. Firewall status"

        case "$selection" in

          0)
              ShowEvaluateDataWindow "Ports and Services"
              quit=$TRUE;;

          1)
              MainPortModbusTcpStatus ${state}
              ;;

          2)
              FirewallServiceInterfaces modbus_tcp MODBUSTCP ${state}
              ;;

          *)
              errorText="Error in wdialog"
              quit=TRUE;;

        esac
    done
}

MainPortModbusTcp

