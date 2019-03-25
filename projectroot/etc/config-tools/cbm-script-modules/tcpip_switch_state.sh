# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

port0_label=`./get_eth_config eth0 device-id`
port1_label=`./get_eth_config eth1 device-id`

# Change order i.e. if port0 -> X2 and port1 -> X1

if [[ $port1_label > $port0_label ]]; then
  display_order_port0_label=$port0_label
  display_order_port1_label=$port1_label
else
  display_order_port0_label=$port1_label
  display_order_port1_label=$port0_label
fi

echo " ($display_order_port0_label/$display_order_port1_label)"
