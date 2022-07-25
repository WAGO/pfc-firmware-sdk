# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function RunningViaSsh
#
# Check if cbm is running via an ssh connection.
#
{
  local STATUS="1"
  local SSH_PROC="(dropbear)"
  local PARENTPID=$$
  local PNAME=""

  while [ $PARENTPID -ne 1 ]; do
    set -- $(< /proc/$PARENTPID/stat)
    PNAME=$2
    PARENTPID=$4
    if [ $PNAME = $SSH_PROC ]; then
      STATUS="0"
      break
    fi
  done
  return $STATUS
}

function DenyDisconnectIfSsh
#
# Show a warning text and let the user accept or deny the loss of a possible
# SSH connection. Returns 0 (TRUE) if the user wants to abort the change.
#
{
  local abort="1"
  if RunningViaSsh; then
    WdialogWrapper --dialogbox abort \
      "$TITLE" \
      "SSH Configuration" " " \
      "Warning: Proceeding this setting will terminate your ssh connection." \
      "In case of disabling SSH this will prevent any system administration" \
      "task via SSH." \
      "Press Q to abort the change or press Enter to confirm loss of " \
      "connection." " "
  fi
  return $abort
}

function MainSSH
#
# Processing of menu SSH
# Show and change the SSH configuration
#
{
  ShowEvaluateDataWindow "SSH Configuration"

  # get the values for the several parameters
  local state=`./get_ssh_config state`
  local port=$(./get_ssh_config port-number)
  local rootLogin=$(./get_ssh_config root-access-state)
  local pwdLogin=$(./get_ssh_config password-request-state)

  # loop until the user wants to quit
  local quit=$FALSE
  local selection
  local retUnused

  while [ "$quit" = "$FALSE" ]; do

    # show selection-menu with the SSH-parameters
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "SSH Configuration" \
              "0. Back to Ports and Services Menu" \
              "1. State..................$state" \
              "2. Port...................$port" \
              "3. Allow root login.......$rootLogin" \
              "4. Allow password login...$pwdLogin" \
              "5. Status of firewalling"

    
    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      1)  # state was selected -> show selection-menu with the possible NTP-states
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change SSH-State ($state)" \
                    "0. Back to SSH Configuration Menu" \
                    "1. Enable" \
                    "2. Disable"
          
          # assign the selected number to the according clock-display-modes
          case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState=""
          esac

          # if a new state was selected - change it, get the actual value again and show possible errors
          # prevent config_ssh to abort due to a terminating ssh connection.
          if [ -n "$newState" ] && [ "$state" != "$newState" ]; then
            if ! DenyDisconnectIfSsh; then
              ShowProcessingDataWindow "SSH Configuration"
              nohup ./config_ssh state="$newState" > /tmp/port_ssh.out 2>&1 < /dev/null &
              wait
              state=`./get_ssh_config state`
              ShowLastError
            fi
          fi
          ;;

      2)  # port was selected -> show input-window to get the new port
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change SSH Port" "Enter new Port:" 5 $port 2> temp
          local newPort=`cat temp`
          rm temp

          # if a new port was given - change it, get the actual value again and show possible errors
          if [ -n "$newPort" ] && [ "$newPort" != "$port" ]; then
            if ! DenyDisconnectIfSsh; then
              ShowProcessingDataWindow "SSH Configuration"
              nohup ./config_ssh port-number=$newPort > /tmp/port_ssh.out 2>&1 < /dev/null &
              wait
              port=`./get_ssh_config port-number`
              ShowLastError
            fi
          fi
          ;;

      3)  # root login was selected
          WdialogWrapper "--menu" selection \
                        "$TITLE" \
                        "Change SSH root login state ($rootLogin)" \
                        "0. Back to SSH Configuration Menu" \
                        "1. Enable" \
                        "2. Disable"
          
          # assign the selected number to the according clock-display-modes
          case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState=""
          esac

          # if a new state was selected - change it, get the new value and show possible errors
          if [ -n "$newState" ] && [ "$rootLogin" != "$newState" ]; then
            if ! DenyDisconnectIfSsh; then
              ShowProcessingDataWindow "SSH Configuration"
              nohup ./config_ssh root-access-state="$newState" > /tmp/port_ssh.out 2>&1 < /dev/null &
              wait
              rootLogin=`./get_ssh_config root-access-state`
              ShowLastError
            fi
          fi
          ;;

      4)  # password-request-state was selected
          WdialogWrapper "--menu" selection \
                         "$TITLE" \
                         "Change SSH password login state ($pwdLogin)" \
                         "0. Back to SSH Configuration Menu" \
                         "1. Enable" \
                         "2. Disable"
          
          # assign the selected number to the according clock-display-modes
          case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState=""
          esac

          # if a new state was selected - change it, get the new value and show possible errors
          if [ -n "$newState" ] && [ "$pdwLogin" != "$newState" ]; then
            if ! DenyDisconnectIfSsh; then
              ShowProcessingDataWindow "SSH Configuration"
              nohup ./config_ssh password-request-state="$newState" > /tmp/port_ssh.out 2>&1 < /dev/null &
              wait
              pwdLogin=`./get_ssh_config password-request-state`
              ShowLastError
            fi
          fi
 
         ;;

      5)  # 
          running=`/etc/config-tools/get_ssh_config state`
          FirewallServiceInterfaces ssh SSH ${running}
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}

# end of MainSSH

MainSSH
