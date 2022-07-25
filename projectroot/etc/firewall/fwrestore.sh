#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#
# Last step of restore procedure for firewall configuration files.
# At this point it is assumed that all files are stored in the $1 directory
# plus sub-dirs. The procedure here should:
# 1. validate all files using standard xsd schemes,
# 2. move all files to their normal positions.
# The move is not done atomically, but file-wise, so it at least preserves the
# old state of single files if anything fails.
#
# Please note that there may still exist old files, which are no longer
# necessary, in the case a file was not present in the backup. Since this is
# currently not probable to happen the case is not taken care of.
# However, this should not have any impact on the overall security level
# since such files will in a normal case be inactive.
#

SRCDIR=$1

[ "x" = "x$SRCDIR" ] || exit 1

#
# Check the files.
#

xmlstarlet val --xsd /etc/firewall/ebtables/ebwlist.xsd $SRCDIR/ebtables/ebwlist.xml 2>&1 > /dev/null
[ "0" != "$?" ] || exit 1

xmlstarlet val --xsd /etc/firewall/iptables/ipcmn.xsd $SRCDIR/iptables/ipcmn.xml 2>&1 > /dev/null
[ "0" != "$?" ] || exit 1

SERVICES=`cd $SRCDIR/services/; ls -A -1 *.xml; cd - > /dev/null`
for SERVICE in $SERVICES ; do
    xmlstarlet val --xsd /etc/firewall/services/service.xsd $SRCDIR/services/$SERVICE 2>&1 > /dev/null
    [ "0" != "$?" ] || exit 1
done


#
# Move all files.
#
mv $SRCDIR/ebtables/ebwlist.xml /etc/firewall/ebtables
mv $SRCDIR/iptables/ipcmn.xml /etc/firewall/iptables
mv $SRCDIR/services/*.xml /etc/firewall/services
