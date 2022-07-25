// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef SYSFS_LIB_H
#define SYSFS_LIB_H

class SYSFS_Access
{
  public:
    int sysfs_write(const char *file, const char *value) const;
    int sysfs_read(const char *file, char *buf, size_t nbyte) const;
    bool sysfs_exist(const char *file) const;
};

class GPIO_Control : public SYSFS_Access
{
  private:
    int const _gpio;
    int gpio_set_direction(const char * direction) const;

  public:
    GPIO_Control(int gpio) :
      _gpio(gpio) {}

    int gpio_export() const;
    bool gpio_exported() const;
    int gpio_unexport() const;
    int gpio_set_direction_in() const;
    int gpio_set_direction_out() const;
    int gpio_write_value(int value) const;
    int gpio_read_value(int &value) const;
};

class MUSB_Control : public SYSFS_Access
{
  private:
    int const _port;

  public:
    MUSB_Control(int port) :
      _port(port) {}

    int musb_bind() const;
    int musb_unbind() const;
    bool musb_bound() const;
};

#endif /* SYSFS_BLIB_H */
