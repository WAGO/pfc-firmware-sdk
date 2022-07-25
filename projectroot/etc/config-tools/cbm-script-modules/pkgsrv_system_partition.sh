# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function get_active_partition_string
{
    local active_partition_index=""
    local active_partition=$(/etc/config-tools/get_filesystem_data active-partition)
    local active_partition_medium=$(/etc/config-tools/get_filesystem_data active-partition-medium)

    case ${active_partition_medium} in
        ${SD_CARD})
            echo "Partition on Memory Card"
            ;;
        ${INTERNAL_FLASH_NAND})
            active_partition_index=$(echo "$active_partition" | sed -n 's/.*dev.*ubi.*\([0-9]\)/\1/p')
            echo "Partition ${active_partition_index} on internal-flash-nand"
            ;;
        ${INTERNAL_FLASH_EMMC})
            active_partition_index=$(echo "$active_partition" | sed -n 's/.*dev.*p\([0-9]\)/\1/p')
            echo "Partition ${active_partition_index} on internal-flash-emmc"
            ;;
        *)
            echo "Unknown"
            ;;
    esac
}

#
# Processing of menu Main -> Package Server -> Start backup system
#
function MainPkgServSystemPartition
{
    local selection
    ./wdialog "--msgbox" \
        "$TITLE" \
        "System Partition" \
        "Changes will take effect when the controller boots from"  \
        "NAND next time." "" \
        "Note: Controller will boot the other NAND system partition" "" \
        "This partition must contain a functional firmware backup!"

    # show menu

    local quit=$FALSE
    
    while [[ "$quit" == "$FALSE" ]]; do
        WdialogWrapper "--menu" selection \
            "$TITLE" \
            "Activate NAND partition" \
            "0. Back to Administration Menu" \
            "1. Current active partition" \
            "2. Set inactive NAND partition active"

        case $selection in
            0)
                quit=$TRUE
                ;;
            1)
                ./wdialog "--msgbox" "$TITLE" "Current Active Partition" "" "$(get_active_partition_string)" "" 
                ;;

            2)
                ./wdialog "--infobox" "$TITLE" "System Partition" " " "Activate inactive internal FLASH partition..." " " 

                ./switch_bootloader 2>&1 > /dev/null
                ShowLastError
                ;;
            *)
                errorText="Error in wdialog"
                quit=$TRUE
                ;;
        esac
    done
}

MainPkgServSystemPartition
