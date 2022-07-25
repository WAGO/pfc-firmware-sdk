#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

ShowEvaluateDataWindow "Information"

#
#  The following trick allows construction of different
# XML-based generic menus depending on dynamic options.
#  In this case, different Information submenus are displayed for
# CODESYS 2 and e!RUNTIME

runtime_version=$(/etc/config-tools/get_runtime_config running-version)

# Parent entry is forced to "PLC Runtime" here to hide the extra indirection step:
# PLC_Runtime->PLC Runtime Information (dummy)->PLC Runtime Information [cds2/eruntime]
if [[ "$runtime_version" == "2" ]]; then
    ShowGenericMenu "PLC Runtime Information" codesys_info_cds2.sh "PLC Runtime"
else
    ShowGenericMenu "PLC Runtime Information" codesys_info_eruntime.sh "PLC Runtime"
fi

