# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainTftp
#
# Processing of menu Main -> TFTP
# Show and change the several TFTP-values
#
{
  ShowEvaluateDataWindow "TFTP Configuration"

  # loop until the user wants to quit
  local quit=$FALSE
  local selection
  local retUnused

  local state=$(./get_tftp_config state)
  local dlDir=$(./get_tftp_config download-dir)

  while [ "$quit" = "$FALSE" ]; do

    # evaluate state of server new after any changes (can switch to disabled due to errors)

    # show selection-menu with the several TFTP-parameters
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "TFTP Configuration" \
              "0. Back to Ports and Services Menu" \
              "1. State................$state" \
              "2. Transfer Directory...$dlDir" \
              "3. Status of firewalling"
    
    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          ShowEvaluateDataWindow "Ports and Services"
          quit=$TRUE;;

      1)  # state was selected -> show selection-menu with the possible TFTP-states
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change TFTP-State ($state$activeState)" \
                    "0. Back to TFTP Configuration Menu" \
                    "1. Enable" \
                    "2. Disable"
          
          # assign the selected number to the according clock-display-modes
          case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState=""
          esac

          # if a new state was selected - change it, get tconfig_sntp port=he actual value again and show possible errors
          if [[ -n "$newState" ]] && [[ "$state" != "$newState" ]] && [[ "$dlDir" != "" ]]; then
            ShowProcessingDataWindow "TFTP Configuration"
            ./config_tftp state=$newState 
            state=$(./get_tftp_config state)
            ShowLastError
          elif [[ "$dlDir" == "" ]]; then
            ./wdialog "--msgbox" "$TITLE" "TFTP Configuration" " " \
            "Please set the transfer directory first!" " "
          fi
          ;;

      2)  # download directory was selected -> show input-window to get the new transfer directory
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change TFTP Transfer Directory" "Enter new directory:" 32 $dlDir 2> temp
          local newDlDir=`cat temp`
          rm temp

          # if a new port was given - change it, get the actual value again and show possible errors
          if [ -n "$newDlDir" ] && [ "$newDlDir" != "$dlDir" ]; then
            ShowProcessingDataWindow "TFTP Configuration"
            ./config_tftp download-dir=$newDlDir
            local dlDir=$(./get_tftp_config download-dir)
            ShowLastError 
          fi
          ;;

      3)  # 
          running=$(./get_tftp_config state)
          FirewallServiceInterfaces tftp TFTP ${running}
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}

# end of MainTftp

MainTftp


