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
 *  CHANGELOG:
 *
 *  v0.1: initial version
 *  v0.2: adapted to boot mode id handling in xloader/barebox
 *  v0.3: replaced the possibility to explicitely set the default root partition
 *        with the "boot other"/"finalize" logic.
 *  v0.4: added special mode for boot_once flag: by calling eeprom-xsection -W -o first
 *        the flag is set or reset in such a way that we use from 1st root partition
 *        after next reboot.
 *  v0.5: Added support for HW03 EMMC.
 *  V0.6: M. Laschinsky: deactivated eeprom boot-options -> obsolete (use rauc instead)
 *  V0.7: O. Ganin: added printing of current state of internal boot mode (enabled/disabled)
 *
*/

#define EEPROM_XSECTION_VERSION        "0.7"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

 /* #define DEBUG */


#include "eeprom-xsection.h"
#include "pac_boot_table.h"
#include "sysfs_gpiolib.h"

static char *default_eeprom_device = "/dev/eeprom";
static char *eeprom_device = NULL;

static struct pac_eeprom_xload_section pac_xsection;

#define PAC_EEPROM__SECTION_OFFSET       0x0
#define EEPROM_GPIO__WP_PIN         170

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static int get_boot_id(u8 *boot_id);
static struct boot_table_entry *pac_boot_table_lookup(u8 boot_id);
static int get_sd_boot_disabled();

static int get_boot_id(u8 *boot_id)
{
	u8 tmp_boot_id = 0;
	char boot_mode[5];

	FILE *boot_id_fd = fopen("/sys/class/wago/system/boot_id", "r");
	if (NULL == boot_id_fd)
	{
		return -1;
	}
	// We expect a value of kind 0xff => 4 chars
	if(4 != fread(boot_mode, sizeof(boot_mode[0]), 4, boot_id_fd))
	{
		fclose(boot_id_fd);
		return -1;
	}
	boot_mode[ARRAY_SIZE(boot_mode) - 1] = '\0';
	// Check if boot id is valid
	tmp_boot_id = strtol(boot_mode, NULL, 0);
	if (NULL == pac_boot_table_lookup(tmp_boot_id))
	{
		fclose(boot_id_fd);
		return -1;
	}
	*boot_id = tmp_boot_id;
	fclose(boot_id_fd);
	return 0;
}

//
// Get the current root partition. Use kernel /sys/class... interface
// because the value in EEPROM might have been changed by user.
// Returns 1 or 2 as valid partition numbers or 0 on error.
u8 get_current_root(void)
{
	u8 current_root_nr = 0; // invalid value
	if (0 == get_boot_id(&current_root_nr))
	{
		current_root_nr &= PAC_BOOT_RECOVER;
		current_root_nr = current_root_nr ? 2 : 1;

	}
	return current_root_nr;
}

void eeprom_wp(int protect)
{
	static int exported = 0;

	if (!exported) {
		sysfs_gpio_export(EEPROM_GPIO__WP_PIN);
		exported = 1;
	}
	sysfs_gpio_direction(EEPROM_GPIO__WP_PIN, SYSFS_GPIO_DOUT);
	sysfs_gpio_set(EEPROM_GPIO__WP_PIN, protect ? 1 : 0);

#ifdef DEBUG
	/* just for testing */
	pac_debug("%s: %d.\n", __func__, sysfs_gpio_get_value(EEPROM_GPIO__WP_PIN));
#endif

	if (exported && protect) {
		sysfs_gpio_unexport(EEPROM_GPIO__WP_PIN);
		exported = 0;
	}
}

static struct boot_table_entry *pac_boot_table_lookup(u8 boot_id)
{
	struct boot_table_entry *entry = NULL;
	int i;

	for (i = 0; i < ARRAY_SIZE(pac_boot_table); i++) {
		if (boot_id == pac_boot_table[i].id) {
			entry = &pac_boot_table[i];
			break;
		}
	}

	return entry;
}

#define STR_YES_NO(x) (x) ? "yes" : "no"
#define STR_ENABLED_DISABLED(x) (x) ? "enabled" : "disabled"
#define STR_ROOTFS1(x) (x) ? "ubi0:ubi_rootfs1" : "/dev/mmcblk1p2"
#define STR_ROOTFS2(x) (x) ? "ubi0:ubi_rootfs2" : "/dev/mmcblk1p3"
#define STR_ROOTFS_MEM(x) (x) ? "NAND" : "EMMC"

