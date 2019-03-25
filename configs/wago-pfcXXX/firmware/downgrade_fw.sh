#!/bin/sh

# get some common print messages
source env/data/print-msg-common

##############################
##### current partitions #####
##############################

PART_MLO_0="/dev/nand0.mlo0"
PART_MLO_1="/dev/nand0.mlo1"
PART_MLO_2="/dev/nand0.mlo2"
PART_MLO_3="/dev/nand0.mlo3"

PART_BOOT_0="/dev/nand0.boot0"
PART_BOOT_1="/dev/nand0.boot1"
PART_BOOT_2="/dev/nand0.boot2"
PART_BOOT_3="/dev/nand0.boot3"

PART_UBI="/dev/nand0.ubidata"

MLO_PARTS="${PART_MLO_0} ${PART_MLO_1} ${PART_MLO_2} ${PART_MLO_3}"
BOOT_PARTS="${PART_BOOT_0} ${PART_BOOT_1} ${PART_BOOT_2} ${PART_BOOT_3}"

###############################
##### recovery partitions #####
###############################

MTD_LINUX1="mtd0_raw_linux1"
MTD_LINUX2="mtd0_raw_linux2"
MTD_DATA="mtd2_fs_data"
MTD_XLOAD="mtd3_raw_xload"
MTD_BAREBOX="mtd4_raw_barebox"
MTD_BAREBOX_ENV="mtd5_raw_barebox-env"
MTD_PRODUCT="mtd6_raw_product"
MTD_RECOVERY="mtd7_raw_recovery"

MTD_PARTS="${MTD_LINUX1} ${MTD_LINUX2} ${MTD_DATA} ${MTD_XLOAD} ${MTD_BAREBOX} ${MTD_BAREBOX_ENV} ${MTD_PRODUCT} ${MTD_RECOVERY}"

RECOVERY_PARTS="512k@0k(${MTD_XLOAD}),1024k@512k(${MTD_BAREBOX}),256k@1536k(${MTD_BAREBOX_ENV}),256k@1792k(${MTD_PRODUCT}),40192k@2048k(${MTD_RECOVERY}),4096k@42240k(${MTD_LINUX1}),4096k@46336k(${MTD_LINUX2}),-@50432k(${MTD_DATA})"

MLO="downgrade.mlo--nand"
BAREBOX="downgrade.barebox.img"
RECOVERY="recovery"

MOUNTPOINT_FAT="/boot"

NAND_HAMMING="/env/data/nand-ecc-hamming"
NAND_BCH8_LEGACY="/env/data/nand-ecc-bch8-legacy"

TIMEOUT=3

WRITE_PROTECTION_SCRIPT="/env/bin/write-protection"
EEPROM_ADDR="/env/data/eeprom-i2c-info"
NEW_INDEX=0x00
BLIDX_ADDR=503 # 0x1F7
DISABLE=0
ENABLE=1

# indicate downgrade mechanism with red-lights
moving_lights -m 20 -c red -t

echo -e ${PRINT_INFO} "starting firmware downgrade..."

# check if mount point is at correct location
if [ ! -e ${MOUNT_FAT} ]; then

	echo -e ${PRINT_ERROR} "mount point ${MOUNTPOINT_FAT} not available"
	exit 1

fi

# check if all recovery files are available
if [ ! -e "${MOUNTPOINT_FAT}/${MLO}" ]; then

	echo -e ${PRINT_ERROR} "recovery file: ${MOUNTPOINT_FAT}/${MLO} not available"
	exit 1

fi

if [ ! -e "${MOUNTPOINT_FAT}/${BAREBOX}" ]; then

	echo -e ${PRINT_ERROR} "recovery file: ${MOUNTPOINT_FAT}/${BAREBOX} not available"
	exit 1

fi

if [ ! -e "${MOUNTPOINT_FAT}/${RECOVERY}" ]; then

	echo -e ${PRINT_ERROR} "recovery file: ${MOUNTPOINT_FAT}/${RECOVERY} not available"
	exit 1

fi

# reset bootloader index in eeprom
echo -e ${PRINT_INFO} "restore bootloader index..."


if [ ! -e ${EEPROM_ADDR} ]; then
	echo "error: ${EEPROM_ADDR} not available"
	exit 1
fi

source ${EEPROM_ADDR}

[ -e ${WRITE_PROTECTION_SCRIPT} ] && ${WRITE_PROTECTION_SCRIPT} ${DISABLE}

i2c_write -a ${eeprom_i2c_addr} -b ${eeprom_i2c_bus} -r ${BLIDX_ADDR} -w ${NEW_INDEX}

