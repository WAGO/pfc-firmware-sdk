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
///  \file     resources.c
///
///  \brief    Config tool specific resource definition and handling for
///            config_linux_user.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "ctparts/common_main_defaults.h"
#include "resources.h"
#include <stdlib.h>
#include <paths.h>

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

/// Function to initialize config tool specific resources of config_linux_user.
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
    // If random does not get enough bytes use urandom
    pstResources->szRandomSource1 = "/dev/random";
    pstResources->szRandomSource2 = "/dev/urandom";
    status = ctutil_PrepareChangeableFile(_PATH_SHADOW, &(pstResources->pShadowFile));
    if(ctutil_IsStatusFailure(status))
    {
      free(pstResources);
      *ppstResources = NULL;
    }
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


/// Function to release config tool specific resources of config_linux_user.
void ctparts_ReleaseSpecificResources(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                      ctutil_ResourcesSpecific_t * * const ppstResources)
{
  ctutil_ResourcesSpecific_t * const pstResources = *ppstResources;
  if(pstResources != NULL)
  {
    ctutil_ReleaseFileForChanges(&(pstResources->pShadowFile));
    free(pstResources);
    *ppstResources = NULL;
  }

  return;
}


//---- End of source file ------------------------------------------------------

