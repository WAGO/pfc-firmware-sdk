# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function SubMenuIntegerInputBox
#
# Input integer window box
#
{
    local parameter=$1
    local parameterText=$2
    local currentValue=$3
    local unit=$4
    
    # show the menu
    WdialogWrapper "--inputbox" retUnused \
        "$TITLE" \
        "BACnet - Change $parameterText ($currentValue$unit)" \
        " Enter new $parameterText($unit):" 5 $currentValue 2> temp
      
    # get value 
    local newValue=`cat temp`
    rm temp

    # if a new value was given - change it, get the actual value again and show possible errors
    if [ -n "$newValue" ] && [ "$newValue" != "$currentValue" ]; then
        ShowProcessingDataWindow "$parameterText - Configuration"
        /etc/config-tools/bacnet_config -s $parameter -v $newValue
        ShowLastError
    fi
}
# end of SubMenuIntegerInputBox


function SubMenuStorageSelection
#
# Storage selection menu
#
{
    local parameter=$1
    local parameterText=$2
    local currentValue=$3

    # get the existing devices and create a string for the selection-menu with them
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
    local menuString=""
    local storageState=`/etc/config-tools/./bacnet_config -i`
    local medium=("internal-flash" "sd-card")
    local deviceNr=1
    local activeDeviceNr=0

    ifsBackup=$IFS
    IFS=$'\t'

    # loop throw array
    for i in {0..1}; do

        # sd-card is available ?
        if [ "${medium[1]}" = "${medium[$i]}" ]; then
            if [ "0" = "$storageState" ]; then
                continue
            fi
        fi

        # active storage ?
        if [ "$currentValue" == "${medium[$i]}" ]; then
            medium[$i]=`echo ${medium[$i]} "(active storage)"`
            activeDeviceNr=$deviceNr
        fi

        # add to menu string
        menuString=`printf "%s$deviceNr. ${medium[$i]}$IFS" "$menuString"`
        deviceNr=$(($deviceNr + 1))

    done

    # show the menu
    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "BACnet - Change $parameterText ($currentValue)" \
        "0. Back to Overview" \
        $menuString

    IFS=$ifsBackup

    # if the user selected a new device
    if (( $selection > 0 )) && (( $selection <= $deviceNr )) && (( $selection != $activeDeviceNr)); then

        # set new storage location
        ShowProcessingDataWindow "BACnet - $parameterText - Configuration"
        /etc/config-tools/bacnet_config -s $parameter -v ${medium[$(($selection - 1))]}
        ShowLastError
    fi
}
# end of SubMenuStorageSelection



function MainBacnet
#
# Processing of menu Main -> BACnet
# Configure BACnet parameter
#
{
    # loop until the user wants to get back to main menu
    local quit=$FALSE
    local selection
    local retUnused
    while [ "$quit" = "$FALSE" ]; do

        # read the values new in every loop, because most of them are addicted to the others
        ShowEvaluateDataWindow "BACnet - Load"
    
        # get values from bacnet_config (c-code)
        local bacnetFieldbus=`/etc/config-tools/./bacnet_config -g fieldbus-state`
        local bacnetFieldbusStatus=""        
        local udpPort=`/etc/config-tools/./bacnet_config -g udp-port`
        local whoIsInterval=`/etc/config-tools/./bacnet_config -g who-is-online-interval`
        local persStorage=`/etc/config-tools/./bacnet_config -g persistence-storage`
        local trendStorage=`/etc/config-tools/./bacnet_config -g trendlog-storage`
        
        # convert true or false 
        if [[ -n "$bacnetFieldbus" && "$bacnetFieldbus" == "true" ]]; then
            bacnetFieldbusStatus="enabled"
        elif [[ -n "$bacnetFieldbus" && "$bacnetFieldbus" == "false" ]]; then
            bacnetFieldbusStatus="disabled"
        fi
        
        # show menu
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "BACnet - Overview" \
            "0. Back to Main Menu" \
            "1. Fieldbus state........................$bacnetFieldbusStatus" \
            "2. UDP-Port..............................$udpPort" \
            "3. Who-Is online interval time (sec).....$whoIsInterval" \
            "4. Persistence storage location..........$persStorage" \
            "5. Trendlog storage location.............$trendStorage" \

        # analyse user selection and do to the according processing
        case "$selection" in
        
          0)  # Quit was selected -> end loop and get back to superior menu
              quit=$TRUE;;
              
          1)  # show the menu
              WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "BACnet - Change BACnet fieldbus status ($bacnetFieldbusStatus)" \
                  "0. Back to BACnet - Overview Menu" \
                  "1. Enable" \
                  "2. Disable"
    
              # assign parameter value according to select menu number
              case "$selection" in
                  1) local newState="true";;
                  2) local newState="false";;
                  *) local newState=""
              esac
      
              # if state changed, set it, get actual value, and show possible errors
              if [[ -n "$newState" && "$bacnetFieldbus" != "$newState" ]]; then
                  ShowProcessingDataWindow "BACnet fieldbus Configuration"
                  /etc/config-tools/bacnet_config -s fieldbus-state -v $newState
                  ShowLastError
              fi
              ;;
            
          2)  SubMenuIntegerInputBox "udp-port" "UDP-Port" $udpPort 
              ;;     
              
          3)  SubMenuIntegerInputBox "who-is-online-interval" "Who-Is online interval time" $whoIsInterval "sec"
              ;;       
              
          4)  SubMenuStorageSelection "persistence-storage" "Persistence storage location" "$persStorage"
              ;;
              
          5)  SubMenuStorageSelection "trendlog-storage" "Trendlog storage location" "$trendStorage"
              ;;
    
          *)  errorText="Error in wdialog"
              quit=TRUE;;
    
        esac
    done
}
# end of MainBacnet

# start (main) of shell script
MainBacnet
