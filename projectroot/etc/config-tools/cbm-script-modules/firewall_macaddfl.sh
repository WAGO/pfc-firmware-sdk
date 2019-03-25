# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

firewall_params_interfaces='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:parameters/f:interfaces/f:interface[@ethernet="yes"]/@name /etc/firewall/params_gen.xml'

firewall_mac_mode='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ethernet/@mode /etc/firewall/ebtables/ebwlist.xml'
firewall_mac_ifstate='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ethernet/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/ebtables/ebwlist.xml'

firewall_mac_rulecount='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v count(/f:firewall/f:ethernet/f:whitelist/f:host) /etc/firewall/ebtables/ebwlist.xml'


firewall_mac_global_state='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ethernet/@mode /etc/firewall/ebtables/ebwlist.xml'
firewall_mac_rule_state='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ethernet/f:whitelist/f:host[${ix}]/@state /etc/firewall/ebtables/ebwlist.xml'
firewall_mac_rule_state_on_count='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v count(/f:firewall/f:ethernet/f:whitelist/f:host[@state="on"]) /etc/firewall/ebtables/ebwlist.xml'
firewall_mac_rule_mac='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ethernet/f:whitelist/f:host[${ix}]/@mac /etc/firewall/ebtables/ebwlist.xml'
firewall_mac_rule_mask='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ethernet/f:whitelist/f:host[${ix}]/@mask /etc/firewall/ebtables/ebwlist.xml'


function FirewallWhitelistElementEditState
{
    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change rule state" \
        "0. Back to Edit Menu" \
        "1. on" \
        "2. off"

    case "$selection" in
        1)
            state="on"
            ;;
        2)
            state="off"
            ;;
        *)
            ;;
    esac
}

function FirewallWhitelistElementEditAddress
{
    element=$1

    if [ "mac" == "$element" ] ; then
        name="mac address"
        value=$mac
    else
        name="address mask"
        value=$mask
    fi

    WdialogWrapper "--inputbox" rval "$TITLE" "Change ${name}" "Enter new ${name}:" 17 ${value} 2> temp
    value=`cat temp`
    rm temp

    if [ "x0" == "x${rval}" ] ; then
        if [ "mac" == "$element" ] ; then
            mac=$value
        else
            mask=$value
        fi
    fi
}

function FirewallWhitelistElementEdit
{
    local ix=$1
    declare -a delete_menu

    delete_menu=()

    if [ $ix -lt 1 ] ; then
        local state="off"
        local mac="00:00:00:00:00:00"
        local mask="ff:ff:ff:ff:ff:ff"
    else
        local state=$(`eval echo $firewall_mac_rule_state`)
        local mac=$(`eval echo $firewall_mac_rule_mac`)
        local mask=$(`eval echo $firewall_mac_rule_mask`)
        delete_menu=("${delete_menu[@]}" \
                     "5. delete")

    fi

    local quit=$FALSE
    local rval=0

    while [ "$quit" = "$FALSE" ]; do

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Edit a whitelist entry" \
            "0. Back to Whitelist entry list Menu (cancel)" \
            "1. MAC address.........................${mac}" \
            "2. MAC mask............................${mask:-unused}" \
            "3. Filter state........................${state}" \
            "4. accept" \
            "${delete_menu[@]}"

        case "$selection" in

            0)
                quit=$TRUE;;


            1)
                FirewallWhitelistElementEditAddress "mac"
                ;;

            2)
                FirewallWhitelistElementEditAddress "mask"
                ;;
            3)
                FirewallWhitelistElementEditState
                ;;

            4)
                if [ $ix -lt 1 ] ; then
                    /etc/config-tools/firewall ebtables --add-host $state $mac ${mask:--} --apply >/dev/null 2>&1
                else
                    /etc/config-tools/firewall ebtables --upd-host $ix $state $mac ${mask:--} --apply >/dev/null 2>&1
                fi
                ShowLastError
                rval=1
                quit=$TRUE
                ;;

            5)
                if [ $ix -gt 0 ] ; then
                    /etc/config-tools/firewall ebtables --rem-host $ix --apply >/dev/null 2>&1
                fi
                ShowLastError
                rval=2
                quit=$TRUE
                ;;

            *)
                ;;

        esac
    done

    return $rval
}

function FirewallWhitelistElement
{
    local ix=$1

    local state=$(`eval echo $firewall_mac_rule_state`)
    local mac=$(`eval echo $firewall_mac_rule_mac`)
    local mask=$(`eval echo $firewall_mac_rule_mask`)

    echo "No ${ix}: ${state/on/on } - ${mac} - ${mask:-unused}"
}

