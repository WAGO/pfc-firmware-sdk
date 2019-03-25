#!/bin/bash
killall -9 udhcpc
killall -9 bootpc-startup
killall -9 bootpc
killall -9 ifplugd
ifconfig br0 192.168.1.17
eventmsg -n omsd 0x80000001
