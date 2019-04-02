# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainPortModbusUdpStatus
{
  local portOutputText="Modbus UDP"
  local portParameterText="set udp enabled"
  local portState=$1
  MainPortGeneric "${portOutputText}" "${portParameterText}" "${portState}"
}

function MainPortModbusUdp
{
    ShowEvaluateDataWindow "Modbus UDP Port Configuration"

    local quit=$FALSE
    local selection

    while [ "$quit" = "$FALSE" ]; do

        local state=$(./modbus_config get udp enabled)
        if [ "$state" = "true" ]; then
            state="enabled"
        else
            state="disabled"
        fi
        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "Modbus UDP Port Configuration" \
                  "0. Back to Ports and Services Menu" \
                  "1. State .............. $state" \
                  "2. Firewall status"

        case "$selection" in

          0)
              ShowEvaluateDataWindow "Ports and Services"
              quit=$TRUE;;

          1)
              MainPortModbusUdpStatus ${state}
              ;;

          2)
              FirewallServiceInterfaces modbus_udp MODBUSUDP ${state}
              ;;

          *)
              errorText="Error in wdialog"
              quit=TRUE;;

        esac
    done
}

MainPortModbusUdp

