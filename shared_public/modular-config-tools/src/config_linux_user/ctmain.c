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
///  \file     ctmain.c
///
///  \brief    Config tool specific main functionality for
///            config_linux_user.
///
///  \author   HFS: WAGO GmbH & Co. KG
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "ctutil/common_main.h"
#include "ctparts/common_main_defaults.h"
#include "ctparts/pmain.h"
#include "ctparts/ctmain.h"
#include "params.h"
#include "resources.h"
#include "shadow_functions.h"
#include <string.h>

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

/// Early function for to prepare config tool main of config_linux_user.
exitCode_t ctparts_PrepareMain(int const argc,
                               char * const argv[])
{
  // Prepare functions
  ctparts_commonMainFunctions_t stFunctions;
  ctparts_SetDefaultCommonMainFunctions(&stFunctions);

  // Call common main function to prepare config tool main
  return ctutil_CommonMain(argc, argv, &stFunctions, NULL);
}


static bool IsPlainMode(ctutil_Options_t const * const pstOptions)
{
  return (pstOptions->pstSpecificOptions->szHash == NULL);
}


/// Main function for config tool core functionality of config_linux_user.
exitCode_t ctparts_ctMain(int const WC_UNUSED_PARAM(argc),
                          char * const WC_UNUSED_PARAM(argv[]),
                          ctutil_Options_t const * const pstOptions,
                          ctutil_Resources_t const * const pstResources)
{
  statusCode_t status = CTUTIL_SUCCESS;

  struct spwd stNewUserData;
  memset(&stNewUserData, 0, sizeof(stNewUserData));

  if(IsPlainMode(pstOptions))
  {
    // Allocation for duplication will be cleaned afterwards (see cleanup section below)
    stNewUserData.sp_namp = strdup(pstOptions->pstSpecificOptions->szUsername);
    status = FillUserData(&stNewUserData, pstResources,
                          pstOptions->pstSpecificOptions->szUsername,
                          pstOptions->pstSpecificOptions->szPassword1,
                          pstOptions->pstSpecificOptions->szPassword2);
  }
  else
  {
    status = FillUserDataFromHash(&stNewUserData, pstResources,
                                  pstOptions->pstSpecificOptions->szUsername,
                                  pstOptions->pstSpecificOptions->szHash);
  }

  if(ctutil_IsStatusOk(status))
  {
    status = SearchShadowEntry(stNewUserData.sp_namp);
  }

  if(ctutil_IsStatusOk(status))
  {
    status = WriteShadow(pstResources, &stNewUserData);
  }

  // Cleanup
  //
  // Note: 'man shadow' is very vague on its internal memory management
  // details. It is unclear whether the user has to clean up struct spwd
  // objects.
  // At least valgrind shows no memory leaks when no cleanup is done there.
  // That's why we only clean up self-allocated strings we use with struct spwd
  // objects.
  if(IsPlainMode(pstOptions))
  {
    free(stNewUserData.sp_namp);
  }

  return ctutil_GetExitCodeFromStatus(status);
}


//---- End of source file ------------------------------------------------------

