# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainVga
# Processing of menu Main -> HMI settings -> VGA Settings
{

  local videoMode=`./show_video_mode string`

 # VGA Configuration was selected -> show selection-menu with the several possible vga-strings
  ShowEvaluateDataWindow "VGA Configuration"

  # ask for the possible vga-configurations and create a string for the selection-menu with them
  # use tabs as IFS, so that the single lines can be splitted at the right place by wdialog
  local menuString=""
  local vgaConfigNr=1
  local vgaString=`./show_video_mode by-index $vgaConfigNr || true`

  ifsBackup=$IFS
  IFS=$'\t'
  while [ -n "$vgaString" ]; do
    menuString=`printf "%s$vgaConfigNr. $vgaString$IFS" "$menuString"`
    vgaConfigNr=$(($vgaConfigNr + 1))
    vgaString=`./show_video_mode by-index $vgaConfigNr || true`
  done

  local selection  

  WdialogWrapper "--menu" selection \
            "$TITLE" \
            "VGA Configuration ($videoMode)" \
            "0. Back to HMI Settings Menu" $menuString 

  # if the user selected a vga-configuration
  if (( $selection > 0 )) && (( $selection < $vgaConfigNr )); then

    ShowProcessingDataWindow "VGA Configuration"

    # get the vga-string which belongs to the selected number
    newVideoMode=`./show_video_mode by-index $selection`

    # if a new vga-configuration was selected - change it
    if [ "$videoMode" != "$newVideoMode" ]; then
      newVideoMode=`ReplaceSpaces $newVideoMode`
      ./change_video_mode video-string=$newVideoMode
      ShowLastError
    fi
  fi
  IFS=$ifsBackup
}

MainVga
