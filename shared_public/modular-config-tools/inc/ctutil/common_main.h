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
///  \file     common_main.h
///
///  \brief    Commonly used, generic config tool main function
///            to prepare config tool specific main.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTUTIL_COMMON_MAIN_H_
#define INC_CTUTIL_COMMON_MAIN_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/api.h"
#include "ctutil/param_handling.h"
#include "ctutil/resource_handling.h"
#include "ctutil/error_codes.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
/// Application specific context for functions called from \link ctutil_CommonMain \endlink.
/// For use an application specific structure definition is needed.
typedef struct ctparts_commonMainContext ctparts_commonMainContext_t;

//------------------------------------------------------------------------------
/// Function pointer type for PrintInfoText.
/// \see ctparts_PrintInfoText
//------------------------------------------------------------------------------
typedef void         (ctparts_PrintInfoText_t)                (ctparts_commonMainContext_t * const pstContext,
                                                              char const * const szCallString);

//------------------------------------------------------------------------------
/// Function pointer type for SetOptionDefaults.
/// \see ctparts_SetOptionDefaults
//------------------------------------------------------------------------------
typedef statusCode_t (ctparts_SetOptionDefaults_t)        (ctparts_commonMainContext_t * const pstContext,
                                                              ctutil_Options_t * const pstOptions);

//------------------------------------------------------------------------------
/// Function pointer type for ReleaseSpecificOptions.
/// \see ctparts_ReleaseSpecificOptions
//------------------------------------------------------------------------------
typedef void         (ctparts_ReleaseSpecificOptions_t)   (ctparts_commonMainContext_t * const pstContext,
                                                              ctutil_Options_t * const pstOptions);

//------------------------------------------------------------------------------
/// Function pointer type for EvaluateEarlyOptions.
/// \see ctparts_EvaluateEarlyOptions
//------------------------------------------------------------------------------
typedef statusCode_t (ctparts_EvaluateEarlyOptions_t)     (ctparts_commonMainContext_t * const pstContext,
                                                              int const argc,
                                                              char * const argv[],
                                                              ctutil_Options_t * const pstOptions);

//------------------------------------------------------------------------------
/// Function pointer type for EvaluateOptions.
/// \see ctparts_EvaluateOptions
//------------------------------------------------------------------------------
typedef statusCode_t (ctparts_EvaluateOptions_t)          (ctparts_commonMainContext_t * const pstContext,
                                                              int const argc,
                                                              char * const argv[],
                                                              ctutil_Options_t * const pstOptions);

//------------------------------------------------------------------------------
/// Function pointer type for OpenSpecificResources.
/// \see ctparts_OpenSpecificResources
//------------------------------------------------------------------------------
typedef statusCode_t (ctparts_InitSpecificResources_t)    (ctparts_commonMainContext_t * const pstContext,
                                                              ctutil_ResourcesSpecific_t * * const ppstResources);

//------------------------------------------------------------------------------
/// Function pointer type for ReleaseSpecificResources.
/// \see ctparts_ReleaseSpecificResources
//------------------------------------------------------------------------------
typedef void         (ctparts_ReleaseSpecificResources_t) (ctparts_commonMainContext_t * const pstContext,
                                                              ctutil_ResourcesSpecific_t * * const ppstResources);

/// Function pointer structure for application specific functions called from \link ctutil_CommonMain \endlink.
/// \see ctutil_CommonMain
typedef struct ctparts_commonMainFunctions
{
  /// Pointer to common main part function PrintInfoText.
  ctparts_PrintInfoText_t *              pfPrintInfoText;

  /// Pointer to common main part function SetOptionDefaults.
  ctparts_SetOptionDefaults_t *          pfSetOptionDefaults;

  /// Pointer to common main part function ReleaseSpecificOptions.
  ctparts_ReleaseSpecificOptions_t *     pfReleaseSpecificOptions;

  /// Pointer to common main part function EvaluateEarlyOptions.
  ctparts_EvaluateEarlyOptions_t *       pfEvaluateEarlyOptions;

  /// Pointer to common main part function EvaluateOptions.
  ctparts_EvaluateOptions_t *            pfEvaluateOptions;

  /// Pointer to common main part function InitSpecificResources.
  ctparts_InitSpecificResources_t *      pfInitSpecificResources;

  /// Pointer to common main part function ReleaseSpecificResources.
  ctparts_ReleaseSpecificResources_t *   pfReleaseSpecificResources;
} ctparts_commonMainFunctions_t;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  //------------------------------------------------------------------------------
  /// Commonly used, generic config tool main function to prepare config tool specific main.
  ///
  /// \note This function calls the specific config tool main function \link ctparts_ctMain \endlink.
  ///
  /// \param argc
  ///   Argument count, directly forwarded from outer main function.
  /// \param argv
  ///   Argument vector, directly forwarded from outer main function.
  /// \param pstFunctions
  ///   Function pointer structure for application specific functions called from \link ctutil_CommonMain \endlink.
  /// \param pstContext
  ///   Application specific context for functions called from \link ctutil_CommonMain \endlink.
  ///   For use an application specific structure definition is needed. If unused, NULL is allowed.
  ///
  /// \return
  ///   Application exit code, \link CTUTIL_EXIT_SUCCESS \endlink on success, error code otherwise.
  ///   Application exit code will be between \link CTUTIL_EXIT_MIN \endlink and \link CTUTIL_EXIT_MAX \endlink.
  ///
  /// \see ctparts_commonMainFunctions_t
  /// \see ctparts_commonMainContext_t
  /// \see exit_codes.h
  /// \see CTUTIL_EXIT_MIN
  /// \see CTUTIL_EXIT_MAX
  /// \see CTUTIL_EXIT_SUCCESS
  /// \see CTUTIL_EXIT_GENERAL_ERROR
  //------------------------------------------------------------------------------
  extern CTUTIL_API exitCode_t ctutil_CommonMain(int const argc,
                                                 char * const argv[],
                                                 ctparts_commonMainFunctions_t const * const pstFunctions,
                                                 ctparts_commonMainContext_t * const pstContext);

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


#endif /* INC_CTUTIL_COMMON_MAIN_H_ */
//---- End of source file ------------------------------------------------------

