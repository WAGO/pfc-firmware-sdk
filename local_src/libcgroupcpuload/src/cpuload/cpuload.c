//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

/*
 * Ziel dieses Tools ist eine einfache Anzeige der CPU-Auslastung
 * Bedienung:
 * - Das Tool wird aufgerufen und bleibt bis zum manuellen Beenden aktiv.
 * - Beim Starten des Tools kann gewählt werden, ob man die last wie bei htop oder SNMP über die Inverse des IDLE messen möchte, oder über cgroups
 * - Eine weitere Auswahlmöglichkeit ist der messinterval, den man in Sekunden wählen kann.
 *
 *  Als Ausgabe bekommt man eine einfache Auslastung in % ohne Anzeige der Einheit "<PROZENZAHL><NEWLINE>" im angegebenen interval.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "cgroupcpuload.h"

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
    return;

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
  uint64_t deltatime = (procload->deltatime * tickstime ) / NSEC_PER_SEC;

  if(deltatime != 0)
  {
    load = (uint32_t) DIV_ROUND_UP(100 * delta_runtime, deltatime);
  }
  return load;
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



static char const szCommandlineOptions[] = "hs:i:v";


static struct option arstCommandlineOptions[] =
{
  { "help",                        no_argument,       NULL, 'h' },
  { "source",                      required_argument, NULL, 's' },
  { "interval",                    required_argument, NULL, 'i' },
  { "verbose",                     no_argument,       NULL, 'v' },
  { NULL,                          no_argument,       NULL,  0  } // End marker, don't remove
};

static char const * const arszCommandlineOptionTexts[] =
{
  "Prints this help text.",
  "Source for CPU load messurement. idle or cgroups. default: cgroups",
  "Interval for messurement in seconds",
  "Verbose",
  NULL // End marker, don't remove
};

static void PrintCommandOptions(void)
{
  printf("Application options:\n");
  size_t i = 0U;
  while(arstCommandlineOptions[i].name != NULL)
  {
    char const * szParam = "";
    if(arstCommandlineOptions[i].has_arg == required_argument)
    {
      szParam = "<param>";
    }
    if(arstCommandlineOptions[i].has_arg == optional_argument)
    {
      szParam = "[param]";
    }
    printf("  --%-25s %-7s   -%c   %s\n", arstCommandlineOptions[i].name,
                                          szParam,
                                          arstCommandlineOptions[i].val,
                                          arszCommandlineOptionTexts[i]);
    i++;
  }
}

static void PrintHelpText(char const * const szName)
{
  printf("Program call: %s\n", szName);
  printf("\n");
  printf("Usage:\n  %s -s <source> -i <interval>\n", szName);
  printf("\n");
  PrintCommandOptions();
  printf("\n");
}


static int EvaluateOptions(int * useProc,int * messurementInterval,int * verbose,int const argc,
                                    char * const argv[])
{
  int status = 0;

  int option;
  int optionIndex;

  opterr = 0;
  optind = 0;
  while(   (0 == status)
        && ((option = getopt_long(argc, argv, szCommandlineOptions, arstCommandlineOptions, &optionIndex)) != -1)) //lint !e960 !e1960 !e9084 suggested usage of getopt
  {
    switch(option)
    {
      case 'h':
        PrintHelpText(argv[0U]);
        exit(EXIT_SUCCESS);
        break;

      case 's':
        if((optarg == NULL) || (optarg[0U] == '\0'))
        {
          printf("missing parameters\n");
          status = -1;
        }
        else
        {
          if(!strcmp(optarg,"idle"))
          {
            *useProc=1;
          }else if(!strcmp(optarg,"cgroups"))
          {
            *useProc=0;
          }
          else
          {
            status = -1;
          }
        }

        break;


      case 'i':
        if((optarg == NULL) || (optarg[0U] == '\0'))
        {
          printf("missing parameters\n");
          status = -1;
        }
        else
        {
          long int const interval = strtol(optarg, NULL, 10);
          if (interval <= 0)
          {
            status = -1;
            break;
          }
          *messurementInterval = interval;
        }
        break;


      case 'v':
        *verbose=1;
        break;


      case '0':
        if(arstCommandlineOptions[(unsigned int)optionIndex].flag != NULL)
        {
          // Option sets a flag, nothing else to do
          break;
        }
        //lint -fallthrough, -e9090 no break
      case '?':
        //lint -fallthrough, no break
      default:
        // Wrong option character or option with missing argument stored in global variable "optopt"
        // getopt_long already printed an error message in case of '?' (if opterr != 0)
        status = -1;
        break;
    }
  }

  return status;
}

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  struct cgroupcpuload *cgroupcpuload_SYS = cgroupcpuload_init("");
  uint32_t load = 0U;
  unsigned int sleepsec = 1;
  tProcLoad * procload =  _procLoad_init();
  int useProc=0;
  int verbose=0;

  if(0!=EvaluateOptions(&useProc,&sleepsec,&verbose,argc,argv))
  {
    PrintHelpText(argv[0U]);
    exit(EXIT_FAILURE);
  }

  if(verbose)
  {
    printf("start messuring CPU-load with source: ");
    if(useProc)
    {
      printf ("idle ");
    }
    else
    {
      printf ("cgroups ");
    }
    printf("and with an Interval of %d Seconds\n",sleepsec);
  }

  while(1)
  {
    sleep(sleepsec);
    if(!useProc)
    {
      load = cgroupcpuload_get_load(cgroupcpuload_SYS);
    }
    else
    {
      procload_read(procload);
      load = 100-procload_get_load(procload,ProcLoadIdle);
    }

    printf("%u\n", load);
  }
}
