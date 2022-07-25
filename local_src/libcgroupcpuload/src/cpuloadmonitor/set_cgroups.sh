#!/bin/bash

#------------------------------------------------------------------------------
# Copyright (c) WAGO GmbH & Co. KG
#
# PROPRIETARY RIGHTS are involved in the subject matter of this material. All
# manufacturing, reproduction, use and sales rights pertaining to this
# subject matter are governed by the license agreement. The recipient of this
# software implicitly accepts the terms of the license.
#------------------------------------------------------------------------------

ps -T -A  -o spid,comm |
awk '
BEGIN {
    known_processes="/etc/specific/known_processes_cgroups.lst"
    process_known[""]=0    
    process_cgroup[""]=0
    FS=";"
    print "Known processes:\n"
	while ((getline < known_processes) > 0){
        #skip commentary or empty line
        if ((match($0, "#") == 1) || $0 == "" || NF != 2)
        {
            continue
        }        
        name=$1
        cgroup=$2
        i=name
        process_known[i]=1
        process_cgroup[i]=cgroup
    }
    close(known_processes)
}
{
        # we have processes with a blank in the name.. thus awks field seperation doesnt work properly
        # output of ps: 1234 process name

        endOfSpid= match($0, "[0-9] ")+1
        process_name=substr($0,endOfSpid+1)
        process_spid=substr($0,1,endOfSpid)
        i=process_name
        if (process_known[i]){
            pid=process_spid
            cgroup=process_cgroup[i]
            system("echo process:" process_name " spid:" process_spid " cgroup: " cgroup)
            system("mkdir -p "  cgroup)
            system("echo" pid ">> "cgroup"/tasks")
        }

}
END {

}
'

sleep 10

cpuloadmonitor.elf -l /,rts,rts/def,rts/iec,rts/iec/profinet,rts/iec/profinet/fifo,rts/iec/can,rts/iec/kbus -o /home/cpu_load.csv
