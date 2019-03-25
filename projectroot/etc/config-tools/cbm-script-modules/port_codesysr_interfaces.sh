# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

. ./cbm-script-modules/port_interfaces.sh

running=`/etc/config-tools/get_plc_config`

if [ "0" != "$running" ] ; then
    running="enabled"
else
    running="disabled"
fi

FirewallServiceInterfaces codesysr "CODESYS Runtime" ${running}

