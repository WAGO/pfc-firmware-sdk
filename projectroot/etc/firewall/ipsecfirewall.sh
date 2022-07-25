#!/bin/bash
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

# Change interface configuration file to set/reset network layer firewall rules
# needed for ipsec/strongSwan
#

umask 0177

DEFAULT_CONF=/etc/firewall/firewall
FIREWAL_NAMESPACE="http://www.wago.com/security/firewall"
IFACE_PATH=/f:firewall/f:parameters/f:interfaces/f:interface

declare    ipsec_value
declare    is_configfile_changed=false
declare -a ipaddress
declare -a ipsecsrv_ip
declare -a active_interface
declare -i number_of_server_ip

#
# Checks if required tools and files are present.
# If not then the script should exit cleanly.
#
check_prerequisites()
{
  if [ ! -s $DEFAULT_CONF ]; then
    eblog "err - $DEFAULT_CONF is missing - can't execute."
    exit 1
  fi

  . $DEFAULT_CONF
  
  FW_PARAMS_XML_TMP="$FW_DIR/temp_params.xml"
  
  FW_PARAMS_XML="$FW_DIR/params.xml"
  if [ ! -s $FW_PARAMS_XML ]; then
    eblog "err - $FW_PARAMS_XML is missing - can't execute."
    exit 1
  fi

  IPSEC_CONFIG_FILE="/etc/ipsec.conf"

  if [ ! -s $IPSEC_CONFIG_FILE ]; then
    eblog "err - $IPSEC_CONFIG_FILE is missing - can't execute."
    exit 1
  fi
}

#
# get value of ipsec_srv parameter
# 
get_ipsec_parameter()
{
  iface_rname=$1
  ipsec_value=off
  ipsec_value=`${FW_XST} sel -N f=$FIREWAL_NAMESPACE -T -t -m "$IFACE_PATH" --if "@rname='$iface_rname' and @ipsec_srv='yes'" -o "on" -n ${FW_PARAMS_XML}`
}

#
# get r-names and ip addresses of available network interfaces
# set ipsec_srv value to "no" for inactive interfaces
#
search_available_interfaces()
{
  local ifaces=($(ls /sys/class/net | grep -E "br|wwan"))
  local number_interfaces=${#ifaces[@]}
  local i=0
  while [[ $i -lt $number_interfaces ]]
  do 
    local ipaddr=$(ip addr show ${ifaces[$i]} | awk '$1 == "inet" {gsub(/\/.*$/, "", $2); print $2}')
    if [ -n "$ipaddr" ]
    then
      active_interface[$i]=${ifaces[$i]}
      ipaddress[$i]=$ipaddr 
    else
       get_ipsec_parameter ${ifaces[$i]}
       if [ "$ipsec_value" == "on" ]; then
          update_interface_config  ${ifaces[$i]} false
       fi    
    fi
    i=$(( $i+1 ))
  done
}

#
# get ip addresses of ipsec servers from ipsec config file
#
get_serv_ip()
{
  local -a srv_ip
  srv_ip=($(cat $IPSEC_CONFIG_FILE | grep -E "left=|right="))
  local i=0
  while [ $i -lt ${#srv_ip[@]} ]
  do
    if [[ ${srv_ip[$i]} =~ .*#.* ]]
    then
      :
    else 
      local ip=($(echo ${srv_ip[$i]} | awk -F "=" '{print $2}'))
      ipsecsrv_ip[$i]=$ip
    fi
    i=$(( $i+1 ))
  done  
  number_of_server_ip=${#ipsecsrv_ip[@]}  
}

#
# update interface configuration file 
# 
update_interface_config()
{
  local iface_rname=$1
  local ipsec_value
  if [ "$2" = true ] ; then
    ipsec_value="yes"
  else
    ipsec_value="no"
  fi
  
  ${FW_XST} ed -L -N f=$FIREWAL_NAMESPACE -u "$IFACE_PATH[@rname='$iface_rname']/@ipsec_srv" -v $ipsec_value ${FW_PARAMS_XML_TMP}
  is_configfile_changed=true
 }
 
#
# Main body of the script.
#
# Return
#   0 if interface configuration file (params.xml) is not changed, firewall should be not restarted
#   3 if interface configuration file (params.xml) changed, firewall should be restarted

#set -xv   # this line will enable debug

check_prerequisites
eblog "notice" "Firewall - set rules for ipsec/strongSwan authentication.."
search_available_interfaces
get_serv_ip
if [[ $number_of_server_ip -ge 0 ]]; then
   # check ipsec configuration and update ipsec_srv parameter for corresponding interfaces   
   i=0
   # backup original configuration file 
   cp -f $FW_PARAMS_XML $FW_PARAMS_XML_TMP
   while [[ $i -lt $number_of_server_ip ]]
   do  
     j=0
     while [[ $j -lt  ${#active_interface[@]} ]]
     do
       get_ipsec_parameter ${active_interface[$j]}
       if [ "${ipsecsrv_ip[$i]}" == "${ipaddress[$j]}" ]; then
         if [ "$ipsec_value" != "on" ]; then
            update_interface_config  ${active_interface[$j]} true
         fi
       else
         if [ "$ipsec_value" == "on" ]; then
            update_interface_config  ${active_interface[$j]} false
         fi   
       fi
        
       j=$(( $j+1 ))
     done
     i=$(( $i+1 ))
   done
   #restore configuration file
   sync
   mv  $FW_PARAMS_XML_TMP $FW_PARAMS_XML
   sync       
fi

if [[ "$is_configfile_changed" == true ]]; then
  exit 3
else  
  exit 0
fi  
 
