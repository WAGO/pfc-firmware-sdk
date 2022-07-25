#!/usr/bin/env python

#
# Copyright (C) 2019-2022 WAGO GmbH & Co. KG
# Released under the terms of the GNU GPL v2.0.
#

#
# Script expects the JSON configuration file as first parameter and the XML
# configuration file as the second parameter. The resulting XML configuration
# is printed to stdout.
#

#
# Expected Format of the JSON configuration file:
#
# {
#   "administration:users": 3,
#   "clock:clock": 2,
#   "codesys:rts_configuration": 2,
#   "codesys_ports:codesys_ports": 3,
#   "create_image:create_image": 3,
#   "dhcpd:dhcpd": 3,
#   "diagnostic:diagnostic": 2,
#   [...]
#   "tcpip_common_configuration:tcpip_common_configuration": 2,
#   "tftp:tftp": 3,
#   "users:users": 3,
#   "vpn:vpn": 3,
#   "wago_licenses:wago_licenses": 2,
#   "webvisu:webvisu": 2
# }
#

import xml.etree.ElementTree as ET
import json
import sys

with open(sys.argv[1]) as fp:
  tab = json.load(fp)

tree = ET.parse(sys.argv[2])
for level in tree.getroot().findall(".//level"):
  for nav in level.findall('nav'):
     key = level.get('id') + ":" + nav.get('fragment')
     nav.set('user_level' , str(tab.get(key, 0)))

tree.write(sys.stdout, encoding="utf8", xml_declaration=True)

