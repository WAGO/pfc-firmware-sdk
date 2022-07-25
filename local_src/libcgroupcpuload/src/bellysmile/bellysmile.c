//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include "cgroupcpuload.h"

#define INVALID_LOAD 99999
#define SLEEPSEC 5
#define INTERVALCOUNT(x) (x*60/SLEEPSEC)
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1)/ (d))
#define NSEC_PER_SEC ((uint64_t)(1000 * 1000 * 1000))
#define TIMESPEC_TO_U64(ts) ((uint64_t)(ts).tv_nsec+(uint64_t)(ts).tv_sec*NSEC_PER_SEC)

typedef struct
{
    uint64_t timestamp;
    uint64_t usertime;
    uint64_t nicetime;
    uint64_t systemtime;
    uint64_t idletime;
    uint64_t iowaittime;
    uint64_t irqtime;
    uint64_t softirqtime;
    uint64_t oldtimestamp;
    uint64_t oldusertime;
    uint64_t oldnicetime;
    uint64_t oldsystemtime;
    uint64_t oldidletime;
    uint64_t oldiowaittime;
    uint64_t oldirqtime;
    uint64_t oldsoftirqtime;
    uint64_t deltatime;
    int load_fd;
}tProcLoad;

typedef enum {
  ProcLoadUser,
  ProcLoadNice,
  ProcLoadSystem,
  ProcLoadIdle,
  ProcLoadIoWait,
  ProcLoadIrq,
  ProcLoadSoftIrq
}tProcLoadEnum;

typedef struct stLoadRing tLoadRing;

struct stLoadRing
{
    uint32_t load;
    tLoadRing *pNext;
    tLoadRing *pBefore;
};

static tLoadRing * initLoadRing(uint32_t size)
{
  uint32_t i;
  tLoadRing * pRoot = NULL;
  for(i = 0; i < size; i++)
  {
    tLoadRing * pNew = malloc(sizeof(tLoadRing));
    pNew->load = INVALID_LOAD;
    if(pRoot == 0)
    {
      pNew->pNext = pNew;
      pNew->pBefore = pNew;
      pRoot = pNew;
    }
    else
    {
      pRoot->pBefore->pNext = pNew;
      pNew->pNext = pRoot;
      pNew->pBefore = pRoot->pBefore;
      pRoot->pBefore = pNew;

    }
  }

  return pRoot;
}

static tLoadRing * LoadRingRotateAdd(uint32_t load,tLoadRing * pRoot)
{
  pRoot=pRoot->pNext;
  pRoot->load = load;
  return pRoot;
}

static uint32_t LoadRingGetAvg(tLoadRing * pRoot,uint32_t maxCount)
{
  tLoadRing * pAct=pRoot;
  uint32_t sum=pAct->load;
  uint32_t anz=1;

  if(sum==INVALID_LOAD)
  {
    return 0;
  }

  while(pAct->pBefore != pRoot)
  {
    pAct = pAct->pBefore;
    if(pAct->load == INVALID_LOAD)
    {
      break;
    }
    if(anz >= maxCount)
    {
      break;
    }
    sum += pAct->load;
    anz++;
  }
  return sum/anz;
}


static void procload_read_runtime(tProcLoad *procload)
{
  char buffer[1024];

  lseek(procload->load_fd, 0, SEEK_SET);

  if (read(procload->load_fd, buffer, sizeof(buffer)))
  {
//    uint64_t result = 0;
    struct timespec timespec;
    //                                  cpu  2635788 0 3312747 62589417 220484 0 14596 0 0 0
    if (clock_gettime(CLOCK_MONOTONIC, &timespec) != 0)
        return;
    procload->timestamp = TIMESPEC_TO_U64(timespec);
    procload->deltatime = procload->timestamp - procload->oldtimestamp;

    const char * const uint64_format = "cpu %" PRIu64 "%" PRIu64 "%" PRIu64 "%" PRIu64 "%" PRIu64 "%" PRIu64 "%" PRIu64 " 0 0 0\n";
    if (sscanf(buffer, uint64_format, &procload->usertime
                                    , &procload->nicetime
                                    , &procload->systemtime
                                    , &procload->idletime
                                    , &procload->iowaittime
                                    , &procload->irqtime
                                    , &procload->softirqtime))
    {
      return;
    }
  }
  return ;
}


static void procload_read(tProcLoad *procload)
{
  if (procload == NULL)
      return ;

  procload->oldtimestamp = procload->timestamp;
  procload->oldusertime = procload->usertime;
  procload->oldnicetime = procload->nicetime;
  procload->oldsystemtime = procload->systemtime;
  procload->oldidletime = procload->idletime;
  procload->oldiowaittime = procload->iowaittime;
  procload->oldirqtime = procload->irqtime;
  procload->oldsoftirqtime = procload->softirqtime;

  procload_read_runtime(procload);

  return;
}


//uint32_t procload_get_loadvalue(tProcLoad *procload,uint64_t old, uint)


