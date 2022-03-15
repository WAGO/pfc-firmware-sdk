# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

firewal_state='/etc/config-tools/firewall firewall --is-enabled'

firewall_params_interfaces='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:parameters/f:interfaces/f:interface/@name /etc/firewall/params_gen.xml'

firewall_echo_policy='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:echo/@policy /etc/firewall/iptables/ipcmn.xml'
firewall_echo_limit='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:echo/@limit /etc/firewall/iptables/ipcmn.xml'
firewall_echo_burst='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:echo/@burst /etc/firewall/iptables/ipcmn.xml'
firewall_echo_bprot='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t --if /f:firewall/f:ipv4/f:echo[@broadcast_protection="off"] -o disabled --else -o enabled /etc/firewall/iptables/ipcmn.xml'

firewall_climits_total='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:climits/@total /etc/firewall/iptables/ipcmn.xml'
firewall_climits_limit='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:climits/@limit /etc/firewall/iptables/ipcmn.xml'
firewall_climits_burst='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:climits/@burst /etc/firewall/iptables/ipcmn.xml'
firewall_climits_tcp='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:climits/@tcp /etc/firewall/iptables/ipcmn.xml'
firewall_climits_udp='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:climits/@udp /etc/firewall/iptables/ipcmn.xml'

firewall_if_state='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:input/f:open/f:interface[@if=\"${interface}\"]/@state /etc/firewall/iptables/ipcmn.xml'

firewall_reqecho_policy='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:echo/f:request[@if=\"${interface}\"]/@policy /etc/firewall/iptables/ipcmn.xml'
firewall_reqecho_limit='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:echo/f:request[@if=\"${interface}\"]/@limit /etc/firewall/iptables/ipcmn.xml'
firewall_reqecho_burst='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:echo/f:request[@if=\"${interface}\"]/@burst /etc/firewall/iptables/ipcmn.xml'

#SERVICES
firewall_service_ftp='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/ftp.xml'
firewall_service_ftps='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/ftps.xml'
firewall_service_http='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/http.xml'
firewall_service_https='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/https.xml'
firewall_service_iocheck='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/iocheck.xml'
firewall_service_codesysr='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/codesysr.xml'
firewall_service_codesysw='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/codesysw.xml'
firewall_service_codesysw='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/codesysw.xml'
firewall_service_ssh='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/ssh.xml'
firewall_service_dhcpd='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/dhcpd.xml'
firewall_service_dns='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/dns.xml'
firewall_service_modbus_tcp='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/modbus_tcp.xml'
firewall_service_modbus_udp='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/modbus_udp.xml'
firewall_service_snmp='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/snmp.xml'
firewall_service_opcua='xmlstarlet sel -N f=http://www.wago.com/security/firewall -T -t -v /f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if=\"${interface}\"]/@state /etc/firewall/services/opcua.xml'

function FirewallMessage
{
    ./wdialog "--msgbox" "$TITLE" "Info" " " "$1" " "
}

