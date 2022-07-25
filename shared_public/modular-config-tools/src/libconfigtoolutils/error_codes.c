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
///  \file     error_codes.c
///
///  \brief    Common exit/error codes and helper functions for modular config tools.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/error_codes.h"
#include "wc/assertion.h"

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
bool ctutil_IsStatusOk(statusCode_t const status)
{
  WC_STATIC_ASSERT(CTUTIL_SUCCESS > CTUTIL_FAILED);
  return (status >= CTUTIL_SUCCESS);
}


bool ctutil_IsStatusFailure(statusCode_t const status)
{
  WC_STATIC_ASSERT(CTUTIL_SUCCESS > CTUTIL_FAILED);
  return (status <= CTUTIL_FAILED);
}


exitCode_t ctutil_GetExitCodeFromStatus(statusCode_t const status)
{
  exitCode_t result;

  // Try to find corresponding exit code
  switch(status)
  {
    case CTUTIL_NOT_FOUND:
      result = CTUTIL_EXIT_NOT_FOUND;
      break;

    case CTUTIL_FILE_OPEN_ERROR:
      result = CTUTIL_EXIT_FILE_OPEN_ERROR;
      break;

    case CTUTIL_SYSTEM_CALL_ERROR:
      result = CTUTIL_EXIT_SYSTEM_CALL_ERROR;
      break;

    case CTUTIL_FILE_WRITE_ERROR:
      result = CTUTIL_EXIT_FILE_WRITE_ERROR;
      break;

    case CTUTIL_MISSING_PARAMETER:
      result = CTUTIL_EXIT_MISSING_PARAMETER;
      break;

    case CTUTIL_INVALID_PARAMETER:
      result = CTUTIL_EXIT_INVALID_PARAMETER;
      break;

    // TODO: Map all exit codes

    case CTUTIL_FAILED:
    default:
      result = ctutil_IsStatusOk(status) ? CTUTIL_EXIT_SUCCESS : CTUTIL_EXIT_GENERAL_ERROR;
  }

  WC_ASSERT(result >= CTUTIL_EXIT_MIN);
  WC_ASSERT(result <= CTUTIL_EXIT_MAX);
  return result;
}


//---- End of source file ------------------------------------------------------

