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
///  \file     params.c
///
///  \brief    Config tool specific parameter definition and handling for
///            detect_feature.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "wc/std_includes.h"
#include "ctutil/log.h"
#include "ctutil/common_functions.h"
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
#define WAGO_CT_NAME          "detect_feature"
#endif
/// Config tool title line (used in program help and info texts).
#define WAGO_CT_TITLE         "* Check if a specific feature exist in the actual feature set *"

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
  "  This tool may be used to check if a feature is currently available.\n";

static const char exampleText[] =
  "Examples:\n"
  "  " WAGO_CT_NAME " --list -j\n"
  "    Prints a JSON list of currently installed features\n"
  "\n"
  "  " WAGO_CT_NAME " --feature \"feature-a\" --json --quiet\n"
  "    Check for feature \"Feature A\" and print result as machine readable (quiet, no information texts) JSON\n"
  "\n"
  "  " WAGO_CT_NAME " -f \"feature-a\" -t\n"
  "    Check for feature \"Feature A\" and print result as human readable plain text\n";

static char const szCommandlineOptions[] = "hxqtjf:ol";

static struct option arstCommandlineOptions[] =
{
  { "help",                        no_argument,       NULL, 'h' },
  { "extract-from-uri-encoding",   no_argument,       NULL, 'x' },
  { "quiet",                       no_argument,       NULL, 'q' },
  { "text",                        no_argument,       NULL, 't' },
  { "json",                        no_argument,       NULL, 'j' },
  { "feature",                     required_argument, NULL, 'f' },
  { "only-value",                  no_argument,       NULL, 'o' },
  { "list",                        no_argument,       NULL, 'l' },
  { NULL,                          no_argument,       NULL,  0  } // End marker, don't remove
};

// Count of CommandlineOptions and short options (subtract ':') should to be equal
WC_STATIC_ASSERT((sizeof(arstCommandlineOptions) / sizeof(struct option)) == (sizeof(szCommandlineOptions) - 1));

static char const * const arszCommandlineOptionTexts[] =
{
  "Prints this help text",
  "Extracts arguments from an URI encoding",
  "Don't print information or error texts to stdout/stderr",
  "Prints output as name=value pairs, one per line",
  "Prints output as JSON format string",
  "Feature to check for availability",
  "Print only value without name, option is only valid combined with --feature/-f",
  "List all available features instead of checking just one specific feature",
  NULL // End marker, don't remove
};

// Count of CommandlineOptions and CommandlineOptionTexts have to be equal
WC_STATIC_ASSERT((sizeof(arstCommandlineOptions) / sizeof(struct option)) == (sizeof(arszCommandlineOptionTexts) / sizeof(char const *)));

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

/// Function to print program information text (for example implementation see HelloWorld example config tool).
void ctparts_PrintInfoText(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                           char const * const szCallString)
{
    ctutil_PrintInfoTextDefault(szCallString, infoText);
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
      pstOptions->stCommonOptions.earlyExit     = true;
      pstOptions->pstSpecificOptions->szFeature = NULL;
      pstOptions->pstSpecificOptions->onlyValue = false;
      pstOptions->pstSpecificOptions->listMode  = false;
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


static void PrintHelp(char const * const szCallString)
{
  ctutil_PrintHelpDefault(szCallString, helpText, additionalText, exampleText,
                          arstCommandlineOptions, arszCommandlineOptionTexts);
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
  if(    (pstOptions->pstSpecificOptions->szFeature != NULL)
      && (pstOptions->pstSpecificOptions->listMode))
  {
    CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet,
                    "Found parameter option 'feature/f' but list mode option 'list/l' is given, ignore option");
    pstOptions->pstSpecificOptions->szFeature = NULL;
  }
  if(    (pstOptions->pstSpecificOptions->onlyValue)
      && (pstOptions->pstSpecificOptions->szFeature == NULL))
  {
    CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet,
                    "Found parameter option 'only-value/o' but option is only valid in combination with 'feature/f', ignore option");
    pstOptions->pstSpecificOptions->onlyValue = false;
  }

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
  bool helpRequested = false;
  while(    (ctutil_IsStatusOk(status))
         && (!helpRequested)
         && ((option = getopt_long(argc, argv, szCommandlineOptions, arstCommandlineOptions, &optionIndex)) != -1)) //lint !e960 !e1960 suggested usage of getopt
  {
    switch(option)
    {
      case 'h':
        helpRequested = true;
        if(!pstOptions->stCommonOptions.quiet)
        {
          PrintHelp(argv[0]);
        }
        pstOptions->stCommonOptions.infoPrint = false;
        pstOptions->stCommonOptions.earlyExit = true;
        break;

      case 'x':
      case 'q':
        // Already done as first step, nothing else to do here
        break;

      case 't':
        pstOptions->stCommonOptions.infoPrint = false;
        pstOptions->stCommonOptions.textOutput = true;
        break;

      case 'j':
        pstOptions->stCommonOptions.infoPrint = false;
        pstOptions->stCommonOptions.jsonOutput = true;
        break;

      case 'f':
        pstOptions->stCommonOptions.infoPrint = false;
        pstOptions->stCommonOptions.earlyExit = false;
        if(    (optarg == NULL)
            || (optarg[0] == '\0'))
        {
          status = CTUTIL_MISSING_PARAMETER;
          CTUTIL_LOG_STATUS_ERROR(pstOptions->stCommonOptions.quiet, status, "Parameter option f");
        }
        else
        {
          WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
          pstOptions->pstSpecificOptions->szFeature = optarg;
        }
        break;

      case 'o':
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->onlyValue = true;
        break;

      case 'l':
        pstOptions->stCommonOptions.infoPrint = false;
        pstOptions->stCommonOptions.earlyExit = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->listMode = true;
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

