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
///  \file     params.c
///
///  \brief    Config tool specific parameter definition and handling for
///            config_linux_user.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "ctutil/log.h"
#include "ctparts/common_main_defaults.h"
#include "params.h"
#include <stdlib.h>
#include <stdio.h>
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

static void PrintHelp(void)
{
    printf("\n* Change the passwords of the linux-user *\n"
           "\n"
           "Usage: config_linux_user < user=user-value new-password=new-password-value confirm-password=confirm-password-value >\n"
           "       config_linux_user < user=user-value hash=new-hash-value >\n"
           "\n"
           "user-value:                                    name of the webserver-user, whose password should be changed\n"
           "new-password-value / confirm-password-value:   both strings must be the same and URI encoded\n"
           "hash:                                          password hash and additional infos in /etc/shadow format\n"
           "\n"
           "Optional parameter options (set by default, added for compatibility):\n"
           "  --quiet (-q)                                 Don't print information or error texts to stdout/stderr\n"
           "  --extract-from-uri-encoding (-x)             Extracts arguments from an URI encoding\n"
          );
}


/// Function to print program information text of config_linux_user.
void ctparts_PrintInfoText(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                           char const * const WC_UNUSED_PARAM(szCallString))
{
  PrintHelp();

  return;
}


/// Function to allocate specific options struct and set the option defaults of config_linux_user.
statusCode_t ctparts_SetOptionDefaults(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                       ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

  WC_ASSERT(pstOptions->pstSpecificOptions == NULL);
  pstOptions->stCommonOptions.infoPrint = true;
  pstOptions->stCommonOptions.quiet = true;
  pstOptions->stCommonOptions.extractFromUri = true;
  pstOptions->pstSpecificOptions = malloc(sizeof(ctutil_OptionsSpecific_t));
  if(pstOptions->pstSpecificOptions == NULL)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    pstOptions->pstSpecificOptions->szUsername = NULL;
    pstOptions->pstSpecificOptions->szPassword1 = NULL;
    pstOptions->pstSpecificOptions->szPassword2 = NULL;
    pstOptions->pstSpecificOptions->szHash = NULL;
  }

  if(ctutil_IsStatusOk(status))
  {
    WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
  }
  else
  {
    WC_ASSERT(pstOptions->pstSpecificOptions == NULL);
  }

  return status;
}


/// Function to release and free the specific option struct of config_linux_user.
void ctparts_ReleaseSpecificOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                    ctutil_Options_t * const pstOptions)
{
  if(pstOptions->pstSpecificOptions != NULL)
  {
    free(pstOptions->pstSpecificOptions);
    pstOptions->pstSpecificOptions = NULL;
  }
}


/// Function to evaluate early options of config_linux_user.
statusCode_t ctparts_EvaluateEarlyOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                          int const argc,
                                          char * const argv[],
                                          ctutil_Options_t * const pstOptions)
{
  return ctutil_EvaluateEarlyOptions(argc, argv, pstOptions);
}


static statusCode_t ValidateOptions(ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

  if(pstOptions->pstSpecificOptions->szUsername == NULL)
  {
    status = CTUTIL_MISSING_PARAMETER;
    CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet,
                     "Found no user parameter option");
  }
  else
  {
    if(    (pstOptions->pstSpecificOptions->szPassword1 == NULL)
        && (pstOptions->pstSpecificOptions->szHash == NULL))
    {
      status = CTUTIL_MISSING_PARAMETER;
      CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet,
                       "Found whether password parameter option nor hash parameter option");
    }
  }
  if(pstOptions->pstSpecificOptions->szPassword2 != NULL)
  {
    if(pstOptions->pstSpecificOptions->szPassword1 == NULL)
    {
      status = CTUTIL_MISSING_PARAMETER;
      CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet,
                       "Found no password parameter option");
    }
  }
  if(pstOptions->pstSpecificOptions->szPassword1 != NULL)
  {
    if(pstOptions->pstSpecificOptions->szPassword2 == NULL)
    {
      status = CTUTIL_MISSING_PARAMETER;
      CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet,
                       "Found no password confirm parameter option");
    }
    else
    {
      if(strcmp(pstOptions->pstSpecificOptions->szPassword1, pstOptions->pstSpecificOptions->szPassword2) != 0)
      {
        status = CTUTIL_INVALID_PARAMETER;
        CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet,
                         "Password confirm parameter value does not match password parameter value");
      }
    }
    if(pstOptions->pstSpecificOptions->szHash != NULL)
    {
      pstOptions->pstSpecificOptions->szHash = NULL;
      CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet,
                      "Found additional hash parameter option, ignore option");
    }
  }

  return status;
}


/// Function to evaluate argument options of config_linux_user.
statusCode_t ctparts_EvaluateOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                     int const argc,
                                     char * const argv[],
                                     ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Parse arguments ( no dashes -> no getopt :( )
  do
  {
    for(int nr = 1; nr < argc; ++nr)
    {
      if(    (strcmp(argv[nr], "-h") == 0)
          || (strcmp(argv[nr], "--help") == 0))
      {
        PrintHelp(); // Do not care about quiet option because it is the standard behavior for this tool
        pstOptions->stCommonOptions.infoPrint = false;
        pstOptions->stCommonOptions.earlyExit = true;
        break;
      }
      if(!strncmp(argv[nr],"user=",strlen("user=")))
      {
        pstOptions->pstSpecificOptions->szUsername = argv[nr] + strlen("user=");
      }
      else if(!strncmp(argv[nr],"new-password=",strlen("new-password=")))
      {
        pstOptions->pstSpecificOptions->szPassword1 = argv[nr] + strlen("new-password=");
      }
      else if(!strncmp(argv[nr],"confirm-password=",strlen("confirm-password=")))
      {
        pstOptions->pstSpecificOptions->szPassword2 = argv[nr] + strlen("confirm-password=");
      }
      else if(!strncmp(argv[nr],"hash=",strlen("hash=")))
      {
        pstOptions->pstSpecificOptions->szHash = argv[nr] + strlen("hash=");
      }
    }
  }
  while(0);

  // Validate argument options if no exit planned
  if(!pstOptions->stCommonOptions.earlyExit)
  {
    status = ValidateOptions(pstOptions);
  }

  return status;
}


//---- End of source file ------------------------------------------------------

