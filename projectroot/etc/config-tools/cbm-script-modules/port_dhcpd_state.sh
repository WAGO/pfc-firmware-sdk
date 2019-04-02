# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

unset IFS
ports=$(xmlstarlet sel -t -v "//ip_settings[show_in_wbm='1']/port_name" /etc/specific/network-interfaces.xml)

state=disabled
for port in $ports; do
  pstate=$(./get_dhcpd_config -p $port -g dhcpd-state)
  if [ "$pstate" == enabled ]; then
    state=".enabled"
  fi
done
echo $state

