#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG


set_defaults()
{
    eventmsg -n omsd 0x00000007
    /etc/config-tools/settings_factory
}


case $1 in
  set)
      rm /tmp/factory_defaults_done

      set_defaults

      touch /tmp/factory_defaults_done
      ;;
  reboot)
      while [ ! -e  /tmp/factory_defaults_done ]
      do
        sleep 1
      done
      reboot
      ;;
esac
