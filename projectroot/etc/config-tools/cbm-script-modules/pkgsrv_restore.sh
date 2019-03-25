# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainFirmwareBackupRestore
{

  ShowProcessingDataWindow "Package server - restore"
  activePartitionMedium=`/etc/config-tools/get_filesystem_data active-partition-medium`
  activePartitionMediumText=`GetPartitionMediumOutputText $activePartitionMedium`

  if [[ "$activePartitionMedium" == "$SD_CARD" ]]; then
    ./wdialog "--msgbox" "$TITLE" "Note" " " "Restore operation is only allowed when booting from internal flash." " "
    return
  fi

  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do
  
    # get the existing devices and create a string for the selection-menu with them
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
    local menuString=""
    local deviceNr=1
    local deviceString=`./get_filesystem_data medium-by-index $deviceNr`
    local deviceStringText=`GetPartitionMediumOutputText "$deviceString"`
    if [ "$deviceString" = "$activePartitionMedium" ]; then
      deviceStringText=`echo $deviceStringText "(active partition)"`
    fi
    ifsBackup=$IFS
    IFS=$'\t'
    while [ -n "$deviceString" ]; do
      menuString=`printf "%s$deviceNr. $deviceStringText$IFS" "$menuString"`
      deviceNr=$(($deviceNr + 1))
      deviceString=`./get_filesystem_data medium-by-index $deviceNr`
      deviceStringText=`GetPartitionMediumOutputText "$deviceString"`
      if [ "$deviceString" = "$activePartitionMedium" ]; then
        deviceStringText=`echo $deviceStringText "(active partition)"`
      fi
    done

    # show the menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Select source to restore from" \
              "0. Back to Package Server Menu" \
              $menuString 
    IFS=$ifsBackup

    quit="$TRUE"
    # if the user selected a device
    if (( $selection > 0 )) && (( $selection <= $deviceNr )); then
    
      quit="$FALSE"
      sourceMedium=`./get_filesystem_data medium-by-index $selection`
      sourceMediumText=`GetPartitionMediumOutputText $sourceMedium`

      # get the existing backup packages on selected source Medium and create a menu string with them
      
      ./wdialog "--infobox" "$TITLE" "Package server - restore" " " \
                "Determine available packages on $sourceMediumText..." " "

      codesysAvailable=0
      /etc/config-tools/firmware_restore testmode=1 device-medium=$sourceMedium package-codesys=1 > /dev/null
      if [ "$?" = "0" ]; then
        codesysAvailable=1
      fi

      settingsAvailable=0
      /etc/config-tools/firmware_restore testmode=1 device-medium=$sourceMedium package-settings=1 > /dev/null
      if [ "$?" = "0" ]; then
        settingsAvailable=1
      fi
      
      systemAvailable=0
      /etc/config-tools/firmware_restore testmode=1 device-medium=$sourceMedium package-system=1 > /dev/null
      if [ "$?" = "0" ]; then
        systemAvailable=1
      fi

      local menuString=""
      ifsBackup=$IFS
      IFS=$'\t'
      
      packageName[1]="codesys"
      packageCount=0
      /etc/config-tools/firmware_restore testmode=1 device-medium=$sourceMedium package-codesys=1 > /dev/null
      if [ "$?" = "0" ]; then
        menuString=`printf "%s1. PLC Runtime Project$IFS"`
        packageCount=$(($packageCount + 1))
        packageAvailable[1]=1
      else
        menuString=`printf "%s1. PLC Runtime Project (not available)$IFS"`
        packageAvailable[1]=0
      fi
        
      packageName[2]="settings"
      /etc/config-tools/firmware_restore testmode=1 device-medium=$sourceMedium package-settings=1 > /dev/null
      if [ "$?" = "0" ]; then
        menuString=`printf "%s2. Settings$IFS" "$menuString"`
        packageCount=$(($packageCount + 1))
        packageAvailable[2]=1
      else
        menuString=`printf "%s2. Settings (not available)$IFS" "$menuString"`
        packageAvailable[2]=0
      fi
        
      packageName[3]="system"
      /etc/config-tools/firmware_restore testmode=1 device-medium=$sourceMedium package-system=1 > /dev/null
      if [ "$?" = "0" ]; then
        menuString=`printf "%s3. System$IFS" "$menuString"`
        packageCount=$(($packageCount + 1))
        packageAvailable[3]=1
      else
        menuString=`printf "%s3. System (not available)$IFS" "$menuString"`
        packageAvailable[3]=0
      fi

      if [ "${packageAvailable[2]}" = "1" ] && [ "${packageAvailable[3]}" = "1" ]; then
        menuString=`printf "%s4. System + Settings$IFS" "$menuString"`
        packageAvailable[4]=1
      else
        menuString=`printf "%s4. System + Settings (not available)$IFS" "$menuString"`
        packageAvailable[4]=0
      fi
      
      if [ "${packageAvailable[1]}" = "1" ] && [ "${packageAvailable[2]}" = "1" ] && [ "${packageAvailable[3]}" = "1" ]; then
        menuString=`printf "%s5. All$IFS" "$menuString"`
        packageAvailable[5]=1
      else
        menuString=`printf "%s4. All (not available)$IFS" "$menuString"`
        packageAvailable[5]=0
      fi

      # show the menu
      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "Select package to restore from $sourceMediumText" \
                "0. Back to Package Server Menu" \
                $menuString 

      IFS=$ifsBackup

      # analyse user selection
      if [ "${packageAvailable[$selection]}" = "0" ]; then
        ./wdialog "--msgbox" "$TITLE" "Package server - restore" " " \
                  "Selected package is not available!" " "
      else

        packageString=""
        rebootNotice=1
        case "$selection" in
          0)  quit=$TRUE;;
          1)  packageString="package-codesys=1"
              rebootNotice=0;;
          2)  packageString="package-settings=1";;
          3)  packageString="package-system=1";;
          4)  packageString="package-settings=1 package-system=1";;
          5)  packageString="package-all=1";;
          *)  errorText="Error in wdialog"
              quit=TRUE;;
        esac

        if [ -n "$packageString" ]; then

          #echo "Restore selected packages from $sourceMedium to $activePartitionMedium..."; read
          if [ "$rebootNotice" = "1" ]; then
            ./wdialog "--infobox" "$TITLE" "Package server - restore" " " \
                      "Restore selected package(s) from $sourceMediumText to $activePartitionMediumText..." " " \
                      "Note: a reboot will be initiated after restoring the package(s)." " "
          else
            ./wdialog "--infobox" "$TITLE" "Package server - restore" " " \
                      "Restore selected package(s) from $sourceMediumText to $activePartitionMediumText..." " "
          fi
          #echo "firmware_restore device-medium=$sourceMedium $packageString"; read
          
          ./firmware_restore device-medium=$sourceMedium $packageString
          #echo "return"; read
          ShowLastError
          quit=$TRUE
        fi
      fi

    fi
  done
}

MainFirmwareBackupRestore
