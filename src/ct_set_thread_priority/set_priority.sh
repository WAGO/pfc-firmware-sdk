#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

ps -T -A  -o spid,comm |
awk '
BEGIN {
	known_processes="/etc/specific/known_processes.lst"
    process_known[""]=0
    process_prio[""]=0
    process_scheduler[""]=0
    FS=";"
    print "Known processes:\n"
	while ((getline < known_processes) > 0){
        #skip commentary or empty line
        if ((match($0, "#") == 1) || $0 == "" || NF != 3)
        {
            continue
        }        
        name=$1
        prio=$3
        sched=$2
        i=name
        process_known[i]=1
        process_prio[i]=prio
        process_scheduler[i]=sched
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
            prio=process_prio[i]
            sched=process_scheduler[i]
            system("echo process:" process_name " spid:" process_spid " prio: " process_prio[i] " scheduler: " process_scheduler[i] )
            system("chrt -p -" sched " " prio " "  pid)
        }

}
END {

}
'
