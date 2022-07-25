//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2020-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     common_main.c
///
///  \brief    Commonly used functions and default implementations.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "wc/std_includes.h"
#include "ctutil/common_functions.h"
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
void ctutil_PrintInfoTextDefault(char const * const szCallString,
                                 char const * const szInfoText)
{
  WC_ASSERT(szCallString[0] != '\0');
  WC_ASSERT(szInfoText[0] != '\0');
  printf("Programm call: %s\n", szCallString);
  printf("\n");
  printf(szInfoText);
  printf("\n");

  return;
}


static void PrintCommandOptions(struct option const * const arstCommandlineOptions,
                                char const * const * const arszCommandlineOptionTexts)
{
  printf("Application options:\n");
  size_t i = 0U;
  while(arstCommandlineOptions[i].name != NULL)
  {
    char const * szParam = "";
    if(arstCommandlineOptions[i].has_arg == required_argument)
    {
      szParam = "<param>";
    }
    if(arstCommandlineOptions[i].has_arg == optional_argument)
    {
      szParam = "[param]";
    }
    printf("  --%-25s %-7s   -%c   %s\n", arstCommandlineOptions[i].name,
                                          szParam,
                                          arstCommandlineOptions[i].val,
                                          arszCommandlineOptionTexts[i]);
    i++;
  }

  return;
}


void ctutil_PrintHelpDefault(char const * const szCallString,
                             char const * const szHelpText,
                             char const * const szAdditionalText,
                             char const * const szExampleText,
                             struct option const * const arstCommandlineOptions,
                             char const * const * const arszCommandlineOptionTexts)
{
    printf("Program call: %s\n", szCallString);
    printf("\n");
    printf(szHelpText);
    printf("\n");
    PrintCommandOptions(arstCommandlineOptions, arszCommandlineOptionTexts);
    printf("\n");
    printf(szAdditionalText);
    printf("\n");
    printf(szExampleText);
    return;
}


void ctutil_FreeConst(void const * const pMemoryToFree)
{
  // The following pointer arithmetics removes the 'const' attribute from pointer avoiding any warnings
  char const * const pConstMemory = pMemoryToFree;
  char * pMemory = 0x0;
  pMemory = pMemory + (pConstMemory - pMemory);
  free(pMemory);
}


statusCode_t ctutil_CopyStringParam(char const * * const pszStringParam,
                                    char const * const szOptionArg)
{
  statusCode_t status = CTUTIL_SUCCESS;

  if((*pszStringParam) != NULL)
  {
    ctutil_FreeConst(*pszStringParam);
  }
  size_t const argLen = strlen(szOptionArg);
  char * szOptionArgCopy = malloc(argLen + 1);
  if(szOptionArgCopy == NULL)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    strncpy(szOptionArgCopy, szOptionArg, argLen);
    szOptionArgCopy[argLen] = '\0';
    (*pszStringParam) = szOptionArgCopy;
  }

  return status;
}


//---- End of source file ------------------------------------------------------

