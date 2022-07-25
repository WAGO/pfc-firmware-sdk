#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2019-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#
#-----------------------------------------------------------------------------#
# Script:   docker init script
#
# Brief:    Startscript for dockerd and all associated processes.
#
# Author:   Fabian Schwamborn, WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

DOCKER_IPTABLES_PATH="/opt/wago-docker/sbin/"
EVENTDIR="/etc/config-tools/events/networking"

DOCKER_PIDFILE="/var/run/docker.pid"
DOCKER_IPT_NAMESPACE="WAGO_DOCKER_IPT"

function firewall_hook_cleanup()
{
    if  ip netns list | grep -q "$DOCKER_IPT_NAMESPACE"; then
        echo -n "cleaning up docker namespace..."
        ip netns del "$DOCKER_IPT_NAMESPACE"
    fi
}

function firewall_hook_activate()
{
    firewall_hook_cleanup                                                  #clean up any leftovers
    ip netns add "$DOCKER_IPT_NAMESPACE"                              #create namespace for iptable rule preservation

}

function firewall_hook_inactivate()
{
    firewall_hook_cleanup                                                   #remove docker firewall hook
    test -e $EVENTDIR && run-parts -a config $EVENTDIR                       #clean up firewall

}

function do_docker_start()
{
    echo "configuring network..."
    firewall_hook_activate
    echo -n "starting docker daemon..."
    start-stop-daemon -S --background --quiet --oknodo --pidfile $DOCKER_PIDFILE --exec /usr/bin/env PATH="/"$DOCKER_IPTABLES_PATH":$PATH"  /usr/bin/dockerd --             #manipulate iptables-binary path!
}

function do_docker_stop()
{
    echo "stopping dockerd..."
    start-stop-daemon -K --quiet --oknodo --pidfile $DOCKER_PIDFILE --
    firewall_hook_inactivate
    
}

function usage()
{
    echo "$0 usage: $0 [start|stop|restart]"
}

case $1 in

    start)      
        if test -r $DOCKER_PIDFILE && ps -p `cat $DOCKER_PIDFILE` > /dev/null 2>&1; then
            echo "no action done - docker already running"
            exit 1
        else
            do_docker_start
            echo " done"
        fi
        ;;

	stop)
        do_docker_stop
        echo " done"
        ;;

    restart)    
        do_docker_stop
        sleep 3
        echo -e "\ndocker stopped, starting again ..."
        do_docker_start
        echo " done"
        ;;
    *)
        usage
        ;;
esac
