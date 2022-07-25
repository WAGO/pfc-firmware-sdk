#ifndef D_OsTrace_H
#define D_OsTrace_H

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

#include <inttypes.h>
#include <stdarg.h>

#include "OsConfig.h"
#include "OsCommon.h"
//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

#ifdef OS_TRACE_ENABLED

#define OsTraceDebug(facility, X, ...) \
do { \
   if (g_OsTrace_TraceLevel & TraceLevel_Debug) {\
      OsTrace_Printf(facility, TraceLevel_Debug, "%s:%d %s(): " X, __FILE__, __LINE__ ,__func__, __VA_ARGS__); }\
} while(0)

#define OsTraceError(X, ...) \
do { \
   if (g_OsTrace_TraceLevel & TraceLevel_Error) \
      OsTrace_Printf(TraceFacility_All, TraceLevel_Error, "%s(): " X, __func__, __VA_ARGS__); \
} while(0)

#define OsTraceWarning(X, ...) \
do { \
   if (g_OsTrace_TraceLevel & TraceLevel_Warning) \
      OsTrace_Printf(TraceFacility_All, TraceLevel_Warning, X, __VA_ARGS__); \
} while(0)

#define OsTraceInfo(facility, X, ...) \
do { \
   if (g_OsTrace_TraceLevel & TraceLevel_Info) \
      OsTrace_Printf(facility, TraceLevel_Info, X,  __VA_ARGS__); \
} while(0)

#else

#define OsTraceDebug(facility, X, ...) do { ;} while(0)

#define OsTraceError(X, ...)  do { ;} while(0)

#define OsTraceWarning(X, ...)   do { ;} while(0)

#define OsTraceInfo(facility, X, ...)  do { ;} while(0)

#endif


/**
 * Fixed string trace routines. Avoids using the gcc's paste extension (##)
 */

#define OsTraceDebugF(facility, X) OsTraceDebug(facility, X, NULL)

#define OsTraceErrorF(X) OsTraceError(X, NULL)

#define OsTraceWarningF(X) OsTraceWarning(X, NULL)

#define OsTraceInfoF(facility, X) OsTraceInfo(facility, X, NULL)

#define _STRING(x) #x

#define VAR_NAME(X) #X

#define _NV(X) #X, X

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

#define FLAG(bit) (1<<bit)

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


typedef enum {
   TraceFacility_OsalThread = FLAG(1),
   TraceFacility_OsalSemaphore = FLAG(2),
   TraceFacility_OsalMutex= FLAG(3),
   TraceFacility_OsalTime= FLAG(4),
   TraceFacility_OsalTimer = FLAG(5),
   TraceFacility_OsalCondition = FLAG(6),
   TraceFacility_OsalRwLock = FLAG(7),
   TraceFacility_OsalMemory = FLAG(8),
   TraceFacility_User8 = FLAG(9),
   TraceFacility_LibSercomm = FLAG(10),
   TraceFacility_User10 = FLAG(11),
   TraceFacility_User11 = FLAG(12),
   TraceFacility_User12 = FLAG(13),
   TraceFacility_User13 = FLAG(14),
   TraceFacility_User14 = FLAG(15),
   TraceFacility_All = 0xffffffff,
}tTraceFacility;




typedef enum {
   TraceLevel_Debug = 0x00000001,
   TraceLevel_Info = 0x00000002,
   TraceLevel_Warning = 0x00000004,
   TraceLevel_Error = 0x00000008,
   TraceLevel_All = 0x0fffffff,
}tTraceLevel;

void OsTrace_Printf(tTraceFacility traceFacility,
                    tTraceLevel traceLevel,
                    const char* szFormat,
                    ...);

void OsTrace_vPrintf(tTraceFacility traceFacility,
                    tTraceLevel traceLevel,
                    const char* szFormat,
                    va_list args);

extern u32 g_OsTrace_TraceLevel;
extern u32 g_OsTrace_TraceFacility;


#endif  // D_OsTrace_H
