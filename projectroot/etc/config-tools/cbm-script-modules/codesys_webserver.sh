# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

PARENT_MENUNAME=$(GetParentMenuName $BASH_SOURCE)

function MainWebserver
#
# Processing of menu Main -> CODESYS -> Webvisu
# Show the menu to change the default webserver
#
{
  # loop until user wants to get back to main menu
  local quit=$FALSE
  local selection
  local redraw="yes"

  while [ "$quit" = "$FALSE" ]; do

    local defaultWebserver=`./get_runtime_config default-webpage`

    # fix display (WAT8299)
    local _defaultWebserver
    case $defaultWebserver in
      Webvisu)
        _defaultWebserver=WebVisu;;
      WBM)
        _defaultWebserver="Web-based Management";;
    esac

    if [[ "$redraw" == "yes" ]]; then
        eval $(./get_runtime_config --shellvar)
    fi
#              "1. CODESYS 2 Webserver State [read-only]....${V2_WEBSERVER_STATE}" \
#              "2. e!RUNTIME Webserver State [read-only]....${V3_WEBSERVER_STATE}" \

    local webserverStatus=""
    local menuLineNr=1

    local availableRuntimes=$(/etc/config-tools/get_possible_runtimes)

    ifsBackup=$IFS
    IFS=$'\t'

    if [[ "${availableRuntimes}" =~ 2 ]]; then
        webserverStatus=$(printf "%s$menuLineNr. CODESYS 2 Webserver State [read-only]....${V2_WEBSERVER_STATE}${IFS}" "$webserverStatus")
        menuLineNr=$[$menuLineNr+1]
    fi
    if [[ "${availableRuntimes}" =~ 3 ]]; then
        webserverStatus=$(printf "%s$menuLineNr. e!RUNTIME Webserver State [read-only]....${V3_WEBSERVER_STATE}${IFS}" "$webserverStatus")
        menuLineNr=$[$menuLineNr+1]
    fi

    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "Webserver Configuration" \
              "0. Back to ${PARENT_MENUNAME} Menu" \
              ${webserverStatus} \
              "$menuLineNr. Default Webserver........................$_defaultWebserver"
    
    IFS=$ifsBackup

    # analyse user selection and jump to the according function
    case "$selection" in
    
      0)  quit=$TRUE;;
      ${menuLineNr})  # default webserver was selected -> show selection menu to get the new value for default webserver
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Default Webserver ($_defaultWebserver)" \
                    "0. Back to Webserver Configuration Menu" \
                    "1. Web-based Management" \
                    "2. CODESYS WebVisu"

          # assign the selected number to the according values
          case "$selection" in
            1) local newDefaultWebserver="WBM";;
            2) local newDefaultWebserver="Webvisu";;
            *) local newDefaultWebserver="";;
          esac

          # if a new value was selected, call the config-script and show possible error
          if [ -n "$newDefaultWebserver" ] && [ "$newDefaultWebserver" != "$defaultWebserver" ]; then
            ShowProcessingDataWindow "Change default webserver"
            ./config_runtime default-webpage=$newDefaultWebserver
            ShowLastError
          fi
          redraw="yes"
          ;;

      *)  if [[ "$selection" -lt "$menuLineNr" ]]; then
              redraw="no"
          else
              errorText="Error in wdialog"
              quit=TRUE
          fi
          ;;
    esac
  done
}

MainWebserver
