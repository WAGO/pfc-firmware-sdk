#!/bin/bash
/etc/config-tools/network_config --fix-ip
eventmsg -n omsd 0x80000001
