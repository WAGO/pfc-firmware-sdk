# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

function MainSnmpConfiguration
#
# Processing of menu Main -> SNMP -> Configuration
#
{
  local deviceName=`/etc/config-tools/get_snmp_data device-name`
  local description=`/etc/config-tools/get_snmp_data description`
  local location=`/etc/config-tools/get_snmp_data physical-location`
  local contact=`/etc/config-tools/get_snmp_data contact`
 
  local snmpState="$(/etc/config-tools/get_port_state snmp)"

  # loop until user wants to get back to main menu
  
  local quit=$FALSE
  local selection
  local status
  while [ "$quit" = "$FALSE" ]; do
    
    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "General SNMP Configuration" \
              "0. Back to SNMP Configuration Menu" \
              "1. SNMP status.........${snmpState}" \
              "2. Name of device......$deviceName" \
              "3. Description.........$description" \
              "4. Physical location...$location" \
              "5. Contact.............$contact"

    # analyse user selection and jump to the according function
    case "$selection" in
    
      0)  quit=$TRUE;;
      
      1)  # state was selected -> show selection-menu with the possible states
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change SNMP state ($snmpState)" \
                    "0. Back to SNMP General Configuration Menu" \
                    "1. Enable" \
                    "2. Disable"
                  
          # assign the selected number to the according clock-display-modes
         case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState=""
          esac

          # if a new state was selected - change it, get actual value again and show possible errors
          if [ -n "$newState" ] && [ "$snmpState" != "$newState" ]; then
            ShowProcessingDataWindow "SNMP Configuration"
            /etc/config-tools/config_snmp set-snmp="$newState"
            ShowLastError
            snmpState="$(/etc/config-tools/get_port_state snmp)"
          fi
          ;;

      # change name of device
      2)  WdialogWrapper "--inputbox" status "$TITLE" "Change name of device" "Enter new name:" "66" "$deviceName" 2> temp
          newValue=`cat temp`
          rm temp

          # if a new name was give - change it, show possible errors and read value again
          if [ "$status" = $SUCCESS ] && [ "$newValue" != "$deviceName" ]; then
            ShowProcessingDataWindow "Configuration of SNMP name of device"
            # replace spaces in new name according as url syntax
            newValue=`UriEncode $newValue`
            if [ -z "$newValue" ]; then
              newValue="\"\""
            fi
            ./config_snmp "device-name=$newValue"
            deviceName=`/etc/config-tools/get_snmp_data device-name`
            ShowLastError
          fi
          ;;

      # change description
      3)  WdialogWrapper "--inputbox" status "$TITLE" "Change description" "Enter new description:" "66" "$description" 2> temp
          newValue=`cat temp`
          rm temp

          # if a new text was give - change it, show possible errors and read value again
          if [ "$status" = "$SUCCESS" ] && [ "$newValue" != "$description" ]; then
            ShowProcessingDataWindow "Configuration of SNMP description"
            # replace spaces in new name according as url syntax
            newValue=`UriEncode $newValue`
            if [ -z "$newValue" ]; then
              newValue="\"\""
            fi
            ./config_snmp "description=$newValue"
            description=`/etc/config-tools/get_snmp_data description`
            ShowLastError
          fi
          ;;

      # change physical location
      4)  WdialogWrapper "--inputbox" status "$TITLE" "Change physical location" "Enter new location:" "66" "$location" 2> temp
          newValue=`cat temp`
          rm temp

          # if a new location was give - change it, show possible errors and read value again
          if  [ "$status" = "$SUCCESS" ] && [ "$newValue" != "$location" ]; then
            ShowProcessingDataWindow "Configuration of SNMP physical location"
            # replace spaces in new location according as url syntax
            newValue=`UriEncode $newValue`
            if [ -z "$newValue" ]; then
              newValue="\"\""
            fi
            ./config_snmp "physical-location=$newValue"
            location=`/etc/config-tools/get_snmp_data physical-location`
            ShowLastError
          fi
          ;;

      # change contact
      5)  WdialogWrapper "--inputbox" status "$TITLE" "Change contact" "Enter new contact:" "66" "$contact" 2> temp
          newValue=`cat temp`
          rm temp

          # if a new contact was give - change it, show possible errors and read value again
          if  [ "$status" = "$SUCCESS" ] && [ "$newValue" != "$contact" ]; then
            ShowProcessingDataWindow "Configuration of SNMP contact"
            # replace spaces in new contact according as url syntax
            newValue=`UriEncode $newValue`
            if [ -z "$newValue" ]; then
              newValue="\"\""
            fi
            ./config_snmp "contact=$newValue"
            contact=`/etc/config-tools/get_snmp_data contact`
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}


