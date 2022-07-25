//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#ifndef _MACH_PAC_BOOT_TABLE_H
#define _MACH_PAC_BOOT_TABLE_H

struct boot_table_entry {
	u8 id;
	char name[64];
	char cmdline[128];
};

#define PAC_BOOT_DEVICE_MASK   0xc0
#define PAC_BOOT_MODE_MASK     0x3f
#define PAC_BOOT_DEVELOP       (1 << 0) // show all mtd partitions in userspace
#define PAC_BOOT_UART          (1 << 1) // use serial console
#define PAC_BOOT_RECOVER       (1 << 2) // use 2nd root partition
#define PAC_BOOT_UPDATE        (1 << 3) // boot from another root partition once
#define PAC_BOOT_OVERWRITE     (1 << 4) // use custom command line
#define PAC_BOOT_SETUPFW       (1 << 5)
#define PAC_DEVICE_ID_MASK     (3 << 6) // mask device_id (bit 6 and 7)
#define PAC_DEVICE_NAND        (1 << 6)
#define PAC_DEVICE_MMC         (1 << 7)
#define PAC_DEVICE_EMMC        (3 << 6)
#define PAC_BOOT_MMC_INVALBIT_MASK   (PXC_BOOT_UPDATE | PXC_BOOT_RECOVER)

#define BOOT_TABLE_ENTRY(_id, _name, _cmdline) \
	{ .id = _id, .name = _name, .cmdline = _cmdline, }

/*
 * boot_id calculation
 * -------------------
 * The boot_id is an 8-bit value consisting of
 * a device_id and a boot_mode_id:
 *
 * boot_id:  [ 7 6 | 5 4 3 2 1 0 ]
 *              |         |
 *         device_id   boot_mode_id
 *
 * device_id:  0: -          boot_mode_id:  [ 5 4 3 2 1 0 ]
 *             1: nand                        | | | | | |
 *             2: mmc                         | | | | | develop mode enable bit
 *             3: emmc                        | | | | uart enable bit
 *                                            | | | recover mode enable bit
 *                                            | | update
 *                                            | overwrite mode
 *                                            setupfw
 */
