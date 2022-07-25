# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainCodesys3
{
    local quit=$FALSE
    eval $(./get_runtime_config --shellvar)
    local webserver=$V3_WEBSERVER_STATE
    local auth=$V3_PORT_AUTH

    while [ "$quit" = "$FALSE" ]; do
        # Show menu.
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "e!RUNTIME configuration" \
            "0. Back to PLC Runtime Services Menu" \
            "1. Webserver enable/disable ............ $webserver" \
            "2. Port Authentication enable/disable .. $auth"

        case "$selection" in
            0)  # Quit was selected -> end loop and get back to superior menu.
                quit=$TRUE
                ;;

            1)  WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change Webserver State ($webserver)" \
                    "0. Back to e!RUNTIME Configuration Menu" \
                    "1. Enable" \
                    "2. Disable"

                # Assign the selected number to the according parameter value.
                case "$selection" in
                    1) local newState="enabled";;
                    2) local newState="disabled";;
                esac

                # If state changed, set it, get actual value, and show possible errors.
                if [ -n "$newState" ] && [ "$state" != "$newState" ]; then
                    ShowProcessingDataWindow "Process Webserver State"
                    ./config_runtime cfg-version=3 webserver-state="$newState"
                    webserver=$(./get_runtime_config cfg-version=3 webserver-state)
                    ShowLastError
                fi
                ;;

            2)  WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change Port Authentication State ($auth)" \
                    "0. Back to e!RUNTIME Configuration Menu" \
                    "1. Enable" \
                    "2. Disable"

                # Assign the selected number to the according parameter value.
                case "$selection" in
                    1) local newState="enabled";;
                    2) local newState="disabled";;
                esac

                # If state changed, set it, get actual value, and show possible errors.
                if [ -n "$newState" ] && [ "$auth" != "$newState" ]; then
                    ShowProcessingDataWindow "Process Port Authentication State"
                    ./config_runtime cfg-version=3 authentication="$newState"
                    auth=$(./get_runtime_config cfg-version=3 authentication)
                    ShowLastError
                fi
                ;;
        esac
    done
} # End of MainCodesys3.

MainCodesys3

