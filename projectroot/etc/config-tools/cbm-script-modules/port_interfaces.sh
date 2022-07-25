# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

firewall_params_interfaces='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:parameters/f:interfaces/f:interface/@name /etc/firewall/params_gen.xml'

firewall_service_interface_state='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/${service}.xml'


#
# PRIVATE - DO NOT CALL DIRECTLY
#
function FirewallServiceInterfacesStatus
{
    local state=$(`eval echo $firewall_service_interface_state`)

    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change ${interface} interface status for ${name} service" \
        "0. Back to ${name} interface status  Menu" \
        "1. open  - allow traffic" \
        "2. close - block traffic"

    case "$selection" in
        1)
            local new_state="on"
            ;;
        2)
            local new_state="off"
            ;;
        *)
            local new_state=""
            ;;
    esac

    if [ -n "$new_state" ] && [ "$state" != "$new_state" ] ; then
        sh /etc/config-tools/firewall_apply.sh ${service} --set-if ${new_state} ${interface}>/dev/null 2>&1
        ShowLastError
    fi
}

#
# A template menu for firewall settings for selected service.
# Allows to open-close interfaces.
# Param $1 - name of service, related to /etc/firewall/services/*.xml files.
# Param $2 - displayed service name.
# Param $3 - enabled/disabled status of service itself
#
function FirewallServiceInterfaces
{
    local service=$1
    local name=$2
    local service_state=$3

    ShowEvaluateDataWindow "${name} interfaces status"

    declare -a interfaces
    local quit=$FALSE

    while [ "$quit" = "$FALSE" ]; do

        local interfaces=()
        if [ -f "/etc/firewall/params_gen.xml" ] ; then
            local interfaces_list=`${firewall_params_interfaces} | sort | uniq`
        else
            local interfaces_list=""
        fi
        local interfaces_count=0

        for interface in ${interfaces_list}
        do
            local state=$(`eval echo $firewall_service_interface_state`)
            local count=17

            let count-=${#interface}

            interfaces=("${interfaces[@]}" \
                        "$((interfaces_count+1)). $interface $(yes "." | head -n${count} | tr -d " \n\r\v") ${state:-unknown}")
            let interfaces_count+=1
        done


        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "${name} interfaces status" \
            "0. Back to ${name} Port Menu" \
            "${interfaces[@]}"

        case "$selection" in

            0)
                quit=$TRUE;;

            *)
                local interface=""
                local element=-1
                local ix=0
                let element=selection-1

                for interf in ${interfaces_list}
                do
                    if [ "x${ix}" == "x${element}" ] ; then
                        interface=${interf}
                    fi
                    let ix+=1
                done

                if [ -n "${interface}" ] ; then
                    FirewallServiceInterfacesStatus
                fi

                ShowEvaluateDataWindow "${name} interfaces status"
                ;;

        esac
    done
}