#define PAC_CMDLINE_UBI1 "rw ubi.mtd=2 rootfstype=ubifs root=ubi0:ubi_rootfs1"
#define PAC_CMDLINE_UBI2 "rw ubi.mtd=2 rootfstype=ubifs root=ubi0:ubi_rootfs2"
#define PAC_CMDLINE_MMC  "rw root=/dev/mmcblk0p2 rootfstype=ext3 rootwait"
#define PAC_CMDLINE_EMMC1 "rw root=/dev/mmcblk1p2 rootfstype=ext4 rootwait"
#define PAC_CMDLINE_EMMC2 "rw root=/dev/mmcblk1p3 rootfstype=ext4 rootwait"
#define PAC_CMDLINE_UART "console=ttyO0,115200"
struct boot_table_entry pac_boot_table[] = {
	BOOT_TABLE_ENTRY(0x40, "nand-default-0-0"    , PAC_CMDLINE_UBI1),                    /* 0b01.000000 */
	BOOT_TABLE_ENTRY(0x41, "nand-default-0-1"    , PAC_CMDLINE_UBI1),                    /* 0b01.000001 */
	BOOT_TABLE_ENTRY(0x42, "nand-default-1-0"    , PAC_CMDLINE_UBI1" "PAC_CMDLINE_UART), /* 0b01.000010 */
	BOOT_TABLE_ENTRY(0x43, "nand-default-1-1"    , PAC_CMDLINE_UBI1" "PAC_CMDLINE_UART), /* 0b01.000011 */
	BOOT_TABLE_ENTRY(0x44, "nand-recover-0-0"    , PAC_CMDLINE_UBI2),                    /* 0b01.000100 */
	BOOT_TABLE_ENTRY(0x45, "nand-recover-0-1"    , PAC_CMDLINE_UBI2),                    /* 0b01.000101 */
	BOOT_TABLE_ENTRY(0x46, "nand-recover-1-0"    , PAC_CMDLINE_UBI2" "PAC_CMDLINE_UART), /* 0b01.000110 */
	BOOT_TABLE_ENTRY(0x47, "nand-recover-1-1"    , PAC_CMDLINE_UBI2" "PAC_CMDLINE_UART), /* 0b01.000111 */
	BOOT_TABLE_ENTRY(0x48, "nand-default-0-0-upd", PAC_CMDLINE_UBI1),                    /* 0b01.001000 */
	BOOT_TABLE_ENTRY(0x49, "nand-default-0-1-upd", PAC_CMDLINE_UBI1),                    /* 0b01.001001 */
	BOOT_TABLE_ENTRY(0x4a, "nand-default-1-0-upd", PAC_CMDLINE_UBI1" "PAC_CMDLINE_UART), /* 0b01.001010 */
	BOOT_TABLE_ENTRY(0x4b, "nand-default-1-1-upd", PAC_CMDLINE_UBI1" "PAC_CMDLINE_UART), /* 0b01.001011 */
	BOOT_TABLE_ENTRY(0x4c, "nand-recover-0-0-upd", PAC_CMDLINE_UBI2),                    /* 0b01.001100 */
	BOOT_TABLE_ENTRY(0x4d, "nand-recover-0-1-upd", PAC_CMDLINE_UBI2),                    /* 0b01.001101 */
	BOOT_TABLE_ENTRY(0x4e, "nand-recover-1-0-upd", PAC_CMDLINE_UBI2" "PAC_CMDLINE_UART), /* 0b01.001110 */
	BOOT_TABLE_ENTRY(0x4f, "nand-recover-1-1-upd", PAC_CMDLINE_UBI2" "PAC_CMDLINE_UART), /* 0b01.001111 */
	BOOT_TABLE_ENTRY(0x50, "nand-overwrite"      , ""),                                  /* 0b01.010000 */
	BOOT_TABLE_ENTRY(0x80, "mmc-default-0-0"     , PAC_CMDLINE_MMC),                     /* 0b10.000000 */
	BOOT_TABLE_ENTRY(0x81, "mmc-default-0-1"     , PAC_CMDLINE_MMC),                     /* 0b10.000001 */
	BOOT_TABLE_ENTRY(0x82, "mmc-default-1-0"     , PAC_CMDLINE_MMC" "PAC_CMDLINE_UART),  /* 0b10.000010 */
	BOOT_TABLE_ENTRY(0x83, "mmc-default-1-1"     , PAC_CMDLINE_MMC" "PAC_CMDLINE_UART),  /* 0b10.000011 */
	BOOT_TABLE_ENTRY(0x90, "mmc-overwrite"       , ""),                                  /* 0b10.010000 */
	BOOT_TABLE_ENTRY(0xc0, "emmc-default-0-0"    , PAC_CMDLINE_EMMC1),                   /* 0b11.000000 */
	BOOT_TABLE_ENTRY(0xc1, "emmc-default-0-1"    , PAC_CMDLINE_EMMC1),                   /* 0b11.000001 */
	BOOT_TABLE_ENTRY(0xc2, "emmc-default-1-0"    , PAC_CMDLINE_EMMC1" "PAC_CMDLINE_UART),/* 0b11.000010 */
	BOOT_TABLE_ENTRY(0xc3, "emmc-default-1-1"    , PAC_CMDLINE_EMMC1" "PAC_CMDLINE_UART),/* 0b11.000011 */
	BOOT_TABLE_ENTRY(0xc4, "emmc-recover-0-0"    , PAC_CMDLINE_EMMC2),                   /* 0b11.000100 */
	BOOT_TABLE_ENTRY(0xc5, "emmc-recover-0-1"    , PAC_CMDLINE_EMMC2),                   /* 0b11.000101 */
	BOOT_TABLE_ENTRY(0xc6, "emmc-recover-1-0"    , PAC_CMDLINE_EMMC2" "PAC_CMDLINE_UART),/* 0b11.000110 */
	BOOT_TABLE_ENTRY(0xc7, "emmc-recover-1-1"    , PAC_CMDLINE_EMMC2" "PAC_CMDLINE_UART),/* 0b11.000111 */
	BOOT_TABLE_ENTRY(0xc8, "emmc-default-0-0-upd", PAC_CMDLINE_EMMC1),                   /* 0b11.001000 */
	BOOT_TABLE_ENTRY(0xc9, "emmc-default-0-1-upd", PAC_CMDLINE_EMMC1),                   /* 0b11.001001 */
	BOOT_TABLE_ENTRY(0xca, "emmc-default-1-0-upd", PAC_CMDLINE_EMMC1" "PAC_CMDLINE_UART),/* 0b11.001010 */
	BOOT_TABLE_ENTRY(0xcb, "emmc-default-1-1-upd", PAC_CMDLINE_EMMC1" "PAC_CMDLINE_UART),/* 0b11.001011 */
	BOOT_TABLE_ENTRY(0xcc, "emmc-recover-0-0-upd", PAC_CMDLINE_EMMC2),                   /* 0b11.001100 */
	BOOT_TABLE_ENTRY(0xcd, "emmc-recover-0-1-upd", PAC_CMDLINE_EMMC2),                   /* 0b11.001101 */
	BOOT_TABLE_ENTRY(0xce, "emmc-recover-1-0-upd", PAC_CMDLINE_EMMC2" "PAC_CMDLINE_UART),/* 0b11.001110 */
	BOOT_TABLE_ENTRY(0xcf, "emmc-recover-1-1-upd", PAC_CMDLINE_EMMC2" "PAC_CMDLINE_UART),/* 0b11.001111 */
};

#define PAC_DEFAULT_BOOT_MODE 0x2 /* default linux, uart active, develop mode off */
struct boot_table_entry *default_nand_entry = &pac_boot_table[2]; /* 0x42 */
struct boot_table_entry *default_mmc_entry = &pac_boot_table[19]; /* 0x82 */
struct boot_table_entry *default_emmc_entry = &pac_boot_table[24];/* 0xc2 */

#endif /* _MACH_PAC_BOOT_TABLE_H */

