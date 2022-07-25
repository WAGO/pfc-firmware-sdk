# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2012-2022 WAGO GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainPortHttpsStatus
{
  local state=`./config_ssl https-status`

  ./wdialog "--msgbox" \
            "$TITLE " \
            "HTTPS Port Configuration ($state)" \
            "" \
            "The HTTPS port can be blocked via the firewall." \
            ""
}

function MainPortHttps
{
    ShowEvaluateDataWindow "HTTPS Port Configuration"

    local quit=$FALSE
    local selection

    while [ "$quit" = "$FALSE" ]; do

        local state=$(./config_ssl https-status)

        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "HTTPS Port Configuration" \
                  "0. Back to Ports and Services Menu" \
                  "1. State .............. $state" \
                  "2. Firewall status"

        case "$selection" in

          0)
              ShowEvaluateDataWindow "Ports and Services"
              quit=$TRUE;;

          1)
              MainPortHttpsStatus
              ;;

          2)
              FirewallServiceInterfaces https HTTPS ${state}
              ;;

          *)
              errorText="Error in wdialog"
              quit=TRUE;;

        esac
    done
}

MainPortHttps

