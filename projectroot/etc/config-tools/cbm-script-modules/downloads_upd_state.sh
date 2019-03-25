#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#
# NOTE: for historical reasons, variables and functions are still called
#      *download*
#

function ShowUpdDLState
{
  local noDownload="no upload";
  
  updateScriptDownloadFile=`ls /var/downloads/update-script 2> /dev/null || true`
  if [ -z $updateScriptDownloadFile ]; then
    updateScriptDownloadFile=$noDownload;
  fi

  echo $updateScriptDownloadFile
}

ShowUpdDLState
