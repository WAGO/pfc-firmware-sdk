# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#function MainRS232
## Processing of menu Main -> Administration -> RS232 Owner
##
#{
#  local rs232Owner=`./get_coupler_details RS232-owner`
#  
#  if [ "$rs232Owner" = "Linux" ]; then
#    rs232Owner="Linux Console"
#  elif [ "$rs232Owner" = "Modbus" ]; then
#    rs232Owner="Modbus RTU"
#  elif [ "$rs232Owner" = "CoDeSys" ]; then
#    rs232Owner="CoDeSys Debug"
#  elif [ "$rs232Owner" = "None" ]; then
#    rs232Owner="Free port (CoDeSys Libs)"
#  fi

  ShowGenericMenu "Owner of Serial Interface" $(basename ${BASH_SOURCE[0]})

#}
#
#MainRS232