function FirewallWhitelistList
{
    local no=0
    local ix=$1
    local length=$2
    local total=$3

    local max_entry_ix=0
    local max_element_ix=0

    let max_entry_ix=$ix+$length
    let max_element_ix=$total+1

    entries=()

    while [ $ix -lt $max_entry_ix ] && [ $ix -lt $max_element_ix ] ; do
        local element=`FirewallWhitelistElement $ix`
        entries=("${entries[@]}" \
                 "$((no+4)). $element")

        let no+=1
        let ix+=1
    done
}

function FirewallWhitelistAdjustIndex
{
    local no=0

    if [ $ix -gt $total ] ; then
        ix=$total
    fi

    if [ $ix -lt 1 ] ; then
        ix=1
    else
        let no=$ix-1
        let no%=$page_length
        let ix-=$no
    fi
}

function FirewallWhitelist
{
    ShowEvaluateDataWindow "MAC Address Filter - Whitelist"

    local total=`${firewall_mac_rulecount}`
    local ix=1
    local page_length=5
    declare -a entries

    local quit=$FALSE

    FirewallWhitelistList $ix $page_length $total

    while [ "$quit" = "$FALSE" ]; do

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Whitelist entry list - total count: ${total}" \
            "0. Back to MAC Address Filter Settings Menu" \
            "1. Add new" \
            "2. Previous page" \
            "3. Next page" \
            "${entries[@]}"

        case "$selection" in

            0)
                quit=$TRUE;;

            1)
                FirewallWhitelistElementEdit 0

                if [ "0" != "$?" ] ; then
                    local new_total=`${firewall_mac_rulecount}`
                    if [ "$total" != "$new_total" ] ; then
                        total=$new_total
                        ix=$total
                        FirewallWhitelistAdjustIndex
                    fi
                    FirewallWhitelistList $ix $page_length $total
                fi

                ;;

            2)
                local nix=0
                let nix=$ix-$page_length

                if [ $nix -lt 1 ] ; then
                    nix=1
                fi

                if [ $nix -ne $ix ] ; then
                    ShowEvaluateDataWindow "MAC Address Filter - Whitelist"
                    ix=$nix
                    FirewallWhitelistList $ix $page_length $total
                fi
                ;;

            3)
                local nix=0
                let nix=$ix+$page_length

                if [ $nix -lt $((total+1)) ] ; then
                    ShowEvaluateDataWindow "MAC Address Filter - Whitelist"
                    ix=$nix
                    FirewallWhitelistList $ix $page_length $total
                fi
                ;;

            *)
                local eix=-1
                let eix=$selection-4
                let eix+=$ix

                FirewallWhitelistElementEdit $eix

                if [ "0" != "$?" ] ; then
                    total=`${firewall_mac_rulecount}`
                    FirewallWhitelistAdjustIndex
                    FirewallWhitelistList $ix $page_length $total
                fi
                ;;

        esac
    done
}

function FirewallWhitelistProtection
{
    local macmode=`${firewall_mac_mode}`

    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change MAC whitelist protection state" \
        "0. Back to MAC Address Filter Settings Menu" \
        "1. enable" \
        "2. disable"

    case "$selection" in
        1)
            local new_macmode="whitelist"
            ;;
        2)
            local new_macmode="all-allow"
            ;;
        *)
            local new_macmode=""
            ;;
    esac

    if [ -n "$new_macmode" ] && [ "$macmode" != "$new_macmode" ] ; then
        /etc/config-tools/firewall ebtables --set-mode ${new_macmode} --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallInterfaceState
{
    local interface=$1
    local state=$(`eval echo $firewall_mac_ifstate`)

    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change state of the ${interface} interface" \
        "0. Back to MAC Address Filter Settings Menu" \
        "1. open" \
        "2. filtered"

    case "$selection" in
        1)
            local new_state="open"
            ;;
        2)
            local new_state="filtered"
            ;;
        *)
            local new_state=""
            ;;
    esac

    if [ -n "$new_state" ] && [ "$state" != "$new_state" ] ; then
        /etc/config-tools/firewall ebtables --set-if ${new_state} ${interface} --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallGlobalMACFilterState
{
    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change general firewall state ($1)" \
        "0. Back to Firewall General Settings Menu" \
        "1. Filtered" \
        "2. Open"

    case "$selection" in
        1)
            local new_state="whitelist"
            ;;
        2)
            local new_state="all-allow"
            ;;
        *)
            local new_state=""
            ;;
    esac

    if [ -n "$new_state" ] && [ "$1" != "${new_state}d" ] ; then
        ShowProcessingDataWindow "Changing state of the firewall"
        /etc/config-tools/firewall ebtables --set-mode ${new_state} --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallMACMessage
{
    ./wdialog "--msgbox" "$TITLE" "Info" " " "$1" " "
}

