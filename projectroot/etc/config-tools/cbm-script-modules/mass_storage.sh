# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MassStorageSubmenuCF
{
  device=$1
  medium=`./get_device_data medium $device`
  mediumText=`GetPartitionMediumOutputText $medium`

  activeDeviceText="not active"
  activeDevice=`./get_filesystem_data active-device`
  if [ "$device" = "$activeDevice" ]; then
    activeDeviceText="active"
  fi

  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    local label=`./get_device_data label $device`

    # get value of bootflag (in every loopcycle, because it can change)
    bootflag=`./get_device_data bootflag $device`
    bootText="not bootable"
    bootAction="Set"
    negatedBootflag="1"
    if [ "$bootflag" = "1" ]; then
      bootText="bootable"
      bootAction="Reset"
      negatedBootflag="0"
    fi

    # create string for selection-menu, depending on kind of device
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog

    ifsBackup=$IFS
    IFS=$'\t'
    local menuString="1. Show information$IFS"

    if [ "$medium" != "$INTERNAL_FLASH" ]; then
      menuString=`printf "%s2. $bootAction bootflag$IFS" "$menuString"`
    fi
  
    if [ "$medium" != "$INTERNAL_FLASH" ] && [ "$device" != "$activeDevice" ]; then
      formatMenuText="3. FAT format medium"
      menuString=`printf "%s3. FAT format medium" "$menuString"`
    fi
  
    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Mass Storage - $mediumText - $device - $activeDeviceText - $bootText" \
              "0. Back to Mass Storage Menu" \
              $menuString
    IFS=$ifsBackup

    case "$selection" in
      0)  quit=$TRUE;;
    
      1)  # show information
          ./wdialog "--msgbox" \
                    "$TITLE" \
                    "Mass Storage Information" \
                    "Medium........$mediumText" \
                    "Device........$device" \
                    "Volume name...$label" \
                    "State.........$activeDeviceText" \
                    "Bootable?.....$bootText"
          ;;
        
      2)  # change bootflag
        ShowProcessingDataWindow "$bootAction bootflag for mass storage $mediumText"
        ./change_bootflag device=$device bootflag=$negatedBootflag
        ShowLastError
        ;;

      3)  # FAT format medium
        WdialogWrapper "--inputbox" retUnused "$TITLE" "FAT format medium $mediumText" "Enter new volume name:" 11 2> temp
        local volumeName=`cat temp`
        rm temp

        if [[ "$volumeName" != "" ]]; then
          ShowProcessingDataWindow "FAT format mass storage $mediumText"
          volumeName=`UriEncode $volumeName`
          ./format_medium device=$device volume-name=$volumeName > /dev/null 2> /dev/null
          ShowLastError
        fi
        ;;

      *)  errorText="Error in wdialog"
        quit=$TRUE;;
  esac
done

}

function MassStorageSubmenuSD
{
  device=$1
  medium=`./get_device_data medium $device`
  mediumText=`GetPartitionMediumOutputText $medium`

  activeDeviceText="not active"
  activeDevice=`./get_filesystem_data active-device`
  if [ "$device" = "$activeDevice" ]; then
    activeDeviceText="active"
  fi

  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    local label=`./get_device_data label $device`

    # create string for selection-menu, depending on kind of device
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog

    ifsBackup=$IFS
    IFS=$'\t'
    local menuString="1. Show information$IFS"

   
    if [ "$medium" != "$INTERNAL_FLASH" ] && [ "$device" != "$activeDevice" ]; then
      formatMenuText="2. FAT format medium"
      menuString=`printf "%s2. FAT format medium" "$menuString"`
    fi
    
    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Mass Storage - $mediumText - $device - $activeDeviceText" \
              "0. Back to Mass Storage Menu" \
              $menuString
    IFS=$ifsBackup

    case "$selection" in
      0)  quit=$TRUE;;
      
      1)  # show information
          ./wdialog "--msgbox" \
                    "$TITLE" \
                    "Mass Storage Information" \
                    "Medium........$mediumText" \
                    "Device........$device" \
                    "Volume name...$label" \
                    "State.........$activeDeviceText" 
          ;;
     
      2)  # FAT format medium
          WdialogWrapper "--inputbox" retUnused "$TITLE" "FAT format medium $mediumText" "Enter new volume name:" 11 2> temp
          local volumeName=`cat temp`
          rm temp

          if [[ "$volumeName" != "" ]]; then
            ShowProcessingDataWindow "FAT format mass storage $mediumText"
            volumeName=`UriEncode $volumeName`
            ./format_medium device=$device volume-name=$volumeName > /dev/null 2> /dev/null
            ShowLastError
          fi

          ;;
      
      *)  errorText="Error in wdialog"
          quit=$TRUE;;
    esac
  done
}

function MassStorageSubmenuNAND
{
  device=$1
  medium=`./get_device_data medium $device`
  mediumText=`GetPartitionMediumOutputText $medium`

  activeDeviceText="not active"
  activeDevice=`./get_filesystem_data active-device`
  if [ "$device" = "$activeDevice" ]; then
    activeDeviceText="active"
  fi
  ./wdialog "--msgbox" \
            "$TITLE" \
            "Mass Storage Information" \
            "Medium........$mediumText" \
            "Device........$device" \
            "State.........$activeDeviceText"
}

function MainMassStorageSubmenu
#
# Process of Main -> MassStorage -> Submenu for a single device
#
{
  medium=`./get_device_data medium $1`
  case ${medium} in
  $INTERNAL_FLASH_NAND)
    MassStorageSubmenuNAND $1
    ;;
  $SD_CARD)
    MassStorageSubmenuSD $1
    ;;
  *)
    MassStorageSubmenuCF $1
    ;;
  esac
}

function MainMassStorage
#
# Processing of menu Main -> MassStorage
#
#
{
  activeDevice=`./get_filesystem_data active-device`
  activePartitionMedium=`/etc/config-tools/get_filesystem_data active-partition-medium`
  activePartitionMediumText=`GetPartitionMediumOutputText $activePartitionMedium`

  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do
  
    # get the existing devices and create a string for the selection-menu with them
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
    local menuString=""
    local deviceNr=1
    local device=`./get_filesystem_data device-by-index 1`
    
    ifsBackup=$IFS
    IFS=$'\t'
    while [ -n "$device" ]; do
    
      # get_device_data not evaluated here -> set || true
      medium=`./get_device_data medium $device || true`
      mediumText=`GetPartitionMediumOutputText $medium`
      
      if [ "$device" = "$activeDevice" ]; then
        mediumText=`echo $mediumText "(active partition)"`
      fi
    
      menuString=`printf "%s$deviceNr. $mediumText$IFS" "$menuString"`
      deviceNr=$(($deviceNr + 1))
      
      # get_device_data not evaluated here -> set || true
      device=`./get_filesystem_data device-by-index $deviceNr || true`
      
    done

    # show the menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Mass Storage - Select partition" \
              "0. Back to Main Menu" \
              $menuString 
    IFS=$ifsBackup

    quit="$TRUE"
    # if the user selected a device
    if (( $selection > 0 )) && (( $selection <= $deviceNr )); then
    
      quit="$FALSE"
      selectedDevice=`./get_filesystem_data device-by-index $selection`
      MainMassStorageSubmenu $selectedDevice
    fi
  done
}

MainMassStorage
