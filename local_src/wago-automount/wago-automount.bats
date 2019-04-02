#!/usr/bin/env bats

setup() {
    source ./wago-automount

    # create a mock to make is_in_blacklist happy
    function get_partitions_from_same_device {
        echo "a"
        echo "b"
    }
}

function blkid_return_magic_nr_label() {
    echo 'LABEL="7761676F"'
}

function blkid_return_magic_nr_uuid() {
    echo 'UUID="7761-676F"'
}

function blkid_return_no_magic_nr() {
    echo "bla"
}

function udevadm_echo_devname() {
    local device=""

    for arg in "$@"; do
        if [[ "${arg}" == --path=* ]]; then
            device="${arg#--path=}"
        fi
    done

    echo "${device/\/sys\/class\/block/DEVNAME=\/dev}"
}

function blkid_mock_with_real_output(){

    local pattern="${1}"

    local devices=(
		'/dev/mmcblk1p5: LABEL="log_backup" UUID="9d90ba3c-ac3a-4a0e-bff2-552d23192a8d" TYPE="ext4" PARTUUID="af852bbb-05"' \
		'/dev/mmcblk1p1: SEC_TYPE="msdos" LABEL="boot" UUID="7761-676F" TYPE="vfat" PARTUUID="af852bbb-01"' \
		'/dev/mmcblk1p10: LABEL="rootfs.2" UUID="f091228c-0266-4ab0-a5c2-e658133d111a" TYPE="ext4" PARTUUID="af852bbb-0a"' \
		'/dev/mmcblk1p8: LABEL="home1" UUID="5d6e3be7-31fb-4387-a5bd-1948c16634c1" TYPE="ext4" PARTUUID="af852bbb-08"' \
		'/dev/mmcblk1p6: LABEL="settings" UUID="a76f9419-537f-4863-b3c9-39bc6067e448" TYPE="ext4" PARTUUID="af852bbb-06"' \
		'/dev/mmcblk1p7: LABEL="rootfs.1" UUID="f813a81e-f07f-46dd-ab2a-cdfd8dbf0b6a" TYPE="ext4" PARTUUID="af852bbb-07"' \
		'/dev/mmcblk1p9: LABEL="home2" UUID="df0385ee-aa12-4b30-9810-6a590729605a" TYPE="ext4" PARTUUID="af852bbb-09"' \
		'/dev/mmcblk0p1: LABEL="junit" UUID="F083-4ABC" TYPE="vfat" PARTUUID="be321b5e-01"' \
		'/dev/mmcblk1: PTUUID="af852bbb" PTTYPE="dos"' \
		'/dev/mmcblk1p11: PARTUUID="af852bbb-0b"' \
		'/dev/mmcblk0: PTUUID="be321b5e" PTTYPE="dos"' \
        )


    for device in "${devices[@]}"; do
        if [[ "${device}" == ${pattern}:* ]]; then
            echo "${device}"
        fi
    done
}

@test "is not in blacklist (simple blkid mock)" {
    local BLKID=blkid_return_no_magic_nr
    local GREP=grep
    local UDEVADM=udevadm_echo_devname    
        
    [[ "$(is_in_blacklist /dev/sdd)" == "FALSE" ]]

}

@test "is in blacklist via label (simple blkid mock)" {
    local BLKID=blkid_return_magic_nr_label
    local GREP=grep
    local UDEVADM=udevadm_echo_devname    

    [[ "$(is_in_blacklist /dev/sdd)" == "TRUE" ]]
}

@test "is in blacklist via uuid (simple blkid mock)" {
    local BLKID=blkid_return_magic_nr_uuid
    local GREP=grep
    local UDEVADM=udevadm_echo_devname    

    [[ "$(is_in_blacklist /dev/sdd)" == "TRUE" ]]
}

@test "top-level device is not in blacklist (real blkid output)" {
    local BLKID=blkid_mock_with_real_output
    local GREP=grep
    local UDEVADM=udevadm_echo_devname    
        
    [[ "$(is_in_blacklist /dev/mmcblk0 \
     "/sys/class/block/loop0" "/sys/class/block/mmcblk0" "/sys/class/block/mmcblk1" \
     "/sys/class/block/mmcblk1boot0" "/sys/class/block/mmcblk1boot1")" == "FALSE" ]]
}

@test "partition device is not in blacklist (real blkid output)" {
    local BLKID=blkid_mock_with_real_output
    local GREP=grep
    local UDEVADM=udevadm_echo_devname    
        
    [[ "$(is_in_blacklist /dev/mmcblk0p1 \
     "/sys/class/block/loop0" "/sys/class/block/mmcblk0" "/sys/class/block/mmcblk1" \
     "/sys/class/block/mmcblk1boot0" "/sys/class/block/mmcblk1boot1")" == "FALSE" ]]
}

