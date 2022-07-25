//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

/*
 * Wago Tooling for PAC platform
 * Author: Heinrich Toews
 *
 */

#ifndef EEPROM_XSECTION_H
#define EEPROM_XSECTION_H

/* Templates from 'barebox/arch/arm/boards/wago-pac200/pac-xload.c' */

#define  OP_XSECTION_NONE        (1 << 0)
#define  OP_XSECTION_HDR         (1 << 1)
#define  OP_XSECTION_CMDLINE     (1 << 2)
#define  OP_XSECTION_QUIET       (1 << 3)
#define  OP_XSECTION_DUMP        (1 << 4)
#define  OP_XSECTION_PRBOOT      (1 << 5)

#define BOOT_MODE_ID_EXT 0x102
#define SD_DISABLE_BIT 1

typedef unsigned char u8;

struct pac_eeprom_xload_section_hdr {
  u8 boot_mode_id;
  u8 cmdline_len;
};

struct pac_eeprom_xload_section {
	struct pac_eeprom_xload_section_hdr hdr;
	u8 cmdline[256];
};

enum pac_eeprom_op {
	PAC_READ,
	PAC_WRITE,
	PAC_UNSET,
};

#ifdef DEBUG
#define pac_debug(format, arg...)	\
	printf("pac-debug: " format , ## arg)
#else
#define pac_debug(format, arg...)       \
	({ if (0) printf("pac-debug: " format , ## arg); 0; })
#endif

#define TO_STRING(x) #x

#endif /* EEPROM_XSECTION_H */
