# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

ITEM_NAME="General PLC Runtime Configuration"
PARENT=$(GetParentMenuName $BASH_SOURCE)

declare -a plc_runtime_versions=()


function CODESYSConfig
{
  # loop until user wants to go back to main menu
  local quit=$FALSE
  local selection

  while [[ "$quit" = "$FALSE" ]]; do
    ShowEvaluateDataWindow "${ITEM_NAME}"

    local homedirSdcard=$(./get_runtime_config homedir-on-sdcard)

    local codesysVersion="$(/etc/config-tools/get_plc_config)"
    local plcRuntimeLabel=$(GetRuntimeLabel ${codesysVersion})

    # show menu
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "$ITEM_NAME" \
              "0. Back to ${PARENT} Menu" \
              "1. PLC runtime version..........${plcRuntimeLabel}" \
              "2. Home Dir On SD Card..........$homedirSdcard"


    # analyse user selection and jump to the according function
    # don't allow e!RUNTIME when cds3 signature is missing in electronic type label
    case "$selection" in

      0)  quit=$TRUE;;

      1)

        local menuString=""
        local runtimeNr=1

        local ifsBackup=${IFS}
        IFS=$'\t'

        for runtimeId in $(/etc/config-tools/get_possible_runtimes | sed 's/ /\t/g') ; do 
            menuString=`printf "%s$runtimeNr. $(GetRuntimeLabel ${runtimeId})${IFS}" "$menuString"`
            runtimeNr=$(($runtimeNr + 1))
        done

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Change PLC Runtime version (${plcRuntimeLabel})" \
            "0. Back to ${ITEM_NAME} Menu" \
            $menuString

        IFS=${ifsBackup}
        
        local newVersion=
        local newVersionArg=

        if [[ "$selection" > 0 ]]; then
            selection=$[$selection - 1]
            local runtimes=($(/etc/config-tools/get_possible_runtimes))

            newVersionArg=${runtimes[${selection}]}
            newVersion=$(GetRuntimeLabel ${newVersionArg})

        fi


        # if a new state was selected - change it, get actual value again and show possible errors
        if [ -n "$newVersion" ] && [ "$newVersionArg" != "$codesysVersion" ]; then
            ShowProcessingDataWindow "Setting PLC Runtime version..."
            /etc/config-tools/config_runtime runtime-version="$newVersionArg" &> /dev/null
            ShowLastError
            codesysVersion="$(/etc/config-tools/get_runtime_config running-version)"
        fi

        ;;

      2)
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Change Home Dir On SD Card ($homedirSdcard)" \
            "0. Back to PLC Runtime Configuration" \
            "1. Enable" \
            "2. Disable"

        # assign parameter value according to select menu number
        case "$selection" in
            1) local newState="enabled";;
            2) local newState="disabled";;
            *) local newState=""
        esac

        # if state changed, set it, get actual value, and show possible errors
        if [ -n "$newState" ] && [ "$homedirSdcard" != "$newState" ]; then
            ShowProcessingDataWindow "$processMsg"
            /etc/config-tools/config_runtime homedir-on-sdcard=$newState
            homedirSdcard=$(/etc/config-tools/get_runtime_config homedir-on-sdcard)
            ShowLastError
        fi
        ;;

    esac
  done
}

CODESYSConfig
