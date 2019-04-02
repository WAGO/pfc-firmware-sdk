# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function GetRs232OwnerState
{
  local rs232Owner=$(./get_coupler_details RS232-owner || echo "")

  if [ "$rs232Owner" = "Linux" ]; then
    rs232Owner="Linux Console"
  elif [ "$rs232Owner" = "Modbus" ]; then
    rs232Owner="Modbus RTU"
  elif [ "$rs232Owner" = "CoDeSys" ]; then
    rs232Owner="CODESYS Debug"
  elif [ "$rs232Owner" = "None" ]; then
    rs232Owner="Unassigned (Applications, CODESYS)"
  fi

  echo ...${rs232Owner}
}

GetRs232OwnerState
