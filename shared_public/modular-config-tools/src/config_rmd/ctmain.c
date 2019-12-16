//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     ctmain.c
///
///  \brief    Config tool specific main functionality for
///            reserved_area.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "ctutil/common_main.h"
#include "ctutil/log.h"
#include "ctparts/common_main_defaults.h"
#include "ctparts/pmain.h"
#include "ctparts/ctmain.h"
#include "params.h"
#include "rmd.h"
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

/// Function to initialize config tool specific resources (for example implementation see HelloWorld example config tool).
statusCode_t ctparts_InitSpecificResources(ctparts_commonMainContext_t * const WC_UNUSED_PARAM( pstContext),
                                           ctutil_ResourcesSpecific_t * * const ppstResources)
{
    WC_ASSERT(*ppstResources == NULL);
    (void)ppstResources;

    // No specific resources to initialize
    return CTUTIL_SUCCESS;
}

/// Function to release config tool specific resources (for example implementation see HelloWorld example config tool).
void ctparts_ReleaseSpecificResources(ctparts_commonMainContext_t * const WC_UNUSED_PARAM( pstContext),
                                      ctutil_ResourcesSpecific_t * * const ppstResources)
{
    // Currently there should be no pointer
    WC_ASSERT(*ppstResources == NULL);
    (void)ppstResources;

    return;
}

/// Main function for config tool core functionality (for example implementation see HelloWorld example config tool).
exitCode_t ctparts_ctMain(int const WC_UNUSED_PARAM( argc),
                          char * const WC_UNUSED_PARAM( argv[]),
                          ctutil_Options_t const * const pstOptions,
                          ctutil_Resources_t const * const pstResources)
{
    (void)(pstResources);
    exitCode_t result = CTUTIL_EXIT_SUCCESS;
    //statusCode_t status = CTUTIL_SUCCESS;

    // Clear area if requested
    if(pstOptions->pstSpecificOptions->checkFPGAVersion)
    {
        if(!check_fpga(pstOptions))
        {
            result = CTUTIL_EXIT_GENERAL_ERROR;
        }
    }

    if(pstOptions->pstSpecificOptions->resetFPGA)
    {
        if(!reset_fpga(pstOptions))
        {
            result = CTUTIL_EXIT_GENERAL_ERROR;
        }
    }

    return result;
}

//---- End of source file ------------------------------------------------------

