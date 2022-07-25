//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     cpuloadmonitor.c
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "../cgroupcpuload/cgroupcpuload.h"
#include "error_codes.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

static char const szCommandlineOptions[] = "hl:o:I:n:";

static struct option arstCommandlineOptions[] =
{
  { "help",                        no_argument,       NULL, 'h' },
  { "list",                        optional_argument, NULL, 'l' },
  { "output-file",                 optional_argument, NULL, 'o' },
  { "interval",                    optional_argument, NULL, 'I' },
  { "count",                       optional_argument, NULL, 'n' },
  { NULL,                          no_argument,       NULL,  0  } // End marker, don't remove
};


static char const * const arszCommandlineOptionTexts[] =
{
  "Prints this help text.",
  "List of cgroups to monitor. The list is comma separated.",
  "Store results to file.",
  "Interval between subsequent readings. (ms)",
  "Count of samples. Measurement ends when the count is reached.",
  NULL // End marker, don't remove
};


static struct Parameters{
    int output;
    size_t group_count;
    char * groups[50];
    __useconds_t interval_ms;
    uint32_t samples;
    bool running;
} Options = {STDOUT_FILENO, 0U, {NULL}, 2000U, 200U, true};

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

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
  printf("Usage:\n  %s -l <input-list> -o <output-file>\n", szName);
  printf("\n");
  PrintCommandOptions();
  printf("\n");
}


static statusCode_t IterateString(
    char const * const input,
    char *groups[],
    size_t groups_size,
    size_t *count)
{
  char * szTmp = strdup(input);
  if (NULL != szTmp)
  {
    char const * token = strtok(szTmp, ",");
    while (NULL != token) {
        groups[(*count)++] = strdup(token);
        if (*count >= groups_size)
        {
          printf("Max number of groups reached\n");
          break;
        }

        token = strtok(NULL, ",");
    }
    free(szTmp);
  }

  return (*count > 0U) ? CPULOADMONITOR_SUCCESS : CPULOADMONITOR_FAILED;
}


static statusCode_t Write(const void* buffer, size_t const size)
{
  ssize_t const bytesWritten = write(Options.output, buffer, size);
  return ((bytesWritten >= 0) && (((size_t)bytesWritten) == size)) ? CPULOADMONITOR_SUCCESS : CPULOADMONITOR_FAILED;
}


static statusCode_t WriteCString(char const * const cstring)
{
  return Write(cstring, strlen(cstring));
}


// Evaluate command line arguments.
static statusCode_t EvaluateOptions(int const argc,
                                    char * const argv[])
{
  statusCode_t status = CPULOADMONITOR_SUCCESS;

  int option;
  int optionIndex;

  opterr = 0;
  optind = 0;
  while(   (CPULOADMONITOR_SUCCESS == status)
        && ((option = getopt_long(argc, argv, szCommandlineOptions, arstCommandlineOptions, &optionIndex)) != -1)) //lint !e960 !e1960 !e9084 suggested usage of getopt
  {
    switch(option)
    {
      case 'h':
        PrintHelpText(argv[0U]);
        break;

      case 'l':
        if((optarg == NULL) || (optarg[0U] == '\0'))
        {
          printf("invalid parameters\n");
          status = CPULOADMONITOR_MISSING_PARAMETER;
        }
        else
        {
          status = IterateString(optarg,
                                 Options.groups,
                                 sizeof(Options.groups)/sizeof(Options.groups[0U]),
                                 &Options.group_count);
        }
        break;

      case 'o':
        if((optarg == NULL) || (optarg[0U] == '\0'))
        {
          status = CPULOADMONITOR_MISSING_PARAMETER;
        }
        else
        {
          int const fd = open(optarg, O_CREAT | O_WRONLY | O_TRUNC, 0666);
          if (fd == -1)
          {
            status = CPULOADMONITOR_FILE_OPEN_ERROR;
            break;
          }

          Options.output = fd;
        }
        break;

      case 'I':
        if((optarg == NULL) || (optarg[0U] == '\0'))
        {
          status = CPULOADMONITOR_MISSING_PARAMETER;
        }
        else
        {
          long int const interval = strtol(optarg, NULL, 10);
          if (interval < 0)
          {
            status = CPULOADMONITOR_INVALID_PARAMETER;
            break;
          }
          Options.interval_ms = (__useconds_t)interval;
        }
        break;

      case 'n':
        if((optarg == NULL) || (optarg[0U] == '\0'))
        {
          status = CPULOADMONITOR_MISSING_PARAMETER;
        }
        else
        {
          long int const samples = strtol(optarg, NULL, 10);
          if (samples < 0)
          {
            status = CPULOADMONITOR_INVALID_PARAMETER;
            break;
          }
          if (samples > 0)
          {
            Options.samples = (uint32_t) samples;
          }
        }
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
        status = CPULOADMONITOR_INVALID_PARAMETER;
        break;
    }
  }

  return status;
}


