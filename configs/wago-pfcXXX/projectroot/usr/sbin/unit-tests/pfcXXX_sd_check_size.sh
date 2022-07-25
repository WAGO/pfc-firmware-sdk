#!/bin/bash

#  Copyright 2018-2022 WAGO GmbH & Co. KG
#
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#  THE SOFTWARE.

#quick and dirty: include return values
source $(cd -P "$(dirname "$0")" && pwd)/../../../../../../projectroot/etc/config-tools/config_tool_defines
source $(cd -P "$(dirname "$0")" && pwd)/../$0


G_PART_ALIGNMENT_MB=8

emmc_partition_labels=(   "boot" "rootfs0" "rootfs1" "EXTENDED" "home" "settings" "log_backup" "reserved")
emmc_partition_sizes=(     64     256       256       " "        2048   8          8           " ")
emmc_partition_types=(    "b"    "L"       "L"       "E"        "L"     "L"       "L"          "L")
emmc_partition_fs_types=( "vfat" "ext4"    "ext4"    " "        "ext4" "ext4"     "ext4"       "ext4")
emmc_partition_mount_pts=("boot" "rootfs0" ""        ""         "home" "settings" "log_backup" "")

# Stubs

command_not_found_handle()
{
    echo "$*"
}

du()
{
    echo "${DU_RETURN}"
}

###############################################################


test_main_emmc_enough_space()
{
    DU_RETURN=4 # smallest partition is 8 mb
    main_emmc "emmc_partition_mount_pts[@]" "emmc_partition_sizes[@]"
    assertTrue $?
}

test_main_emmc_not_enough_space()
{
    DU_RETURN=1024
    main_emmc "emmc_partition_mount_pts[@]" "emmc_partition_sizes[@]"
    assertEquals "${NOT_ENOUGH_SPACE_ERROR}" "$?"
}

# load shunit2
. shunit2