function MainSnmpV1V2cManagerConfiguration
#
# Processing of menu Main -> SNMP -> v1/v2c Manager Configuration
#
{
  state=`/etc/config-tools/get_snmp_data v1-v2c-state`
  communityName=`/etc/config-tools/get_snmp_data v1-v2c-community-name`
  
  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  local status
  while [ "$quit" = "$FALSE" ]; do
  
    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Configuration of SNMP v1/v2c Manager" \
              "0. Back to SNMP Configuration Menu" \
              "1. Protocol state..........$state" \
              "2. Local community name....$communityName"

    # analyse user selection and jump to the according function
    case "$selection" in
    
      0)  quit=$TRUE;;
      
      # change protocoll status
      1)  WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change state of v1/v2c protocol ($state)" \
                    "0. Back to SNMP v1/v2c Menu" \
                    "1. Enable" \
                    "2. Disable"

          # analyse user selection and jump to the according function
          newState=""
          case "$selection" in
            0)  newState=$state;;
            1)  newState="enabled";;
            2)  newState="disabled";;
            *)  errorText="Error in wdialog"
                quit=TRUE;;
          esac

          if [ "$newState" != "$state" ]; then
          
            if [ "disabled" = "$newState" ]; then
              ./config_snmp v1-v2c-state=disabled
              
            elif [ "enabled" = "$newState" ]; then
          
              WdialogWrapper "--inputbox" status "$TITLE" "Enable v1/v2c protocol" "Enter community name:" "66" "$communityName" 2> temp
              newValue=`cat temp`
              rm temp
              
              # if a new name was give - change it, show possible errors and read value again
              if  [ "$status" = "$SUCCESS" ]; then
                ./config_snmp "v1-v2c-state=enabled v1-v2c-community-name=$newValue"
              fi
            fi
            
            ShowLastError
            state=`/etc/config-tools/get_snmp_data v1-v2c-state`
            communityName=`/etc/config-tools/get_snmp_data v1-v2c-community-name`
          fi
          ;;
      
      #change community name
      2)  WdialogWrapper "--inputbox" status "$TITLE" "Change v1/v2c community name" "Enter new name:" "66" "$communityName" 2> temp
          newValue=`cat temp`
          rm temp

          # if a new name was give - change it, show possible errors and read value again
          if  [ "$status" = "$SUCCESS" ] && [ "$newValue" != "$communityName" ]; then
            ShowProcessingDataWindow "Configuration of SNMP v1/v2c community name"
            # replace spaces in new location according as url syntax
            newValue=`UriEncode $newValue`
            ./config_snmp "v1-v2c-community-name=$newValue"
            communityName=`/etc/config-tools/get_snmp_data v1-v2c-community-name`
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}