// Read all flags in EEPROM and get the current root partition from user.
// The RECOVER flag in EEPROM can be changed by user so that we have to
// get the real boot-time root partition from the kernel.
void dump_boot_mode(u8 boot_mode_id){
  
  u8 overwrite_auto_cmdline    = boot_mode_id & PAC_BOOT_OVERWRITE;
  u8 nand_use_other_root_once  = boot_mode_id & PAC_BOOT_UPDATE; 
  u8 nand_root_part_recovery   = boot_mode_id & PAC_BOOT_RECOVER;
  u8 kernel_use_com_port       = boot_mode_id & PAC_BOOT_UART;
  u8 enable_all_mtd_devices    = boot_mode_id & PAC_BOOT_DEVELOP;
  u8 booting_from_nand = 0;
  u8 booting_from_emmc = 0;
  u8 boot_id = 0;

  if (0 == get_boot_id(&boot_id))
  {
	  int device_id = boot_id & PAC_DEVICE_ID_MASK;
	  booting_from_nand = (device_id == PAC_DEVICE_NAND);
	  booting_from_emmc = (device_id == PAC_DEVICE_EMMC);
  }

  const char *partLabelCurrent;
  const char *partLabelDefault;
  const char *partLabelNext;

  // First determine the name of the default root partition
  if(nand_root_part_recovery) 
  {
    partLabelDefault = STR_ROOTFS2(booting_from_nand);
  }
  else
  {
    partLabelDefault = STR_ROOTFS1(booting_from_nand);
  }

  u8 nand_root_part_current = 0;

  // get_current_root() makes no sense when booting from SD card
  if(booting_from_nand || booting_from_emmc) {

    // Determine which partition we did boot from.
    // This value is taken from the kernel
    nand_root_part_current = get_current_root(); 
  }
  else
  {
    // SD card: current root equivalent to value stored in EEPROM
    nand_root_part_current = (nand_root_part_recovery ? 2 : 1);
  }

  if(nand_root_part_current > 0 && nand_root_part_current < 3)
  {
    if(1 == nand_root_part_current)
    {
      partLabelCurrent = STR_ROOTFS1(booting_from_nand);
      // first assume use_other_root_once == 1
      partLabelNext    = STR_ROOTFS2(booting_from_nand);
    }
    else // part_current == 2
    {
      partLabelCurrent = STR_ROOTFS2(booting_from_nand);
      // first assume use_other_root_once == 1
      partLabelNext    = STR_ROOTFS1(booting_from_nand);
    }

    if(0 == nand_use_other_root_once)
    {
      // If use_other_root is not set, the next boot partition is determined by
      // the value stored in the EEPROM (PAC_BOOT_RECOVER) 
      partLabelNext = partLabelDefault;
    }
  }
  else  // invalid value in /sys/class/wago/...
  {
    partLabelCurrent = "unknown";
    partLabelNext    = "unknown";
  }

  printf("Boot mode:                     0x%02x\n", boot_mode_id);
  printf("Boot id:                       0x%02x\n\n", boot_id);

  //TODO: sync ubifs labels with bootloader
  printf("%s: Default root partition is %s\n",
	 STR_ROOTFS_MEM(booting_from_nand), partLabelDefault);

  if(booting_from_nand || booting_from_emmc) {
    printf("%s: Current root partition is %s\n",
	   STR_ROOTFS_MEM(booting_from_nand), partLabelCurrent);
  }
  printf("%s: Next time will boot from  %s\n",
  STR_ROOTFS_MEM(booting_from_nand), partLabelNext);
  printf("\n");
  printf("Linux: use serial console:     %s\n", STR_YES_NO(kernel_use_com_port));
  printf("Linux: SD Card boot disabled:  %s\n", STR_YES_NO(get_sd_boot_disabled() == SD_DISABLE_BIT));
  printf("\n     ~~Expert options~~\n");
  printf("Linux: enable all mtd devices: %s\n", STR_YES_NO(enable_all_mtd_devices));
  printf("Linux: overwrite command line: %s\n", STR_YES_NO(overwrite_auto_cmdline));

}

