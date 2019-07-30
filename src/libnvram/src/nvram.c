//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of PTXdist package libnvram.
//
// Copyright (c) 2008-2018 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     nvram.c
///
///  \brief    NVRAM interface for WAGO PFC200
///
///  \author   HJH : WAGO Kontakttechnik GmbH & Co. KG
///  \author   HFS : WAGO Kontakttechnik GmbH & Co. KG
///  \author   LHe : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <libUIO.h>

static uint8_t *nvram_baseaddr;
static unsigned long nvram_size;
static struct uio_info_t *nvram_uio_device;
static char errormsg[128];

static void set_errormsg(char *emsg, int error)
{
    char *msgheader = "libnvram error";
    char *errnomsg = strerror(error);

    size_t const length = snprintf(errormsg, sizeof(errormsg), "%s: %s %s\n", msgheader, emsg, errnomsg);
    if (length > sizeof(errormsg))
    {
      sprintf(errormsg, "%s\n", msgheader);
    }

}

int8_t nvram_init_with_addr(void* map_addr)
{
	/* TODO: Resolve code-duplication here */
	nvram_uio_device = uio_find_devices_by_name("UIO_NVRAM");
	if (!nvram_uio_device) {
		set_errormsg("libnvram error: Find NVRAM UIO device", errno);
		return -ENODEV;
	}

	nvram_baseaddr = uio_mmap_with_addr(map_addr, nvram_uio_device, 0);
	nvram_size = nvram_uio_device->maps[0].size;

	if (nvram_baseaddr == MAP_FAILED) {
		uio_free_info(nvram_uio_device);
		set_errormsg("NVRAM mapping failed", errno);
		return -EIO;
	}

	return 0;	
}

int8_t nvram_init(void)
{
	nvram_uio_device = uio_find_devices_by_name("UIO_NVRAM");
	if (!nvram_uio_device) {
	    set_errormsg("libnvram error: Find NVRAM UIO device", errno);
		return -ENODEV;
	}

	nvram_baseaddr = uio_mmap(nvram_uio_device, 0);
	nvram_size = nvram_uio_device->maps[0].size;

	if (nvram_baseaddr == MAP_FAILED) {
	    uio_free_info(nvram_uio_device);
		set_errormsg("NVRAM mapping failed", errno);
		return -EIO;
	}

	return 0;
}

const char *nvram_get_errmsg(void)
{
    return errormsg;
}

void nvram_close(void)
{
	if (nvram_baseaddr)
		uio_munmap(nvram_baseaddr, nvram_size);
	if (nvram_uio_device)
		uio_free_info(nvram_uio_device);
	nvram_baseaddr = NULL;
	nvram_size = 0;
}

void * nvram_get_mapping(void)
{
	return nvram_baseaddr;
}

unsigned long nvram_get_size(void)
{
	if (!nvram_size) 
	{
		nvram_uio_device = uio_find_devices_by_name("UIO_NVRAM");
		if (!nvram_uio_device) 
		{
			set_errormsg("libnvram error: Find NVRAM UIO device", errno);
			return 0;
		}
		nvram_size = nvram_uio_device->maps[0].size;
		uio_free_info(nvram_uio_device);
	}

	return nvram_size;
}

void nvram_erase(void)
{
  memset(nvram_baseaddr, 0, nvram_size);
}