function MainSnmpV1V2cTrapReiceiverConfigurationShow 
{
  local receiverNo=$1
  local address=`./get_snmp_data v1-v2c-trap-receiver-address $receiverNo`
  local communityName=`./get_snmp_data v1-v2c-trap-receiver-community-name $receiverNo`
  local version=`./get_snmp_data v1-v2c-trap-receiver-version $receiverNo`
  
  if [ -n "$address" ]; then

    # loop until user wants to get back to main menu
    local quit=$FALSE
    local selection
    while [ "$quit" = "$FALSE" ]; do
  
      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "SNMP v1/v2c Trap Receiver Number $receiverNo" \
                "0. Back to SNMP v1/v2c Trap Receiver Menu" \
                "1. Delete" \
                "-  IP Address..$address" \
                "-  Community Name....$communityName" \
                "-  Trap Version......$version"

      # analyse user selection and jump to the according function
      if [ "$selection" = "0" ]; then
        quit="$TRUE"
    
      # delete receiver
      elif [ "$selection" = "1" ]; then
        communityName=`UriEncode $communityName`
        /etc/config-tools/config_snmp v1-v2c-trap-receiver-edit=delete \
                                      v1-v2c-trap-receiver-address=$address \
                                      v1-v2c-trap-receiver-community-name="$communityName" \
                                      v1-v2c-trap-receiver-version=$version
        ShowLastError
        quit="$TRUE"
      fi
      # other selection - do nothing
      
    done
  fi
}


function MainSnmpV1V2cTrapReiceiverConfigurationAdd
{
  local status
  local selection
  WdialogWrapper "--inputbox" status "$TITLE" "Add new Trap Receiver" "Enter receiver IP address:" "15" "" 2> temp
  address=`cat temp`
  rm temp

  # if a value was given - continue with community name
  if  [ "$status" = "$SUCCESS" ]; then
    WdialogWrapper "--inputbox" status "$TITLE" "Add new Trap Receiver" "Enter community name:" "66" "" 2> temp
    communityName=`cat temp`
    rm temp

    # if a value was given - continue with version
    if  [ "$status" = "$SUCCESS" ]; then
      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "Select trap version" \
                "0. Back to SNMP v1/v2c Receiver Menu" \
                "1. v1" \
                "2. v2c"

      version=""
      if [ "$selection" = "1" ]; then
        version="v1"
      elif [ "$selection" = "2" ]; then
        version="v2c"
      fi
      
      if [ -n "$version" ]; then
      
        ShowProcessingDataWindow "Add new trap receiver..."
        # replace spaces in strings as url syntax
        address=`UriEncode $address`
        communityName=`UriEncode $communityName`
        
        /etc/config-tools/config_snmp v1-v2c-trap-receiver-edit=add \
                                      v1-v2c-trap-receiver-address=$address \
                                      v1-v2c-trap-receiver-community-name=$communityName \
                                      v1-v2c-trap-receiver-version=$version
        ShowLastError
      fi
    fi
  fi
}


function MainSnmpV1V2cTrapReiceiverConfiguration
#
# Processing of menu Main -> SNMP -> v1/v2c Trap Receiver Configuration
#
{
  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do
  
    # get the existing trap receiver and create a string for the selection-menu with them
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
    local menuString=""
    local receiverNo=1
    local receiverAddress=`./get_snmp_data v1-v2c-trap-receiver-address $receiverNo`
    
    ifsBackup=$IFS
    IFS=$'\t'
    while [ -n "$receiverAddress" ]; do
    
      menuString=`printf "%s$receiverNo. Trap Receiver $receiverNo ($receiverAddress)$IFS" "$menuString"`
      receiverNo=$(($receiverNo + 1))
      receiverAddress=`./get_snmp_data v1-v2c-trap-receiver-address $receiverNo`
      
    done
    
    addReceiverMenuNo=$receiverNo
    menuString=`printf "%s$addReceiverMenuNo. Add new Trap Receiver$IFS" "$menuString"`

    # show the menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Configuration of SNMP v1/v2c Trap Receiver" \
              "0. Back to SNMP Configuration Menu" \
              $menuString 
    IFS=$ifsBackup

    # if the user selected a trap receiver
    if (( $selection > 0 )) && (( $selection < $addReceiverMenuNo )); then
    
      MainSnmpV1V2cTrapReiceiverConfigurationShow $selection
      
    elif (( $selection == $addReceiverMenuNo )); then
      MainSnmpV1V2cTrapReiceiverConfigurationAdd
    
    else
      quit="$TRUE"
    fi
    
  done
}


