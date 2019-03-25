# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainTcpIpDnsServerEdit
#
# Processing of menu Main -> TCP/IP -> DNS-Server -> Edit DNS-Server
# Delete or change a specified DNS-Server
#
# input: number of DNS-server
#
{
  local dnsServerNr=$1

  ShowEvaluateDataWindow "TCP/IP Configuration - Edit DNS Server"

  # get name of DNS-server by its number
  local dnsServerName=`./get_dns_server $dnsServerNr`

  # if dns-server is existing - show selection-menu to rename or delete it
  if [ -n "$dnsServerName" ]; then

    # loop until user wants to got back to DNS-server-menu or dns-server was deleted
    local quit=$FALSE
    local selection
    while [ "$quit" = "$FALSE" ]; do

      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "TCP/IP Configuration DNS Server $dnsServerNr ($dnsServerName)" \
                "0. Back to DNS Server Configuration Menu" \
                "1. Edit" \
                "2. Delete"

      # analyse user selection and do to the according processing
      case "$selection" in
    
        0)  # Quit was selected -> end loop and get back to superior menu
            quit=$TRUE;;

        1)  # Edit was selected -> show input-box to get the new name of DNS-server
            WdialogWrapper "--inputbox" retUnused "$TITLE" "Change DNS Server" "Enter new DNS Server" 15 $dnsServerName 2> temp
            newServerName=`cat temp`
            rm temp
        
            # new name was given - change it, get the actual value again and show possible error
            if [ -n "$newServerName" ] && [ "$newServerName" != "$dnsServerName" ]; then
              ShowProcessingDataWindow "TCP/IP Configuration DNS Server"
              ./edit_dns_server $DNS_SERVER_NR=$dnsServerNr $DNS_SERVER_NAME=$newServerName change=change
              dnsServerName=`./get_dns_server $dnsServerNr`
              ShowLastError
            fi
            ;;

        2)  # Delete was selected - do it, show possible error and go directly back to superior menu
            ShowProcessingDataWindow "TCP/IP Configuration DNS Server"
            ./edit_dns_server $DNS_SERVER_NR=$dnsServerNr delete=delete
            ShowLastError
            quit=$TRUE
            ;;

        *)  errorText="Error in wdialog"
            quit=TRUE;;

      esac
    done
  fi
}
# end of MainTcpIpDnsServerEdit



function MainTcpIpDnsServer
#
# Processing of menu Main-Menu -> TCP/IP -> DNS-Server
# Change domain-name, create new DNS-server, edit existing DNS-server
#
{
  # loop until the user wants to get back to TCP/IP-menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    ShowEvaluateDataWindow "TCP/IP Configuration DNS Server"

    # create string for menu with the several different DNS-Server (count may be variable in every loop-cycle) and show menu
    # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
    local dnsServerString=""
    local dnsServerNr=1
    local dnsServerName=`./get_dns_server $dnsServerNr`
    ifsBackup=$IFS
    IFS=$'\t'
    while [ -n "$dnsServerName" ]; do
      dnsServerString=`printf "%s$(($dnsServerNr)). DNS Server $dnsServerNr ..............................$dnsServerName$IFS" "$dnsServerString"`
      dnsServerNr=$(($dnsServerNr + 1))
      dnsServerName=`./get_dns_server $dnsServerNr`
    done

    # remove JSON markup symbols from list
    local dns_servers_from_dhcp=$(./get_dns_server get-dynamic)

    local firstReadonlyIndex=$dnsServerNr
    for dnsServerIP in ${dns_servers_from_dhcp}; do
        dnsServerString=`printf "%s$((${dnsServerNr})). DNS Server ${dnsServerNr} [from dhcp: read-only]........${dnsServerIP}${IFS}" "$dnsServerString"`
        dnsServerNr=$(($dnsServerNr + 1))
    done
    addSelectNr=$(($dnsServerNr))

    # show menu    
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "TCP/IP Configuration DNS Server" \
              "0. Back to TCP/IP Menu" \
              $dnsServerString \
              "$addSelectNr. Add new DNS Server"
    IFS=$ifsBackup

    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)            # Quit was selected -> end loop and get back to superior menu
                    quit=$TRUE;;

        
      $addSelectNr) # add new DNS-server was selected -> show input-window to get the name of the new server
                    ./wdialog "--inputbox" "$TITLE" "Add new DNS Server" "Enter new IP address:" 15 2> temp
                    newValue=`cat temp`
                    rm temp

                    # if a name was given, add server and show possible error
                    if [ -n "$newValue" ]; then
                      ShowProcessingDataWindow "TCP/IP Configuration DNS Server"
                      ./edit_dns_server add=add $DNS_SERVER_NAME=$newValue
                      ShowLastError
                    fi
                    ;;
        
      *)            # an existing DNS-server was selected 
                    # show submenu to rename or delete this DNS-server 
                    if [[ "$selection" < "$firstReadonlyIndex" ]]; then
                        MainTcpIpDnsServerEdit $selection
                    fi
                    ;;
    esac
  done
}
# end of MainTcpIpDnsServer

MainTcpIpDnsServer 
