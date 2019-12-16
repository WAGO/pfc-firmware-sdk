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
///  \file     params.c
///
///  \brief    Config tool specific parameter definition and handling for
///            reserved_area.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "wc/std_includes.h"
#include "ctutil/log.h"
#include "ctparts/common_main_defaults.h"
#include "params.h"
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#ifndef WAGO_CT_NAME
/// Config tool name (used in program help and info texts).
#define WAGO_CT_NAME          "config_rmd"
#endif
/// Config tool title line (used in program help and info texts).
#define WAGO_CT_TITLE         "* Command line interface tool for handling the RMD-Rocket Master Device *"

/// Type definition for argument options returned by getopt() and getopt_long().
typedef int argOption_type;

/// Type definition for argument option index used by getopt() and getopt_long().
typedef int argOptionIndex_type;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
static const char infoText[] =
  WAGO_CT_TITLE "\n"
  "\n"
  "For detailed usage information use:\n"
  "  " WAGO_CT_NAME " --help\n";

static const char helpText[] =
  WAGO_CT_TITLE "\n"
  "\n"
  "Usage:\n"
  "  " WAGO_CT_NAME " <command option> [<param>]\n";

static const char additionalText[] =
  "Additional information:\n"
  "  This tool should be used exclusively to manage RMD device.\n";

static const char exampleText[] =
  "Examples:\n"
  "  " WAGO_CT_NAME " -c \n"
  "    Checks the FPGA image version\n"
  "\n"
  "  " WAGO_CT_NAME " -r \n"
  "    Resets the FPGA\n"
  "\n";

static char const szCommandlineOptions[] = "hcr";

static struct option arstCommandlineOptions[] =
{
  { "help",                        no_argument,       NULL, 'h' },
  { "check",                       no_argument,       NULL, 'c' },
  { "reset",                       no_argument,       NULL, 'r' },
  { NULL,                          no_argument,       NULL,  0  } // End marker, don't remove
};


static char const * const arszCommandlineOptionTexts[] =
{
  "Prints this help text",
  "Checks the FPGA image version",
  "Resets the FPGA",
  NULL // End marker, don't remove
};

// Count of CommandlineOptions and CommandlineOptionTexts have to be equal
WC_STATIC_ASSERT((sizeof(arstCommandlineOptions) / sizeof(struct option)) == (sizeof(arszCommandlineOptionTexts) / sizeof(char const *)));

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

// TODO: Check for possible default implementation in libconfigtoolutils
/// Function to print program information text (for example implementation see HelloWorld example config tool).
void ctparts_PrintInfoText(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                           char const * const szCallString)
{
  WC_ASSERT(szCallString[0] != '\0');
  printf("Programm call: %s\n", szCallString);
  printf("\n");
  printf(infoText);
  printf("\n");

  return;
}


/// Function to allocate specific options struct and set the option defaults (for example implementation see HelloWorld example config tool).
statusCode_t ctparts_SetOptionDefaults(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                       ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

  WC_ASSERT(pstOptions->pstSpecificOptions == NULL);
  pstOptions->stCommonOptions.infoPrint = true;
  pstOptions->pstSpecificOptions = calloc(1, sizeof(ctutil_OptionsSpecific_t));
  if(pstOptions->pstSpecificOptions == NULL)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    pstOptions->pstSpecificOptions->checkFPGAVersion = false;
    pstOptions->pstSpecificOptions->resetFPGA = false;
    pstOptions->stCommonOptions.quiet = false;
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


/// Function to release and free the specific option struct (for example implementation see HelloWorld example config tool).
void ctparts_ReleaseSpecificOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                    ctutil_Options_t * const pstOptions)
{
  if(pstOptions->pstSpecificOptions != NULL)
  {
    free(pstOptions->pstSpecificOptions);
    pstOptions->pstSpecificOptions = NULL;
  }
}


// TODO: Check for use in libconfigtoolutils
static void PrintCommandOptions(void)
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


// TODO: Check for use in libconfigtoolutils
static void PrintHelp(char const * const szName)
{
  printf("Program call: %s\n", szName);
  printf("\n");
  printf(helpText);
  printf("\n");
  PrintCommandOptions();
  printf("\n");
  printf(additionalText);
  printf("\n");
  printf(exampleText);
  return;
}


/// Function to evaluate early options (for example implementation see HelloWorld example config tool).
statusCode_t ctparts_EvaluateEarlyOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                          int const argc,
                                          char * const argv[],
                                          ctutil_Options_t * const pstOptions)
{
  return ctutil_EvaluateEarlyOptions(argc, argv, pstOptions);
}


static void ValidateOptions(ctutil_Options_t * const pstOptions)
{

  (void)(pstOptions);
  return;
}


/// Function to evaluate argument options (for example implementation see HelloWorld example config tool).
statusCode_t ctparts_EvaluateOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                     int const argc,
                                     char * const argv[],
                                     ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Get argument options
  argOption_type option;
  argOptionIndex_type optionIndex;
  if(pstOptions->stCommonOptions.quiet)
  {
    opterr = 0;
  }
  optind = 0;
  while(    (ctutil_IsStatusOk(status))
         && ((option = getopt_long(argc, argv, szCommandlineOptions, arstCommandlineOptions, &optionIndex)) != -1)) //lint !e960 !e1960 suggested usage of getopt
  {
    switch(option)
    {
      case 'h':
        if(!pstOptions->stCommonOptions.quiet)
        {
          PrintHelp(argv[0]);
        }
        pstOptions->stCommonOptions.infoPrint = false;
        pstOptions->stCommonOptions.earlyExit = true;
        break;
      case 'c':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->checkFPGAVersion = true;
        break;
      case 'r':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->resetFPGA = true;
        break;
      case '0':
        if(arstCommandlineOptions[optionIndex].flag != NULL)
        {
          // Option sets a flag, nothing else to do
          break;
        }
        //lint -fallthrough, no break
      default:
        //lint -fallthrough, no break
      case '?':
        // Wrong option character or option with missing argument stored in global variable "optopt"
        // getopt_long already printed an error message in case of '?' (if opterr != 0)
        status = CTUTIL_INVALID_PARAMETER;
        CTUTIL_LOG_STATUS_ERROR(pstOptions->stCommonOptions.quiet, status,
                                "Wrong option character or option with missing argument");
        pstOptions->stCommonOptions.earlyExit = true;
        break;
    }
  }

  // Validate argument options if no exit planned
  if(!pstOptions->stCommonOptions.earlyExit)
  {
    ValidateOptions(pstOptions);
  }

  return status;
}


//---- End of source file ------------------------------------------------------

