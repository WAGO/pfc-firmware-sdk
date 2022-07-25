#!/bin/bash
#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

#-----------------------------------------------------------------------------#
# Script-name: pfc200_config_default_gateway.sh
#
# Change the configuration of the default-gateway
# 
# Note: this is basically the IPC script with eth1 stuff removed.
#
# Author: Stefanie Meihöfer, WAGO GmbH
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

#-------------- main-programm ------------------------#

status=$SUCCESS

# show help-text if requested
if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
  echo
  echo "* Configure value and/or location of the default-gateway *"
  echo
  echo "Usage: config_default_gateway < interface=interface-value default-gateway-value=default-gateway-value >"
  echo
  echo "interface: none | eth0"
  echo "default-gateway-value: value of default-gateway"
  echo
  exit $status
fi

# Disable standard error handler: there are error conditions that have to be handled by the script
# i.e. changes to /etc/network/interfaces have to be reverted when the gw could not be enabled
trap - EXIT
set +o errexit

# filter input parmeter of input-string
newInterface=`GetParameterValue interface $*`
newValue=`GetParameterValue default-gateway-value $*`
#echo "newInterface:"$newInterface "newValue:"$newValue > /etc/config-tools/test

currentInterface=`${CONFIG_TOOLS_PATH}get_coupler_details default-gateway`
currentValue=`${CONFIG_TOOLS_PATH}get_eth_config eth0 default-gateway`
#echo "currentInterface:"$currentInterface "currentValue:"$currentValue >> /etc/config-tools/test

#currentStateEth0=`${CONFIG_TOOLS_PATH}get_eth_config eth0 state` 
# On PFC200, eth0 is always enabled
currentStateEth0="enabled"

# if a value for new default-gateway-address is denoted and new value differs from old value -> change default-gateway 
if [ -n "$newValue" ] && [ "$currentValue" != "$newValue" ]; then
  
  ${CONFIG_TOOLS_PATH}config_interfaces interface=eth0 state=$currentStateEth0 default-gateway=$newValue
  status=$?
  # If the gateway was already enabled, config_interfaces restarts eth0.
  # Regardless whether eth0 restart was successful, the settings
  # file is changed; the gw state (on/off) is deduced from its contents.
  # That means we have to restore the old settings to undo changes to the
  # settings file if an error occured.
  if [[ "$status" != "$SUCCESS" ]]; then
    ${CONFIG_TOOLS_PATH}config_interfaces interface=eth0 state=$currentStateEth0 default-gateway=$currentValue
  fi
fi


# if default gw should be turned on/off
if [ -n "$newInterface" ] && [ "$currentInterface" != "$newInterface" ]; then

  # check if new interface is valid
  if [ "$newInterface" != "none" ] && [ "$newInterface" != "eth0" ] ; then
    status=$INVALID_PARAMETER
    ReportError $INVALID_PARAMETER "(interface)"
    SetLastError "Interface invalid"
  else 

    currentConfigTypeEth0=`${CONFIG_TOOLS_PATH}get_eth_config eth0 config-type`

    # check if new interface is enabled
    if [ "$newInterface" = "eth0" ] && [ "$currentConfigTypeEth0" != "static" ] ; then
      status=$INVALID_PARAMETER
      ReportError $status "interface for default-gateway is not static enabled"
      SetLastError "Default gateway can only be set to static enabled interface"
    else

      # if new location is not eth0 - comment default-gateway on eth0 out
      if [ "$newInterface" = "none" ]; then
        if [ "$currentStateEth0" = "enabled" ]; then
          ${CONFIG_TOOLS_PATH}config_interfaces interface=eth0 state=$currentStateEth0 default-gateway=off
        fi
      fi
    
      # if an error for html-page is written by now, delete it - we are only interested in errors at the last step
      if [ -f $LAST_ERROR_FILENAME ]; then
        rm $LAST_ERROR_FILENAME
      fi

      # if new location is eth0 - remove comment at default-gateway on eth0
      if [ "$newInterface" = "eth0" ]; then
        ${CONFIG_TOOLS_PATH}config_interfaces interface=eth0 state=$currentStateEth0 default-gateway=on
        status=$?
        # Regardless whether gw could be successfully enabled, the settings
        # file is changed; the gw state (on/off) is deduced from its contents.
        # That means we have to call "gw disable" to undo changes to the
        # settings file on error.
        if [[ "$status" != "$SUCCESS" ]]; then
          ${CONFIG_TOOLS_PATH}config_interfaces interface=eth0 state=$currentStateEth0 default-gateway=off
        fi
      fi
    fi
  fi
fi

exit $status
