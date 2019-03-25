# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

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


    # create a note, if bootp or dhcp is configured for an interface, but got no
    # address by now

    ports=$(/etc/config-tools/get_eth_config --print-ports)

    for port in $ports; do

        local ipAddress=`./get_actual_eth_config $port ip-address`
        local configType=`./get_eth_config $port config-type`

        local waitingText=""
        if [ "$configType" = "bootp" ] && [ -z "$ipAddress" ]; then
            waitingText=", waiting for IP addr. by BootP"
        elif [ "$configType" = "dhcp" ] && [ -z "$ipAddress" ]; then
            waitingText=", waiting for IP addr. by DHCP"
        fi

        local portLabel
        if [[ "$(./get_dsa_mode)" == "0" ]]; then
            # create sth. like X1/X2
            # 
            # sed script: see http://stackoverflow.com/questions/4594319/shell-replace-cr-lf-by-comma
            # The H appends the pattern space to the hold space (saving the current line in the hold space). 
            # The ${...} surrounds actions that apply to the last line only. 
            # Those actions are: x swap hold and pattern space; s/\n/,/g substitute embedded newlines with commas; 
            # s/^,// delete the leading comma (there's a newline at the start of the hold space); and p print.
            
            portLabel=$(xmlstarlet sel -t -v "//ip_settings/port_name" ${NETWORK_INTERFACES_XML} | sed -ne 'H;${x;s+\n+/+g;s+^/++;p;}')
        else
            portLabel=$port
        fi

        local ipAddressString=${ipAddress}
        if [[ "${ipAddressString}" != "" ]]; then
            ipAddressString="${ipAddressString} ($(./get_eth_config $port config-type))"
        fi 

        menu_params_port=("$portLabel"
                          "State........................`./get_actual_eth_config $port state`${waitingText}" 
                          "Mac Address..................`./get_actual_eth_config $port mac-address`" 
                          "IP Address...................${ipAddressString}"
                          "Subnet Mask..................`./get_actual_eth_config $port subnet-mask`"
                          "")

        menu_params=("${menu_params[@]}" "${menu_params_port[@]}")


    done

    ./wdialog "--msgbox" "${TITLE}" "Network Details" "${menu_params[@]}"
}

ShowNetworkDetails
