#!/usr/bin/env python

#
# Copyright (C) 2019 WAGO Kontakttechnik GmbH & Co. KG
# Released under the terms of the GNU GPL v2.0.
#

#
# Script expects the XML configuration file as the first parameter.
# The resulting XML configuration is printed to stdout.
#

#
# Format of the JSON configuration file:
#
# {
#   "administration:users": 3,
#   "clock:clock": 3,
#   "codesys:rts_configuration": 3,
#   "codesys_ports:codesys_ports": 3,
#   "create_image:create_image": 3,
#   "dhcpd:dhcpd": 3,
#   "diagnostic:diagnostic": 3,
#   [...]
#   "tcpip_common_configuration:tcpip_common_configuration": 3,
#   "tftp:tftp": 3,
#   "users:users": 3,
#   "vpn:vpn": 3,
#   "wago_licenses:wago_licenses": 3,
#   "webvisu:webvisu": 3
# }
#

import xml.etree.ElementTree as ET
import json
import sys

user_level = {}
tree = ET.parse(sys.argv[1])
for level in tree.getroot().findall(".//level"):
  for nav in level.findall('nav'):
     key = level.get('id') + ":" + nav.get('fragment')
     # everything will be visible by admin only
     user_level[key] = '3'

json.dump(user_level, sys.stdout, sort_keys=True, indent=2)
print('')

