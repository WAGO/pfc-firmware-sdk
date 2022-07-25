# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG


function MainAdministrationReboot
#
# Processing of menu Main -> Administration -> Reboot
#
{
  ./wdialog "--msgbox" \
            "$TITLE" \
            "Reboot Controller" \
            "Note: after reboot, the controller" \
            "will be unavailable for a few minutes."

  local selection
  # show menu
  WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Reboot Controller" \
            "0. Back to Administration Menu" \
            "1. Reboot"

  if [ "$selection" = 1 ]; then
    ./wdialog "--infobox" \
              "$TITLE" \
              "Reboot Controller" \
              "Reboot..."

    ./start_reboot
    ShowLastError
  fi
}

MainAdministrationReboot
# end of MainAdministrationReboot

