# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2020-2022 WAGO GmbH & Co. KG

function ShowNetworkDetails
#
# Show window with current network settings
# (Menu: Main -> Information -> Network Details)
# Values cant't be changed here.
#
# AGa: refactored from ShowCouplerDetails in the original ipcconfig
#
# (Menu: Main -> Information -> Network Details)
#
#
{
    ShowEvaluateDataWindow "Network Details"
    # read a few values first, which we must evaluate
    declare -a menu_params
    declare -a menu_params_port

    local bridge
    for bridge in $(GetBridges); do
        local ipaddr
        local netmask
        local source
        local macaddress
        
        local p
        local parameters=$(GetCurrentIpConfig $bridge)
        for p in $parameters; do
            case "$p" in
                ipaddr=*)
                    ipaddr="${p#ipaddr=}"
                    ValidateParameter ipaddr $ipaddr
                    ;;
                netmask=*)
                    netmask="${p#netmask=}"
                    ValidateParameter netmask $netmask
                    ;;
                source=*)
                    source="${p#source=}"
                    ValidateParameter source $source
                    ;;
            esac
        done
        macaddress=$(GetMacAddress $bridge)

        local note=""
        if [ "$ipaddr" == "0.0.0.0" ]; then
            if [ "$source" = "bootp" ] || [ "$source" = "dhcp" ]; then
                note=", waiting for ip address"
            fi
        fi

        bridge_parameters=(
            "$bridge"
            "Mac Address..................$macaddress"
            "IP Address...................$ipaddr (${source}${note})"
            "Subnet Mask..................$netmask"
            "")

        menu_params=("${menu_params[@]}" "${bridge_parameters[@]}")
    done

		local interface
		for interface in $(GetInterfaces); do
        
        local state
        local autonegotiation
        local speed
        local duplex
        
        local p
        local parameters=$(GetInterfaceConfigOfDevice "$interface" text)
        for p in $parameters; do
            case "$p" in
                state=*)
                    state="${p#state=}"
                    ValidateParameter state $state
                    ;;
                autonegotiation=*)
                    autonegotiation="${p#autonegotiation=}"
                    ValidateParameter autonegotiation $autonegotiation
                    ;;
                speed=*)
                    speed="${p#speed=}"
                    ValidateParameter speed $speed
                    ;;
                duplex=*)
                    duplex="${p#duplex=}"
                    ValidateParameter duplex $duplex
                    ;;
            esac
        done

        port_parameters=(
            "$interface"
            "State........................$state"
            "Autonegotiation..............$autonegotiation"
            "Speed........................$speed"
            "Duplex.......................$duplex"
            "")

        menu_params=("${menu_params[@]}" "${port_parameters[@]}")
    done

    ./wdialog "--msgbox" "${TITLE}" "Network Details" "${menu_params[@]}"
}

ShowNetworkDetails
