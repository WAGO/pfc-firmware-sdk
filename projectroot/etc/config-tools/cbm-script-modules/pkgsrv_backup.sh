# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

# First partition of WAGO FW SD cards has a label 7761676F which is 'WAGO' in ASCII code
function IsWagoFwSD
{
    local devName=$(/etc/config-tools/get_device_data name $1)

    local dev=/dev/${devName}p1

    if [ -b "${dev}" ]; then
        blkid ${dev} | grep -q 'LABEL="7761676F"'
    else
        false
    fi
}

#
# Creating a list of targets might be an overkill for PFC200 with the only possible
# "SD card" target option but may be required for coming hw (usb sticks etc.)
#
function GetValidTargetList
{
  local deviceNr=1
  local deviceString=`./get_filesystem_data medium-by-index $deviceNr`
  local deviceStringText=`GetPartitionMediumOutputText $deviceString`

  activePartitionMedium=`/etc/config-tools/get_filesystem_data active-partition-medium`
  
  local targetList

  while [[ -n "$deviceString" ]]; do

    # - cannot create backup on active partition
    # - NAND is not a valid target
    # - SD card with WAGO FW is not a valid target
    
    if [[ "$deviceString" != "$activePartitionMedium" ]] && \
       [[ "$deviceString" != "$INTERNAL_FLASH_NAND" ]] &&
       ! IsWagoFwSD "$deviceString"; then

      targetList=$(printf "%s %s" $targetList "$deviceStringText")
    fi


    deviceNr=$(($deviceNr+1))
    deviceString=`./get_filesystem_data medium-by-index $deviceNr`
    deviceStringText=`GetPartitionMediumOutputText $deviceString`
  done

  echo $targetList
}

function MainFirmwareBackupSave
{
  activePartitionMedium=`/etc/config-tools/get_filesystem_data active-partition-medium`
  activePartitionMediumText=`GetPartitionMediumOutputText $activePartitionMedium`

  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection 

  while [ "$quit" = "$FALSE" ]; do
    # get the existing devices and create a string for the selection-menu with them
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
    local menuString=""
    local menuEntryNr=1

    ifsBackup=$IFS
    IFS=$'\t'

    targetList=$(GetValidTargetList)

    for target in $targetList; do
      menuString=`printf "%s$menuEntryNr. $target$IFS" "$menuString"`
      menuEntryNr=$(($menuEntryNr+1))
    done

    if [[ "$menuEntryNr" == "1" ]]; then # targetList empty
      ./wdialog "--msgbox" "$TITLE" "Package server - save" " " \
          "No valid backup targets found. Make sure a non-system external" \
          "medium (i.e. a memory card) is inserted."  " "
      quit=$TRUE
    else

      # What do we want to backup?
      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "Select packages to save (active partition: $activePartitionMediumText)" \
                "0. Back to Package Server Menu" \
                "1. All" \
                "2. PLC Runtime project" \
                "3. Settings" \
                "4. System" 

      # analyse user selection
      packageCodesys=0
      packageSettings=0
      packageSystem=0

      case "$selection" in
        0)  quit=$TRUE;;
        1)  packageCodesys=1
            packageSettings=1
            packageSystem=1
            ;;
        2)  packageCodesys=1;;
        3)  packageSettings=1;;
        4)  packageSystem=1;;
        *)  errorText="Error in wdialog"
            quit=${TRUE};;
      esac
      
      # package(s) were selected
      if [ "$packageCodesys" = "1" ] || [ "$packageSettings" = "1" ] || [ "$packageSystem" = "1" ]; then
      
        # now get auto update feature flag
        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "Select auto update feature (active partition: $activePartitionMediumText)" \
                  "0. Back to Package Server Menu" \
                  "1. No" \
                  "2. Yes" 

        # analyse user selection
        autoUpdate=""
        case "$selection" in
          0)  quit=${TRUE};;
          1)  autoUpdate=0;;
          2)  autoUpdate=1;;
          *)  errorText="Error in wdialog"
              quit=${TRUE};;
        esac

        # show the menu
        WdialogWrapper "--menu" selection \
          "$TITLE" \
          "Select destination to save packages" \
          "0. Back to Package Server Menu" \
          $menuString 
        IFS=$ifsBackup

        # if the user selected a device
        # last menuEntryNr is (last valid entry + 1)
        if [[ $selection -gt 0 ]] && [[ $selection -lt $menuEntryNr ]]; then

          destinationMedium=`./get_filesystem_data medium-by-index $selection`

          #echo package-codesys="$packageCodesys" package-settings="$packageSettings" package-system="$packageSystem"   auto-update="$autoUpdate" device-medium="$destinationMedium"
          #read
          activePartitionMediumText=`GetPartitionMediumOutputText $activePartitionMedium`
          destinationMediumText=`GetPartitionMediumOutputText $destinationMedium`
          ./wdialog "--infobox" "$TITLE" "Package server - save" " " \
            "Save package(s) from $activePartitionMediumText to $destinationMediumText" \
           "- can take a few minutes..."  " "

          ./firmware_backup package-codesys="$packageCodesys" package-settings="$packageSettings" \
            package-system="$packageSystem"   auto-update="$autoUpdate" \
            device-medium="$destinationMedium" callMode="cbm"
          #echo "return"; read
          ShowLastError

        else
          quit=$TRUE
        fi # 0 < selection <= deviceNr
      fi  # user selected some packages
    fi # targetList empty?
  done
}

MainFirmwareBackupSave

