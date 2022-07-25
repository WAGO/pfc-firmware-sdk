# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainCodesys2
{
    local quit=$FALSE
    eval $(./get_runtime_config --shellvar)
    local webserver=$V2_WEBSERVER_STATE
    local communication=$V2_SERVICE_DISABLED
    local port_no=$V2_SERVICE_PORT_NO
    local auth=$V2_PORT_AUTH

    while [ "$quit" = "$FALSE" ]; do
        # Show menu.
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "CODESYS2 configuration" \
            "0. Back to PLC Runtime Services Menu" \
            "1. Webserver enable/disable ............ $webserver" \
            "2. Communication enable/disable ........ $communication" \
            "3. Communication Port Number ........... $port_no" \
            "4. Port Authentication enable/disable .. $auth"

        case "$selection" in
            0)  # Quit was selected -> end loop and get back to superior menu.
                quit=$TRUE
                ;;

            1)  WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change Webserver State ($webserver)" \
                    "0. Back to Codesys2 Configuration Menu" \
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
                    ./config_runtime cfg-version=2 webserver-state="$newState"
                    webserver=$(./get_runtime_config cfg-version=2 webserver-state)
                    ShowLastError
                fi
                ;;

            2)  WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change Communication Port State ($communication)" \
                    "0. Back to Codesys2 Configuration Menu" \
                    "1. Enable" \
                    "2. Disable"

                # Assign the selected number to the according parameter value.
                case "$selection" in
                    1) local newState="enabled";;
                    2) local newState="disabled";;
                esac

                # If state changed, set it, get actual value, and show possible errors.
                if [ -n "$newState" ] && [ "$state" != "$newState" ]; then
                    ShowProcessingDataWindow "Process Communication Port State"
                    ./config_runtime cfg-version=2 service-state="$newState"
                    communication=$(./get_runtime_config service-state)
                    ShowLastError
                fi
                ;;

            3)  WdialogWrapper "--inputbox" retUnused "$TITLE" "Change Communication Port No" \
                    "Enter a port number greater zero, less than 65536." 5 $port_no 2> temp
                local newPortNo=`cat temp`
                rm temp

                # If new port was given - change it, get the actual value again and show possible errors.
                if [ -n "$newPortNo" ] && [ "$newPortNo" != "$port_no" ]; then
                    ShowProcessingDataWindow "Process Port Number"
                    ./config_runtime cfg-version=2 comm-port=$newPortNo
                    port_no=$(./get_runtime_config comm-port)
                    ShowLastError
                fi
                ;;

            4)  WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Change Port Authentication State ($auth)" \
                    "0. Back to Codesys2 Configuration Menu" \
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
                    ./config_runtime cfg-version=2 authentication="$newState"
                    auth=$(./get_runtime_config cfg-version=2 authentication)
                    ShowLastError
                fi
                ;;
        esac
    done
} # End of MainCodesys2.

MainCodesys2

