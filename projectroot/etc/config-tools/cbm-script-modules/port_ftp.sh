# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainPortFtpStatus
{
  local portOutputText="FTP"
  local portParameterText="ftp"
  local state=`./config_ssl ftp-status`

  MainPortGeneric "${portOutputText}" "${portParameterText}" "${state}"
}

function MainPortFtp
{
    ShowEvaluateDataWindow "FTP Port Configuration"

    local quit=$FALSE
    local selection

    while [ "$quit" = "$FALSE" ]; do

        local state=$(./config_ssl ftp-status)

        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "FTP Port Configuration" \
                  "0. Back to Ports and Services Menu" \
                  "1. State .............. $state" \
                  "2. Firewall status"

        case "$selection" in

          0)
              ShowEvaluateDataWindow "Ports and Services"
              quit=$TRUE;;

          1)
              MainPortFtpStatus
              ;;

          2)
              FirewallServiceInterfaces ftp FTP ${state}
              ;;

          *)
              errorText="Error in wdialog"
              quit=TRUE;;

        esac
    done
}

MainPortFtp

