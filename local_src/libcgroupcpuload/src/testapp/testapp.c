//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include "cgroupcpuload.h"

int main (int argc, char * argv[])
{
    (void)argc;
    (void)argv;

    struct cgroupcpuload *cgroupcpuload_SYS = cgroupcpuload_init("");
    struct cgroupcpuload *cgroupcpuload_RTS = cgroupcpuload_init("rts");
    struct cgroupcpuload *cgroupcpuload_PERF = cgroupcpuload_init("perf");

    uint32_t load_sys = 0U;
    uint32_t load_rts = 0U;
    uint32_t load_perf = 0U;

    //for(int i= 0; i < 20; i++)
    while(1)
    {
      load_sys = cgroupcpuload_get_load(cgroupcpuload_SYS);
      load_rts = cgroupcpuload_get_load(cgroupcpuload_RTS);
      load_rts = cgroupcpuload_get_load(cgroupcpuload_PERF);
      printf("System:%.4d%%  RTS:%.4d%%  PERF:%.4d%%\r", load_sys, load_rts, load_perf);
      fflush(stdout);
      fflush(stdout);
      sleep(1);
    }
    return 0;
}
