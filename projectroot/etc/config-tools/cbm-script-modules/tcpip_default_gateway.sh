# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function SelectState
{
    local gwnumber=$1
    local state=$2

    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change Default Gateway $gwnumber State ($state)" \
        "0. Back to Gateway Config Menu" \
        "1. Enable" \
        "2. Disable"

    # Assign the selected number to the according parameter value.
    case "$selection" in
        1) local newState="enabled";;
        2) local newState="disabled";;
        *) local newState=""
    esac

    # If state changed, set it and show possible errors.
    if [ -n "$newState" -a "$state" != "$newState" ]; then
        ShowProcessingDataWindow "Setting Default Gateway State"
        ./config_default_gateway number=$gwnumber state="$newState"
        ShowLastError
    fi
    return 0
}

function SetGateway
{
    local gwnumber=$1
    local ipaddress=$2

    WdialogWrapper "--inputbox" retUnused "$TITLE" "Change Default Gateway $gwnumber IP Address" \
        "Enter/Edit IP Address" 15 $ipaddress 2> temp
    local newIpaddress=$(<temp)
    rm temp

    # If ip address changed, set it and show possible errors.
    if [ "$newIpaddress" -a "$ipaddress" != "$newIpaddress" ]; then
        ShowProcessingDataWindow "Setting Default Gateway IP Address"
        ./config_default_gateway number=$gwnumber value="$newIpaddress"
        ShowLastError
    fi
    return 0
}

function SetMetric
{
    local gwnumber=$1
    local metric=$2

    WdialogWrapper "--inputbox" retUnused "$TITLE" "Change Default Gateway $gwnumber Metric" \
        "Enter/Edit Metric Value" 10 $metric 2> temp
    local newMetric=$(<temp)
    rm temp

    # If ip address changed, set it and show possible errors.
    if [ "$newMetric" -a "$metric" != "$newMetric" ]; then
        ShowProcessingDataWindow "Setting Default Gateway Metric"
        ./config_default_gateway number=$gwnumber metric="$newMetric"
        ShowLastError
    fi
    return 0
}

function GwMenu
{
    local gwnumber=$1
    local selection
    local quit=$FALSE
    local statename
    local ipaddressname
    local metricname

    # show menu
    while [ $quit = $FALSE ]; do
        statename="DEFAULT_GW_STATE_$gwnumber"
        ipaddressname="DEFAULT_GW_VALUE_$gwnumber"
        metricname="DEFAULT_GW_METRIC_$gwnumber"

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "TCP/IP Configuration Default Gateway $gwnumber" \
            "0. Back to Default Gateway Selection Menu" \
            "1. State .................. ${!statename}" \
            "2. Gateway IP Address ..... ${!ipaddressname}" \
            "3. Gateway Metric ......... ${!metricname}"

        case $selection in
            0)
                quit=$TRUE
                return
                ;;
            1)
                SelectState $gwnumber ${!statename}
                ;;
            2)
                SetGateway $gwnumber ${!ipaddressname}
                ;;
            3)
                SetMetric $gwnumber ${!metricname}
                ;;
            *)
                :
        esac
        eval $(./get_default_gateway_config --shell-var)
    done
}

function MainTcpIpDefaultGateway
#
# Processing of Menu Main -> TCP/IP -> Default Gateway
# Show and change the configuration of default-gateway.
#
{
    local quit=$FALSE

    # load config parameters
    eval $(./get_default_gateway_config --shell-var)

    # show menu
    while [ $quit = $FALSE ]; do
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "TCP/IP Default Gateway Selection" \
            "0. Back to TCP/IP Menu" \
            "1. Default gateway 1 .......... ($DEFAULT_GW_STATE_1)" \
            "2. Default gateway 2 .......... ($DEFAULT_GW_STATE_2)"

        case $selection in
            0)
                quit=$TRUE
                return
                ;;
            1)
                GwMenu 1
                ;;
            2)
                GwMenu 2
                ;;
            *)
                return
        esac
    done
}

MainTcpIpDefaultGateway
