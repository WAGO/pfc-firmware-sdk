#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#
# NOTE: For historical reasons, functions and variables are still
#      called *download*.
#

function ShowKBusDLState
{

  local noDownload="no upload";
  kbusDownloadFile=`ls /var/downloads/kbus 2> /dev/null || true`
  if [ -z $kbusDownloadFile ]; then
    kbusDownloadFile=$noDownload;
  fi

  echo $kbusDownloadFile
}

ShowKBusDLState
