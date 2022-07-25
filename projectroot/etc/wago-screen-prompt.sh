#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

resize
clear
echo -e "\nINFO: screen manager 'screen' was added as terminal emulator.\n"
echo -e "\tBy default the following 3 screen-windows are active:\n"
echo -e "\t\t0. administration terminal (admin)"
echo -e "\t\t1. screensaver daemon (screend)"
echo -e "\t\t2. Codesys output screen (PLC)\n"
echo -e "To cycle through the windows press the keys 'C-a C-n' or 'C-a C-p' (C = Ctrl)."
echo -e "To detach from the screen session use 'C-a C-d'.\n"
echo -e "To start or stop Codesys or the screensaver daemon do the following:\n"
echo -e "\t/etc/init.d/codesys start | stop\n\t/etc/init.d/screend start | stop\n"
echo -e "\nPress <Enter> to step into administration terminal or 'C-c' to cancel."
read
#screen -S wago -p 0 -X clear
screen -r wago -p 0
