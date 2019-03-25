# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainModbusUDP
#
# Processing of menu Main -> Modbus -> Modbus UDP
# See and change the parameters of Modbus UDP
#
{
  # loop until user wants to get back to superior modbus menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    ShowEvaluateDataWindow "Configuration of MODBUS RTU"

    # get port-states and show selection-menu with the different ports
    local state=`./get_rts3scfg_value MODBUS_UDP state`

    # show menu with the several parameters and their values
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Configuration of the MODBUS UDP parameter" \
              "0. Back to MODBUS Configuration Menu" \
              "1. State.............$state"

    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      1)  # State was selected -> show menu to select the new state of modbus
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "State of MODBUS UDP ($state)" \
                    "0. Back to MODBUS UDP Menu" \
                    "1. Enabled" \
                    "2. Disabled"

          # assign the selected number to the according values
          case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState="";;
          esac

          # if a new value was selected, call the config-script, read parameter again and show possible error
          if [ -n "$newState" ] && [ "$newState" != "$state" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS UDP"
            ./change_rts_config area=MODBUS_UDP state=$newState
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;
    esac
  done
}
# end of MainModbusUDP



function MainModbusTCP
#
# Processing of menu Main -> Modbus -> Modbus TCP
# See and change the state of Modbus TCP
#
{
  ShowEvaluateDataWindow "Configuration of MODBUS TCP"

  # get Modbus TCP values 
  local state=`./get_rts3scfg_value MODBUS_TCP state`
  local timeout=`./get_rts3scfg_value MODBUS_TCP ModbusTCPTimeout`

  # loop until user wants to get back to superior modbus menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    # show menu with the several parameters and their values
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Configuration of the MODBUS TCP parameter" \
              "0. Back to MODBUS Configuration Menu" \
              "1. State.............$state" \
              "2. Timeout (msec)....$timeout"

    # analyse user selection and start processing to change the selected parameter if necessairy
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      1)  # State was selected -> show menu to select the new state of modbus
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "State of MODBUS TCP ($state)" \
                    "0. Back to MODBUS TCP Menu" \
                    "1. Enabled" \
                    "2. Disabled"

          # assign the selected number to the according values
          case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState="";;
          esac

          # if a new value was selected, call the config-script, read parameter again and show possible error
          if [ -n "$newState" ] && [ "$newState" != "$state" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS TCP"
            ./change_rts_config area=MODBUS_TCP state=$newState
            state=`./get_rts3scfg_value MODBUS_TCP state`
            ShowLastError
          fi
          ;;

      2)  # Timeout was selected -> show input-window to get the new timeout
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change MODBUS TCP Timeout" "Enter new timeout (msec):" 6 $timeout 2> temp
          local newTimeout=`cat temp`
          rm temp

          # if a new value was given, call the config-script, read parameter again and show possible error
          if [ -n "$newTimeout" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS TCP"
            ./change_rts_config area=MODBUS_TCP timeout=$newTimeout  
            timeout=`./get_rts3scfg_value MODBUS_TCP ModbusTCPTimeout`
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;
    esac
  done
}
# end of MainModbusTCP



