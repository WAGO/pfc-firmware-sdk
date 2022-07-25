# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainMousepointer
# Processing of menu Main -> HMI settings -> Mouse pointer
#
{
  # Show Mousepointer? was selected -> show selection-menu with yes/no
 
  local mousePointerConfig=`./get_touchscreen_config mouse-pointer`
  local selection

  local quit=$FALSE
  
  while [ "$quit" = "$FALSE" ]; do
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Show mouse pointer ($mousePointerConfig)" \
              "0. Back to HMI Settings Menu" \
              "1. Yes" \
              "2. No"
  
    ShowProcessingDataWindow "Configuration of the mouse pointer"

    # call the processing routine; signalize that calibrate-touchscreen-flag shall not be changed
    case "$selection" in
      0)  quit=$TRUE;;
      1)  ./config_mousepointer show-mouse-pointer=yes;;
      2)  ./config_mousepointer show-mouse-pointer=no;;
      *)  errorText="Error in wdialog"
          quit=$TRUE;;
    esac

    mousePointerConfig=`./get_touchscreen_config mouse-pointer`
    ShowLastError
  done
}

MainMousepointer
