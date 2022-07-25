#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#
# NOTE: cd - lines are necessary if FWDIR is not absolute and a single starting
# point directory shall be used.
#

FWDIR=/etc/firewall

if [ "xlocal" = "x$1" ]; then
    FWDIR=.
fi

cd ${FWDIR}/ebtables > /dev/null 2>&1 || exit 1
xmlstarlet tr ebwbkp.xsl ebwlist.xml || exit 1
cd - > /dev/null 2>&1 || exit 1

cd ${FWDIR}/iptables > /dev/null 2>&1 || exit 1
xmlstarlet tr ipbkp.xsl ipcmn.xml || exit 1
cd - > /dev/null 2>&1 || exit 1

cd ${FWDIR}/services > /dev/null 2>&1 || exit 1
xmlstarlet tr servicebkp.xsl *.xml || exit 1
cd - > /dev/null 2>&1 || exit 1
