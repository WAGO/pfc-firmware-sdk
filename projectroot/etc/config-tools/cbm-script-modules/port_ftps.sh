# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainPortFtpsStatus
{
  local portOutputText="FTPS"
  local portParameterText="ftps"
  local state=`./config_ssl ftps-status`

  MainPortGeneric "${portOutputText}" "${portParameterText}" "${state}"
}

function MainPortFtps
{
    ShowEvaluateDataWindow "FTPS Port Configuration"

    local quit=$FALSE
    local selection

    while [ "$quit" = "$FALSE" ]; do

        local state=$(./config_ssl ftps-status)

        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "FTPS Port Configuration" \
                  "0. Back to Ports and Services Menu" \
                  "1. State .............. $state" \
                  "2. Firewall status"

        case "$selection" in

          0)
              ShowEvaluateDataWindow "Ports and Services"
              quit=$TRUE;;

          1)
              MainPortFtpsStatus
              ;;

          2)
              FirewallServiceInterfaces ftps FTPS ${state}
              ;;

          *)
              errorText="Error in wdialog"
              quit=TRUE;;

        esac
    done
}

MainPortFtps

