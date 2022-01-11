# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2020 WAGO Kontakttechnik GmbH & Co. KG

DATA_EXCHANGE_FILE=/tmp/cbm_data_exchange

function GetSourceLabel {
  local source="$1"

  if [[ "$source" = "static" ]]; then
    echo "Static IP"
  else
    echo $source
  fi
}

function GetUserInput {
  local value=$(< $DATA_EXCHANGE_FILE)
  rm -f $DATA_EXCHANGE_FILE
  echo $value
}

function SourceMenu {
  local bridge=$1
  local source=$2
  local ipaddr=$3
  local netmask=$4
  local sourceLabel=$(GetSourceLabel $source)

  WdialogWrapper "--menu" selection \
            "$TITLE" \
            "TCP/IP Configuration ${portLabel}${deviceId} - Type of IP Addr. Config ($sourceLabel)" \
            "0. Back to TCP/IP Configuration Menu" \
            "1. Static IP" \
            "2. DHCP" \
            "3. BootP" \
            "4. None"

  # assign the selected number to the according config-types
  local newSource
  case "$selection" in
    1) newSource=static;;
    2) newSource=dhcp;;
    3) newSource=bootp;;
    4) newSource=none;;
    *) newSource="";
  esac

  if [ -e "$newSource" ] || [ "$newSource" == "$source" ]; then
    return 0
  fi

  if [[ "$newSource" == "static" ]]; then
    IpAddressMenu $bridge $newSource $ipaddr $netmask
  else
    ShowProcessingDataWindow "TCP/IP Configuration ${portLabel}${deviceId}"
    SetIpConfig $bridge $newSource &> /dev/null
    ShowLastError
  fi
}

function IpAddressMenu {
  local bridge=$1
  local source=$2
  local ipaddr=$3
  local netmask=$4
  
  WdialogWrapper "--inputbox" retUnused "$TITLE" "Change IP Address ${portLabel}${deviceId}" "Enter new IP Address:" 15 $ipaddr 2> $DATA_EXCHANGE_FILE
  local newIpAddress=$(GetUserInput)
  local newNetmask=""
  if [ -n "$newIpAddress" ]; then
      WdialogWrapper "--inputbox" retUnused "$TITLE" "Change Subnet Mask ${portLabel}${deviceId}" "Enter new Subnet Mask:" 15 $netmask 2> $DATA_EXCHANGE_FILE
      newNetmask=$(GetUserInput)
  fi

  if [ -n "$newNetmask" ]; then
      ShowProcessingDataWindow "TCP/IP Configuration ${portLabel}${deviceId}"
      SetIpConfig $bridge $source $newIpAddress $newNetmask &> /dev/null
      ShowLastError
  fi
}

function NetmaskMenu {
  IpAddressMenu $1 $2 $3 $4
}

function IpConfigurationMenu {
  ShowEvaluateDataWindow "TCP/IP Configuration ${portLabel}${deviceId}"
  declare -a menuEntries

  local quit=$FALSE
  local selection=0
  local bridge="$1"
  while [ "$quit" = "$FALSE" ]; do
    local p
    local parameters=$(GetCurrentIpConfig $bridge)
    for p in $parameters; do
      case "$p" in
        ipaddr=*)
            ipaddr="${p#ipaddr=}"
            ValidateParameter ipaddr $ipaddr
            ;;
        netmask=*)
            netmask="${p#netmask=}"
            ValidateParameter netmask $netmask
            ;;
        source=*)
            source="${p#source=}"
            ValidateParameter source $source
            ;;
      esac
    done
    sourceLabel=$(GetSourceLabel $source)

    menuEntries=( "TCP/IP Configuration of ${bridge}" \
                  "0. Back to TCP/IP Menu" \
                  "1. Type of IP Address Configuration....$sourceLabel" \
                  "2. IP Address..........................$ipaddr" \
                  "3. Subnet Mask.........................$netmask")

    WdialogWrapper "--menu" selection "$TITLE" "${menuEntries[@]}"

    case "$selection" in
      0) quit=$TRUE;;
      1) SourceMenu $bridge $source $ipaddr $netmask;;
      2) IpAddressMenu $bridge $source $ipaddr $netmask;;
      3) NetmaskMenu $bridge $source $ipaddr $netmask;;
      *) errorText="Error in wdialog"
         quit=TRUE;;
    esac
  done
}

function IpConfigurationMainMenu {
  declare -a bridges
  declare -a menu_items
  local nr=1

  for bridge in $(GetBridges); do
    bridges=("${bridges[@]}" "$bridge")
    menu_items=("${menu_items[@]}" "${nr}. $bridge")
    nr=$((nr+1))
  done

  local quit=$FALSE
  local selection=0
  declare -a menu_items

  while [ "$quit" = "$FALSE" ]; do

    if [ ${#bridges[*]} -gt 1 ]; then
      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "TCP/IP Configuration" \
                "0. Back to TCP/IP Menu" \
                "${menu_items[@]}"
    else
      selection=1
      quit=$TRUE
    fi

    case "$selection" in
      0) quit=$TRUE;;
      *)
        if [[ $selection -le ${#menu_items[@]} ]]; then
          IpConfigurationMenu "${bridges[$(($selection - 1))]}"
        else
          errorText="Error in wdialog"
          quit=$TRUE
        fi
        ;;
    esac
  done
}

IpConfigurationMainMenu