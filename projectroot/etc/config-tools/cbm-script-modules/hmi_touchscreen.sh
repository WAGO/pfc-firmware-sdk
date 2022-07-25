# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainAdministrationTouchscreen
#
# Processing of menu Main -> HMI -> Touchscreen-Configuration
# (used to be Administration -> HMI)
#
{
  ShowEvaluateDataWindow "Touchscreen Configuration"

  # loop until user wants to get back to superior-menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    # get values for the parameters and show them in a selection-menu
    local driverName=`./get_touchscreen_config driver-name`
    local deviceName=`./get_touchscreen_config device-name`
    local deviceNotAvailableInfo=`./get_touchscreen_config device-not-available-info`

    local calibrateTouchscreenFlag=`./get_touchscreen_config calibrate-touchscreen-flag`
    if [ "$calibrateTouchscreenFlag" = "checked" ]; then
      calibrateTouchscreenFlag=Yes
    else
      calibrateTouchscreenFlag=No
    fi

    local deviceNotAvailable="No"
    if [ -z "$deviceNotAvailableInfo" ]; then
      deviceNotAvailable="Yes"
    fi

    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Touchscreen Configuration" \
              "0. Back to HMI Settings Menu" \
              "1. Device Name..............$deviceName" \
              "2. Device available?........$deviceNotAvailable" \
              "3. Driver Name..............$driverName" \
              "4. Calibrate at next Start..$calibrateTouchscreenFlag"

    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      1|2) # Device Name was selected -> show selection-menu with the several possible device names

          # show an info text first, if actual registered device is currently not available
          if [ -n "$deviceNotAvailableInfo" ]; then
 
            ./wdialog "--msgbox" "$TITLE" "Touchscreen Configuration" " " \
                      "Please note, that the actual registered device" \
                      "\"$deviceName\"" \
                      "is currently not available." \
                      " "
          fi

          ShowEvaluateDataWindow "Touchscreen Configuration"

          # ask for the possible device-names and create a string for the selection-menu with them
          # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
          local menuString=""
          local deviceNr=1
          local deviceNameString=`./get_touchscreen_config device-name-by-nr $deviceNr`
          ifsBackup=$IFS
          IFS=$'\t'
          while [ -n "$deviceNameString" ]; do
            menuString=`printf "%s$deviceNr. $deviceNameString$IFS" "$menuString"`
            deviceNr=$(($deviceNr + 1))
            deviceNameString=`./get_touchscreen_config device-name-by-nr $deviceNr`
          done

          # show the menu
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Touchscreen Device ($deviceName)" \
                    "0. Back to Touchscreen Configuration Menu" \
                    $menuString
          IFS=$ifsBackup

          # if the user selected a driver
          if (( $selection > 0 )) && (( $selection < $deviceNr )); then

            ShowProcessingDataWindow "Touchscreen Configuration"

            # get the driver-name-string which belongs to the selected number
            newDeviceName=`./get_touchscreen_config device-name-by-nr $selection`
        
            # if a new device-name was selected - change it (and let the other parameters stay as they are)
            if [ "$deviceName" != "$newDeviceName" ]; then
              newDeviceName=`ReplaceSpaces $newDeviceName`
              driverName=`ReplaceSpaces $driverName`
              ./config_touchscreen device-name=$newDeviceName driver-name=$driverName calibrate-touchscreen=-
              ShowLastError
            fi
          fi
          ;;

      3)  # Driver Name was selected -> show selection-menu with the several possible driver names

          # if actual registered device is currently not available, don't allow any changes
          if [ -n "$deviceNotAvailableInfo" ]; then
 
            ./wdialog "--msgbox" "$TITLE" "Error" " " \
                      "The touchscreen configuration can't be changed, because" \
                      "the actual registered device is currently not available" \
                      " "
          else            

            ShowEvaluateDataWindow "Touchscreen Configuration"

            # ask for the possible driver-names and create a string for the selection-menu with them
            # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
            local menuString=""
            local driverNr=1
            local driverNameString=`./get_touchscreen_config driver-name-by-nr $driverNr`
            ifsBackup=$IFS
            IFS=$'\t'
            while [ -n "$driverNameString" ]; do
              menuString=`printf "%s$driverNr. $driverNameString$IFS" "$menuString"`
              driverNr=$(($driverNr + 1))
              driverNameString=`./get_touchscreen_config driver-name-by-nr $driverNr`
            done

            # show the menu
            WdialogWrapper "--menu" selection \
                      "$TITLE" \
                      "Touchscreen Driver Name ($driverName)" \
                      "0. Back to Touchscreen Configuration Menu" \
                      $menuString 
            IFS=$ifsBackup

            # if the user selected a driver
            if (( $selection > 0 )) && (( $selection < $driverNr )); then

              ShowProcessingDataWindow "Touchscreen Configuration"

              # get the driver-name-string which belongs to the selected number
              newDriverName=`./get_touchscreen_config driver-name-by-nr $selection`
        
              # if a new driver-name was selected - change it (and let the other parameters stay as they are)
              if [ "$driverName" != "$newDriverName" ]; then
                newDriverName=`ReplaceSpaces $newDriverName`
                deviceName=`ReplaceSpaces $deviceName`
                ./config_touchscreen device-name=$deviceName driver-name=$newDriverName calibrate-touchscreen=-
                ShowLastError
              fi
            fi
          fi
          ;;

      4)  # Execute calibration at next Start? was selected -> show selection-menu with yes/no
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Execute calibration of touchscreen at next start ($calibrateTouchscreenFlag)" \
                    "0. Back to Touchscreen Configuration Menu" \
                    "1. Yes" \
                    "2. No"
          
          ShowProcessingDataWindow "Configuration of the flag for calibrate touchscreen at next start"

          # call the processing routine; signalize that show-mouse-pointer-flag shall not be changed
          case "$selection" in
            1)  ./config_touchscreen calibrate-touchscreen=yes;;
            2)  ./config_touchscreen calibrate-touchscreen=no;;
            *)  ;;
          esac

          ShowLastError
          ;;

      *)  errorText="Error in wdialog"
          quit=$TRUE;;

     esac
  done
}
# end of MainAdministrationTouchscreen

MainAdministrationTouchscreen