void dump_section(unsigned char xsection_id)
{
	pac_debug("This is %s [%d].\n", __func__, __LINE__);

	if (xsection_id & OP_XSECTION_QUIET ||
	    xsection_id & OP_XSECTION_NONE)
		return;

	if (xsection_id & OP_XSECTION_HDR ||
      xsection_id & OP_XSECTION_PRBOOT) {
		char *buf;
		int size;
		int i;

		buf = (char *) &pac_xsection.hdr;
		size = sizeof(struct pac_eeprom_xload_section_hdr);
		if (xsection_id & OP_XSECTION_DUMP) {
			for (i = 0; i < size; i++)
				printf("%x\n", *(buf + i));
		}
		else if (xsection_id & OP_XSECTION_PRBOOT) {
		  // print state of "internal" boot mode (inverted to SD Card boot mode)
		  printf("%s\n", STR_ENABLED_DISABLED(get_sd_boot_disabled() == SD_DISABLE_BIT));
		}
		else {
      dump_boot_mode(pac_xsection.hdr.boot_mode_id);
		}
	}

	if (xsection_id & OP_XSECTION_CMDLINE) {
		if (!(xsection_id & OP_XSECTION_DUMP))
			printf("EEPROM XSECTION Command Line:\n");
		printf("%s\n", pac_xsection.cmdline);
	}

	pac_debug("This is %s [%d].\n", __func__, __LINE__);
}

int eeprom(enum pac_eeprom_op op, unsigned int offset, u8 *buf, int size)
{
	int fd = 0;
	int ret = 0;

	pac_debug("This is %s [%d].\n", __func__, __LINE__);

	if (!eeprom_device)
		eeprom_device = default_eeprom_device;

	fd = open(eeprom_device, O_RDWR);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	lseek(fd, offset, SEEK_SET);

	switch (op) {
	case PAC_READ:
		while (ret >= 0 && ret != size)
			ret += read(fd, buf + ret, size - ret);
		break;
	case PAC_WRITE:
		eeprom_wp(0);
		while (ret >= 0 && ret != size)
			ret += write(fd, buf + ret, size - ret);
		eeprom_wp(1);
		break;
	default:
		fprintf(stderr, "%s: Unknown eeprom operation!\n", __func__);
		ret = -1;
	}

	close(fd);

	pac_debug("This is %s [%d].\n", __func__, __LINE__);

	return ret;
}

static int get_sd_boot_disabled()
{
	u8 val;
	int r = eeprom(PAC_READ, BOOT_MODE_ID_EXT, &val, sizeof val);
	return r < 0 ? r : val & 1;
}

static int set_sd_boot_disabled(bool set)
{
	u8 val;
	int r = eeprom(PAC_READ, BOOT_MODE_ID_EXT, &val, sizeof val);
	if (r < 0)
		return r;
	val = (val & ~SD_DISABLE_BIT) | set ? SD_DISABLE_BIT : 0;

	return eeprom(PAC_WRITE, BOOT_MODE_ID_EXT, &val, sizeof val);
}

int eeprom_xsection(enum pac_eeprom_op op, unsigned char xsection_id)
{
	u8 *buf = NULL;
	int size = 0, offset = 0;

	if (xsection_id & OP_XSECTION_NONE)
		return -1;

	if (xsection_id & OP_XSECTION_CMDLINE) {
		pac_debug("This is %s [%d].\n", __func__, __LINE__);
		buf = pac_xsection.cmdline;
		offset = sizeof(struct pac_eeprom_xload_section_hdr);
		size += pac_xsection.hdr.cmdline_len ? (pac_xsection.hdr.cmdline_len + 1) : 256; /* with '\0' */
	}

	if (xsection_id & OP_XSECTION_HDR ||
	    xsection_id & OP_XSECTION_PRBOOT) {
		pac_debug("This is %s [%d].\n", __func__, __LINE__);
		buf = (u8*)&pac_xsection.hdr;
		offset = 0;
		size += sizeof(struct pac_eeprom_xload_section_hdr);
	} 

	/* now access eeprom */
	return eeprom(op, PAC_EEPROM__SECTION_OFFSET + offset, buf, size);
}

