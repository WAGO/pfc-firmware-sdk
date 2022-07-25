//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
/// SPDX-License-Identifier: MPL-2.0
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \author WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "OsTrace.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#ifndef OS_TRACE_LEVEL
//#define OS_TRACE_LEVEL (TraceLevel_Debug | TraceLevel_Info | TraceLevel_Warning | TraceLevel_Error)
#define OS_TRACE_LEVEL TraceLevel_Error
#endif

#ifndef OS_TRACE_FACILITY
#define OS_TRACE_FACILITY TraceFacility_All
#endif

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

u32 g_OsTrace_TraceFacility = OS_TRACE_FACILITY;
u32 g_OsTrace_TraceLevel =   OS_TRACE_LEVEL;

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

static pthread_mutex_t s_PrintMutex = PTHREAD_MUTEX_INITIALIZER;


/*****************************************************************************/
/*! User trace function
 *   right while cifXTKitAddDevice is being processed
 *   \param ptDevInstance  Device instance
 *   \param ulTraceLevel   Trace level //[JSO] Changed TRACE_LEVEL_* defines to an enum type
 *   \param szFormat       Format string                                      */
/*****************************************************************************/
void OsTrace_Printf(tTraceFacility traceFacility,
                    tTraceLevel traceLevel,
                    const char* szFormat,
                    ...)
{
  va_list ap;
  va_start(ap, szFormat);
  OsTrace_vPrintf(traceFacility, traceLevel, szFormat, ap);
  va_end(ap);
}


void OsTrace_vPrintf(tTraceFacility traceFacility,
                    tTraceLevel traceLevel,
                    const char* szFormat,
                    va_list args)
{
   FILE* stream = traceLevel == TraceLevel_Error ? stderr : stdout;
   if ((traceLevel & g_OsTrace_TraceLevel)
         && (traceFacility & g_OsTrace_TraceFacility))
   {
      pthread_mutex_lock(&s_PrintMutex);
      if (traceLevel & (TraceLevel_Debug | TraceLevel_Info))
      {
         struct timespec ts;
         if (clock_gettime(CLOCK_MONOTONIC, &ts) != -1)
         {
            fprintf(stream, "%3lds%03ld.%03ld.%03ldns [%08lx] ",
                  ts.tv_sec % 1000, ts.tv_nsec / 1000000,
                  (ts.tv_nsec / 1000) % 1000, ts.tv_nsec % 1000,
                  pthread_self());
         }
      }
      vfprintf(stream, szFormat, args);
      fprintf(stream, "\n");
      pthread_mutex_unlock(&s_PrintMutex);
   }
}
