//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2019-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     ctmain.c
///
///  \brief    Config tool specific main functionality for
///            detect_feature.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "ctutil/common_main.h"
#include "ctutil/log.h"
#include "ctutil/json.h"
#include "ctutil/common_functions.h"
#include "ctparts/common_main_defaults.h"
#include "ctparts/pmain.h"
#include "ctparts/ctmain.h"
#include "params.h"
#include "resources.h"

#include <inttypes.h>
#include <errno.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define MAX_FEATURES                                                        1024

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

/// Early function to prepare config tool main of detect_feature.
exitCode_t ctparts_PrepareMain(int const argc,
                               char * const argv[])
{
  // Prepare functions
  ctparts_commonMainFunctions_t stFunctions;
  ctparts_SetDefaultCommonMainFunctions(&stFunctions);

  // Call common main function to prepare config tool main
  return ctutil_CommonMain(argc, argv, &stFunctions, NULL);
}


static statusCode_t PrintTextAvailability(ctutil_OptionsSpecific_t const * const pstSpecificOptions,
                                          bool const available)
{
  statusCode_t status = CTUTIL_SUCCESS;

  char const * const value = available ? "true" : "false";
  int printResult = 0;
  if(pstSpecificOptions->onlyValue)
  {
    printResult = printf("%s", value);
  }
  else
  {
    printResult = printf("%s=%s", pstSpecificOptions->szFeature, value);
  }
  if(printResult < 0)
  {
    status = CTUTIL_FAILED;
  }

  return status;
}


static statusCode_t PrintJsonAvailability(ctutil_OptionsSpecific_t const * const pstSpecificOptions,
                                          bool const available)
{
  statusCode_t status = CTUTIL_SUCCESS;

  int printResult = 0;
  if(pstSpecificOptions->onlyValue)
  {
    printResult = printf("%s", available ? "true" : "false");
  }
  else
  {
    char szJsonObject[PATH_MAX + 1 + 5];
    size_t freeBufferSize = PATH_MAX + 1 + 5;
    char * szObjectStringPos = szJsonObject;
    size_t addedChars = 0;
    status = ctutil_JsonObjectStringOpen(szObjectStringPos, freeBufferSize, &addedChars);
    freeBufferSize    -= addedChars;
    szObjectStringPos += addedChars;

    if(ctutil_IsStatusOk(status))
    {
      status = ctutil_JsonObjectStringWriteBoolValue(szObjectStringPos, pstSpecificOptions->szFeature,
                                                     available, freeBufferSize, &addedChars);
      freeBufferSize    -= addedChars;
      szObjectStringPos += addedChars;
    }

    if(ctutil_IsStatusOk(status))
    {
      status = ctutil_JsonObjectStringClose(szObjectStringPos, freeBufferSize, &addedChars);
      freeBufferSize    -= addedChars;
      szObjectStringPos += addedChars;
    }

    if(ctutil_IsStatusOk(status))
    {
      printResult = printf("%s", szJsonObject);
    }
  }
  if(printResult < 0)
  {
    status = CTUTIL_FAILED;
  }

  return status;
}


static statusCode_t PrintTextFeatureList(char const * const * const arszFeatures,
                                         size_t const featureCount)
{
  statusCode_t status = CTUTIL_SUCCESS;

  for(size_t i = 0; i < featureCount; ++i)
  {
    int const printResult = printf("%s=%s\n", arszFeatures[i], "true");
    if(printResult < 0)
    {
      status = CTUTIL_FAILED;
      break;
    }
  }

  return status;
}


static statusCode_t PrintJsonFeatureList(char const * const * const arszFeatures,
                                         size_t const featureCount)
{
  statusCode_t status = CTUTIL_SUCCESS;

  char szJsonObject[(10 * PATH_MAX) + 1 + 5];
  size_t freeBufferSize = (10 * PATH_MAX) + 1 + 5;
  char * szObjectStringPos = szJsonObject;
  size_t addedChars = 0;
  status = ctutil_JsonObjectStringOpen(szObjectStringPos, freeBufferSize, &addedChars);
  freeBufferSize    -= addedChars;
  szObjectStringPos += addedChars;

  if(ctutil_IsStatusOk(status))
  {
    status = ctutil_JsonObjectStringWriteArrayStart(szObjectStringPos, "features", freeBufferSize, &addedChars);
    freeBufferSize    -= addedChars;
    szObjectStringPos += addedChars;
  }

  for(size_t i = 0; i < featureCount; ++i)
  {
    if(ctutil_IsStatusOk(status))
    {
      status = ctutil_JsonObjectStringWriteArrayStringValue(szObjectStringPos, arszFeatures[i], freeBufferSize, &addedChars);
      freeBufferSize    -= addedChars;
      szObjectStringPos += addedChars;
    }
  }

  if(ctutil_IsStatusOk(status))
  {
    status = ctutil_JsonObjectStringWriteArrayEnd(szObjectStringPos, freeBufferSize, &addedChars);
    freeBufferSize    -= addedChars;
    szObjectStringPos += addedChars;
  }

  if(ctutil_IsStatusOk(status))
  {
    status = ctutil_JsonObjectStringClose(szObjectStringPos, freeBufferSize, &addedChars);
    freeBufferSize    -= addedChars;
    szObjectStringPos += addedChars;
  }

  if(ctutil_IsStatusOk(status))
  {
    int const printResult = printf("%s", szJsonObject);
    if(printResult < 0)
    {
      status = CTUTIL_FAILED;
    }
  }

  return status;
}


