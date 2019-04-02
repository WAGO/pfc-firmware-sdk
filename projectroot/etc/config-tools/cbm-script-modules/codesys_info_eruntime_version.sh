#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function PLCRuntimeInformationVersion
{
    local version=$(/etc/config-tools/get_runtime_config running-version)
    #TODO: map output to match WBM labels (i.e. 3 = e!RUNTIME etc.)
    ./wdialog "--msgbox" \
        "$TITLE" \
        "PLC Runtime" \
        "Version..............$(GetRuntimeLabel ${version})"
}

PLCRuntimeInformationVersion
