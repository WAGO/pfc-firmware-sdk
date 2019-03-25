# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function TcpIpConfigEth
#
# Processing of the menus Main -> TCP/IP -> TCP/IP Configuration for the active ports
#                         Main -> TCP/IP -> TCP/IP Configuration eth1
#
# Show and change the several parameters for the interfaces.
#
#
{
  local port=$1

  ShowEvaluateDataWindow "TCP/IP Configuration ${portLabel}${deviceId}"

  # get the values of the several eth-parameters
  local state=`./get_eth_config $port state`
  local configType=`./get_eth_config $port config-type`
  local ipAddress=`./get_eth_config $port ip-address`
  local subnetMask=`./get_eth_config $port subnet-mask`
  declare -a menuEntries

  if [ "$configType" = "static" ]; then
    showedConfigType="Static IP"
  else
    showedConfigType=$configType
  fi

  # loop until the user wants to get back to TCP/IP-menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do
    
    # show menu

    menuEntries=( "TCP/IP Configuration of ${port}" \
                  "0. Back to TCP/IP Menu" \
                  "1. Type of IP Address Configuration....$showedConfigType" \
                  "2. IP Address..........................$ipAddress" \
                  "3. Subnet Mask.........................$subnetMask")

    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "${menuEntries[@]}"

    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;


      1)  # type of ip-address-configuration was selected -> show menu to select new config-type and change it
          # if port is disabled, first show a message that config-type will not be permanently stored
          # (because in ports-file, a selected config-type announces that the port is enabled at the same time)
          if [ "$state" = "disabled" ]; then
            ./wdialog "--msgbox" "$TITLE" "TCP/IP Configuration ${portLabel}${deviceId} - Change type of IP Addr. Config" \
                      "Note:" \
                      "The type of IP address configuration will only be" \
                      "permanently stored as long as the port is enabled."
          fi


          # show selection-menu
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "TCP/IP Configuration ${portLabel}${deviceId} - Type of IP Addr. Config ($showedConfigType)" \
                    "0. Back to TCP/IP Configuration Menu" \
                    "1. Static IP" \
                    "2. DHCP" \
                    "3. BootP" 

          # assign the selected number to the according config-types
          case "$selection" in
            1) local newConfigType=static;;
            2) local newConfigType=dhcp;;
            3) local newConfigType=bootp;;
            *) local newConfigType="";
          esac

          # if a new value for config-type was selected
          if [ -n "$newConfigType" ] && [ "$newConfigType" != "$configType" ]; then

            # if port is enabled, change config-type directly (else just memorize it)
            if [ "$state" = "enabled" ]; then

                if [[ "$newConfigType" == "static" ]]; then
                    # static config type needs special treatment: the user has to be able to double-check ip and netmask settings
                    
                    WdialogWrapper "--inputbox" retUnused "$TITLE" "Change IP Address ${portLabel}${deviceId}" "Enter new IP Address:" 15 $ipAddress 2> temp
                    newIpAddress=`cat temp`
                    rm temp

                    newSubnetMask=
                    if [[ "$newIpAddress" != "" ]]; then
                        WdialogWrapper "--inputbox" retUnused  "$TITLE" "Change Subnet Mask ${portLabel}${deviceId}" "Enter new Subnet Mask:" 15 $subnetMask 2> temp
                        newSubnetMask=`cat temp`
                        rm temp
                    fi

                    if [[ "$newSubnetMask" != "" ]]; then
                        ShowProcessingDataWindow "TCP/IP Configuration ${portLabel}${deviceId}"
                        ./config_interfaces interface=$port config-type=$newConfigType state=enabled ip-address=$newIpAddress subnet-mask=$newSubnetMask > /dev/null 2> /dev/null
                        ShowLastError
            
                        ipAddress=`./get_eth_config $port ip-address`
                        subnetMask=`./get_eth_config $port subnet-mask`

                    fi
                else # configType dhcp/bootp
                    ShowProcessingDataWindow "TCP/IP Configuration ${portLabel}${deviceId}"
                    ./config_interfaces interface=$port config-type=$newConfigType state=enabled > /dev/null 2> /dev/null
                    ShowLastError
                fi

                configType=`./get_eth_config $port config-type`
                ShowLastError
            else # portState disabled
              configType=$newConfigType
            fi

            if [ "$configType" = "static" ]; then
              showedConfigType="Static IP"
            else
              showedConfigType=$configType
            fi

          fi
          ;;

      2)  # ip-address was selected -> show inputbox to get new address
          WdialogWrapper "--inputbox" retUnused "$TITLE" "Change IP Address ${portLabel}${deviceId}" "Enter new IP Address:" 15 $ipAddress 2> temp
          newIpAddress=`cat temp`
          rm temp

          # if new ip-address was given - change it, get actual value again and show possible error
          if [ -n "$newIpAddress" ] && [ "$newIpAddress" != "$ipAddress" ]; then
            ShowProcessingDataWindow "TCP/IP Configuration ${portLabel}${deviceId}"
            ./config_interfaces interface=$port config-type=$configType state=$state ip-address=$newIpAddress > /dev/null 2> /dev/null
            ipAddress=`./get_eth_config $port ip-address`
            ShowLastError
          fi
          ;;

      3)  # subnet-mask was selected -> show inputbox to get new subnet-mask
          WdialogWrapper "--inputbox" retUnused  "$TITLE" "Change Subnet Mask ${portLabel}${deviceId}" "Enter new Subnet Mask:" 15 $subnetMask 2> temp
          newSubnetMask=`cat temp`
          rm temp

          # if new subnet-mask was given - change it, get actual value again and show possible error
          if [ -n "$newSubnetMask" ] && [ "$newSubnetMask" != "$subnetMask" ]; then
            ShowProcessingDataWindow "TCP/IP Configuration ${portLabel}${deviceId}"
            ./config_interfaces interface=$port config-type=$configType state=$state subnet-mask=$newSubnetMask > /dev/null 2> /dev/null
            subnetMask=`./get_eth_config $port subnet-mask`
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}

function MainTcpIpConfigPorts
{


  local ports=$(xmlstarlet sel -t -v "//ip_settings[show_in_wbm='1']/port_name" ${NETWORK_INTERFACES_XML})

  declare -a ports_array
  declare -a menu_params_array

  local nr=1

  for port in $ports; do
    ports_array=("${ports_array[@]}" "$port")
    menu_params_array=("${menu_params_array[@]}" "$nr. $port")
    nr=$[$nr + 1]
  done

  local quit=$FALSE
  local selection
  declare -a menu_params_array
  
  while [ "$quit" = "$FALSE" ]; do
    
    # show menu
    # omit menu if only one port available
    if [ ${#ports_array[*]} -gt 1 ]; then
      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "TCP/IP Configuration" \
                "0. Back to TCP/IP Menu" \
                "${menu_params_array[@]}"
    else
      selection=1
      quit=$TRUE
    fi

      case "$selection" in
        
          0)  # Quit was selected -> end loop and get back to superior menu
              quit=$TRUE;;
          *)
            if [[ $selection -le ${#menu_params_array[@]} ]]; then
              TcpIpConfigEth "${ports_array[$(($selection - 1))]}"
            else
              errorText="Error in wdialog"
              quit=$TRUE 
            fi
            ;;
      esac
  done
}

MainTcpIpConfigPorts





