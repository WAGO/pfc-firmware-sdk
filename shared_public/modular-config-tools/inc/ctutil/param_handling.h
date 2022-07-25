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
///  \file     param_handling.h
///
///  \brief    Helper functions for config tool parameter handling.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTUTIL_PARAM_HANDLING_H_
#define INC_CTUTIL_PARAM_HANDLING_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/api.h"
#include "ctutil/error_codes.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Common config tool options structure.
/// \see ctutil_Options
/// \see ctutil_OptionsSpecific_t
typedef struct ctutil_OptionsCommon
{
  /// Flag to enter an early exit and skip main functionality.
  bool earlyExit;

  /// Flag to disable every output, also on error.
  bool quiet;

  /// Flag to extracts arguments from an URI encoding.
  bool extractFromUri;

  /// Flag to enable program information print.
  bool infoPrint;

  /// Flag to enable normal text output.
  bool textOutput;

  /// Flag to enable JSON output.
  bool jsonOutput;
} ctutil_OptionsCommon_t;

/// Specific config tool options structure.
/// \see ctutil_Options
/// \see ctutil_OptionsCommon_t
typedef struct ctutil_OptionsSpecific ctutil_OptionsSpecific_t;

/// Config tool options structure with common and specific part.
/// \see ctutil_OptionsCommon_t
/// \see ctutil_OptionsSpecific_t
typedef struct ctutil_Options
{
  /// Common options.
  ctutil_OptionsCommon_t stCommonOptions;

  /// Program specific options.
  ctutil_OptionsSpecific_t * pstSpecificOptions;
} ctutil_Options_t;


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  //------------------------------------------------------------------------------
  /// Initializes a given \link ctutil_Options_t \endlink structure.
  ///
  /// \param pstOptions
  ///   Option structure to initialize.
  //------------------------------------------------------------------------------
  extern CTUTIL_API void ctutil_InitOptions(ctutil_Options_t * const pstOptions);

  //------------------------------------------------------------------------------
  /// Decodes an URI encoded and null terminated string into a standard ASCII string.
  /// The input string content is replaced by the decoded string in case of success.
  /// In case of an error the original input string is left untouched.
  ///
  /// \warning Decoding of non URI encoded strings may result in wrong or unexpected
  ///          results, in some cases also without detection and signaling of an error/failure.
  ///
  /// \param szArgument
  ///   URI encoded and null terminated input string to decode.
  ///   String content is replaced in case of success with decoded string content.
  ///   Buffer/string size left untouched (decoded string is always shorter or equal).
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully decoded input, an error code otherwise.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_DecodeUriArgument(char * const szArgument);

  //------------------------------------------------------------------------------
  /// Evaluates common early options
  /// (options have to been checked before any further operations started).
  ///
  /// \note Evaluated common options are: -q/--quiet and -x/--extract-from-uri-encoding
  ///
  /// \param argc
  ///   Argument count, forwarded from outer main function.
  /// \param argv
  ///   Argument vector, forwarded from outer main function.
  /// \param pstOptions
  ///   Option structure pointer.
  ///   Inside user specific option pointer \link ctutil_OptionsSpecific_t \endlink may be NULL.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of success, an error code otherwise.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_EvaluateEarlyOptions(int const argc,
                                                             char * const argv[],
                                                             ctutil_Options_t * const pstOptions);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* INC_CTUTIL_PARAM_HANDLING_H_ */
//---- End of source file ------------------------------------------------------