function FirewallState
{
    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change general firewall state ($1)" \
        "0. Back to Firewall General Settings Menu" \
        "1. Enable" \
        "2. Disable"

    case "$selection" in
        1)
            local new_state="enable"
            ;;
        2)
            local new_state="disable"
            ;;
        *)
            local new_state=""
            ;;
    esac

    if [ -n "$new_state" ] && [ "$1" != "${new_state}d" ] ; then
        ShowProcessingDataWindow "Changing state of the firewall"
        /etc/config-tools/firewall firewall --${new_state} >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallBPState
{
    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change broadcast protection state ($1)" \
        "0. Back to Firewall General Settings Menu" \
        "1. Enable" \
        "2. Disable"

    case "$selection" in
        1)
            local new_state="enabled"
            local bprot="on"
            ;;
        2)
            local new_state="disabled"
            local bprot="off"
            ;;
        *)
            local new_state=""
            ;;
    esac

    if [ -n "$new_state" ] && [ "$1" != "$new_state" ] ; then
        local policy=`${firewall_echo_policy}`
        local limit=`${firewall_echo_limit}`
        local burst=`${firewall_echo_burst}`
        /etc/config-tools/firewall iptables --set-echo ${policy} ${limit:--} ${burst:--} ${bprot} --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallMaxUDPConn
{
    local udp=`$firewall_climits_udp`

    WdialogWrapper "--inputbox" retUnused "$TITLE" "Change max UDP connections limit" "Enter new max UDP connections limit (per second, 0 to disable):" 6 ${udp%%/*} 2> temp
    new_limit=`cat temp`
    rm temp

    if [ "x-" == "x${new_limit}" ] || [ "x0" == "x${new_limit}" ]; then
        local total=`$firewall_climits_total`
        local limit=`$firewall_climits_limit`
        local burst=`$firewall_climits_burst`
        local tcp=`$firewall_climits_tcp`
        ShowProcessingDataWindow "Max UDP connections limit disabled"
        /etc/config-tools/firewall iptables --set-climits ${total:--} ${limit:--} ${burst:--} ${tcp:--} - --apply >/dev/null 2>&1
        ShowLastError
    elif [ -n "$new_limit" ] && [ "$new_limit" != "${udp%%/*}" ]; then
        local total=`$firewall_climits_total`
        local limit=`$firewall_climits_limit`
        local burst=`$firewall_climits_burst`
        local tcp=`$firewall_climits_tcp`
        ShowProcessingDataWindow "Max UDP connections limit ${new_limit}/second"
        /etc/config-tools/firewall iptables --set-climits ${total:--} ${limit:--} ${burst:--} ${tcp:--} ${new_limit}/second --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallMaxTCPConn
{
    local tcp=`$firewall_climits_tcp`

    WdialogWrapper "--inputbox" retUnused "$TITLE" "Change max TCP connections limit" "Enter new max TCP connections limit (per second, 0 to disable):" 6 ${tcp%%/*} 2> temp
    new_limit=`cat temp`
    rm temp

    if [ "x-" == "x${new_limit}" ] || [ "x0" == "x${new_limit}" ]; then
        local total=`$firewall_climits_total`
        local limit=`$firewall_climits_limit`
        local burst=`$firewall_climits_burst`
        local udp=`$firewall_climits_udp`
        ShowProcessingDataWindow "Max TCP connections limit disabled"
        /etc/config-tools/firewall iptables --set-climits ${total:--} ${limit:--} ${burst:--} - ${udp:--} --apply >/dev/null 2>&1
        ShowLastError
    elif [ -n "$new_limit" ] && [ "$new_limit" != "${tcp%%/*}" ]; then
        local total=`$firewall_climits_total`
        local limit=`$firewall_climits_limit`
        local burst=`$firewall_climits_burst`
        local udp=`$firewall_climits_udp`
        ShowProcessingDataWindow "Max TCP connections limit ${new_limit}/second"
        /etc/config-tools/firewall iptables --set-climits ${total:--} ${limit:--} ${burst:--} ${new_limit}/second ${udp:--} --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallInterfaceState
{
    local interface=${1}
    local state=${2}

    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change interface state ($interface)" \
        "0. Back to ${interface} Interface Settings Menu" \
        "1. Open" \
        "2. Filtered"

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
        if [ "open" == "$new_state" ]; then
            /etc/config-tools/firewall iptables --set-open-if on ${interface} --apply >/dev/null 2>&1
        else
            /etc/config-tools/firewall iptables --rem-open-if ${interface} --apply >/dev/null 2>&1
        fi
        ShowLastError
    fi
}

function FirewallInterfacePolicy
{
    local interface=${1}
    local policy=${2}
    local limit=${3}
    local burst=${4}

    WdialogWrapper "--menu" selection \
        "$TITLE" \
        "Change echo policy for ${interface} interface" \
        "0. Back to ${interface} Interface Settings Menu" \
        "1. accept" \
        "2. drop"

    case "$selection" in
        1)
            local new_policy="accept"
            ;;
        2)
            local new_policy="drop"
            ;;
        *)
            local new_policy="drop"
            ;;
    esac

    if [ -n "$new_policy" ] && [ "$policy" != "$new_policy" ] ; then
        if [ "x" = "x${limit}" ] ; then
            burst=""
        fi
        /etc/config-tools/firewall iptables --set-echo-if ${new_policy} ${interface} ${limit:--} ${burst:--} --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallServiceConf ()
{
    local interface=${1}
    local service=${2}
    
    WdialogWrapper "--menu" selection \
    "$TITLE" \
    "Firewall Service Configuration on ${interface} for ${service}" \
    "on" \
    "off"

    case "$selection" in
        0)
            local new_state="on"
            ;;
        1)
            local new_state="off"
            ;;
    esac

    /etc/config-tools/firewall_apply.sh ${service} --set-if ${new_state} ${interface} >/dev/null 2>&1
    ShowLastError
}

function FirewallInterfaceLimit
{
    local interface=${1}
    local policy=${2}
    local limit=${3}
    local burst=${4}

    WdialogWrapper "--inputbox" rval "$TITLE" "Change ICMP (echo) limit" "Enter new ICMP (echo) limit (per second, 0 to disable):" 6 ${limit%%/*} 2> temp
    new_limit=`cat temp`
    rm temp

    if [ "x0" == "x${rval}" ] ; then
        if [ "x" == "x${new_limit}" ] || [ "x-" == "x${new_limit}" ] || [ "x0" == "x${new_limit}" ] ; then
            new_limit="-"
            burst="-"
        else
            new_limit="${new_limit}/second"
        fi

        ShowProcessingDataWindow "Setting a new ICMP (echo) limit"
        /etc/config-tools/firewall iptables --set-echo-if ${policy} ${interface} ${new_limit:--} ${burst:--} --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallInterfaceBurst
{
    local interface=${1}
    local policy=${2}
    local limit=${3}
    local burst=${4}

    if [ "x" == "x${limit}" ] || [ "x-" == "x${limit}" ] ; then
        FirewallMessage "ICMP Burst can't be set unless limit is set first."
        return
    fi

    WdialogWrapper "--inputbox" rval "$TITLE" "Change ICMP (echo) burst" "Enter new ICMP (echo) burst (0 to disable):" 6 ${burst} 2> temp
    new_burst=`cat temp`
    rm temp

    if [ "x0" == "x${rval}" ] ; then
        if [ "x" == "x${new_burst}" ] || [ "x-" == "x${new_burst}" ] || [ "x0" == "x${new_burst}" ] ; then
            new_burst="-"
        fi

        ShowProcessingDataWindow "Setting a new ICMP (echo) burst"
        /etc/config-tools/firewall iptables --set-echo-if ${policy} ${interface} ${limit:--} ${new_burst:--} --apply >/dev/null 2>&1
        ShowLastError
    fi
}

function FirewallInterface
{
    local interface=${1}
    local interface_label=${2}
    local state=$(`eval echo $firewall_if_state`)
    local policy=$(`eval echo $firewall_reqecho_policy`)
    local limit=$(`eval echo $firewall_reqecho_limit`)
    local burst=$(`eval echo $firewall_reqecho_burst`)

    #SERVICES
    local service_ftp=$(`eval echo $firewall_service_ftp`)
    local service_ftps=$(`eval echo $firewall_service_ftps`)
    local service_http=$(`eval echo $firewall_service_http`)
    local service_https=$(`eval echo $firewall_service_https`)
    local service_iocheck=$(`eval echo $firewall_service_iocheck`)
    local service_codesysr=$(`eval echo $firewall_service_codesysr`)
    local service_codesysw=$(`eval echo $firewall_service_codesysw`)
    local service_ssh=$(`eval echo $firewall_service_ssh`)
    local service_dhcpd=$(`eval echo $firewall_service_dhcpd`)
    local service_dns=$(`eval echo $firewall_service_dns`)
    local service_modbus_tcp=$(`eval echo $firewall_service_modbus_tcp`)
    local service_modbus_udp=$(`eval echo $firewall_service_modbus_udp`)
    local service_snmp=$(`eval echo $firewall_service_snmp`)
    local service_opcua=$(`eval echo $firewall_service_opcua`)

    local quit=$FALSE

    if [ "on" == "$state" ]; then
        local state="open"
    else
        local state="filtered"
    fi

    while [ "$quit" = "$FALSE" ]; do

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "${interface_label} Interface Settings" \
            "0. Back to Firewall General Settings Menu" \
            "1. Interface state.................${state}" \
            "2. ICMP Policy.....................${policy:-drop}" \
            "3. ICMP Limit......................${limit:-disabled}" \
            "4. ICMP Burst......................${burst:-disabled}" \
            "5. FTP.............................${service_ftp}" \
            "6. FTPS............................${service_ftps}" \
            "7. HTTP............................${service_http}" \
            "8. HTTPS...........................${service_https}" \
            "9.I/O-Check.......................${service_iocheck}" \
            "10.PLC Runtime.....................${service_codesysr}" \
            "11.PLC WebVisu (Port 8080).........${service_codesysw}" \
            "12.SSH.............................${service_ssh}" \
            "13.BootP/DHCP......................${service_dhcpd}" \
            "14.DNS.............................${service_dns}" \
            "15.MODBUS TCP......................${service_modbus_tcp}" \
            "16.MODBUS UDP......................${service_modbus_udp}" \
            "17.SNMP............................${service_snmp}" \
            "18.OPC UA..........................${service_opcua}"
       
        case "$selection" in

            0)
                quit=$TRUE;;
            1)
                FirewallInterfaceState ${interface} ${state}
                local state=$(`eval echo $firewall_if_state`)
                if [ "on" == "$state" ]; then
                    local state="open"
                else
                    local state="filtered"
                fi
                ;;
            2)
                FirewallInterfacePolicy ${interface} ${policy:-drop} ${limit:--} ${burst:--}
                local policy=$(`eval echo $firewall_reqecho_policy`)
                ;;
            3)
                FirewallInterfaceLimit ${interface} ${policy:-drop} ${limit:--} ${burst:--}
                local limit=$(`eval echo $firewall_reqecho_limit`)
                local burst=$(`eval echo $firewall_reqecho_burst`)
                ;;
            4)
                FirewallInterfaceBurst ${interface} ${policy:-drop} ${limit:--} ${burst:--}
                local burst=$(`eval echo $firewall_reqecho_burst`)
                ;;
            5) #FTP
                FirewallServiceConf ${interface} "ftp"
                local service_ftp=$(`eval echo $firewall_service_ftp`)
                ;;
            6) #FTPS
                FirewallServiceConf ${interface} "ftps"
                local service_ftps=$(`eval echo $firewall_service_ftps`)
                ;;
            7) #HTTP
                FirewallServiceConf ${interface} "http"
                local service_http=$(`eval echo $firewall_service_http`)
                ;;
            8) #HTTPS
                FirewallServiceConf ${interface} "https"
                local service_https=$(`eval echo $firewall_service_https`)
                ;;
           9) #I/O-Check
                FirewallServiceConf ${interface} "iocheck"
                local service_iocheck=$(`eval echo $firewall_service_iocheck`)
                ;;
           10) #PLC Runtime
                FirewallServiceConf ${interface} "codesysr"
                local service_codesysr=$(`eval echo $firewall_service_codesysr`)
                ;;
           11) #PLC WebVisu
                FirewallServiceConf ${interface} "codesysw"
                local service_codesysw=$(`eval echo $firewall_service_codesysw`)
                ;;
           12) #SSH
                FirewallServiceConf ${interface} "ssh"
                local service_ssh=$(`eval echo $firewall_service_ssh`)
                ;;
           13) #BootP/DHCP
                FirewallServiceConf ${interface} "dhcpd"
                local service_dhcpd=$(`eval echo $firewall_service_dhcpd`)
                ;;
           14) #DNS
                FirewallServiceConf ${interface} "dns"
                local service_dns=$(`eval echo $firewall_service_dns`)
                ;;
           15) #MODBUS TCP
                FirewallServiceConf ${interface} "modbus_tcp"
                local service_modbus_tcp=$(`eval echo $firewall_service_modbus_tcp`)
                ;;
           16) #MODBUS UDP
                FirewallServiceConf ${interface} "modbus_udp"
                local service_modbus_udp=$(`eval echo $firewall_service_modbus_udp`)
                ;;
           17) #SNMP
                FirewallServiceConf ${interface} "snmp"
                local service_snmp=$(`eval echo $firewall_service_snmp`)
                ;;
           18) #OPC UA
                FirewallServiceConf ${interface} "opcua"
                local service_opcua=$(`eval echo $firewall_service_opcua`)
                ;;
            *)
                ;;
        esac

    done
}

function FirewallGeneral
{
    ShowEvaluateDataWindow "Firewall General Settings"

    local fstate=`${firewal_state}`
    local bprot=`${firewall_echo_bprot}`
    local udp=`${firewall_climits_udp}`
    local tcp=`${firewall_climits_tcp}`
    local interfaces_list=`${firewall_params_interfaces} | sort | uniq`
    local interfaces_count=0
    declare -a interfaces
    declare -a interface_labels

    local quit=$FALSE

    local all_ports=$(/etc/config-tools/get_eth_config --print-all-ports)
    local active_ports=$(/etc/config-tools/get_eth_config --print-ports)
    for interf in ${all_ports}
    do
        local interf_label=$interf

        if [[ "$active_ports" != *"$interf"* ]]; then
            interf_label="$interf [separated mode only]"
        fi

        # switched mode: - mark inactive interfaces
        #                - [ugly hack] show label for X1/X2
        if [ "`/etc/config-tools/get_dsa_mode`" == "0" ]; then
            if [[ "$interf" == "X1" ]]; then
                interf_label="X1/X2"
            fi
        fi

        interface_labels=("${interface_labels[@]}" \
                          "$interf_label")

        interfaces=("${interfaces[@]}" \
                    "$((interfaces_count+7)). Interface..........................$interf_label")
        let interfaces_count+=1
    done

    while [ "$quit" = "$FALSE" ]; do

        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Firewall General Settings" \
            "0. Back to Firewall Menu" \
            "1. Firewall enabled entirely..........${fstate}" \
            "2. ICMP echo broadcast protection.....${bprot}" \
            "3. Max UDP connections per second.....${udp:-disabled}" \
            "4. Max TCP connections per second.....${tcp:-disabled}" \
            "5. Interface..........................VPN" \
            "6. Interface..........................WAN" \
            "${interfaces[@]}"

        case "$selection" in

            0)
                quit=$TRUE;;

            1)
                FirewallState ${fstate}
                local fstate=`${firewal_state}`
                ;;

            2)
                FirewallBPState ${bprot}
                local bprot=`${firewall_echo_bprot}`
                ;;

            3)
                FirewallMaxUDPConn
                local udp=`${firewall_climits_udp}`
                ;;

            4)
                FirewallMaxTCPConn
                local tcp=`${firewall_climits_tcp}`
                ;;

            5)
                FirewallInterface "VPN" "VPN"
                ;;
            6)
                FirewallInterface "WAN" "WAN"
                ;;
            *)
                local element=-1
                local ix=1
                local element_name=""
                let element=selection-6

                for interf in ${all_ports}
                do
                    if [ "x${ix}" == "x${element}" ] ; then
                        if [[ "${interf}" == X1 ]] ; then
                            element_name=br0
                        elif [[ "${interf}" == X2 ]] ; then
                            element_name=br1
                        else
                            element_name=${interf}
                        fi
                        element_label=${interface_labels[$ix-1]}
                    fi
                    let ix+=1
                done

                if [ -n "${element_name}" ] ; then
                    FirewallInterface ${element_name} ${element_label}
                fi
                ;;

        esac
    done
}

FirewallGeneral

