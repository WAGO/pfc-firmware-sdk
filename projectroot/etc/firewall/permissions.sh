#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

# The script sets the permission intended for the entire 'firewall' directory
# content. The permissions are intended to protect the content from
# an unauthorized modification, which basically means nobody except root should
# have an access to these settings. All modifications should be performed using
# 'firewall' config-tool. Also reading the file is done with help of the tool.
# This is not ment to provide security-through-obscurity.
# Also the umask for root should be set to 7177 (0177) for new *.rls files, but
# this is enforced directly in the scripts creating such files.

cd /etc/firewall

chown -R root:root . *

chmod 0600 *
chmod 0700 . ebtables/ iptables/ services/ templates/

chmod 0600 ebtables/*
chmod 0600 iptables/*
chmod 0600 services/*

cd - > /dev/null 2>&1