function FirewallMACFilter
{
    ShowEvaluateDataWindow "MAC Address Filter Settings"

    local macmode=`${firewall_mac_mode}`
    local macmodeed="disabled"
    local interfaces_list=`${firewall_params_interfaces} | sort | uniq`
    local interfaces_count=0

    if [ "`${firewall_mac_global_state}`" == "all-allow" ]; then
        local globalfilterstate="open"
    else
        local globalfilterstate="filtered"
    fi

    declare -a interfaces

    local quit=$FALSE


    while [ "$quit" = "$FALSE" ]; do

        if [ "whitelist" == "$macmode" ] ; then
            local macmodeed="enabled"
        else
            local macmodeed="disabled"
        fi

        interfaces=()
        interfaces_count=0

        case $(/etc/config-tools/get_dsa_mode) in
        0)
            # switched mode:
            #                - mark inactive interfaces
            #                - [ugly hack] show label for X1/X2

            local all_ports=$(/etc/config-tools/get_eth_config --print-all-ports)
            local active_ports=$(/etc/config-tools/get_eth_config --print-ports)
            for interface in ${all_ports}
            do
                local state=$(`eval echo $firewall_mac_ifstate`)
                local count=28

                local interf_label=$interface

                if [[ "$active_ports" != *"$interface"* ]]; then
                    interf_label="$interface [separated mode only]"
                fi

                if [[ $interface == "X1" ]]; then
                    interf_label="X1/X2"
                fi

                let count-=${#interf_label}
                if [[ $count < 0 ]]; then
                    count=0
                fi

                interfaces=("${interfaces[@]}" \
                            "$((interfaces_count+3)). MAC address filter state ${interf_label}$(yes "." | head -n${count} | tr -d " \n\r\v")${state:-filtered}")
                let interfaces_count+=1
            done

            ;;
        1)
            # separated mode

            for interface in ${interfaces_list}
            do
                local state=$(`eval echo $firewall_mac_ifstate`)
                local count=17

                let count-=${#interface}
                    if [ ${interface} != "VPN" ] && [ ${interface} != "WAN" ]; then
                    interfaces=("${interfaces[@]}" \
                                "$((interfaces_count+3)). MAC address filter state ${interface}$(yes "." | head -n${count} | tr -d " \n\r\v")${state:-filtered}")
                    let interfaces_count+=1
                fi
            done
            ;;
        esac

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "MAC Address Filter Settings" \
            "0. Back to Firewall Menu" \
            "1. Global MAC address filter state......................${globalfilterstate}" \
            "2. MAC address filter whitelist" \
            "${interfaces[@]}"
#            "2. MAC whitelist protection.....${macmodeed}" \

        case "$selection" in

            0)
                quit=$TRUE;;
            1)
                FirewallGlobalMACFilterState
                if [ "`${firewall_mac_global_state}`" == "all-allow" ]; then
                        local globalfilterstate="open"
                else
                        local globalfilterstate="filtered"
                fi
                ;;

            2)
                FirewallWhitelist
                ;;

#            2)
#                FirewallWhitelistProtection
#                local macmode=`${firewall_mac_mode}`
#                ;;

            *)
                local element=0
                local ix=0
                local element_name=""
                local rule_cnt=`${firewall_mac_rulecount}`
                local state_on_cnt=`${firewall_mac_rule_state_on_count}`
                ShowLastError
                let element=selection-1

                if [ "$rule_cnt" -gt "0" ] && [ "$state_on_cnt" -gt "0" ]; then
                  for interf in ${interfaces_list}
                  do
                    if [ "x${ix}" == "x${element}" ] ; then
                      element_name=${interf}
                    fi
                    let ix+=1
                  done

                  if [ -n "${element_name}" ] ; then
                    FirewallInterfaceState ${element_name}
                  fi
                else
                  FirewallMACMessage "No rule defined in whitelist! Unable to change filter state."
                fi
                local interfaces_list=`${firewall_params_interfaces} | sort | uniq`
                ;;

        esac
    done
}

FirewallMACFilter

