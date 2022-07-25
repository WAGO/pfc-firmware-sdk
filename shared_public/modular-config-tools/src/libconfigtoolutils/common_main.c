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
///  \file     common_main.c
///
///  \brief    Commonly used, generic config tool main function
///            to prepare config tool specific main.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "wc/std_includes.h"
#include "ctutil/common_main.h"
#include "ctutil/log.h"
#include "ctparts/ctmain.h"
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
exitCode_t ctutil_CommonMain(int const argc,
                             char * const argv[],
                             ctparts_commonMainFunctions_t const * const pstFunctions,
                             ctparts_commonMainContext_t * const pstContext)
{
  exitCode_t result = CTUTIL_EXIT_SUCCESS;
  statusCode_t status = CTUTIL_SUCCESS;

  WC_ASSERT(pstFunctions != NULL);

  // Set default options
  ctutil_Options_t stOptions;
  ctutil_InitOptions(&stOptions);
  WC_ASSERT(stOptions.pstSpecificOptions == NULL);
  WC_ASSERT(pstFunctions->pfSetOptionDefaults != NULL);
  status = pstFunctions->pfSetOptionDefaults(pstContext, &stOptions);
  if(ctutil_IsStatusFailure(status))
  {
    WC_ASSERT(stOptions.pstSpecificOptions == NULL);
  }

  // Early option evaluation to check priority options (e. g. argument decoding or quiet option)
  if(ctutil_IsStatusOk(status))
  {
    if(pstFunctions->pfEvaluateEarlyOptions != NULL)
    {
      status = pstFunctions->pfEvaluateEarlyOptions(pstContext, argc, argv, &stOptions);
    }
  }

  // Evaluate commandline options if any
  if(ctutil_IsStatusOk(status))
  {
    if(pstFunctions->pfEvaluateOptions != NULL)
    {
      status = pstFunctions->pfEvaluateOptions(pstContext, argc, argv, &stOptions);
    }
  }

  // Print info text if requested (normally when no action options are given)
  if(    !stOptions.stCommonOptions.quiet
      && (ctutil_IsStatusOk(status))
      && stOptions.stCommonOptions.infoPrint)
  {
    if(pstFunctions->pfPrintInfoText != NULL)
    {
      pstFunctions->pfPrintInfoText(pstContext, argv[0]);
    }
  }

  // Basic init and open of common resources if necessary
  ctutil_Resources_t stResources;
  bool releaseCommonResources = false;
  if(ctutil_IsStatusOk(status))
  {
    if(!stOptions.stCommonOptions.earlyExit)
    {
      ctutil_InitResources(&stResources);
      status = ctutil_OpenCommonResources(&(stResources.stCommonResources));
      if(ctutil_IsStatusOk(status))
      {
        releaseCommonResources = true;
      }
    }
  }

  // Open specific resources if necessary
  bool releaseSpecificResources = false;
  if(ctutil_IsStatusOk(status))
  {
    if(!stOptions.stCommonOptions.earlyExit)
    {
      WC_ASSERT(stResources.pstSpecificResources == NULL);
      if(pstFunctions->pfInitSpecificResources != NULL)
      {
        status = pstFunctions->pfInitSpecificResources(pstContext, &(stResources.pstSpecificResources));
        if(ctutil_IsStatusFailure(status))
        {
          WC_ASSERT(stResources.pstSpecificResources == NULL);
        }
        else
        {
          releaseSpecificResources = true;
        }
      }
    }
  }

  // Check for errors so far
  if(ctutil_IsStatusFailure(status))
  {
    stOptions.stCommonOptions.earlyExit = true;
    CTUTIL_LOG_STATUS_ERROR(stOptions.stCommonOptions.quiet, status, "Failed to prepare config tool");
    result = ctutil_GetExitCodeFromStatus(status);
  }

  // Check for exit request or enter config tool main
  if(!stOptions.stCommonOptions.earlyExit)
  {
    result = ctparts_ctMain(argc, argv, &stOptions, &stResources);
  }

  // Release resources and specific options if any
  if(releaseSpecificResources)
  {
    WC_ASSERT(pstFunctions->pfReleaseSpecificResources != NULL);
    pstFunctions->pfReleaseSpecificResources(pstContext, &(stResources.pstSpecificResources));
  }
  if(releaseCommonResources)
  {
    ctutil_ReleaseCommonResources(&(stResources.stCommonResources));
  }
  if(stOptions.pstSpecificOptions != NULL)
  {
    WC_ASSERT(pstFunctions->pfReleaseSpecificOptions != NULL);
    pstFunctions->pfReleaseSpecificOptions(pstContext, &stOptions);
  }

  WC_ASSERT(result >= CTUTIL_EXIT_MIN);
  WC_ASSERT(result <= CTUTIL_EXIT_MAX);
  return result;
}


//---- End of source file ------------------------------------------------------

