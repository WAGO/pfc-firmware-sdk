// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysfs_lib.h"


/*********** class SYSFS_Access ***********/
int SYSFS_Access::sysfs_write(const char *file, const char *value) const
{
  int result = 0;
	const int fd = open(file, O_WRONLY);
	if (fd < 0) {
		result = -1;
	}
	else {
	  const size_t length = strlen(value);
	  const int written = write(fd, (const void *)value, length);
	  if ((written < 0) || ((size_t(written) != length))) {
	    result = -1;
	  }
	  if (close(fd) == -1) {
	    /*ignore possible failure*/
	  }
	}
	return result;
}

int SYSFS_Access::sysfs_read(const char *file, char *buf, size_t nbyte) const
{
  int result = 0;
  const int fd = open(file, O_RDONLY);
  if (fd < 0) {
    result = -1;
  }
  else {
    const int bytes_read = read(fd, (void *)buf, nbyte);
    if (bytes_read == -1) {
      result = -1;
    }
    if (close(fd) == -1) {
      /*ignore possible failure*/
    }
  }
  return result;
}

bool SYSFS_Access::sysfs_exist(const char *file) const
{
  bool result;
  if (access(file, F_OK) == 0)
  {
    result = true;
  }
  else
  {
    result = false;
  }
  return result;
}

/*********** class GPIO_Control ***********/
#define SYSFS_GPIO_PATH     "/sys/class/gpio"
int GPIO_Control::gpio_export() const
{
  int result = 0;
	char *gpiostr = nullptr;
	if (asprintf(&gpiostr, "%d", _gpio) == -1) {
	  result = -1;
	}
	else {
	  result = sysfs_write(SYSFS_GPIO_PATH"/export", gpiostr);
	  free(gpiostr);
	}
	return result;
}

bool GPIO_Control::gpio_exported() const
{
  bool result;
  char *filepath = nullptr;
  if (asprintf(&filepath, SYSFS_GPIO_PATH"/gpio%d/value", _gpio) == -1) {
    result = false;
  }
  else
  {
    result = sysfs_exist(filepath);
    free(filepath);
  }
  return result;
}

int GPIO_Control::gpio_unexport() const
{
  int result = 0;
  char *gpiostr = nullptr;
  if (asprintf(&gpiostr, "%d", _gpio) == -1) {
    result = -1;
  }
  else {
    result = sysfs_write(SYSFS_GPIO_PATH"/unexport", gpiostr);
    free(gpiostr);
  }
  return result;
}

int GPIO_Control::gpio_set_direction(const char * direction) const
{
  int result = 0;
  char *filepath = nullptr;
  if (asprintf(&filepath, SYSFS_GPIO_PATH"/gpio%d/direction", _gpio) == -1) {
    result = -1;
  }
  else {
    result = sysfs_write(filepath, direction);
    free(filepath);
  }
  return result;
}

int GPIO_Control::gpio_set_direction_in() const
{
  return gpio_set_direction("in");
}

int GPIO_Control::gpio_set_direction_out() const
{
  return gpio_set_direction("out");
}

int GPIO_Control::gpio_write_value(int value) const
{
  int result = 0;
  char *filepath = nullptr;
  if (asprintf(&filepath, SYSFS_GPIO_PATH"/gpio%d/value", _gpio) == -1) {
    result = -1;
  }
  else {
    if (value == 0)
    {
      result = sysfs_write(filepath, "0");
    }
    else
    {
      result = sysfs_write(filepath, "1");
    }
    free(filepath);
  }
  return result;
}

int GPIO_Control::gpio_read_value(int &value) const
{
  int result = 0;
  char *filepath = nullptr;
  if (asprintf(&filepath, SYSFS_GPIO_PATH"/gpio%d/value", _gpio) == -1) {
    result = -1;
  }
  else {
    char buf[2];
    result = sysfs_read(filepath, buf, 1);
    buf[1] = '\0';
    if (result != -1) {
      value = atoi(buf);
    }
    free(filepath);
  }
  return result;
}


/*********** class MUSB_Control ***********/
#define SYSFS_MUSB_DRIVER_PATH  "/sys/bus/platform/drivers/musb-hdrc"
int MUSB_Control::musb_bind() const
{
  int result = 0;
  char *portstr = nullptr;
  if (asprintf(&portstr, "musb-hdrc.%d.auto", _port) == -1) {
    result = -1;
  }
  else {
    result = sysfs_write(SYSFS_MUSB_DRIVER_PATH"/bind", portstr);
    free(portstr);
  }
  return result;
}

int MUSB_Control::musb_unbind() const
{
  int result = 0;
  char *portstr = nullptr;
  if (asprintf(&portstr, "musb-hdrc.%d.auto", _port) == -1) {
    result = -1;
  }
  else {
    result = sysfs_write(SYSFS_MUSB_DRIVER_PATH"/unbind", portstr);
    free(portstr);
  }
  return result;
}

bool MUSB_Control::musb_bound() const
{
  bool result;
  char *filepath = nullptr;
  if (asprintf(&filepath, "%s/musb-hdrc.%d.auto", SYSFS_MUSB_DRIVER_PATH, _port) == -1) {
    result = false;
  }
  else
  {
    result = sysfs_exist(filepath);
    free(filepath);
  }
  return result;
}