static void InitializeCGroups(
    struct cgroupcpuload * cgroups[],
    char * const groups[],
    size_t count)
{
  assert(NULL != cgroups);
  for(size_t i = 0U; i < count; ++i)
  {
    cgroups[i] = cgroupcpuload_init(groups[i]);
  }
}


static statusCode_t WriteCsvHeader(char * const groups[], size_t const count)
{
  bool fOK = false;
  for(size_t i = 0U; i < count; ++i)
  {
    fOK = (   (CPULOADMONITOR_SUCCESS == WriteCString(groups[i]))
           && (CPULOADMONITOR_SUCCESS == WriteCString(",")) );

    if(!fOK)
    {
      break;
    }
  }

  if(fOK)
  {
    fOK = (CPULOADMONITOR_SUCCESS == WriteCString("\n"));
  }

  return (fOK) ? CPULOADMONITOR_SUCCESS : CPULOADMONITOR_FILE_WRITE_ERROR;
}


static void DestroyCGroups(struct cgroupcpuload * cgroups[], size_t count)
{
  assert(NULL != cgroups);
  for(size_t i = 0U; i < count; ++i)
  {
    cgroupcpuload_destroy(cgroups[i]);
  }
}


static statusCode_t EvaluateCGroups(
    struct cgroupcpuload * cgroups[],
    size_t const count,
    __useconds_t const interval_us,
    uint32_t samples,
    bool *run_flag)
{
  assert(NULL != cgroups);
  bool fOK = false;
  uint32_t * const load = calloc(count, sizeof( uint32_t));

  if(NULL != load)
  {
    char buffer[sizeof("100%;")];
    fOK = true;
    while(   (fOK)
          && (samples-- != 0U)
          && ((bool) __atomic_load_n(run_flag, __ATOMIC_RELAXED) ))
    {
      //lint -e(534) Ignoring return value
      usleep(interval_us);

      for(size_t i = 0U; i < count; ++i)
      {
        load[i] = cgroupcpuload_get_load(cgroups[i]);
      }

      for(size_t i = 0U; i < count; ++i)
      {
        sprintf(buffer, "%" PRIu32 ",", load[i]);
        fOK = (CPULOADMONITOR_SUCCESS == WriteCString(buffer));
        if(!fOK)
        {
          break;
        }
      }
      if(fOK)
      {
        fOK = (CPULOADMONITOR_SUCCESS == WriteCString("\n"));
      }
    }
  }

  free(load);

  return (fOK) ? CPULOADMONITOR_SUCCESS : CPULOADMONITOR_FILE_WRITE_ERROR;
}


static statusCode_t MonitorCGroups(
    char * const groups[],
    size_t const count,
    __useconds_t const interval_ms,
    uint32_t samples,
    bool *run_flag)
{

  bool fOK;
  struct cgroupcpuload ** cgroups = calloc(count, sizeof(struct cgroupcpuload *));

  InitializeCGroups(cgroups,
                    groups,
                    count);

  fOK = (   (CPULOADMONITOR_SUCCESS == WriteCsvHeader(groups, count))
         && (CPULOADMONITOR_SUCCESS == EvaluateCGroups(cgroups, count, interval_ms * 1000U, samples, run_flag)) );

  DestroyCGroups(cgroups, count);
  free(cgroups);

  return (fOK) ? CPULOADMONITOR_SUCCESS : CPULOADMONITOR_FILE_WRITE_ERROR;
}


static void TerminationHandler(int signum)
{
  (void)signum;
  __atomic_store_n(&Options.running, false, __ATOMIC_RELAXED);
}


static void InstallSingalHandler(void)
{
  struct sigaction action;
  action.sa_handler = &TerminationHandler;
  (void)sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  (void)sigaction(SIGINT, &action, NULL);
  (void)sigaction(SIGTERM, &action, NULL);
}


static void Cleanup(void)
{
  (void)fsync(Options.output);
  if (Options.output != STDOUT_FILENO)
  {
    close(Options.output);
  }

  for(size_t i = 0U; i < Options.group_count; ++i)
  {
    free(Options.groups[i]);
  }
}


static void PrintMeasurementAction(void)
{
  printf("Begin measurement:\n\tGroups:");
  for(size_t i = 0U; i < Options.group_count; ++i)
  {
    printf("%s,", Options.groups[i]);
  }
  printf("\n");
  printf("\tInterval: %ud [ms]\n", Options.interval_ms);
  printf("\tSamples: %ud\n", Options.samples);
}


int main (int argc, char * argv[])
{
  statusCode_t status;

  InstallSingalHandler();

  status = EvaluateOptions(argc, argv);

  if (   (status == CPULOADMONITOR_SUCCESS)
      && (Options.group_count > 0U))
  {
    PrintMeasurementAction();

    status = MonitorCGroups(Options.groups,
                            Options.group_count,
                            Options.interval_ms,
                            Options.samples,
                            &Options.running);
  }

  Cleanup();

  return status;
}

//---- End of source file ------------------------------------------------------
