# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

firewall_params_interfaces='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:parameters/f:interfaces/f:interface/@name /etc/firewall/params_gen.xml'

firewall_uf_rulecount='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v count(/f:firewall/f:ipv4/f:input/f:filter/f:rule) /etc/firewall/iptables/ipcmn.xml'

firewall_uf_rule_state='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@state /etc/firewall/iptables/ipcmn.xml'
firewall_uf_rule_if='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@if /etc/firewall/iptables/ipcmn.xml'
firewall_uf_rule_proto='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@proto /etc/firewall/iptables/ipcmn.xml'
firewall_uf_rule_dport='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@dst_port /etc/firewall/iptables/ipcmn.xml'

firewall_uf_rule='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@state            -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@if               -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@proto            -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@src_ip           -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@src_mask         -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@src_port         -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@dst_ip           -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@dst_mask         -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@dst_port         -o / \
                 -v /f:firewall/f:ipv4/f:input/f:filter/f:rule[${ix}]/@policy                \
                 /etc/firewall/iptables/ipcmn.xml'

# firewall_if_rule_parts='s/^\(\([^\/]*\)\/\)\{${pos}\}.*/\2/'

function FirewallUFElementEditState
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

function FirewallPolicyEdit
{
    WdialogWrapper "--menu" selection \
    "$TITLE" \
    "Change policy rule" \
    "0. Back to Edit Menu" \
    "1. accept" \
    "2. drop"

    case "$selection" in
        1)
            policy="accept"
            ;;
        2)
            policy="drop"
            ;;
        *)
            ;;
    esac
}

function FirewallUFElementEditInterface
{
    local interfaces_list=`${firewall_params_interfaces} | sort | uniq`
    declare -a interfaces

    interfaces=("${interfaces[@]}" "4. X1")
    #if in seperate mode add X2 Interface
    if [[ "$(/etc/config-tools/get_dsa_mode)" == "1" ]]; then
            interfaces=("${interfaces[@]}" "5. X2")
    fi

    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Choose the interface" \
        "0. Back to Edit Menu" \
        "1. Any" \
        "2. VPN" \
        "3. WAN" \
        "${interfaces[@]}"

    case "$selection" in
        1)
            if=""
            ;;
        2)
            if="VPN"
            ;;
        3)
            if="WAN"
            ;;
        4)
            if="X1"
            ;;
        5)
            if="X2"
            ;;
    esac
}

function FirewallUFElementEditProtocol
{
    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Choose protocol" \
        "0. Back to Edit Menu" \
        "1. tcp" \
        "2. udp" \
        "3. tcp&udp"

    case "$selection" in
        1)
            proto="tcp"
            ;;
        2)
            proto="udp"
            ;;
        3)
            proto="tcpudp"
            ;;
        *)
            ;;
    esac
}

function FirewallUFElementEditGeneral
{
    local text=$1
    local field_length=$2
    local variable=$3
    local value=$4

    WdialogWrapper "--inputbox" rval "$TITLE" "Change ${text}" "Enter new ${text}:" ${field_length} ${value} 2> temp
    new_value=`cat temp`
    rm temp

    if [ "x0" == "x${rval}" ] ; then
        eval $variable="'$new_value'"
    fi
}