static uint32_t procload_get_load(tProcLoad *procload,tProcLoadEnum loadEnum)
{
  uint64_t delta_runtime = 1;
  uint32_t load = 0;

  switch(loadEnum)
  {
    case ProcLoadUser:
      delta_runtime = procload->usertime - procload->oldusertime;
      break;
    case ProcLoadNice:
      delta_runtime = procload->nicetime - procload->oldnicetime;
      break;
    case ProcLoadSystem:
      delta_runtime = procload->systemtime - procload->oldsystemtime;
      break;
    case ProcLoadIdle:
      delta_runtime = procload->idletime - procload->oldidletime;
      break;
    case ProcLoadIoWait:
      delta_runtime = procload->iowaittime - procload->oldiowaittime;
      break;
    case ProcLoadIrq:
      delta_runtime = procload->irqtime - procload->oldirqtime;
      break;
    case ProcLoadSoftIrq:
      delta_runtime = procload->softirqtime - procload->oldsoftirqtime;
      break;
  }

  uint32_t tickstime = (uint32_t)sysconf(_SC_CLK_TCK);
  uint64_t deltatime = (procload->deltatime * tickstime ) /NSEC_PER_SEC;

  if(deltatime != 0)
  {
    load = (uint32_t) DIV_ROUND_UP(100 * delta_runtime, deltatime);
  }
  return load;
}


static void procload_destroy(tProcLoad *procload)
{
  if (procload != NULL)
  {
    close(procload->load_fd);
    free(procload);
  }
}


static tProcLoad * _procLoad_init(void)
{
  char path[1024];
  snprintf(path, sizeof(path), "/proc/stat");
  int fd = open(path, O_RDONLY | O_NOATIME);
  if (fd == -1)
  {
    //ERROR("open (%s, ..) failed: %s", path, strerror(errno));
    return NULL;
  }
  tProcLoad *procload = calloc(1, sizeof(tProcLoad));
  procload->load_fd = fd;
  //(void)cgroupcpuload_get_load(cgroupcpuload);
  return procload;
}


int main (int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  struct cgroupcpuload *cgroupcpuload_RTS = cgroupcpuload_init("rts");
  struct cgroupcpuload *cgroupcpuload_SYS = cgroupcpuload_init("");
  tProcLoad * procload =  _procLoad_init();
  uint32_t load_RTS = cgroupcpuload_get_load(cgroupcpuload_RTS);
  uint32_t load_SYS = cgroupcpuload_get_load(cgroupcpuload_SYS);

  procload_read(procload);

//  uint32_t load1Avg_SYS=load_SYS,load1Avg_RTS=load_RTS;
//  uint32_t load5Avg_SYS=load_SYS,load5Avg_RTS=load_RTS;
//  uint32_t load15Avg_SYS=load_SYS,load15Avg_RTS=load_RTS;
  tLoadRing * loadRing_RTS = initLoadRing(INTERVALCOUNT(15));
  tLoadRing * loadRing_SYS = initLoadRing(INTERVALCOUNT(15));
  tLoadRing * loadRing_PROC = initLoadRing(INTERVALCOUNT(15));

  while(1)
  {
    load_RTS = cgroupcpuload_get_load(cgroupcpuload_RTS);
    load_SYS = cgroupcpuload_get_load(cgroupcpuload_SYS);
    procload_read(procload);
    loadRing_RTS = LoadRingRotateAdd(load_RTS,loadRing_RTS);
    loadRing_SYS = LoadRingRotateAdd(load_SYS,loadRing_SYS);
    system("clear");

    printf("CPU cgroups RTS: act:%d%% 1min:%d%% 5min:%d%% 15min:%d%%\n",
           load_RTS,
           LoadRingGetAvg(loadRing_RTS,INTERVALCOUNT(1)),
           LoadRingGetAvg(loadRing_RTS,INTERVALCOUNT(5)),
           LoadRingGetAvg(loadRing_RTS,INTERVALCOUNT(15)));

    printf("CPU cgroups Sys: act:%d%% 1min:%d%% 5min:%d%% 15min:%d%%\n",
           load_SYS,
           LoadRingGetAvg(loadRing_SYS,INTERVALCOUNT(1)),
           LoadRingGetAvg(loadRing_SYS,INTERVALCOUNT(5)),
           LoadRingGetAvg(loadRing_SYS,INTERVALCOUNT(15)));

    loadRing_PROC = LoadRingRotateAdd(100-procload_get_load(procload,ProcLoadIdle),loadRing_PROC);

    printf("CPU  /proc/stat: act:%d%% 1min:%d%% 5min:%d%% 15min:%d%%\n",
           100-procload_get_load(procload,ProcLoadIdle),
           LoadRingGetAvg(loadRing_PROC,INTERVALCOUNT(1)),
           LoadRingGetAvg(loadRing_PROC,INTERVALCOUNT(5)),
           LoadRingGetAvg(loadRing_PROC,INTERVALCOUNT(15)));

    printf("CPU by Proc: run: %d%%, idle %d%%, user %d%%, nice %d%%, sys %d%%\n",
           100-procload_get_load(procload,ProcLoadIdle),
           procload_get_load(procload,ProcLoadIdle),
           procload_get_load(procload,ProcLoadUser),
           procload_get_load(procload,ProcLoadNice),
           procload_get_load(procload,ProcLoadSystem));

    sleep(SLEEPSEC);
  }


    //for(int i= 0; i < 20; i++)
  /*  while(1)
    {
        printf("System:%.4d%% RTS:%.4d%%\r", cgroupcpuload_get_load(cgroupcpuload_SYS),cgroupcpuload_get_load(cgroupcpuload_RTS));
        fflush(stdout);
        fflush(stdout);
        sleep(5);
    }*/


  return 0;
}
