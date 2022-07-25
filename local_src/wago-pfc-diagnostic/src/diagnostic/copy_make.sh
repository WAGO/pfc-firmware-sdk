#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

echo "COPY_MAKE:cp $1 $PTXDIST_WORKSPACE/src/led_server/diagnostic"

cp $1 $PTXDIST_WORKSPACE/src/led_server/src/diagnostic

exit 0