function FirewallUFElementEdit
{
    local ix=$1
    local rule=$(`eval echo $firewall_uf_rule`)
    declare -a delete_menu

    delete_menu=()

    if [ $ix -lt 1 ] ; then
        local state="off"
        local if=""
        local proto="tcp"
        local sip=""
        local smask=""
        local sport=""
        local dip=""
        local dmask=""
        local dport=""
        local policy="accept"
    else
        local state=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{1\}.*/\2/")
        local if=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{2\}.*/\2/")
        local proto=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{3\}.*/\2/")
        local sip=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{4\}.*/\2/")
        local smask=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{5\}.*/\2/")
        local sport=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{6\}.*/\2/")
        local dip=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{7\}.*/\2/")
        local dmask=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{8\}.*/\2/")
        local dport=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{9\}.*/\2/")
        local policy=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{9\}\([^\/]*\).*/\3/" )

        delete_menu=("${delete_menu[@]}" \
                     "12. delete")
    fi

    local quit=$FALSE
    local rval=0

    while [ "$quit" = "$FALSE" ]; do

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Edit a user filter entry" \
            "0. Back to User Filter Menu (cancel)" \
            "1. Policy.......................${policy}" \
            "2. Source IP address............${sip}" \
            "3. Source netmask...............${smask}" \
            "4. Source port..................${sport}" \
            "5. Destination IP address.......${dip}" \
            "6. Destination netmask..........${dmask}" \
            "7. destination port.............${dport}" \
            "8. protocol.....................${proto/tcpudp/tcp&udp}" \
            "9. interface....................${if}" \
            "10. state.......................${state}" \
            "11. accept" \
            "${delete_menu[@]}"

        case "$selection" in

            0)
                quit=$TRUE;;

            1)
                FirewallPolicyEdit
                ;;

            2)
                FirewallUFElementEditGeneral "Source IP address" 15 sip $sip
                ;;

            3)
                FirewallUFElementEditGeneral "Source netmask" 15 smask $smask
                ;;

            4)
                FirewallUFElementEditGeneral "Source port" 11 sport $sport
                ;;

            5)
                FirewallUFElementEditGeneral "Destination IP address" 15 dip $dip
                ;;

            6)
                FirewallUFElementEditGeneral "Destination netmask" 15 dmask $dmask
                ;;

            7)
                FirewallUFElementEditGeneral "Destination port" 11 dport $dport
                ;;

            8)
                FirewallUFElementEditProtocol
                ;;
            9)
                FirewallUFElementEditInterface
                ;;
            10)
                FirewallUFElementEditState
                ;;

            11)
                if [ $ix -lt 1 ] ; then
                    /etc/config-tools/firewall iptables --add-filter $state ${if:--} ${proto} ${sip:--} ${smask:--} ${sport:--} ${dip:--} ${dmask:--} ${dport:--} ${policy:--} --apply >/dev/null 2>&1
                else
                    /etc/config-tools/firewall iptables --upd-filter $ix $state ${if:--} ${proto} ${sip:--} ${smask:--} ${sport:--} ${dip:--} ${dmask:--} ${dport:--} ${policy:--} --apply >/dev/null 2>&1
                fi
                ShowLastError
                rval=1
                quit=$TRUE
                ;;

            12)
                if [ $ix -gt 0 ] ; then
                    /etc/config-tools/firewall iptables --rem-filter $ix --apply >/dev/null 2>&1
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

function FirewallUFElement
{
    local ix=$1
    local rule=$(`eval echo $firewall_uf_rule`)

    local state=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{1\}.*/\2/")
    local if=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{2\}.*/\2/")
    local proto=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{3\}.*/\2/")
    local sip=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{4\}.*/\2/")
    local sport=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{6\}.*/\2/")
    local dip=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{7\}.*/\2/")
    local dport=$(echo "$rule" | sed -e "s/^\(\([^\/]*\)\/\)\{8\}\([^\/]*\).*/\3/")

    echo "No ${ix}: $state ${if:-any} ${proto/tcpudp/tcp&udp} ${dip:--}:${dport:--} ${sip:--}:${sport:--}"
}

function FirewallUFList
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
        local element=`FirewallUFElement $ix`
        entries=("${entries[@]}" \
                 "$((no+4)). $element")

        let no+=1
        let ix+=1
    done
}

function FirewallUFAdjustIndex
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

function FirewallUserFilter
{
    ShowEvaluateDataWindow "Firewall User Filter"

    local ix=1
    local page_length=5
    local total=`${firewall_uf_rulecount}`
    declare -a entries

    local quit=$FALSE

    FirewallUFList $ix $page_length $total

    while [ "$quit" = "$FALSE" ]; do

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Firewall User Filter Settings - total: ${total}" \
            "0. Back to Firewall Menu" \
            "1. Add new" \
            "2. Previous page" \
            "3. Next page" \
            "${entries[@]}"

        case "$selection" in

            0)
                quit=$TRUE;;

            1)
                FirewallUFElementEdit 0

                if [ "0" != "$?" ] ; then
                    local new_total=`${firewall_uf_rulecount}`
                    if [ "$total" != "$new_total" ] ; then
                        total=$new_total
                        ix=$total
                        FirewallUFAdjustIndex
                    fi
                    FirewallUFList $ix $page_length $total
                fi
                ;;

            2)
                local nix=0
                let nix=$ix-$page_length

                if [ $nix -lt 1 ] ; then
                    nix=1
                fi

                if [ $nix -ne $ix ] ; then
                    ShowEvaluateDataWindow "User Filter"
                    ix=$nix
                    FirewallUFList $ix $page_length $total
                fi
                ;;

            3)
                local nix=0
                let nix=$ix+$page_length

                if [ $nix -lt $((total+1)) ] ; then
                    ShowEvaluateDataWindow "User Filter"
                    ix=$nix
                    FirewallUFList $ix $page_length $total
                fi
                ;;

            *)
                local eix=-1
                let eix=$selection-4
                let eix+=$ix

                FirewallUFElementEdit $eix

                if [ "0" != "$?" ] ; then
                    ShowEvaluateDataWindow "User Filter"
                    total=`${firewall_uf_rulecount}`
                    FirewallUFAdjustIndex
                    FirewallUFList $ix $page_length $total
                fi
                ;;

        esac
    done
}

FirewallUserFilter

