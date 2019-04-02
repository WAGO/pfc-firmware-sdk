# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainNtp
#
# Processing of menu Main -> Ports and Services -> NTP
# Show and change the several NTP-values
#
{
  ShowEvaluateDataWindow "NTP Configuration"

  # get the values for all parameters
  eval $(./get_ntp_config)

  # loop until the user wants to quit
  local quit=$FALSE
  local selection
  local retUnused
  local dialogstatus

  while [ "$quit" = "$FALSE" ]; do

    # evaluate state of server new after any changes (can switch to disabled due to errors)
    local state=`./get_ntp_config state`
    local activeState=`./get_ntp_config active-state`

    # create entries for dynamic NTP servers (gotten via DHCP)
    local entryNr=7 # after "6. Time Server 4"
    local ntpServerStringDHCP=
    
    for ntpServer in ${timeServersDHCP}; do
        ntpServerStringDHCP=`printf "%s$(($entryNr)). Time Server $[$entryNr-2] [from dhcp: read-only]...$ntpServer$IFS" "$ntpServerStringDHCP"`
        entryNr=$[$entryNr + 1]
    done


    local entryNrUpdTime=${entryNr}
    local entryNrImmediateUpd=$[$entryNr+1]

    ifsBackup=$IFS
    IFS=$'\t'
    # show selection-menu with the several NTP-parameters
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "NTP Configuration" \
              "0. Back to Ports and Services Menu" \
              "1. State..................................$state $activeState" \
              "2. Port...................................$port" \
              "3. Time Server 1..........................$timeServer1" \
              "4. Time Server 2..........................$timeServer2" \
              "5. Time Server 3..........................$timeServer3" \
              "6. Time Server 4..........................$timeServer4" \
              ${ntpServerStringDHCP} \
              "$entryNrUpdTime. Update Time (sec).....................$updateTime" \
              "$entryNrImmediateUpd. Issue immediate update"
    
    IFS=$ifsBackup
    
    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          ShowEvaluateDataWindow "Ports and Services"
          quit=$TRUE;;

      1)  # state was selected -> show selection-menu with the possible NTP-states
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change NTP-State ($state$activeState)" \
                    "0. Back to NTP Configuration Menu" \
                    "1. Enable" \
                    "2. Disable"
          
          # assign the selected number to the according clock-display-modes
          case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState=""
          esac

          # if a new state was selected - change it, get tconfig_sntp port=he actual value again and show possible errors
          if [ -n "$newState" ] && [ "$state" != "$newState" ]; then
            ShowProcessingDataWindow "NTP Configuration"
            ./config_sntp state=$newState 
            state=`./get_ntp_config state`
            ShowLastError
          fi
          ;;

      2)  # port was selected -> show input-window to get the new port
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change NTP Port" "Enter new Port:" 5 $port 2> temp
          local newPort=`cat temp`
          rm temp

          # if a new port was given - change it, get the actual value again and show possible errors
          if [ -n "$newPort" ] && [ "$newPort" != "$port" ]; then
            ShowProcessingDataWindow "NTP Configuration"
            ./config_sntp port=$newPort  
            port=`./get_ntp_config port`
            ShowLastError
          fi
          ;;

      3)  # time-server-1 was selected -> show input-window to get new time-server
          WdialogWrapper "--inputbox" dialogstatus "$TITLE" "Change NTP Time Server 1" "Enter new Time Server:" 15 $timeServer1 2> temp
          local newTimeServer=`cat temp`
          rm temp

          # if a new value for time-server is given - change it, get the actual value again and show possible errors
          if [ $dialogstatus -eq 0 ] && [ "$newTimeServer" != "$timeServer1" ]; then
            ShowProcessingDataWindow "NTP Configuration"
            ./config_sntp time-server-1=$newTimeServer
            timeServer1=`./get_ntp_config time-server-1`
            ShowLastError
          fi
          ;;

      4)  # time-server-2 was selected -> show input-window to get new time-server
          WdialogWrapper "--inputbox" dialogstatus "$TITLE" "Change NTP Time Server 2" "Enter new Time Server:" 15 $timeServer2 2> temp
          local newTimeServer=`cat temp`
          rm temp

          # if a new value for time-server is given - change it, get the actual value again and show possible errors
          if [ $dialogstatus -eq 0 ] && [ "$newTimeServer" != "$timeServer2" ]; then
            ShowProcessingDataWindow "NTP Configuration"
            ./config_sntp time-server-2=$newTimeServer
            timeServer2=`./get_ntp_config time-server-2`
            ShowLastError
          fi
          ;;

      5)  # time-server-3 was selected -> show input-window to get new time-server
          WdialogWrapper "--inputbox" dialogstatus "$TITLE" "Change NTP Time Server 3" "Enter new Time Server:" 15 $timeServer3 2> temp
          local newTimeServer=`cat temp`
          rm temp

          # if a new value for time-server is given - change it, get the actual value again and show possible errors
          if [ $dialogstatus -eq 0 ] && [ "$newTimeServer" != "$timeServer3" ]; then
            ShowProcessingDataWindow "NTP Configuration"
            ./config_sntp time-server-3=$newTimeServer
            timeServer3=`./get_ntp_config time-server-3`
            ShowLastError
          fi
          ;;

      6)  # time-server-4 was selected -> show input-window to get new time-server
          WdialogWrapper "--inputbox" dialogstatus "$TITLE" "Change NTP Time Server 4" "Enter new Time Server:" 15 $timeServer4 2> temp
          local newTimeServer=`cat temp`
          rm temp

          # if a new value for time-server is given - change it, get the actual value again and show possible errors
          if [ $dialogstatus -eq 0 ] && [ "$newTimeServer" != "$timeServer4" ]; then
            ShowProcessingDataWindow "NTP Configuration"
            ./config_sntp time-server-4=$newTimeServer
            timeServer4=`./get_ntp_config time-server-4`
            ShowLastError
          fi
          ;;

      ${entryNrUpdTime})  # update-time was selected -> show input-window to get new update-time
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change NTP Update Time" "Enter new Update Time (sec):" 7 $updateTime 2> temp
          local newUpdateTime=`cat temp`
          rm temp

          # if a new value for update-time was given - change it, get actual value again and show possible errors
          if [ -n "$newUpdateTime" ] && [ "$newUpdateTime" != "$updateTime" ]; then
            ShowProcessingDataWindow "NTP Configuration"
            ./config_sntp update-time=$newUpdateTime 
            updateTime=`./get_ntp_config update-time`
            ShowLastError
          fi
          ;;

      ${entryNrImmediateUpd})  # Issue immediate update command.
          ShowProcessingDataWindow "NTP Configuration"
          ./config_sntp update
          ShowLastError
          ;;

      *)  if [[ "$selection" -gt "${entryNrImmediateUpd}" ]]; then
              errorText="Error in wdialog"
              quit=TRUE
          fi
          ;;
    esac
  done
}

# end of MainNtp

MainNtp


