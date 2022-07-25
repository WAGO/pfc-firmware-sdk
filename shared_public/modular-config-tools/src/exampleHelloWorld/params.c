//------------------------------------------------------------------------------
// Copyright 2017-2020 WAGO GmbH & Co. KG
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
///  \file     params.c
///
///  \brief    Config tool specific parameter definition and handling for
///            HelloWorld example.
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
#define WAGO_CT_NAME          "helloworld"
#endif
/// Config tool title line (used in program help and info texts).
#define WAGO_CT_TITLE         "* Command line interface tool for hello world example demonstration *"

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
  "  This tool is an example to show how to build a modular config tool.\n";

static const char exampleText[] =
  "Examples:\n"
  "  " WAGO_CT_NAME " -t\n"
  "    Prints result as key/value text pair: outText=Hello World\n"
  "\n"
  "  " WAGO_CT_NAME " --json\n"
  "    Prints result as JSON object: {\"outText\": \"Hello World\"}\n"
  "\n"
  "  " WAGO_CT_NAME " -t --out=\"Hi WAGO\"\n"
  "    Prints \"Hi WAGO\" instead of an \"Hello World\"\n";

static char const szCommandlineOptions[] = "hxqtjo:";

static struct option arstCommandlineOptions[] =
{
  { "help",                        no_argument,       NULL, 'h' },
  { "extract-from-uri-encoding",   no_argument,       NULL, 'x' },
  { "quiet",                       no_argument,       NULL, 'q' },
  { "text",                        no_argument,       NULL, 't' },
  { "json",                        no_argument,       NULL, 'j' },
  { "out",                         required_argument, NULL, 'o' },
  { NULL,                          no_argument,       NULL,  0  } // End marker, don't remove
};

static char const * const arszCommandlineOptionTexts[] =
{
  "Prints this help text",
  "Extracts arguments from an URI encoding",
  "Don't print information or error texts to stdout/stderr",
  "Prints parameter output as name=value pairs, one per line (enabled as default)",
  "Prints parameter output as JSON format string",
  "Prints given text instead of an Hello World (param: <text to print>)",
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
  pstOptions->pstSpecificOptions = malloc(sizeof(ctutil_OptionsSpecific_t));
  if(pstOptions->pstSpecificOptions == NULL)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    pstOptions->pstSpecificOptions->szCustomText = NULL;
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
  // FIXME: Free internals (memory leak)
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


/// Function to evaluate argument options (for example implementation see HelloWorld example config tool).
statusCode_t ctparts_EvaluateOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                     int const argc,
                                     char * const argv[],
                                     ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

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

      case 'o':
        if(    (optarg == NULL)
            || (optarg[0] == '\0'))
        {
          status = CTUTIL_MISSING_PARAMETER;
          CTUTIL_LOG_STATUS_ERROR(pstOptions->stCommonOptions.quiet, status, "Parameter option o");
        }
        else
        {
          WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
          status = ctutil_CopyStringParam(&(pstOptions->pstSpecificOptions->szCustomText), optarg);
        }
        pstOptions->stCommonOptions.infoPrint = false;
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
        CTUTIL_LOG_STATUS_ERROR(pstOptions->stCommonOptions.quiet, status, "Wrong option character or option with missing argument");
        break;
    }
  }

  return status;
}


//---- End of source file ------------------------------------------------------

