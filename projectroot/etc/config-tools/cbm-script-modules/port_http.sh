# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainPortHttpStatus
{
  local portOutputText="HTTP"
  local portParameterText="http"
  local state=`./config_ssl http-status`

  MainPortGeneric "${portOutputText}" "${portParameterText}" "${state}"
}

function MainPortHttp
{
    ShowEvaluateDataWindow "HTTP Port Configuration"

    local quit=$FALSE
    local selection

    while [ "$quit" = "$FALSE" ]; do

        local state=$(./config_ssl http-status)

        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "HTTP Port Configuration" \
                  "0. Back to Ports and Services Menu" \
                  "1. State .............. $state" \
                  "2. Firewall status"

        case "$selection" in

          0)
              ShowEvaluateDataWindow "Ports and Services"
              quit=$TRUE;;

          1)
              MainPortHttpStatus
              ;;

          2)
              FirewallServiceInterfaces http HTTP ${state}
              ;;

          *)
              errorText="Error in wdialog"
              quit=TRUE;;

        esac
    done
}

MainPortHttp

