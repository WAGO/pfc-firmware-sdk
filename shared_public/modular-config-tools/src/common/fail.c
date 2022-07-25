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
///  \file     fail.c
///
///  \brief    Common fail function implementation for config tools.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "wc/assertion.h"
#include "ctutil/error_codes.h"
#include "ctutil/log.h"
#include <stdlib.h>

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

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Common config tool fail function to handle failures
/// (e.g. for a failed assertion).
///
/// \param szReason
///   Null terminated string with failure reason.
/// \param szFile
///   Null terminated string with source file name of failure source.
/// \param szFunction
///   Null terminated string with function name of failure source.
/// \param line
///   Line number of failure source in source file.
//------------------------------------------------------------------------------
void wc_Fail(char const * const szReason,
             char const * const szFile,
             char const * const szFunction,
             int const line)
{
  // Log fatal error
  ctutil_Log(ctutil_LogLevelEnum_fatal, NULL, szFile, szFunction, line, szReason);

  // Exit application
  exit(CTUTIL_EXIT_GENERAL_ERROR);
}


//---- End of source file ------------------------------------------------------