static bool IsFeatureAvailable(ctutil_OptionsCommon_t const * const pstCommonOptions,
                               ctutil_Resources_t const * const pstResources,
                               char const * const szFeature)
{
    bool result = false;

    char const * const szDeviceFeaturePath = pstResources->pstSpecificResources->szDeviceFeaturePath;
    char szPathBuffer[PATH_MAX];
    strncpy(szPathBuffer, szDeviceFeaturePath, PATH_MAX - 1U);
    szPathBuffer[PATH_MAX - 1U] = '\0';
    strncat(szPathBuffer, "/", PATH_MAX - 1U);
    strncat(szPathBuffer, szFeature, PATH_MAX - 1U);
    char szResolvedPathBuffer[PATH_MAX];
    char const * const szResolvedPath = pstResources->pstSpecificResources->pfResolveRealPath(szPathBuffer, szResolvedPathBuffer);
    if(    (szResolvedPath != NULL)
        && (strncmp(szDeviceFeaturePath, szResolvedPath, strlen(szDeviceFeaturePath)) == 0)
        && (pstResources->pstSpecificResources->pfIsFileAvailable(pstCommonOptions, szResolvedPath, NULL))
        && (pstResources->pstSpecificResources->pfIsDependencyComplete(szResolvedPath)))
    {
      result = true;
    }

    return result;
}


static statusCode_t CreateFeatureList(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                      ctutil_Resources_t const * const pstResources,
                                      char const * * const arszFeatureMap,
                                      size_t const featureMapSize,
                                      size_t * const pFeatureCount)
{
    statusCode_t status = CTUTIL_SUCCESS;

  DIR * pDirectory = pstResources->pstSpecificResources->pfOpenDir(pstResources->pstSpecificResources->szDeviceFeaturePath);
  if(pDirectory == NULL)
  {
    status = CTUTIL_SYSTEM_CALL_ERROR;
    char const * const szErrorString = strerror(errno);
    CTUTIL_LOG_DEBUG(pstCommonOptions->quiet, szErrorString);
    CTUTIL_LOG_STATUS_ERROR(pstCommonOptions->quiet, status,
                            "Failed to get feature files from feature directory");
  }
  else
  {
    struct dirent * pDirectoryEntry;
    while((pDirectoryEntry = pstResources->pstSpecificResources->pfReadDir(pDirectory)) != NULL)
    {
      if(pDirectoryEntry->d_name[0] != '.')
      {
        if(IsFeatureAvailable(pstCommonOptions, pstResources, pDirectoryEntry->d_name))
        {
          if(*pFeatureCount >= featureMapSize)
          {
            CTUTIL_LOG_WARN(pstCommonOptions->quiet, "Found to many features, not all features will be listed");
            break;
          }
          arszFeatureMap[*pFeatureCount] = strdup(pDirectoryEntry->d_name);
          ++(*pFeatureCount);
        }
      }
    }
    pstResources->pstSpecificResources->pfCloseDir(pDirectory);
  }

  return status;
}


/// Main function for config tool core functionality (for example implementation see HelloWorld example config tool).
exitCode_t ctparts_ctMain(int const WC_UNUSED_PARAM(argc),
                          char * const WC_UNUSED_PARAM(argv[]),
                          ctutil_Options_t const * const pstOptions,
                          ctutil_Resources_t const * const pstResources)
{
  exitCode_t result = CTUTIL_EXIT_SUCCESS;
  statusCode_t status = CTUTIL_SUCCESS;

  if(pstOptions->pstSpecificOptions->listMode)
  {
    char const * arszFeatures[MAX_FEATURES] = { NULL };
    size_t featureCount = 0;
    status = CreateFeatureList(&(pstOptions->stCommonOptions), pstResources, arszFeatures, MAX_FEATURES, &featureCount);

    if(ctutil_IsStatusOk(status) && pstOptions->stCommonOptions.textOutput)
    {
      status = PrintTextFeatureList(arszFeatures, featureCount);
    }
    if(ctutil_IsStatusOk(status) && pstOptions->stCommonOptions.jsonOutput)
    {
      status = PrintJsonFeatureList(arszFeatures, featureCount);
    }

    for(size_t i = 0; (i < MAX_FEATURES) && (arszFeatures[i] != NULL); ++i)
    {
      ctutil_FreeConst(arszFeatures[i]);
    }
  }
  else if(pstOptions->pstSpecificOptions->szFeature != NULL)
  {
    bool const available = IsFeatureAvailable(&(pstOptions->stCommonOptions), pstResources, pstOptions->pstSpecificOptions->szFeature);
    if(ctutil_IsStatusOk(status) && pstOptions->stCommonOptions.textOutput)
    {
      status = PrintTextAvailability(pstOptions->pstSpecificOptions, available);
    }
    if(ctutil_IsStatusOk(status) && pstOptions->stCommonOptions.jsonOutput)
    {
      status = PrintJsonAvailability(pstOptions->pstSpecificOptions, available);
    }
    bool const outputOptionSet = pstOptions->stCommonOptions.textOutput || pstOptions->stCommonOptions.jsonOutput;
    if(ctutil_IsStatusOk(status) && (!pstOptions->stCommonOptions.quiet) && outputOptionSet)
    {
      // Enhance readability for human users when not called with quiet parameter for machine/software usage
      printf("\n");
    }
  }
  else
  {
    CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet, "No option given, nothing to do");
  }
  if(!ctutil_IsStatusOk(status))
  {
    result = CTUTIL_EXIT_GENERAL_ERROR;
  }

  return result;
}


//---- End of source file ------------------------------------------------------