function MainModbusRTU
#
# Processing of menu Main -> Modbus -> Modbus RTU
# See and change the parameters of Modbus RTU
#
{
  ShowEvaluateDataWindow "Configuration of MODBUS RTU"

  # get Modbus RTU values
  local state=`./get_rts3scfg_value MODBUS_RTU state`
  local nodeId=`./get_rts3scfg_value MODBUS_RTU Node_ID`
  local timeout=`./get_rts3scfg_value MODBUS_RTU Timeout`
  local interface=`./get_rts3scfg_value MODBUS_RTU Interface`
  local baudrate=`./get_rts3scfg_value MODBUS_RTU Baud`
  local databits=`./get_rts3scfg_value MODBUS_RTU Data_Bits`
  local parity=`./get_rts3scfg_value MODBUS_RTU Parity`
  local stopBits=`./get_rts3scfg_value MODBUS_RTU Stop_Bits`
  local flowControl=`./get_rts3scfg_value MODBUS_RTU Flow_control`

  # loop until user wants to get back to superior modbus menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    # show menu with the several parameters and their values
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Configuration of the MODBUS RTU parameter" \
              "0. Back to MODBUS Configuration Menu" \
              "1. State.............$state" \
              "2. Node ID...........$nodeId" \
              "3. Timeout (msec)....$timeout" \
              "4. Baudrate..........$baudrate" \
              "5. Databits..........$databits" \
              "6. Parity............$parity" \
              "7. Stop Bits.........$stopBits" \
              "8. Flow Control......$flowControl"

    # analyse user selection and start processing to change the selected parameter if necessairy
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      1)  # State was selected -> just show infotext about changing the state of modbus RTU
          ./wdialog "--msgbox" "$TITLE" "State of MODBUS RTU ($state)" " " \
                    "Because MODBUS RTU uses the serial interface," \
                    "changes of state can only be executed via menu" \
                    "Administration - Configuration of Serial Interface" " "
          ;;

      2)  # Node-ID was selected -> show input-window to get the new value 
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change MODBUS RTU Node ID" "Enter new Node ID:" 3 $nodeId 2> temp
          local newNodeId=`cat temp`
          rm temp

          # if a new value was given, call the config-script, read new parameter again and show possible error 
          if [ -n "$newNodeId" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS RTU"
            ./change_rts_config area=MODBUS_RTU node-id=$newNodeId
            nodeId=`./get_rts3scfg_value MODBUS_RTU Node_ID`
            ShowLastError
          fi
          ;;

      3)  # Timeout was selected -> show input-window to get the new value 
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change MODBUS RTU Timeout" "Enter new timeout (msec):" 6 $timeout 2> temp
          local newTimeout=`cat temp`
          rm temp

          # if a new value was given, call the config-script, read parameter again and show possible error
          if [ -n "$newTimeout" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS RTU"
            ./change_rts_config area=MODBUS_RTU timeout=$newTimeout  
            timeout=`./get_rts3scfg_value MODBUS_RTU Timeout`
            ShowLastError
          fi
          ;;

      4)  # Baudrate was selected -> show selection-menu to get the new value for baudrate
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Baudrate of MODBUS RTU ($baudrate)" \
                    "0. Back to MODBUS RTU Menu" \
                    "1. 2400" \
                    "2. 4800" \
                    "3. 9600" \
                    "4. 19200" \
                    "5. 38400" \
                    "6. 57600" \
                    "7. 115200"

          # assign the selected number to the according values
          case "$selection" in
            1) local newBaudrate="2400";;
            2) local newBaudrate="4800";;
            3) local newBaudrate="9600";;
            4) local newBaudrate="19200";;
            5) local newBaudrate="38400";;
            6) local newBaudrate="57600";;
            7) local newBaudrate="115200";;
            *) local newBaudrate="";;
          esac

          # if a new value was selected, call the config-script, read parameter again and show possible error
          if [ -n "$newBaudrate" ] && [ "$newBaudrate" != "$baudrate" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS RTU"
            ./change_rts_config area=MODBUS_RTU Baud=$newBaudrate
            baudrate=`./get_rts3scfg_value MODBUS_RTU Baud`
            ShowLastError
          fi
          ;;

      5)  # Databits was selected -> show selection-menu to get the new value for databits
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Databits of MODBUS RTU ($databits)" \
                    "0. Back to MODBUS RTU Menu" \
                    "1. 8"

          # assign the selected number to the according values
          case "$selection" in
            1) local newDatabits="8";;
            *) local newDatabits="";;
          esac

          # if a new value was selected, call the config-script, read parameter again and show possible error
          if [ -n "$newDatabits" ] && [ "$newDatabits" != "$databits" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS RTU"
            ./change_rts_config area=MODBUS_RTU Data_Bits=$newDatabits
            databits=`./get_rts3scfg_value MODBUS_RTU Data_Bits`
            ShowLastError
          fi
          ;;

      6)  # Parity was selected -> show selection-menu to get the new value for parity
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Parity of MODBUS RTU ($parity)" \
                    "0. Back to MODBUS RTU Menu" \
                    "1. None" \
                    "2. Odd" \
                    "3. Even"

          # assign the selected number to the according values
          case "$selection" in
            1) local newParity="None";;
            2) local newParity="Odd";;
            3) local newParity="Even";;
            *) local newParity="";;
          esac

          # if a new value was selected, call the config-script, read parameter again and show possible error
          if [ -n "$newParity" ] && [ "$newParity" != "$parity" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS RTU"
            ./change_rts_config area=MODBUS_RTU Parity=$newParity
            parity=`./get_rts3scfg_value MODBUS_RTU Parity`
            ShowLastError
          fi
          ;;

      7)  # Stop-Bits was selected -> show selection-menu to get the new value for stop-bits
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Stop Bits of MODBUS RTU ($stopBits)" \
                    "0. Back to MODBUS RTU Menu" \
                    "1. 1" \
                    "2. 2" 

          # assign the selected number to the according values
          case "$selection" in
            1) local newStopBits="1";;
            2) local newStopBits="2";;
            *) local newStopBits="";;
          esac

          # if a new value was selected, call the config-script, read parameter again and show possible error
          if [ -n "$newStopBits" ] && [ "$newStopBits" != "$stopBits" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS RTU"
            ./change_rts_config area=MODBUS_RTU Stop_Bits=$newStopBits
            stopBits=`./get_rts3scfg_value MODBUS_RTU Stop_Bits`
            ShowLastError
          fi
          ;;

      8)  # flow-control was selected -> show selection-menu to get the new value for flow-control
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Flow-Control of MODBUS RTU ($flowControl)" \
                    "0. Back to MODBUS RTU Menu" \
                    "1. None"

          # assign the selected number to the according values
          case "$selection" in
            1) local newFlowControl="None";;
            *) local newFlowControl="";;
          esac

          # if a new value was selected, call the config-script, read parameter again and show possible error
          if [ -n "$newFlowControl" ] && [ "$newFlowControl" != "$flowControl" ]; then
            ShowProcessingDataWindow "Configuration of MODBUS RTU"
            ./change_rts_config area=MODBUS_RTU Flow_control=$newFlowControl
            flowControl=`./get_rts3scfg_value MODBUS_RTU Flow_control`
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=${TRUE};;

    esac
  done
}
# end of MainModbusRTU




function MainModbus
#
# Processing of menu Main -> Modbus
# Show the menu to select the kind of modbus (UDP, TCP or RTU), the user wants to confiure
#
{
  # loop until user wants to get back to main menu
  local quit=$FALSE
  while [ "$quit" = "$FALSE" ]; do

    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "MODBUS Configuration" \
              "0. Back to Main Menu" \
              "1. MODBUS UDP" \
              "2. MODBUS TCP" \
              "3. MODBUS RTU"

    # analyse user selection and jump to the according function
    case "$selection" in
    
      0)  quit=$TRUE;;
      1)  MainModbusUDP;;
      2)  MainModbusTCP;;
      3)  MainModbusRTU;;
      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}

MainModbus
