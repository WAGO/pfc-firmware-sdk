//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file dal_logging.h
///
/// \version $Revision: 4084 $
///
/// \brief Macros for logging/debug output.
///
/// \author <AGa> : WAGO GmbH
//------------------------------------------------------------------------------

#ifndef _LibraryLoader_Logging_H
#define _LibraryLoader_Logging_H

#include <stdio.h>

#define LOGLEVEL_DEBUG    10
#define LOGLEVEL_INFO     6
#define LOGLEVEL_WARNING  4
#define LOGLEVEL_ERROR    2
#define LOGLEVEL_QUIET    0

#ifndef LOGLEVEL
#define LOGLEVEL LOGLEVEL_DEBUG
#endif

// Note: at least 2 arguments are mandatory => To print a single string literal,
// use i.e. DBG("%s", "Debug string").

#define DBG( fmt, ...) \
  do {\
     /*lint -e{506} suppress "Constant value Boolean"*/\
     /*lint -e{948} suppress ">= always true"*/\
     /*lint -e{774} suppress "Boolean within if always true"*/\
     if (LOGLEVEL >= LOGLEVEL_DEBUG)\
     {\
        fprintf(stdout, \
        "DEBUG: %s:%d:%s(): " fmt "\n", \
        __FILE__,__LINE__,__func__, \
        __VA_ARGS__); \
     } \
  }while (0)

#define INFO( fmt, ...) \
   do {\
       /*lint -e{506}*/\
       /*lint -e{948} suppress ">= always true"*/\
       /*lint -e{774} suppress "Boolean within if always true"*/\
       if (LOGLEVEL >= LOGLEVEL_INFO)\
       {\
          fprintf(stdout, \
           "INFO: %s():" fmt "\n", \
           __func__, \
           __VA_ARGS__); \
       }\
    } while (0)

#define WARN( fmt, ...) \
    do {\
       /*lint -e{506}*/\
       /*lint -e{948} suppress ">= always true"*/\
       /*lint -e{774} suppress "Boolean within if always true"*/\
       if (LOGLEVEL >= LOGLEVEL_WARNING)\
       {\
          fprintf(stdout, \
           "WARNING: %s():" fmt "\n", \
           __func__, \
           __VA_ARGS__); \
       }\
    } while (0)

#define ERROR( fmt, ...) \
    do {\
       /*lint -e{506}*/\
       /*lint -e{948} suppress ">= always true"*/\
       /*lint -e{774} suppress "Boolean within if always true"*/\
       if (LOGLEVEL >= LOGLEVEL_ERROR)\
       {\
          fprintf(stderr, \
           "ERROR: %s(): " fmt "\n", \
           __func__, \
           __VA_ARGS__); \
       }\
    } while (0)



#endif