void print_usage(char *progname)
{
	fprintf(stderr, \
		"Wago EEPROM XSECTION Config Tool Version %s\n\n"  \
		"Usage: %s -R [OPTIONS] | -W [OPTIONS]\n\n"	   \
		"Read Mode Options:\n"				   \
		"            -h:  print xsection [h]eader\n"	   \
		"            -c:  print xsection [c]mdline\n"	   \
		"            -d:  print in [d]ump mode\n"		   \
		"            -b:  print state of internal [b]oot mode (invert boot from SD card mode)\n"
		"Write Mode Options:\n"				   \
		" (obsolete) -F:              [F]inalize boot partition (set current root as default)\n"	   \
		" (obsolete) -o [1/0/first]:  if 1: boot from the currently inactive NAND root partition [o]nce\n"		   \
		"                             if \'first\': boot from first NAND root partition (at least) once\n\n" \
		" (obsolete) -c [1/0]:        if 1: use [c]ustom cmdline (EEPROM/SD card) (on/off) \n"	   \
		"            -s [1/0]:        set linux use [s]erial console usage status (on/off) \n"	   \
		"            -D [1/0]:        disable boot from SD card\n"
    "\n [Expert options]:\n" \
    "            -d [1/0]:        set [d]eveloper mode (enable all mtd devices in linux) (on/off) \n"	   \
		" (obsolete) -t [cmdline]:    specify a cus[t]om command line (max. 255 Bytes)\n"	\
		" (obsolete) -B [mode]:       write [B]oot mode directly as hex value (all other parameters are ignored)\n\n"	\
		"Note: use rauc commands for obsolete options!\n" \
		, EEPROM_XSECTION_VERSION, progname);
}

void xsection_set_bit_value(struct pac_eeprom_xload_section *pac_xsection,
                            u8 *xsection_id,
                            u8 mask, 
                            u8 value, 
                            char *param_name){

  *xsection_id |= OP_XSECTION_HDR;

  switch (value){
  case 0:
    pac_xsection->hdr.boot_mode_id &= ~mask;
    break;
  case 1:
    pac_xsection->hdr.boot_mode_id |= mask;
    break;
  default:
    printf("Illegal value for parameter %s: %d\n", param_name, value);
    exit(EXIT_FAILURE);
    break;
  } 

}

int main(int argc, char **argv)
{
	unsigned char xsection_id = 0;
	enum pac_eeprom_op mode = PAC_UNSET;
	int opt;

	opt = getopt(argc, argv, "WR");
	switch (opt) {
	case 'R':
		pac_debug("This is %s [%d], argc=%d.\n", __func__, __LINE__, argc);
		mode = PAC_READ;
		break;
	case 'W':
		pac_debug("This is %s [%d].\n", __func__, __LINE__);
		mode = PAC_WRITE;
		break;
	default:
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&pac_xsection, 0, sizeof(pac_xsection));

	if (mode == PAC_READ) {
		while ((opt = getopt(argc, argv, "hcdb")) != -1) {
			switch (opt) {
			default: /* '?' */
			case 'h': /* short: only header */
				pac_debug("This is %s [%d].\n", __func__, __LINE__);
				xsection_id |= OP_XSECTION_HDR;
				break;
			case 'c': /* cmdline: only cmdline */
				pac_debug("This is %s [%d].\n", __func__, __LINE__);
				xsection_id |= OP_XSECTION_CMDLINE;
        		break;
			case 'd': /* dump only mode */
				pac_debug("This is %s [%d].\n", __func__, __LINE__);
				xsection_id |= OP_XSECTION_DUMP;
				break;
      case 'b': /* print state of internal boot mode */
        pac_debug("This is %s [%d].\n", __func__, __LINE__);
        xsection_id |= OP_XSECTION_PRBOOT;
        break;
			}
		}
	} 
  else { /* mode = PAC_WRITE */

    /* get header first */
    eeprom_xsection(PAC_READ, OP_XSECTION_HDR);

    u8 new_bootmode = 0;

    while ((opt = getopt(argc, argv, "B:Fo:c:s:d:t:f:qD:")) != -1 && new_bootmode == 0) {
      switch (opt) {
        case 's':
          xsection_set_bit_value( &pac_xsection, &xsection_id, PAC_BOOT_UART, atoi(optarg), "\'-s\'"); 
          break;
        case 'd':
          xsection_set_bit_value( &pac_xsection, &xsection_id, PAC_BOOT_DEVELOP, atoi(optarg), "\'-d\'"); 
          break;
        case 'D':
	  set_sd_boot_disabled(atoi(optarg));
          break;
        case 'q': /* quite mode */
          xsection_id |= OP_XSECTION_QUIET;
          break;

        case 'B':
        case 'c':
        case 'F':
        case 'o':
        case 't':
          fprintf(stderr, "\nError! [%c] is an obsolete option: use rauc commands for options -B, -F and -o!\n\n", opt);
          //fall through to print the usage and exit
        default: /* '?' */
          print_usage(argv[0]);
          exit(EXIT_FAILURE);
      }
    }
  }

  /* now read/write data */
  eeprom_xsection(mode, xsection_id);
  dump_section(xsection_id);

  return EXIT_SUCCESS;
}