function MainSnmpV3User
#
# Processing of menu Main -> SNMP -> V3 -> User 1/2
#
{
  userNo=$1
  
  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Configuration of SNMP v3 for user $userNo" \
              "0. Back to Main Menu.." \
              "1. State............................." \
              "2. Authentification type............." \
              "3. Seciruty authentification name...." \
              "4. Authentification key.............." \
              "5. Privacy enable...................." \
              "6. Privacy Key......................." \
              "7. Notification/Trap enable.........." \
              "8. Notification receiver IP.........." 

    # analyse user selection and jump to the according function
    case "$selection" in
    
      0)  quit=$TRUE;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}


function MainSnmpV3ConfigurationShow
{
  local userNo=$1
  local authName=`./get_snmp_data v3-auth-name $userNo`
  local authType=`./get_snmp_data v3-auth-type $userNo`
  local authKey=`./get_snmp_data v3-auth-key $userNo`
  local privacy=`./get_snmp_data v3-privacy $userNo`
  local privacyKey=`./get_snmp_data v3-privacy-key $userNo`
  local notificationReceiver=`./get_snmp_data v3-notification-receiver $userNo`
  
  if [ -n "$authName" ]; then

    # loop until user wants to get back to main menu
    local quit=$FALSE
    local selection
    while [ "$quit" = "$FALSE" ]; do
  
      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "SNMP v3 User Number $userNo" \
                "0. Back to SNMP v3 Configuration Menu" \
                "1. Delete" \
                "-  Authentication Name........$authName" \
                "-  Authentication Type........$authType" \
                "-  Authentication Key.........$authKey" \
                "-  Privacy....................$privacy" \
                "-  Privacy Key................$privacyKey" \
                "-  Notification Receiver IP...$notificationReceiver"

      # analyse user selection and jump to the according function
      if [ "$selection" = "0" ]; then
        quit="$TRUE"
    
      # delete user
      elif [ "$selection" = "1" ]; then
        /etc/config-tools/config_snmp v3-edit=delete \
                                      v3-auth-name=$authName \
                                      v3-auth-type=$authType \
                                      v3-auth-key=$authKey \
                                      v3-privacy=$privacy \
                                      v3-privacy-key=$privacyKey \
                                      v3-notification-receiver=$notificationReceiver
        #read
        ShowLastError
        quit="$TRUE"
      fi
      # other selection - do nothing
      
    done
  fi
}


function MainSnmpV3ConfigurationAdd
{
  # get all necessary values one after the other - start with user name
  local status
  local selection
  WdialogWrapper "--inputbox" status "$TITLE" "Add new v3 User" "Enter security authentication name:" "66" "" 2> temp
  authName=`cat temp`
  rm temp

  # if a value was given - continue with authentication type
  if  [ "$status" = "$SUCCESS" ]; then
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Select authentication type" \
              "0. Back to SNMP v3 Configuration Menu" \
              "1. none" \
              "2. MD5" \
              "3. SHA"

    authType=""
    case "$selection" in
      1) authType="none";;
      2) authType="MD5";;
      3) authType="SHA";;
      *) ;;
    esac
      
    # get authentification key
    if [ -n "$authType" ]; then
      WdialogWrapper "--inputbox" status "$TITLE" "Add new v3 user" "Enter authentication key (min. 8 char):" "66" "" 2> temp
      authKey=`cat temp`
      rm temp

      # if a value was given - continue with privacy
      if  [ "$status" = "$SUCCESS" ]; then
        WdialogWrapper "--menu" selection \
                  "$TITLE" \
                  "Select privacy" \
                  "0. Back to SNMP v3 Configuration Menu" \
                  "1. none" \
                  "2. DES" \
                  "3. AES"

        privacy=""
        case "$selection" in
          1) privacy="none";;
          2) privacy="DES";;
          3) privacy="AES";;
          *) ;;
        esac
      
        # get privacy key
        if [ -n "$privacy" ]; then
          WdialogWrapper "--inputbox" status "$TITLE" "Add new v3 user" "Enter privacy key (min. 8 char; if empty, auth. key will be used):" "66" "" 2> temp
          privacyKey=`cat temp`
          rm temp

          # if a value was given - continue with privacy
          if  [ "$status" = "$SUCCESS" ]; then
            
            WdialogWrapper "--inputbox" status "$TITLE" "Add new v3 user" "Enter notification receiver IP address (optional):" "15" "" 2> temp
            notificationReceiver=`cat temp`
            rm temp

            # if a value was given - continue with version
            if [ "$status" = "$SUCCESS" ]; then
              ShowProcessingDataWindow "Add new v3 user..."
              # replace spaces in strings as url syntax
              authName=`UriEncode $authName`
              authType=`UriEncode $authType`
              privacyKey=`UriEncode $privacyKey`
              
              #echo "authName:"$authName "authType:"$authType "authKey:"$authKey 
              #echo "privacy:"$privacy "privacyKey:"$privacyKey "notificationReceiver:"$notificationReceiver
              #read
        
              /etc/config-tools/config_snmp v3-edit=add \
                                            v3-auth-name=$authName \
                                            v3-auth-type=$authType \
                                            v3-auth-key=$authKey \
                                            v3-privacy=$privacy \
                                            v3-privacy-key=$privacyKey \
                                            v3-notification-receiver=$notificationReceiver
              ShowLastError
            fi
          fi
        fi
      fi
    fi
  fi
}


