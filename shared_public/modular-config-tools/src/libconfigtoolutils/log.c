//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2017-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     log.c
///
///  \brief    Log functionality implementation.
///            This functionality should be moved to a more common library.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/log.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#ifdef DEBUGPRINT
#define PRINT_DEBUG_LOG true
#else
#define PRINT_DEBUG_LOG false
#endif

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

// TODO: Move whole functionality to a more common library


void ctutil_Log(ctutil_LogLevelEnum_t const logLevel,
                ctutil_Timestamp_t const * const WC_UNUSED_PARAM(pTimestamp),
                char const * const szFile,
                char const * const szFunction,
                int const line,
                char const * const szMessage)
{
  char const szFormat[] = "[%u] %s, %s (line %i): %s\n";
  // TODO: Replace by suitable solution (e. g. write to syslog)
  // TODO: Evaluate log level more detailed
  if(logLevel <= ctutil_LogLevelEnum_warning)
  {
    fprintf(stderr, szFormat, logLevel, szFile, szFunction, line, szMessage);
  }
  else if(logLevel <= ctutil_LogLevelEnum_info)
  {
    fprintf(stdout, szFormat, logLevel, szFile, szFunction, line, szMessage);
  }
  else
  {
    if(PRINT_DEBUG_LOG)
    {
      fprintf(stdout, szFormat, logLevel, szFile, szFunction, line, szMessage);
    }
  }
}


void ctutil_LogErrno(ctutil_LogLevelEnum_t const logLevel,
                     ctutil_Timestamp_t const * const WC_UNUSED_PARAM(pTimestamp),
                     char const * const szFile,
                     char const * const szFunction,
                     int const line,
                     int const errorNumber,
                     char const * const szPreText)
{
  char const szFormat[] = "[%u] %s, %s (line %i): %s (error %i: %s)\n";
  // TODO: Replace by suitable solution (e. g. write to syslog)
  // TODO: Evaluate log level more detailed
  if(logLevel <= ctutil_LogLevelEnum_warning)
  {
    fprintf(stderr, szFormat, logLevel, szFile, szFunction, line, szPreText, errorNumber, strerror(errorNumber));
  }
  else if(logLevel <= ctutil_LogLevelEnum_info)
  {
    fprintf(stdout, szFormat, logLevel, szFile, szFunction, line, szPreText, errorNumber, strerror(errorNumber));
  }
  else
  {
    if(PRINT_DEBUG_LOG)
    {
      fprintf(stdout, szFormat, logLevel, szFile, szFunction, line, szPreText, errorNumber, strerror(errorNumber));
    }
  }
}


//---- End of source file ------------------------------------------------------