[ -e ${WRITE_PROTECTION_SCRIPT} ] && ${WRITE_PROTECTION_SCRIPT} ${ENABLE}


echo -e ${PRINT_INFO} "restore recovery partitions..."

# delete current mlo partitions
for partition in ${MLO_PARTS}; do
	nand -d ${partition}.bb
	delpart ${partition}
done

# delete current boot partitions
for partition in ${BOOT_PARTS}; do
	nand -d ${partition}.bb
	delpart ${partition}
done

# delete current data partition
nand -d ${PART_UBI}.bb
delpart ${PART_UBI}

# restore recovery partitions
mtdparts-add -b -d nand0 -p ${RECOVERY_PARTS}

# do additional check for created recovery partitions
for partition in ${MTD_PARTS}; do
	[ ! -e "/dev/${partition}" ] && echo -e ${PRINT_ERROR} "recovery-partition /dev/${partition} not available" && exit 1
	[ ! -e "/dev/${partition}.bb" ] && echo -e ${PRINT_ERROR} "recovery-partition /dev/${partition}.bb not available" && exit 1
done

# check if all nand ecc-modes are available
[ ! -e "${NAND_HAMMING}" ] && echo -e ${PRINT_ERROR} "ecc-mode ${NAND_HAMMING} not available" && exit 1
[ ! -e "${NAND_BCH8_LEGACY}" ] && echo -e ${PRINT_ERROR} "ecc-mode ${NAND_BCH8_LEGACY} not available" && exit 1

echo ""
echo -e ${PRINT_WARNING} "####################################################################"
echo -e ${PRINT_WARNING} "ATTENTION: DO NOT SWITCH OFF THE DEVICE DURING ERASE AND FLASH STEPS"
echo -e ${PRINT_WARNING} "####################################################################"
echo ""
echo -e -n ${PRINT_WARNING} "PRESS ANY KEY TO ABORT ERASE AND FLASH STEPS: "
timeout -a 3

res=$?
if [ ${res} -eq 1 ]; then

	echo -e ${PRINT_WARNING} "ABORTED BY USER...BOOT TO SHELL"
	exit 1

fi

echo ""
echo -e ${PRINT_WARNING} "erase bootloader partitions..."

# erase all bootloader partitions
erase /dev/${MTD_XLOAD}
erase /dev/${MTD_BAREBOX}.bb

echo -e ${PRINT_WARNING} "flash mlo and barebox..."

# flash mlo
${NAND_HAMMING}
#    -- first  128k
memcpy -s ${MOUNTPOINT_FAT}/${MLO} -d /dev/${MTD_XLOAD} 0x0 0x0
#    -- second 128k
memcpy -s ${MOUNTPOINT_FAT}/${MLO} -d /dev/${MTD_XLOAD} 0x0 0x20000
#    -- third  128k
memcpy -s ${MOUNTPOINT_FAT}/${MLO} -d /dev/${MTD_XLOAD} 0x0 0x40000
#    -- fourth 128k
memcpy -s ${MOUNTPOINT_FAT}/${MLO} -d /dev/${MTD_XLOAD} 0x0 0x60000


# flash barebox
${NAND_BCH8_LEGACY}
memcpy -s ${MOUNTPOINT_FAT}/${BAREBOX} -d /dev/${MTD_BAREBOX}.bb 0x0 0x0

echo -e ${PRINT_WARNING} "flash recovery image..."
erase /dev/${MTD_RECOVERY}.bb 0x0
memcpy -s ${MOUNTPOINT_FAT}/${RECOVERY} -d /dev/${MTD_RECOVERY}.bb 0x0 0x0

echo -e ${PRINT_WARNING} "done..."
echo ""
echo -e ${PRINT_WARNING} "############################################################"
echo -e ${PRINT_WARNING} "ATTENTION: DO NOT SWITCH OFF THE DEVICE DURING FACTORY RESET"
echo -e ${PRINT_WARNING} "############################################################"
echo ""


# setup bootargs
echo -e "\t\e[00;31mFACTORY\e[00m: setup bootargs"

global linux.bootargs.dyn.root="root=/dev/ram0 rdinit=/init.sh"
global linux.bootargs.dyn.ip=""
global linux.bootargs.dyn.macaddr=""
global linux.bootargs.bootmode="pac_boot_id=0x1"
global linux.bootargs.productionmode="production_mode=2"
global linux.bootargs.devconf="devconf=0x0"

# load production-firmware from nand partition and start it
echo -e "\t\e[00;31mFACTORY\e[00m: now boot recovery image in factory-reset-mode"
echo ""
bootm -l -v -f /dev/${MTD_RECOVERY}.bb
