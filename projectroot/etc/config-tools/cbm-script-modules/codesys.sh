# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function GetRuntimeLabel
{
    local runtimeVersion=$1

    case ${runtimeVersion} in
        0) echo "none";;
        2) echo "CODESYS 2";;
        3) echo "e!RUNTIME";;
    esac
}

ShowGenericMenu "CODESYS" $(basename ${BASH_SOURCE[0]})
