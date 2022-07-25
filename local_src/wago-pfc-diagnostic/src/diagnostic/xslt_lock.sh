#!/usr/bin/env bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG


# The scripts in this folder use temporary files with fixed names AND write to a common file ($HOST_DIR/diagnostics.xml).
# In order to be able to build the workspace parallelly, it it necessary that these scripts can be called from multiple 
# processes at the same time without corrupting these files. To accomplish this, each of the scripts includes this file
# which tries to acquire a file lock (mutex).


if [[ "$0" == "${BASH_SOURCE[0]}" ]]
then
    echo "This file (${BASH_SOURCE[0]}) is only meaningful if it is sourced!"
    exit 1
fi


function lockMyself()
{
  # See aslo https://linuxaria.com/howto/linux-shell-introduction-to-flock
  # scriptname will contain the base name of this file in order to lock the same file regardless of the sourcing script.
  scriptname="$(basename "${BASH_SOURCE[1]}")"
  lock="/var/lock/${scriptname}"

  # Open the file ${lock} and assign handle "200" to it.
  exec 200>"${lock}"

  # Exclusively lock the file, wait for a maximum of 10s.
  flock --timeout 10 200 || exit 1
}


# lock the file
lockMyself
