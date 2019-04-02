# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

PARENT=$(GetParentMenuName ${BASH_SOURCE})
ITEM_NAME=$(GetSelfMenuName ${BASH_SOURCE})

function MainEthernetInterface
#
# Processing of the menus Main -> Ethernet -> Ethernet Configuration port 0
#                         Main -> Ethernet -> Ethernet Configuration port 1
#
# Show and change the several parameters of ethernet-configuration.
#
# input: label of port 0/1 (i.e. X1/X2)
#
{
  local portLabel=$1

  ShowEvaluateDataWindow "Port Configuration $portLabel"

  # loop until the user want's to get back to TCP/IP-menu
  local quit=$FALSE
  local selection

  while [ "$quit" = "$FALSE" ]; do
    
    # get the values of the several ethernet-parameters
    local state=$(./get_eth_config $portLabel state) 
    local autoneg=
    local speed=
    local duplex=

    # show menu

    if [[ "$state" == "$DISABLED" ]]; then
      
      WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Port Configuration $portLabel" \
        "0. Back to ${PARENT} Menu" \
        "1. Port..................$state"

    else # state = enabled
      autoneg=`./get_eth_config $portLabel autoneg`

      if [ "$autoneg" = "$ENABLED" ]; then
        WdialogWrapper "--menu" selection \
          "$TITLE" \
          "Port Configuration $portLabel" \
          "0. Back to ${ITEM_NAME} Menu" \
          "1. Port....................$state" \
          "2. Autonegotiation.........$autoneg"

      else #autoneg=off
    
        speed=`./get_eth_config $portLabel speed`
        duplex=`./get_eth_config $portLabel duplex`
    
        WdialogWrapper "--menu" selection \
          "$TITLE" \
          "Port Configuration $portLabel" \
          "0. Back to ${ITEM_NAME} Menu" \
          "1. Port....................$state" \
          "2. Autonegotiation.........$autoneg" \
          "3. Speed/Duplex............$speed/$duplex"

      fi # autoneg on/off

    fi # port on/off

    # 0 / Quit was selected -> end loop and get back to superior menu
    if [ "$selection" = 0 ]; then
      quit=$TRUE
    else

      # 1 / state was selected
      if [[ "$selection" == "1" ]];then
        
        local stateSelection=0
        WdialogWrapper "--menu" autonegSelection \
                  "$TITLE" \
                  "Port Configuration $portLabel - Port State ($state)" \
                  "0. Back to Port Configuration $portLabel Menu" \
                  "1. Enable" \
                  "2. Disable"

        # assign the selected number to the according state
        case "$autonegSelection" in
          1)  local newState="enabled";;
          2)  local newState="disabled";;
          *)  local newState="";;
        esac

        if [ -n "$newState" ] && [ "$newState" != "$state" ]; then
            ./config_ethernet interface="eth0" port=$portLabel state=$newState
             ShowLastError
        fi

      fi

      # 2 / autoneg was selected -> show menu to select the new state
      if [ "$selection" = 2 ]; then

        local autonegSelection=0
        WdialogWrapper "--menu" autonegSelection \
                  "$TITLE" \
                  "Port Configuration $portLabel - Change Autonegotiation ($autoneg)" \
                  "0. Back to Port Configuration $portLabel Menu" \
                  "1. Enable" \
                  "2. Disable"

        # assign the selected number to the according state
        case "$autonegSelection" in
          1)  local newAutoneg="on";;
          2)  local newAutoneg="off";;
          *)  local newAutoneg="";;
        esac

        # a new value for autoneg was selected
        if [ -n "$newAutoneg" ] && [ "$newAutoneg" != "$autoneg" ]; then

          #  autoneg should be switched on - call the processing script, get actual value again and show possible errors
          if [ "$newAutoneg" = "on" ]; then

            ./config_ethernet interface="eth0" port=$portLabel autoneg=$newAutoneg
            ShowLastError
 
          # autoneg should be switched off - we must get specified new speed- and duplex-values from user first
          else
            selection=3
            speed="-"
            duplex="-"
          fi
        fi
      fi

      # 3 / speed and duplex was selected or respectivly autoneg was switched off -> show menu to select new speed / duplex
      if [ "$selection" = 3 ]; then

        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "Port Configuration $portLabel - Change speed/duplex ($speed/$duplex)" \
                  "0. Back to Port Configuration $portLabel Menu" \
                  "1. 10/half" \
                  "2. 10/full" \
                  "3. 100/half" \
                  "4. 100/full" 

        # assign the selected number to the according state
        case "$selection" in
          1)  local newSpeed="10"
              local newDuplex="half"
              ;;
          2)  local newSpeed="10"
              local newDuplex="full"
              ;;
          3)  local newSpeed="100"
              local newDuplex="half"
              ;;
          4)  local newSpeed="100"
              local newDuplex="full"
              ;;
          *)  local newSpeed="";;
        esac

        # if a new values were selected - change them, get actual values again and show possible errors
        if [ -n "$newSpeed" ] && [ -n "$newDuplex" ]; then
          if [ "$newSpeed" != "$speed" ] || [ "$newDuplex" != "$duplex" ]; then
           
            ./config_ethernet interface="eth0" port=$portLabel autoneg=off speed="${newSpeed}M" duplex="$newDuplex"
            ShowLastError
          fi
        fi
      fi
    fi

  done
}
# end of MainEthernetInterface



function MainEthernet
#
# Processing of menu Main -> Ethernet
# Show the submenu with the ethernet configurations
#
{
  # loop until the user wants to get back to main menu
  local quit=$FALSE
  local selection
 
   local ports=$(xmlstarlet sel -t -v "//ethernet_settings/port_name" ${NETWORK_INTERFACES_XML})

  declare -a ports_array
  declare -a menu_params_array

  local nr=1

  for port in $ports; do
    ports_array=("${ports_array[@]}" "$port")
    menu_params_array=("${menu_params_array[@]}" "$nr. Interface $port")
    nr=$[$nr + 1]
  done

  while [ "$quit" = "$FALSE" ]; do
    
    # show menu, analyse user-selection and call the according function

    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "${ITEM_NAME}" \
              "0. Back to ${PARENT} Menu" \
              "${menu_params_array[@]}"
    
    case "$selection" in
    
      0)  quit=$TRUE;;
      *)  if [[ $selection -le ${#menu_params_array[@]} ]]; then
            MainEthernetInterface "${ports_array[$(($selection - 1))]}"
          else
            errorText="Error in wdialog"
            quit=TRUE
        fi
        ;;
    esac
  done
}

MainEthernet
