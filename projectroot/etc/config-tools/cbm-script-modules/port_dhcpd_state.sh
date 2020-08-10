# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2020 WAGO Kontakttechnik GmbH & Co. KG

unset IFS

state=disabled
for bridge in $(GetBridges); do
  pstate=$(./get_dhcpd_config -p $bridge -g dhcpd-state)
  if [ "$pstate" == enabled ]; then
    state=".enabled"
  fi
done
echo $state
