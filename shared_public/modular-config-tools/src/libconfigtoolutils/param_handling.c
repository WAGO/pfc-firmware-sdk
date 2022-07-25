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
///  \file     param_handling.c
///
///  \brief    Helper functions for config tool parameter handling.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "wc/structuring.h"
#include "wc/std_includes.h"
#include "ctutil/param_handling.h"
#include "ctutil/log.h"
#include <stdlib.h>
#include <string.h>
#include <glib.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
typedef struct ctutil_optionFace
{
  char const * const szName;
  int const code;
} ctutil_optionFace_t;


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
static ctutil_optionFace_t earlyOptions[] =
{
  { "quiet",                                   'q' },
  { "extract-from-uri-encoding",               'x' },
  { NULL,                                       0  } // End marker, don't remove
};


//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
void ctutil_InitOptions(ctutil_Options_t * const pstOptions)
{
  memset(pstOptions, 0, sizeof(*pstOptions));
}


statusCode_t ctutil_DecodeUriArgument(char * const szArgument)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Decode URI argument and rewrite/replace argument (decoded argument is always shorter)
  gpointer pUriDecodedParam = g_uri_unescape_string(szArgument, NULL);
  status = (pUriDecodedParam != NULL) ? CTUTIL_SUCCESS : CTUTIL_FAILED;
  if(ctutil_IsStatusOk(status))
  {
    char const * const szUriDecodedParam = pUriDecodedParam;

    // Check assumption that decoded string is always shorter than encoded string
    WC_ASSERT(strlen(szUriDecodedParam) <= strlen(szArgument));

    // Replace original argument string
    size_t  maxParamLength = strlen(szArgument);
    strncpy(szArgument, szUriDecodedParam, maxParamLength);
    szArgument[maxParamLength] = '\0';
    g_free(pUriDecodedParam);
  }

  return status;
}


// TODO: Check for publishing in libconfigtoolutils
static bool ContainsOption(char const * const szTestString,
                           ctutil_optionFace_t const * const pOptionFace)
{
  bool result = false;

  if(szTestString != NULL)
  {
    if(szTestString[0] == '-')
    {
      if(szTestString[1] == '-')
      {
        WC_ASSERT(pOptionFace->szName != NULL);
        if(strcmp(szTestString + 2, pOptionFace->szName) == 0)
        {
          result = true;
        }
      }
      else
      {
        for(unsigned i = 1; szTestString[i] != '\0'; ++i)
        {
          if(szTestString[i] == pOptionFace->code)
          {
            result = true;
          }
        }
      }
    }
  }

  return result;
}


statusCode_t ctutil_EvaluateEarlyOptions(int const argc,
                                         char * const argv[],
                                         ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Check all arguments for quiet option
  for(int i = 0; i < argc; ++i)
  {
    WC_ASSERT(earlyOptions[0].code == 'q');
    if(ContainsOption(argv[i], &(earlyOptions[0])))
    {
      pstOptions->stCommonOptions.quiet = true;
      break;
    }
  }

  // Check all arguments for URI extraction option
  for(int i = 0; i < argc; ++i)
  {
    WC_ASSERT(earlyOptions[1].code == 'x');
    if(    pstOptions->stCommonOptions.extractFromUri
        || (ContainsOption(argv[i], &(earlyOptions[1]))))
    {
      pstOptions->stCommonOptions.extractFromUri = true;
      for(int j = 0; j < argc; ++j)
      {
        status = ctutil_DecodeUriArgument(argv[j]);
        if(ctutil_IsStatusFailure(status))
        {
          CTUTIL_LOG_STATUS_ERROR(pstOptions->stCommonOptions.quiet, status, "Failed to extract argument from URI encoding");
        }
      }
      break;
    }
  }

  return status;
}


//---- End of source file ------------------------------------------------------

