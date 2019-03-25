# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainScreensaver
#
# Processing of menu Main -> HMI settings -> Screensaver/Cleanmode
#
{

  local displayState=`./get_touchscreen_config display-state`
  local screensaverState=`./get_rts3scfg_value SCREENSAVER Enabled`
  local screensaverWaitTime=`./get_rts3scfg_value SCREENSAVER WaitTime` 
  local cleanmodeState=`./get_touchscreen_config cleanmode-state`
  local cleanmodeTimeout=`./get_rts3scfg_value CLEANMODE Timeout`

  # loop until user wants to get back to main menu
  local quit=$FALSE
  local settings
  while [ "$quit" = "$FALSE" ]; do

    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Screensaver/Cleanmode" \
              "0. Back to HMI Settings" \
              "1. Display State...............$displayState" \
              "2. Screensaver State...........$screensaverState" \
              "3. Screensaver Wait Time (sec).$screensaverWaitTime" \
              "4. Cleanmode State.............$cleanmodeState" \
              "5. Cleanmode Timeout (sec).....$cleanmodeTimeout" 

    case ${selection} in
       1)  # switch display on or off
      
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Display State ($displayState)" \
                    "0. Back to HMI Settings Menu" \
                    "1. On" \
                    "2. Off"
          
          ShowProcessingDataWindow "Changing display state"

          # call the processing routine
          case "$selection" in
            1)  newDisplayState="on";;
            2)  newDisplayState="off";;
            *)  newDisplayState=$displayState;;
          esac

          if [ "$newDisplayState" != "$displayState" ]; then
            ./change_screen_state display-state=$newDisplayState
            sleep 2
            displayState=`./get_touchscreen_config display-state`
            ShowLastError
          fi
          ;;
          
      2)  # activate or deactivate screensaver
      
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Screensaver State ($screensaverState)" \
                    "0. Back to HMI Settings Menu" \
                    "1. Enable" \
                    "2. Disable"
          
          ShowProcessingDataWindow "Changing screensaver state"

          # call the processing routine
          case "$selection" in
            1)  newState="enabled";;
            2)  newState="disabled";;
            *)  newState=$screensaverState;;
          esac

          if [ "$newState" != "$screensaverState" ]; then
            ./change_rts_config area=SCREENSAVER state=$newState
            sleep 1
            screensaverState=`./get_rts3scfg_value SCREENSAVER Enabled`
            ShowLastError
          fi
          ;;
          
      3)  # change screensaver wait time
      
          WdialogWrapper "--inputbox" retUnused \
                         "$TITLE" "Change screensaver wait time ($screensaverWaitTime seconds)" \
                         "Enter new wait time (sec):" 11 $screensaverWaitTime 2> temp
          local newWaitTime=`cat temp`
          rm temp

          if [ -n "$newWaitTime" ]; then
            ShowProcessingDataWindow "Changing screensaver wait time"
            ./change_rts_config area=SCREENSAVER WaitTime=$newWaitTime
            screensaverWaitTime=`./get_rts3scfg_value SCREENSAVER WaitTime`
            ShowLastError
          fi
          ;;
          
      4)  # configure cleanmode state
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Cleanmode State ($cleanmodeState)" \
                    "0. Back to HMI Settings Menu" \
                    "1. On" \
                    "2. Off"
          
          ShowProcessingDataWindow "Changing cleanmode state"

          # call the processing routine
          case "$selection" in
            1)  newState="on";;
            2)  newState="off";;
            *)  newState=$cleanmodeState;;
          esac

          if [ "$newState" != "$cleanmodeState" ]; then
            ./change_screen_state cleanmode-state=$newState
            sleep 1
            cleanmodeState=`./get_touchscreen_config cleanmode-state`
            ShowLastError
          fi
          ;;
          
      5)  # configure cleanmode timeout
          WdialogWrapper "--inputbox" retUnused "$TITLE" \
                         "Change cleanmode timeout ($cleanmodeTimeout seconds)" \
                         "Enter new timeout (sec):" 11 $cleanmodeTimeout 2> temp
          local newTimeout=`cat temp`
          rm temp

          if [ -n "$newTimeout" ]; then
            ShowProcessingDataWindow "Changing cleanmode timeout"
            ./change_rts_config area=CLEANMODE Timeout=$newTimeout
            cleanmodeTimeout=`./get_rts3scfg_value CLEANMODE Timeout`
            ShowLastError
          fi
          ;;
      *)  errorText="Error in wdialog"
          quit=$TRUE;;
    esac
  done 
}

MainScreensaver
