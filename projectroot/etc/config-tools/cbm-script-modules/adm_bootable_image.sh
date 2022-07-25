# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

# $1: variable to store the result in
# $2: source location (directory expected)
# $3: target location (device expected)

function CheckSpace
{
  __resultvar=$1
  local result

  local source_location="$2"

  # should be a ubifs volume name
  local target_device=$3

  local source_location_size_kb=$(du --apparent-size --summarize --one-file-system \
                                   ${source_location} | awk '{print $1}')

  local target_device_size_kb=$(./get_device_data size ${target_device})

  if [[ ${source_location_size_bytes} -lt ${target_device_size_bytes} ]]; then

    difference=$((${target_device_size_bytes} - ${source_location_size_bytes}))

    # integer division, error of <20 bytes possible
    local five_percent_of_target=$((${target_device_size_bytes} / 20))

    if [[ ${difference} -lt ${five_percent_of_target} ]]; then
      result=${NARROW_SPACE_WARNING}
    else
      result=${SUCCESS}
    fi
  else
    result=${NOT_ENOUGH_SPACE_ERROR}
  fi

  eval $__resultvar="'$result'"
}

function MainAdministrationCreateBootImage
#
# Processing of menu main -> administration -> create bootable image
# Copy systemfiles from boot device to another
#
{
  processorType=`/etc/config-tools/get_coupler_details processor-type`
  activeDevice=`/etc/config-tools/get_filesystem_data active-device`
  activePartitionMedium=`/etc/config-tools/get_device_data medium $activeDevice`
  activePartitionMediumText=`GetPartitionMediumOutputText $activePartitionMedium`

  newSize=""

  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do
  
    # get the existing devices and create a string for the selection-menu with them
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
    local menuString=""
    local deviceNr=1
    local deviceString=`./get_filesystem_data medium-by-index $deviceNr`
    local deviceStringText=`GetPartitionMediumOutputText $deviceString`
    if [ "$deviceString" = "$activePartitionMedium" ]; then
      deviceStringText=`echo $deviceStringText "(active partition)"`
    fi
    ifsBackup=$IFS
    IFS=$'\t'
    while [ -n "$deviceString" ]; do
      menuString=`printf "%s$deviceNr. $deviceStringText$IFS" "$menuString"`
      deviceNr=$(($deviceNr + 1))
      deviceString=`./get_filesystem_data medium-by-index $deviceNr || true`

      if [ -n "$deviceString" ]; then
        deviceStringText=`GetPartitionMediumOutputText $deviceString`

        # mark active partition
        if [ "$deviceString" = "$activePartitionMedium" ]; then
          deviceStringText=`echo $deviceStringText "(active partition)"`
        fi
      fi
    done

    # show the menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Select destination for bootable image copy" \
              "0. Back to Administration Menu" \
              $menuString 
    IFS=$ifsBackup
  
    # if the user selected a device
    if (( $selection > 0 )) && (( $selection <= $deviceNr )); then
  
      ShowProcessingDataWindow "Create bootable image"  
      destinationMedium=`./get_filesystem_data medium-by-index $selection`

      if [ "$destinationMedium" = "$activePartitionMedium" ]; then
        ./wdialog "--msgbox" "$TITLE" "Create bootable image" " " \
                  "Can't copy active partition to itself" " "
      else

        echo "$destinationMedium" | grep "usb"
        if [ $? = $SUCCESS ] && [ "$processorType" = "Geode SC 1200" ]; then
            ./wdialog "--msgbox" "$TITLE" "Create bootable image" " " \
                      "Geode processor is not able to boot via USB stick." " "
        else
          echo "$destinationMedium" | grep "usb"
          if [ $? = $SUCCESS ]; then
            ./wdialog "--msgbox" "$TITLE" "Create bootable image" " " \
            "Note: if USB stick should be used as boot medium," \
            "legacy USB support must be enabled." \
            "(BIOS -> Advanced -> USB configuration -> Legacy USB support)" " "
          fi

          local CREATE_IMG=$TRUE
          # no raw copy via dd possible; one has to assume that the external
          # storage is much larger than the internal flash: check size before
          # copying.
          # Also check if CODESYS uses any files on the sd card.
       
          if [[ "$destinationMedium" == "$INTERNAL_FLASH_NAND"  ]]; then
            ./copy_firmware_image source-medium=$activePartitionMedium destination-medium=$destinationMedium action="nand-check-size" &> /dev/null
            status=$?
            ShowLastError

            case "$status" in
            "$SUCCESS")
              CREATE_IMG=$TRUE  
              ;;
            "$NARROW_SPACE_WARNING")
              
              ./wdialog "--msgbox" "$TITLE" "Warning!" \
              "NAND is less than 5% larger than the estimated amount" \
              "of data to be copied." \
              "Operation may fail if image creation is continued."

              WdialogWrapper "--menu" selection \
                             "$TITLE" \
                             "Create bootable image" \
                             "0. Back to Administration Menu" \
                             "1. Continue image creation"

              if [[ "$selection" == "1" ]]; then
                CREATE_IMG=$TRUE
              else
                CREATE_IMG=$FALSE
              fi
              ;; 
            "$NOT_ENOUGH_SPACE_ERROR")
              CREATE_IMG=$FALSE
              ;;
            esac
          elif [[ "$destinationMedium" == "$SD_CARD" ]]; then

            local sd_card_min_size=$(./get_min_sd_card_size)
            local sd_card_max_size=$(( $(/etc/config-tools/get_device_data size $(/etc/config-tools/get_device_data name sd-card)) / 1024))

            ./wdialog --inputbox "$TITLE" "Set image size" "Enter space in MB to be used on SD card (${sd_card_min_size}-${sd_card_max_size}):" 16 ${sd_card_max_size} 2>temp
            if [[ "$?" != 0 ]]; then
                continue
            fi

            newSize=$(cat temp)
            rm temp

            ShowLastError

            ShowProcessingDataWindow "Create bootable image"

            # SD card in use by CODESYS?
            if [[ "$CREATE_IMG" == "$TRUE" ]]; then
              ./copy_firmware_image source-medium=$activePartitionMedium destination-medium=$destinationMedium action="check-in-use" &> /dev/null
              status=$?
              ShowLastError

              case "$status" in
                $FALSE)
                  CREATE_IMG=$TRUE  
                  ;;
                $TRUE)
                  ./wdialog "--msgbox" "$TITLE" "Warning!" \
                    "CODESYS uses files located on the memory card and will be paused" \
                    "if image creation is continued."

                  WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Create bootable image" \
                    "0. Back to Administration Menu" \
                    "1. Continue image creation"

                  if [[ "$selection" == "1" ]]; then
                    CREATE_IMG=$TRUE
                  else
                    CREATE_IMG=$FALSE
                  fi
                  ;; 
              esac
            fi
          fi # main if-else (sd-card or nand)

          if [[ "$CREATE_IMG" == "$TRUE" ]]; then
            destinationMediumText=`GetPartitionMediumOutputText $destinationMedium`
            ./wdialog "--infobox" "$TITLE" "Create bootable image" " " \
                      "Copy systemfiles from $activePartitionMediumText to $destinationMediumText" " " \
                      "- can take a few minutes..."  " "
       

            if [[ ${DO_PRINT_DBG} == "yes" ]]; then
              ./copy_firmware_image source-medium=$activePartitionMedium destination-medium=$destinationMedium action="stop-codesys" size=$newSize
            else
              ./copy_firmware_image source-medium=$activePartitionMedium destination-medium=$destinationMedium action="stop-codesys" size=$newSize &> /dev/null
            fi
            #echo "return"; read
            ShowLastError
          fi
        fi
      fi

    else
      quit=$TRUE
    fi
  
  done
}

ShowProcessingDataWindow "Select destination for bootable image copy"
MainAdministrationCreateBootImage
ShowProcessingDataWindow "Administration"
