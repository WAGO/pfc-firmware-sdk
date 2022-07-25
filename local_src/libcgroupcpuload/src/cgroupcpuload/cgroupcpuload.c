//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     cgroupcpuload.c
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "cgroupcpuload.h"
#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define NSEC_PER_SEC        ((uint64_t)(1000 * 1000 * 1000))
#define TIMESPEC_TO_U64(ts) ((uint64_t)(ts).tv_nsec+(uint64_t)(ts).tv_sec*NSEC_PER_SEC)
#define DIV_ROUND_UP(n,d)   ((((n) + (d)) - 1U)/ (d))

struct cgroupcpuload
{
  uint64_t timestamp;
  uint64_t runtime;
  int      cpuacct_usage_fd;
};

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
static uint64_t cgroupcpuload_read_runtime(struct cgroupcpuload * cgroupcpuload);


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
DLL_DECL struct cgroupcpuload * cgroupcpuload_init(char const * const cgroup_path)
{
  char path[1024];
  snprintf(path, sizeof(path), "/sys/fs/cgroup/cpuacct/%s/cpuacct.usage", cgroup_path);
  int fd = open(path, O_RDONLY | O_NOATIME);

  if (fd == -1)
  {
    ERROR("open (%s, ..) failed: %s", path, strerror(errno));
    return NULL;
  }
  struct cgroupcpuload *cgroupcpuload = calloc(1, sizeof(*cgroupcpuload));
  cgroupcpuload->cpuacct_usage_fd = fd;
  (void)cgroupcpuload_get_load(cgroupcpuload);

  return cgroupcpuload;
}


//------------------------------------------------------------------------------
DLL_DECL void cgroupcpuload_destroy(struct cgroupcpuload * cgroupcpuload)
{
  if (cgroupcpuload != NULL)
  {
    close(cgroupcpuload->cpuacct_usage_fd);
    free(cgroupcpuload);
  }
}


//------------------------------------------------------------------------------
DLL_DECL uint32_t cgroupcpuload_get_load(struct cgroupcpuload * cgroupcpuload)
{
  struct timespec timespec;
  uint32_t load = 0U;

  if (cgroupcpuload == NULL)
  {
    return 0U;
  }

  uint64_t runtime = cgroupcpuload_read_runtime(cgroupcpuload);

  if (clock_gettime(CLOCK_MONOTONIC, &timespec) != 0)
  {
    return 0U;
  }

  uint64_t delta_runtime = (runtime - cgroupcpuload->runtime);
  uint64_t timestamp     = TIMESPEC_TO_U64(timespec);
  uint64_t delta_time    = (timestamp - cgroupcpuload->timestamp);

  DBG("ts.tv_sec %d, ts.tv_nsec %d", (int) timespec.tv_sec, (int) timespec.tv_nsec);

  DBG("runtime: %"PRIu64 "old_runtime: %"PRIu64" delta_runtime: %"PRIu64"\n"
      "timestamp: %"PRIu64 "old_timestamp: %"PRIu64" delta_time: %"PRIu64"\n",
      runtime, cgroupcpuload->runtime, delta_runtime,
      timestamp, cgroupcpuload->timestamp, delta_time);

  cgroupcpuload->timestamp = timestamp;
  cgroupcpuload->runtime   = runtime;

  if(delta_time != 0U)
  {
    load = (uint32_t) DIV_ROUND_UP(100U * delta_runtime, delta_time);
  }

  INFO("cpu load: %"PRIu32"d%%", load);

  return load;
}


//------------------------------------------------------------------------------
static uint64_t cgroupcpuload_read_runtime(struct cgroupcpuload * cgroupcpuload)
{
  char buffer[64];

  lseek(cgroupcpuload->cpuacct_usage_fd, 0, SEEK_SET);
  ssize_t const length = read(cgroupcpuload->cpuacct_usage_fd, buffer, sizeof(buffer) - 1U);
  if (length > 0)
  {
    buffer[length] = '\0';
    uint64_t result = 0;
    const char * const uint64_format = "%" PRIu64 "\n";
    if (sscanf(buffer, uint64_format, &result) > 0)
    {
      return result;
    }
  }
  return 0;
}



//---- End of source file ------------------------------------------------------
