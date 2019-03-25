# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainTcpIpHostname
#
# Processing of Menu Main -> TCP/IP -> Hostname
# Show and change hostname.
#
{

  # loop until user wants to go back to TCP/IP-Menu
  local quit=$FALSE
  local selection
  local firstRun=yes

  while [ "$quit" = "$FALSE" ]; do
    if [[ "$firstRun" == "yes" ]]; then
      ShowEvaluateDataWindow "TCP/IP Configuration Host-/Domain Name"
      firstRun="no"
    fi

    local hostname=`./get_coupler_details hostname`

    # get actual hostname
    # default hostname is used if /etc/hostname is empty
    local currentHostname=`./get_coupler_details actual-hostname`

    local domainName=`./get_coupler_details domain-name`
    local currentDomainName=$(./get_coupler_details actual-domain-name)
    
    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "TCP/IP Configuration Host-/Domain Name" \
              "0. Back to Networking Menu" \
              "1. Hostname...............$hostname [current: $currentHostname]" \
              "2. Domain Name............$domainName [current: $currentDomainName]"

    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      # hostname should be changed -> get new name by inputbox
      1)  
          local dialogStatus
          WdialogWrapper "--inputbox" dialogStatus "$TITLE" "Change Hostname" "Enter new Hostname:" "50" "$hostname" 2> temp
          newValue=`cat temp`
          rm temp

          # if a new hostname was given - change it, show possible errors and read value again
          if [ "$dialogStatus" = "$SUCCESS" ] && [ "$newValue" != "$hostname" ]; then
            ShowProcessingDataWindow "TCP/IP Configuration Hostname"
            # replace spaces in new hostname according as url syntax
            newValue=`UriEncode $newValue`
            ./change_hostname "hostname=$newValue"
            hostname=`./get_coupler_details hostname`
            ShowLastError
          fi
          ;;
      2)
          # Domain-name was selected -> show input-window to get the new domain-name
          WdialogWrapper "--inputbox" dialogStatus "$TITLE" "Change Domain Name" "Enter new Domain Name:" 50 $domainName 2> temp
          newDomainName=`cat temp`
          rm temp

          # if a new domain-name was given - change it and show possible errors
          if [ "$dialogStatus" = "$SUCCESS" ] && [ "$newDomainName" != "$domainName" ]; then
            ShowProcessingDataWindow "TCP/IP Configuration Domain Name"
            # replace spaces in new value according as url syntax
            newDomainName=`UriEncode $newDomainName`
            ./change_hostname dnsdomain=$newDomainName  
            ShowLastError
          fi
          ;;

      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}

MainTcpIpHostname

