# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2020 WAGO Kontakttechnik GmbH & Co. KG

PARENT=$(GetParentMenuName ${BASH_SOURCE})

function MainDSA
#
# Processing of the menus Main -> TCP/IP -> DSA Mode
#
#
#
{
  ShowEvaluateDataWindow "Switch Configuration"

  # loop until the user want's to get back to TCP/IP-menu
  local quit=$FALSE
  local selection

  local SWITCHED_STATE_DESC="switched"
  local SEPARATED_STATE_DESC="separated"

  local SWITCHED_MENU_LINE="Switched"
  local SEPARATED_MENU_LINE="Separated"

  while [ "$quit" = "$FALSE" ]; do
    
    local state=$(GetDsaMode)

    case $state in
      0)
        state=$SWITCHED_STATE_DESC
        ;;
      1)
        state=$SEPARATED_STATE_DESC
        ;;
      *)
        state="unknown"
        ;;
    esac

    # show menu

    WdialogWrapper "--menu" selection \
      "$TITLE" \
      "Switch Configuration Mode" \
      "0. Back to ${PARENT} Menu" \
      "1. Network interfaces.........$state"

    case "$selection" in
    
      0)  
          quit=$TRUE
          ;;
      1)
          WdialogWrapper "--menu" autonegSelection \
                    "$TITLE" \
                    "Switch Configuration Mode ($state)" \
                    "0. Back to Switch Configuration Menu" \
                    "1. $SEPARATED_MENU_LINE" \
                    "2. $SWITCHED_MENU_LINE"

          case "$autonegSelection" in
            1)  local stateVal="1";;
            2)  local stateVal="0";;
            *)  local stateVal="";;
          esac

          if [[ "$stateVal" == "1" && "$state" == "$SWITCHED_STATE_DESC" ]] ||
             [[ "$stateVal" == "0" && "$state" == "$SEPARATED_STATE_DESC" ]]; then
 
            ShowProcessingDataWindow "TCP/IP Setting Switch Configuration Mode"
            local ret=$(SetDsaMode $stateVal)
            if [[ $ret -ne 0 ]]; then
              ShowLastError
            fi
          fi
          ;;

        *)  
            errorText="Error in wdialog"
            quit=TRUE
            ;;
    esac

  done
}
# end of MainDSA

MainDSA
