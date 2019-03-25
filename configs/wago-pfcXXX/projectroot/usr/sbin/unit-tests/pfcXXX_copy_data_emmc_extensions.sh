#!/bin/bash

#  Copyright 2018 WAGO Kontakttechnik GmbH & Co. KG
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

source $(cd -P "$(dirname "$0")" && pwd)/../pfcXXX_copy_data_emmc_extensions.sh

G_PART_ALIGNMENT_MB=8

emmc_partition_labels=(   "boot" "rootfs0" "rootfs1" "EXTENDED" "home" "settings" "log_backup" "reserved")
emmc_partition_sizes=(     64     256       256       " "        2048   8          8           " ")
emmc_partition_types=(    "b"    "L"       "L"       "E"        "L"     "L"       "L"          "L")
emmc_partition_fs_types=( "vfat" "ext4"    "ext4"    " "        "ext4" "ext4"     "ext4"       "ext4")
emmc_partition_mount_pts=("boot" "rootfs0" ""        ""         "home" "settings" "log_backup" "")

# Stubs

get_array_as_arg()
{
    local arr=("${!1}")
    echo "${arr[@]}"
}

do_mount()
{
    echo "mount -t $1 $2 $3"
}

variable_parameter_subshell()
{
    local myvar=${1:-"$(echo bla)"}

    echo "${myvar}"
}

mkfs.vfat()
{
    echo "${FUNCNAME[0]} $*"
}

mkfs.ext3()
{
    echo "${FUNCNAME[0]} $*"
}

mkfs.ext4()
{
    echo "${FUNCNAME[0]} $*"
}

tune2fs()
{
    echo "${FUNCNAME[0]} $*"
}

dd()
{
    echo "${FUNCNAME[0]} $*"
}

mkdir()
{
    echo "${FUNCNAME[0]} $*"
}

command_not_found_handle()
{
    echo "$*"
}

###############################################################

test_command_not_found_handle()
{
    assertEquals "dummy_cmd Aetsch" "$(dummy_cmd "Aetsch")"
}

test_generate_partition_config_line_first_is_bootable()
{
    assertEquals "8,64,b,*\n" "$(generate_partition_config_line 8 64 b 0)"
}

test_generate_partition_config_line_second_is_not_bootable()
{
    assertEquals "8,64,b,\n" "$(generate_partition_config_line 8 64 b 1)"
}

test_pass_array_as_argument()
{
    local -a testarray=("abc" "def")
    assertEquals "abc def" "$(get_array_as_arg testarray[@])"

}

test_generate_partition_dev_mmc_device_0()
{
    assertEquals "/dev/mmcblk0p1" "$(generate_partition_dev "/dev/mmcblk0"  0 "mmc")"
}

test_generate_partition_dev_mmc_device_1()
{
    assertEquals "/dev/mmcblk0p2" "$(generate_partition_dev "/dev/mmcblk0"  1 "mmc")"
}

#test_generate_partition_dev_illegal_device_type()
#{
#    assertEquals "/dev/mmcblk0p1" "$(generate_partition_dev "/dev/mmcblk0"  1 "dummy")"
#}
#
#test_generate_partition_dev_empty_device()
#{
#    assertEquals "/dev/mmcblk0p1" "$(generate_partition_dev ""  1 "dummy")"
#}

test_variable_parameter_subshell_param()
{
    assertEquals "foo" "$(variable_parameter_subshell foo)"
}

test_variable_parameter_subshell_no_param()
{
    assertEquals "bla" "$(variable_parameter_subshell)"
}

test_generate_partition_config_sfdisk()
{

    assertEquals "8,64,b,*\n72,256,L,\n328,256,L,\n584, ,E,\n584,2048,L,\n2632,8,L,\n2640,8,L,\n2648, ,L,\n" \
                 "$(generate_partition_config_sfdisk "emmc_partition_sizes[@]" \
                                                     "emmc_partition_types[@]")"

}

test_mount_target_emmc()
{
    mount_pts=("boot" "" "rootfs0")
    fs_types=("vfat" " " "ext4")

    assertEquals "$(printf "mkdir -p /tmp/emmc/boot\nmount -t vfat /dev/mmcblk1p1 /tmp/emmc/boot\nmkdir -p /tmp/emmc/rootfs0\nmount -t ext4 /dev/mmcblk1p3 /tmp/emmc/rootfs0\n")" \
                 "$(mount_target_emmc "mount_pts[@]" \
                                      "fs_types[@]" \
                                      "/dev/mmcblk1")"
}

test_do_mkfs_ext3()
{
    assertEquals "$(printf "mkfs.ext3 -L DUMMY /dev/dummy\ntune2fs /dev/dummy\n")"\
                 "$(do_mkfs "/dev/dummy" "DUMMY" "ext3")"
}

test_do_mkfs_ext4()
{
    assertEquals "$(printf "mkfs.ext4 -L DUMMY /dev/dummy\ntune2fs /dev/dummy\n")"\
                 "$(do_mkfs "/dev/dummy" "DUMMY" "ext4")"
}

test_do_mkfs_vfat()
{
    assertEquals "$(printf "dd if=/dev/zero of=/dev/dummy bs=512 count=1\nmkfs.vfat -n DUMMY /dev/dummy\n")"\
                 "$(do_mkfs "/dev/dummy" "DUMMY" "vfat")"

}

test_do_mkfs_unknown()
{
    do_mkfs "/dev/dummy" "DUMMY" "ext2"
    assertFalse "$?"
}


# load shunit2
. shunit2

