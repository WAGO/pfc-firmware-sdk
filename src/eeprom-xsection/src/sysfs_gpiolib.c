//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

/* #define DEBUG */

#include "sysfs_gpiolib.h"

#define SYSFS_GPIO_PATH     "/sys/class/gpio"

static int sysfs_gpio_write(const char *file, const char *value)
{
	int fd;
	ssize_t r;

	gpio_debug("%s[%d]: '%s' --> '%s'\n",
		   __func__, __LINE__, value, file);

	fd = open(file, O_WRONLY);
	if (fd < 0) {
		gpio_debug("%s: could not open '%s'!\n", __func__, file);
		return -1;
	}
	r = write(fd, value, strlen(value));
	close(fd);

	gpio_debug("%s[%d]: finished!\n", __func__, __LINE__);
	return r;
}

static int sysfs_gpio_read(const char *file)
{
	int fd;
	char buf[2] = { 0, 0 };
	ssize_t r;

	gpio_debug("%s[%d]: read '%s'\n", __func__, __LINE__, file);

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		gpio_debug("%s: could not open '%s'!\n", __func__, file);
		return -1;
	}
	r = read(fd, buf, 1);
	close(fd);
	gpio_debug("%s[%d]: finished!\n", __func__, __LINE__);

	return r == -1 ? -1 : atoi(buf);
}


/* don't forget to make a free() afterwards! */
static char *gpio_to_string(int gpio)
{
	char *str;
	return asprintf(&str, "%d", gpio) == -1 ? NULL : str;
}

void sysfs_gpio_export(int gpio)
{
	char *gpiostr = gpio_to_string(gpio);
	sysfs_gpio_write(SYSFS_GPIO_PATH"/export", gpiostr);
	free(gpiostr);
}

void sysfs_gpio_unexport(int gpio)
{
	char *gpiostr = gpio_to_string(gpio);
	sysfs_gpio_write(SYSFS_GPIO_PATH"/unexport", gpiostr);
	free(gpiostr);
}

void sysfs_gpio_direction(int gpio, enum sysfs_gpio_direction direction)
{
	char filepath[256];
	char *gpiostr = gpio_to_string(gpio);

	sprintf(filepath, SYSFS_GPIO_PATH"/gpio%s/direction", gpiostr);
	if (direction == SYSFS_GPIO_DOUT)
		sysfs_gpio_write(filepath, "out");
	else
		sysfs_gpio_write(filepath, "in");
	free(gpiostr);
}

void sysfs_gpio_set(int gpio, int value) 
{
	char filepath[256];
	char *gpiostr = gpio_to_string(gpio);

 	sprintf(filepath, SYSFS_GPIO_PATH"/gpio%s/value", gpiostr);
	sysfs_gpio_write(filepath, value ? "1" : "0");
	free(gpiostr);
}

int sysfs_gpio_get_value(int gpio) 
{
	char filepath[256];
	char *gpiostr = gpio_to_string(gpio);

 	sprintf(filepath, SYSFS_GPIO_PATH"/gpio%s/value", gpiostr);
	free(gpiostr);

	return sysfs_gpio_read(filepath);
}
