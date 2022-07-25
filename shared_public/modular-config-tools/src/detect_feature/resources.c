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
///  \file     resources.c
///
///  \brief    Config tool specific resource definition and handling for
///            detect_feature.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "ctparts/common_main_defaults.h"
#include "ctutil/filesystem.h"
#include "resources.h"
#include "params.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Define for feature file path on device.
#define FEATURE_PATH_DEVICE   "/etc/specific/features"

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

static bool IsDependencyComplete(char const * const szFilePath)
{
  bool result = true;

  // Open file stream
  FILE * const pFileStream = fopen(szFilePath, "r");
  if(pFileStream == NULL)
  {
    result = false;
  }

  // Check dependencies line by line
  if(pFileStream != NULL)
  {
    char * szLineBuffer = NULL;
    size_t bufferLength = 0;
    ssize_t readChars;
    while((readChars = getline(&szLineBuffer, &bufferLength, pFileStream)) != -1)
    {
      if(bufferLength > 0)
      {
        char const * const szValueStart = strchr(szLineBuffer, '=');
        if(szValueStart != NULL)
        {
          if(    (strcmp(szValueStart, "=true") == 0)
              || (strcmp(szValueStart, "=true\n") == 0)
              || (strcmp(szValueStart, "=true\r\n") == 0)
              || (strcmp(szValueStart, "=true\n\r") == 0))
          {
              result = true;
          }
          else
          {
              result = false;
          }
        }
      }
    }
    fclose(pFileStream);

    // Free line buffer
    if(szLineBuffer != NULL)
    {
      free(szLineBuffer);
    }
  }

  return result;
}


/// Function to initialize config tool specific resources of detect_feature.
statusCode_t ctparts_InitSpecificResources(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                           ctutil_ResourcesSpecific_t * * const ppstResources)
{
  statusCode_t status = CTUTIL_SUCCESS;

  WC_ASSERT(*ppstResources == NULL);
  *ppstResources = calloc(1, sizeof(ctutil_ResourcesSpecific_t));
  ctutil_ResourcesSpecific_t * const pstResources = *ppstResources;
  if(pstResources == NULL)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    pstResources->szDeviceFeaturePath = FEATURE_PATH_DEVICE;
    pstResources->pfResolveRealPath = &realpath;
    pstResources->pfIsFileAvailable = &ctutil_IsFileAvailable;
    pstResources->pfIsDependencyComplete = &IsDependencyComplete;
    pstResources->pfOpenDir = &opendir;
    pstResources->pfReadDir = &readdir;
    pstResources->pfCloseDir = &closedir;
  }

  if(ctutil_IsStatusOk(status))
  {
    WC_ASSERT(*ppstResources != NULL);
  }
  else
  {
    WC_ASSERT(*ppstResources == NULL);
  }

  return status;
}


/// Function to release config tool specific resources of detect_feature.
void ctparts_ReleaseSpecificResources(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                      ctutil_ResourcesSpecific_t * * const ppstResources)
{
  ctutil_ResourcesSpecific_t * const pstResources = *ppstResources;
  if(pstResources != NULL)
  {
    free(pstResources);
    *ppstResources = NULL;
  }

  return;
}


//---- End of source file ------------------------------------------------------

