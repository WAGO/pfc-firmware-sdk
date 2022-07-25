# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function SetCODESYSPasswordMain
{
  # show input-window to get new password
  local oldUmask=$(umask)
  umask 0077
  WdialogWrapper "--inputbox" status "$TITLE" "Change password for PLC runtime user \"admin\"" "Port authentication password:" 66 2> temp
  local newPassword1=`cat temp`
  rm temp

  if [[ "$status" != "0" ]]; then # user chose "abort"
    umask $oldUmask
    return
  fi

  # fail on empty password
  if [ ! "$newPassword1" ]; then
    SetLastError "Password may not be empty."
    ShowLastError
    umask $oldUmask
    return
  fi

  WdialogWrapper "--inputbox" status "$TITLE" "Change password for PLC runtime user \"admin\"" "Confirm password:" 66 2> temp
  local newPassword2=`cat temp`
  rm temp

  umask $oldUmask

  if [[ "$status" != "0" ]]; then # user chose "abort"
    return
  fi

  if [[ "$newPassword1" == "$newPassword2" ]]; then
    /etc/config-tools/config_linux_user user=admin \
                                        new-password=$(/etc/config-tools/urlencode -e "$newPassword1") \
                                        confirm-password=$(/etc/config-tools/urlencode -e "$newPassword2")
  else
    SetLastError "Passwords don't match."
  fi
  ShowLastError
}


SetCODESYSPasswordMain
