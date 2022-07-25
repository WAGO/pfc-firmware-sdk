# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainUsers
#
# Processing of menu Main -> Users
# Change the passwords for the WBM-Users
#
{
  ShowEvaluateDataWindow "WBM User Configuration"

  # create string for menu with the several different users 
  # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
  local menuString=""
  local userNr=1
  local userString=`./get_user $userNr`
  ifsBackup=$IFS
  IFS=$'\t'
  while [ -n "$userString" ]; do
    menuString=`printf "%s$userNr. $userString$IFS" "$menuString"`
    userNr=$(($userNr + 1))
    userString=`./get_user $userNr || true`
  done
  IFS=$ifsBackup

  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    # show menu (with tab as IFS so that the single strings of the automatic generated menu-text can be splitted)
    IFS=$'\t'
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "WBM User Configuration - Change Password" \
              "0. Back to Administration Menu" \
              $menuString 
    IFS=$ifsBackup

    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      * ) # any other selection - normally number of user 
          # get the user-name which belongs to the selected number
          user=`./get_user $selection`

          # if the user exists, get the new password from user two times and configure it
          if [ -n "$user" ]; then

            local newPassword=""
            local confirmPassword=""
            local currentPassword=""

            # show input-window to get new password
            WdialogWrapper "--pwdbox" retUnused "$TITLE" "Change password for WBM user \"$user\"" "Enter new password:" 66 2> temp
            local newPassword=`cat temp`
            rm temp

            if [ -n "$newPassword" ]; then
              
              # show input-window to confirm new password
              WdialogWrapper "--pwdbox" retUnused "$TITLE" "Change password for WBM user \"$user\"" "Confirm new password:" 66 2> temp
              local confirmPassword=`cat temp`
              rm temp

              if [ -n "$confirmPassword" ]; then
              
                #show input window to get current password
                
                WdialogWrapper "--pwdbox" retUnused "$TITLE" "Change password for WBM user \"$user\"" "Old password:" 66 2> temp
                local currentPassword=`cat temp`
                rm temp

                if [ -n "$currentPassword" ]; then
                
                  ShowProcessingDataWindow "WBM User Configuration - Change Password"
                  ./config_user user=$user \
                                new-password=$(/etc/config-tools/urlencode -e "$newPassword") \
                                confirm-password=$(/etc/config-tools/urlencode -e "$confirmPassword") \
                                old-password=$(/etc/config-tools/urlencode -e "$currentPassword")
                  ShowLastError
                fi
              fi
            fi
          fi
          ;;                  
     esac
  done
}
# end of MainUsers

MainUsers
