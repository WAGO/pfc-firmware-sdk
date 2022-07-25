# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainPortGeneric
#
# Processing of menu Main -> Port
# See and change the state of CODESYS-webserver and FTP-port
#
{
  local portOutputText=$1
  local portParameterText=$2
  local state=$3

  # loop until user wants to get back to main menu
  local selection
  local quit=$FALSE

  while [[ "$quit" == "$FALSE" ]]; do
  
    ShowEvaluateDataWindow "Port Configuration"

    # if the user selected a port, show a selection menu with the possible port-states

    WdialogWrapper "--menu" selection \
      "$TITLE" \
      "Configuration of $portOutputText port ($state)" \
      "0. Back to Ports and Services Menu" \
      "1. Enable" \
      "2. Disable"

    # assign the port-state to the number returned by the selection-menu
    case "$selection" in
      1) local newState="enabled";;
      2) local newState="disabled";;
      *) local newState="";
         quit="${TRUE}"
         ShowEvaluateDataWindow "Ports and Services"

         ;;
    esac

    # if the user selected a new state -> configure the selected port with the selected state
    if [ -n "$newState" ] && [ "$newState" != "$state" ]; then
      ShowProcessingDataWindow "Port Configuration"
      local _newState

      if [ "$portOutputText" = "CODESYS Services" ]; then
        if [ "$newState" = "enabled" ]; then
          _newState="NO"
        else
          _newState="YES"
        fi
        ./change_rts_config area=PLC disable-tcpip=$_newState

      elif [ "$portOutputText" = "CODESYS3 Webserver" ]; then
        ./config_codesys3 codesys3-webserver-state=$newState
        
      elif [ "$portOutputText" = "OPC UA" ]; then
        ./config_opcua state=$newState
      else
        if [ "$portOutputText" = "Modbus TCP" ] || [ "$portOutputText" = "Modbus UDP" ]; then
          if [ "$newState" = "enabled" ]; then
            _newState="true"
          else
            _newState="false"
          fi
          ./modbus_config $portParameterText $_newState
        else
          ./config_port port=$portParameterText state=$newState
        fi
      fi

      if [[ "$?" == "0" ]]; then
        state=$newState
      fi
      ShowLastError
    fi

  done # main loop
}

ShowEvaluateDataWindow "Ports and Services"
ShowGenericMenu "Ports and Services" $(basename ${BASH_SOURCE[0]})
