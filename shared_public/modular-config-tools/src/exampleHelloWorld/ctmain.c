//------------------------------------------------------------------------------
// Copyright 2017 WAGO GmbH & Co. KG
//
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     ctmain.c
///
///  \brief    Config tool specific main functionality for
///            HelloWorld example.
///
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
#include <stdio.h>

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

/// Early function for to prepare config tool main (for example implementation see HelloWorld example config tool).
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
statusCode_t ctparts_InitSpecificResources(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                           ctutil_ResourcesSpecific_t * * const ppstResources)
{
  WC_ASSERT(*ppstResources == NULL);
  (void)ppstResources;

  // No specific resources to initialize
  return CTUTIL_SUCCESS;
}


/// Function to release config tool specific resources (for example implementation see HelloWorld example config tool).
void ctparts_ReleaseSpecificResources(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                      ctutil_ResourcesSpecific_t * * const ppstResources)
{
  // Currently there should be no pointer
  WC_ASSERT(*ppstResources == NULL);
  (void)ppstResources;

  return;
}


/// Main function for config tool core functionality (for example implementation see HelloWorld example config tool).
exitCode_t ctparts_ctMain(int const WC_UNUSED_PARAM(argc),
                          char * const WC_UNUSED_PARAM(argv[]),
                          ctutil_Options_t const * const pstOptions,
                          ctutil_Resources_t const * const WC_UNUSED_PARAM(pstResources))
{
  // Check for user defined output text
  char const * szText = "Hello World";
  if(pstOptions->pstSpecificOptions->szCustomText != NULL)
  {
    szText = pstOptions->pstSpecificOptions->szCustomText;
  }

  // Print output text
  if(pstOptions->stCommonOptions.textOutput)
  {
    printf("outText=%s", szText);
  }
  if(pstOptions->stCommonOptions.jsonOutput)
  {
    printf("{\"outText\": \"%s\"}", szText);
  }

  return CTUTIL_EXIT_SUCCESS;
}


//---- End of source file ------------------------------------------------------

