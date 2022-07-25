# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function ShowCODESYSWebserverVersion
{
  local webservVersion=$(./get_coupler_details codesys-webserver-version)
  ./wdialog "--msgbox" "$TITLE" "PLC Runtime" \
                                "Webserver Version.................$webservVersion"
}

ShowCODESYSWebserverVersion
