# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainKeyboard
#
# Processing of menu Main -> HMI settings -> Keyboard
# 
{

  local keyboardLayout=`./get_coupler_details keyboard-layout`
  local selection
  # keyboard layout was selected -> show selection-menu to get the new value
  
  WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Configuration of Keyboard Layout ($keyboardLayout)" \
            "0. Back to HMI Settings Menu" \
            "1. German" \
            "2. English"

  ShowProcessingDataWindow "Configuration of Keyboard Layout"

  # assign the keyboard-layout to the number returned by the selection-menu
  local newKeyboardLayout=""
  case "$selection" in
    0)  quit=$TRUE;;
    1)  local newKeyboardLayout=German;;
    2)  local newKeyboardLayout=English;;
    *)  errorText="Error in wdialog"
        quit=$TRUE;;
 
  esac

  if [ -n "$newKeyboardLayout" ] && [ "$newKeyboardLayout" != "$keyboardLayout" ]; then
    ./change_keyboard_layout keyboard-layout=$newKeyboardLayout
    keyboardLayout=`./get_coupler_details keyboard-layout`
    ShowLastError
  fi
}

MainKeyboard 
