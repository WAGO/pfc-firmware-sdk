#!/bin/bash
eventmsg -n omsd 0x80000001
/etc/config-tools/network_config --fix-ip
