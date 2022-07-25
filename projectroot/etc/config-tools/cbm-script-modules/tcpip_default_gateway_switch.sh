# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainTcpIpDefaultGatewaySwitch
#
# Processing of Menu Main -> TCP/IP -> Default Gateway
# Show and change the configuration of default-gateway.
#
{
  # get actual location and value of default-gateway
  ShowEvaluateDataWindow "TCP/IP Configuration Default Gateway"
  local interface=`./get_coupler_details default-gateway`
  local value=`./get_eth_config eth0 default-gateway`

  # change interface of location to its device-id
  local interfaceDeviceId="none"
  if [ "$interface" == "none" ]; then
    interfaceStatus="disabled"
  else
    interfaceStatus="enabled"
  fi

  # loop until user wants to go back to TCP/IP-Menu
  local quit=$FALSE
  local selection
  local retUnused

  while [ "$quit" = "$FALSE" ]; do
   
    local ifsBackup=$IFS
    IFS=$'\t'

    local gwMenuLine="1. Status.................$interfaceStatus" 
    if [[ "$interfaceStatus" == "enabled" ]]; then
      gwMenuLine=$(printf "%s\t%s" "$gwMenuLine" "2. Gateway................$value" )
    fi

    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "TCP/IP Configuration Default Gateway" \
              "0. Back to TCP/IP Menu" \
              $gwMenuLine
    IFS=$ifsBackup    

    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      1)  # interface was selected -> show menu to select the new one
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "TCP/IP Configuration Default Gateway - ($interfaceStatus)" \
                    "0. Back to Default Gateway Configuration Menu" \
                    "1. Enable" \
                    "2. Disable" 

          # assign the selected number to the according state
          case "$selection" in

            # user choice: Back

            0)  local newInterface=""
                ;; 

            # user choice: Enable

            1)  local newInterface="eth0"
                if [[ "$interfaceStatus" == "disabled" ]]; then
                  # force user to enter a new GW value (or submit preset)
                  WdialogWrapper "--inputbox" retUnused "$TITLE" "TCP/IP Configuration Default Gateway" "Enter new IP address for default gateway:" "15" "$value" 2> temp
                  newValue=`cat temp`
                  rm temp
                else
                  newValue=$value
                fi
                ;;

            # user choice: Disable

            2)  local newInterface="none"
                newValue=$value # value hasn't changed; is needed as argument for config_default_gateway below
                ;;

            *)  local newInterface="";; # error
          esac
 
         
          # if a new interface was selected - change it, get actual value again and show possible errors
          if [ -n "$newInterface" ] && [ -n "$newValue" ] && \
             ( [ "$newInterface" != "$interface" ] || [ "$newValue" != "$value" ] ) ; then

            ShowProcessingDataWindow "TCP/IP Configuration Default Gateway"
            ./config_default_gateway interface=$newInterface default-gateway-value=$newValue > /dev/null 2> /dev/null

            # get new value of location-interface and change it to device-id
            interfaceStatus="disabled"
            
            interface=`./get_coupler_details default-gateway`
            if [ "$interface" != "none" ]; then
              interfaceStatus="enabled"
              value=$newValue
            fi

            ShowLastError
          fi
          ;;

      2)  # value should be changed -> get new value
          if [[ "$interfaceStatus" == "disabled" ]]; then
            # There is no GW choice if default GW is disabled!
            continue
          fi

          WdialogWrapper "--inputbox" retUnused "$TITLE" "TCP/IP Configuration Default Gateway" "Enter new value for default gateway:" "15" "$value" 2> temp
          newValue=`cat temp`
          rm temp

          # if an input was made, change default-gateway-value, show possible errors and read value again
          if [ -n "$newValue" ]; then
            ShowProcessingDataWindow "TCP/IP Configuration Default Gateway"
            ./config_default_gateway default-gateway-value=$newValue > /dev/null 2> /dev/null
            value=`./get_eth_config eth0 default-gateway`
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}

MainTcpIpDefaultGatewaySwitch