function MainSnmpV3
#
# Processing of menu Main -> SNMP -> V3
#
{
  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do
  
    # get the existing v3 user and create a string for the selection-menu with them
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
    local menuString=""
    local userNo=1
    local authName=`./get_snmp_data v3-auth-name $userNo`
    #echo "authName:"$authName
    
    ifsBackup=$IFS
    IFS=$'\t'
    while [ -n "$authName" ]; do
    
      menuString=`printf "%s$userNo. Username $authName $IFS" "$menuString"`
      userNo=$(($userNo + 1))
      authName=`./get_snmp_data v3-auth-name $userNo`
      
    done
    
    addUserMenuNo=$userNo
    menuString=`printf "%s$addUserMenuNo. Add new v3 User$IFS" "$menuString"`

    # show the menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Select SNMP v3 user" \
              "0. Back to SNMP Configuration Menu" \
              $menuString 
    IFS=$ifsBackup

    # if the user selected a user
    if (( $selection > 0 )) && (( $selection < $addUserMenuNo )); then
      MainSnmpV3ConfigurationShow $selection

    elif (( $selection == $addUserMenuNo )); then
      MainSnmpV3ConfigurationAdd

    else
      quit="$TRUE"
    fi
    
  done

}


function MainSnmp
#
# Processing of menu Main -> SNMP
#
{
  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    local running=`/etc/config-tools/get_port_state snmp`
    local running_snmp="disabled"
    local running_snmps="disabled"
    if [ "enabled" == "$running" ] ; then
        local v1v2state=`/etc/config-tools/get_snmp_data v1-v2c-state`
        if [ "enabled" == "$v1v2state" ] ; then
            local running_snmp="enabled"
        else
            local running_snmps="enabled"
        fi
    fi

    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "SNMP Configuration" \
              "0. Back to Main Menu" \
              "1. General SNMP Configuration" \
              "2. SNMP v1/v2c Manager Configuration" \
              "3. SNMP v1/v2c Trap Receiver Configuration" \
              "4. SNMP v3 Configuration" \
              "5. SNMP firewalling" \
              "6. Secure SNMP firewalling"

    # analyse user selection and jump to the according function
    case "$selection" in
    
      0)  quit=$TRUE;;

      1)  MainSnmpConfiguration;;
      2)  MainSnmpV1V2cManagerConfiguration;;
      3)  MainSnmpV1V2cTrapReiceiverConfiguration;;
      4)  MainSnmpV3;;

      5)  FirewallServiceInterfaces snmp SNMP ${running_snmp};;
      6)  FirewallServiceInterfaces snmps SNMPS ${running_snmps};;
      
      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}

MainSnmp
