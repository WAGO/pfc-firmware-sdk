//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#ifndef SYSFS_GPIOLIB_H
#define SYSFS_GPIOLIB_H

enum sysfs_gpio_direction {
	SYSFS_GPIO_DOUT,
	SYSFS_GPIO_DIN,
};

#ifdef DEBUG
#define gpio_debug(format, arg...)	\
	printf("gpio_debug: " format , ## arg)
#else
#define gpio_debug(format, arg...)       \
	({ if (0) printf("gpio_debug: " format , ## arg); 0; })
#endif

extern void sysfs_gpio_export(int gpio);
extern void sysfs_gpio_unexport(int gpio);
extern void sysfs_gpio_direction(int gpio, enum sysfs_gpio_direction direction);
extern void sysfs_gpio_set(int gpio, int value);
extern int sysfs_gpio_get_value(int gpio);

#endif /* SYSFS_GPIOLIB_H */
