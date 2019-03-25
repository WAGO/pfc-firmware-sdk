#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#

case "$1" in 
  ethtool)
    echo -e "up sudo /etc/config-tools/pfc200_ethtool -W -c /etc/eth_settings.conf"
    ;;
  phy-settings)
    echo " pre-up sudo /sbin/ip link set eth0 up && sudo /etc/config-tools/pfc200_ethtool -W -c /etc/eth_settings.conf"
    ;;
  usage-options-names)
    echo
    ;;
  usage-options-description)
    echo
    ;;
esac