@test "top-level device is in blacklist (real blkid output)" {
    local BLKID=blkid_mock_with_real_output
    local GREP=grep
    local UDEVADM=udevadm_echo_devname    
        
    [[ "$(is_in_blacklist /dev/mmcblk1 \
     "/sys/class/block/loop0" "/sys/class/block/mmcblk0" "/sys/class/block/mmcblk1" \
     "/sys/class/block/mmcblk1boot0" "/sys/class/block/mmcblk1boot1")" == "TRUE" ]]
}

@test "partition device is in blacklist (real blkid output)" {
    local BLKID=blkid_mock_with_real_output
    local GREP=grep
    local UDEVADM=udevadm_echo_devname

    [[ "$(is_in_blacklist /dev/mmcblk1p7 \
        "/sys/class/block/loop0" "/sys/class/block/mmcblk0" "/sys/class/block/mmcblk1" \
        "/sys/class/block/mmcblk1boot0" "/sys/class/block/mmcblk1boot1")" == "TRUE" ]]
}


@test "get parent device for partition" {

    local UDEVADM=udevadm_echo_devname

    run get_parent_device "/dev/mmcblk0p1" \
        "/sys/class/block/loop0" "/sys/class/block/mmcblk0" "/sys/class/block/mmcblk1" \
        "/sys/class/block/mmcblk1boot0" "/sys/class/block/mmcblk1boot1"
    
    echo "output=${output}"

    [[ "$output" == "/dev/mmcblk0" ]]
}

@test "get parent device for disk device" {

    local UDEVADM=udevadm_echo_devname

    run get_parent_device "/dev/mmcblk0" \
        "/sys/class/block/loop0" "/sys/class/block/mmcblk0" "/sys/class/block/mmcblk1" \
        "/sys/class/block/mmcblk1boot0" "/sys/class/block/mmcblk1boot1"
    
    echo "output=${output}"

    [[ "$output" == "/dev/mmcblk0" ]]
}

@test "get parent device for a bogus device" {

    local UDEVADM=udevadm_echo_devname

    run get_parent_device "/dev/dummy" \
        "/sys/class/block/loop0" "/sys/class/block/mmcblk0" "/sys/class/block/mmcblk1" \
        "/sys/class/block/mmcblk1boot0" "/sys/class/block/mmcblk1boot1"
    
    echo "output=${output}"

    [[ "$output" == "/dev/dummy" ]]
}

@test "get parent device for an mmcblk1 partition device" {

    local UDEVADM=udevadm_echo_devname

    run get_parent_device "/dev/mmcblk1p7" \
        "/sys/class/block/loop0" "/sys/class/block/mmcblk0" "/sys/class/block/mmcblk1" \
        "/sys/class/block/mmcblk1boot0" "/sys/class/block/mmcblk1boot1"
    
    echo "output=${output}"

    [[ "$output" == "/dev/mmcblk1" ]]
}


@test "udevadm mock" {

    run udevadm_echo_devname info --query=property --path=/sys/class/block/loop0

    echo "output=$output"

    [[ "$output" == "DEVNAME=/dev/loop0" ]]
}

@test "blkid mock" {

    run blkid_mock_with_real_output /dev/mmcblk0

    echo "output=$output"

    [[ "$output" == '/dev/mmcblk0: PTUUID="be321b5e" PTTYPE="dos"' ]]
}


#@test "get_partitions_from_same_device: disk" {
#    # get the real get_partitions_from_same_device
#    source ./wago-automount
#
#    local BLKID=blkid_return_sample_g2_output
#
#    run get_partitions_from_same_device /dev/mmcblk0
#    echo "output=${output}"
#    [ "$status" -eq "0" ]
#    
#    [[ "$output" == "/dev/mmcblk0 /dev/mmcblk0p1 /dev/mmcblk0p2" ]]
#}
#
#@test "get_partitions_from_same_device: partition" {
#    # get the real get_partitions_from_same_device
#    source ./wago-automount
#
#    local BLKID=blkid_return_sample_g2_output
#    
#    run get_partitions_from_same_device /dev/mmcblk0p1
#    echo "output=${output}"
#    [ "$status" -eq "0" ]
#
#    [[ "$output" == "/dev/mmcblk0 /dev/mmcblk0p1 /dev/mmcblk0p2" ]]
#}
#
#
