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
///  \file     params.c
///
///  \brief    Config tool specific parameter definition and handling for
///            reserved_area.
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
#define WAGO_CT_NAME          "reserved_area"
#endif
/// Config tool title line (used in program help and info texts).
#define WAGO_CT_TITLE         "* Command line interface tool for handling of reserved flash area *"

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
  "  This tool should be used exclusively to manage the reserved partition.\n";

static const char exampleText[] =
  "Examples:\n"
  "  " WAGO_CT_NAME " -m /tmp/reserved\n"
  "    Mounts reserved partition to /tmp/reserved\n"
  "\n"
  "  " WAGO_CT_NAME " -fm /tmp/reserved --json\n"
  "    Mounts reserved partition to /tmp/reserved and prints free capacity in bytes as JSON object:\n"
  "    {\"freeCapacity\": \"126577\"}\n"
  "\n"
  "  " WAGO_CT_NAME " -m /tmp/reserved -c\n"
  "    Mounts reserved partition to /tmp/reserved and removes any content\n";

static char const szCommandlineOptions[] = "hxqtjm:urCfapc";

static struct option arstCommandlineOptions[] =
{
  { "help",                        no_argument,       NULL, 'h' },
  { "extract-from-uri-encoding",   no_argument,       NULL, 'x' },
  { "quiet",                       no_argument,       NULL, 'q' },
  { "text",                        no_argument,       NULL, 't' },
  { "json",                        no_argument,       NULL, 'j' },
  //{ "backward-compatibility",      no_argument,       NULL, 'b' },
  { "mount",                       required_argument, NULL, 'm' },
  { "umount",                      no_argument,       NULL, 'u' },
  { "readonly",                    no_argument,       NULL, 'r' },
  { "create",                      no_argument,       NULL, 'C' },
  { "print-free-capacity",         no_argument,       NULL, 'f' },
  { "print-available-capacity",    no_argument,       NULL, 'a' },
  { "print-mount-point",           no_argument,       NULL, 'p' },
  { "clear",                       no_argument,       NULL, 'c' },
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
  //"Use compatibility mode if no reserved area present",
  "Mount reserved partition to given target directory",
  "Unmount reserved partition",
  "Mount reserved partition readonly (requires -m)",
  "Create given mount point if necessary (requires -m)",
  "Print out free capacity of reserved partition",
  "Print out (user) available capacity of reserved partition",
  "Print out mount point of reserved partition (\"" MARKER_NOT_MOUNTED "\" if not mounted)",
  "Clear reserved partition, all content will be removed",
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
    pstOptions->pstSpecificOptions->compatibilityMode = true;
    pstOptions->pstSpecificOptions->szTargetDirectory = NULL;
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
  if(    (pstOptions->pstSpecificOptions->createMountPoint)
      && (!pstOptions->pstSpecificOptions->mount))
  {
    // TODO: Build structures to bind ID, short option, long option, flag, required options and description together
    CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet,
                    "Found parameter option 'C' but not required option 'm', ignore option");
  }
  if(    (pstOptions->pstSpecificOptions->readonly)
      && (!pstOptions->pstSpecificOptions->mount))
  {
    CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet,
                    "Found parameter option 'r' but not required option 'm', ignore option");
  }
  if(    (pstOptions->stCommonOptions.textOutput)
      && (!pstOptions->pstSpecificOptions->printFreeCapacity)
      && (!pstOptions->pstSpecificOptions->printAvailableCapacity)
      && (!pstOptions->pstSpecificOptions->printMountPoint))
  {
    CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet,
                    "Found parameter option 't' for text output but no specific output requested, ignore option");
  }
  if(    (pstOptions->stCommonOptions.jsonOutput)
      && (!pstOptions->pstSpecificOptions->printFreeCapacity)
      && (!pstOptions->pstSpecificOptions->printAvailableCapacity)
      && (!pstOptions->pstSpecificOptions->printMountPoint))
  {
    CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet,
                    "Found parameter option 'j' for JSON output but no specific output requested, ignore option");
  }
  if(    (pstOptions->pstSpecificOptions->mount)
      && (pstOptions->pstSpecificOptions->umount))
  {
    CTUTIL_LOG_WARN(pstOptions->stCommonOptions.quiet,
                    "Found parameter option 'm' for mount and 'u' for unmount, do nothing of both");
    pstOptions->pstSpecificOptions->mount = false;
    pstOptions->pstSpecificOptions->umount = false;
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

      case 'b':
        // Don't disable info print if this is the only option
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->compatibilityMode = true;
        break;

      case 'm':
        pstOptions->stCommonOptions.infoPrint = false;
        if(    (optarg == NULL)
            || (optarg[0] == '\0'))
        {
          status = CTUTIL_MISSING_PARAMETER;
          CTUTIL_LOG_STATUS_ERROR(pstOptions->stCommonOptions.quiet, status, "Parameter option m");
        }
        else
        {
          WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
          pstOptions->pstSpecificOptions->mount = true;
          pstOptions->pstSpecificOptions->szTargetDirectory = optarg;
        }
        break;

      case 'u':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->umount = true;
        break;

      case 'r':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->readonly = true;
        break;

      case 'C':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->createMountPoint = true;
        break;

      case 'f':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->printFreeCapacity = true;
        break;

      case 'a':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->printAvailableCapacity = true;
        break;

      case 'p':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->printMountPoint = true;
        break;

      case 'c':
        pstOptions->stCommonOptions.infoPrint = false;
        WC_ASSERT(pstOptions->pstSpecificOptions != NULL);
        pstOptions->pstSpecificOptions->clear = true;
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

