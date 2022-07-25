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
///  \file     common_main_defaults.h
///
///  \brief    Commonly used prototypes for config tool common main function.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTPARTS_COMMON_MAIN_DEFAULTS_H_
#define INC_CTPARTS_COMMON_MAIN_DEFAULTS_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "ctutil/common_main.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  //------------------------------------------------------------------------------
  /// Prototype description<br>
  /// ======================<br>
  /// Prototype to print short info text about program.
  ///
  /// \note The function implementation has to be provided application specific.
  /// \note For programs with only a few options this may be the same as the program help text.
  ///
  /// \param pstContext
  ///   Application specific context. If unused, NULL is allowed.
  /// \param szCallString
  ///   Call string from program call, normally this is "argv[0]".
  ///
  /// \see ctparts_commonMainContext_t
  //------------------------------------------------------------------------------
  void ctparts_PrintInfoText(ctparts_commonMainContext_t * const pstContext,
                             char const * const szCallString);

  //------------------------------------------------------------------------------
  /// Prototype description<br>
  /// ======================<br>
  /// Prototype to set application specific default application options.
  ///
  /// \note The function implementation has to be provided application specific.
  /// \note An eventually set pointer for a specific options structure have to be released by
  ///       \link ctparts_ReleaseSpecificOptions \endlink.
  ///
  /// \param pstContext
  ///   Application specific context. If unused, NULL is allowed.
  /// \param pstOptions
  ///   Option structure pointer to be equipped with defaults.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully set defaults, an error code otherwise.
  ///
  /// \pre
  ///   Application specific option pointer \link ctutil_OptionsSpecific_t \endlink is NULL.
  ///
  /// \post
  ///   Application specific option pointer \link ctutil_OptionsSpecific_t \endlink is NULL
  ///   or a valid structure in case of success.
  ///   Application specific option pointer \link ctutil_OptionsSpecific_t \endlink is NULL in case of an error.
  ///
  /// \see ctutil_Options_t
  /// \see ctutil_OptionsCommon_t
  /// \see ctutil_OptionsSpecific_t
  /// \see ctutil_ReleaseSpecificOptions
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  statusCode_t ctparts_SetOptionDefaults(ctparts_commonMainContext_t * const pstContext,
                                         ctutil_Options_t * const pstOptions);

  //------------------------------------------------------------------------------
  /// Prototype description<br>
  /// ======================<br>
  /// Prototype to release application specific application options.
  ///
  /// \note The function implementation has to be provided application specific.
  ///
  /// \param pstContext
  ///   Application specific context. If unused, NULL is allowed.
  /// \param pstOptions
  ///   Option structure pointer.
  ///   Inside user specific option pointer \link ctutil_OptionsSpecific_t \endlink may be NULL.
  ///
  /// \pre
  ///   Application specific option pointer \link ctutil_OptionsSpecific_t \endlink is NULL
  ///   or points to a valid structure.
  ///
  /// \post
  ///   Application specific option pointer \link ctutil_OptionsSpecific_t \endlink is NULL.
  ///
  /// \see ctutil_Options_t
  /// \see ctutil_OptionsCommon_t
  /// \see ctutil_OptionsSpecific_t
  /// \see ctutil_SetOptionDefaults
  //------------------------------------------------------------------------------
  void ctparts_ReleaseSpecificOptions(ctparts_commonMainContext_t * const pstContext,
                                      ctutil_Options_t * const pstOptions);

  //------------------------------------------------------------------------------
  /// Prototype description<br>
  /// ======================<br>
  /// Prototype to evaluate early options
  /// (options have to been checked before any further operations started).
  ///
  /// \note The function implementation has to be provided application specific.
  /// \note As inner implementation \link ctparts_EvaluateEarlyOptions \endlink
  ///       may be used.
  ///
  /// \param pstContext
  ///   Application specific context. If unused, NULL is allowed.
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
  statusCode_t ctparts_EvaluateEarlyOptions(ctparts_commonMainContext_t * const pstContext,
                                            int const argc,
                                            char * const argv[],
                                            ctutil_Options_t * const pstOptions);

  //------------------------------------------------------------------------------
  /// Prototype description<br>
  /// ======================<br>
  /// Prototype to evaluate options.
  ///
  /// \note The function implementation has to be provided application specific.
  ///
  /// \param pstContext
  ///   Application specific context. If unused, NULL is allowed.
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
  statusCode_t ctparts_EvaluateOptions(ctparts_commonMainContext_t * const pstContext,
                                       int const argc,
                                       char * const argv[],
                                       ctutil_Options_t * const pstOptions);

  //------------------------------------------------------------------------------
  /// Prototype description<br>
  /// ======================<br>
  /// Prototype to initialize application specific resources.
  ///
  /// \note The function implementation has to be provided application specific.
  /// \note An eventually set pointer for a specific resources structure have to be released by
  ///       \link ctparts_ReleaseSpecificResources \endlink.
  ///
  /// \param pstContext
  ///   Application specific context. If unused, NULL is allowed.
  /// \param ppstResources
  ///   Pointer to resource structure pointer to be equipped with application specific resource data or pointers.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully acquired resources, an error code otherwise.
  ///
  /// \pre
  ///   Application specific resource pointer \link ctutil_ResourcesSpecific_t \endlink points to NULL.
  ///
  /// \post
  ///   Application specific resource pointer \link ctutil_ResourcesSpecific_t \endlink points to NULL
  ///   or a valid structure in case of success.
  ///   Application specific resource pointer \link ctutil_ResourcesSpecific_t \endlink points to NULL in case of an error.
  ///
  /// \see ctutil_Resources_t
  /// \see ctutil_ResourcesCommon_t
  /// \see ctutil_ResourcesSpecific_t
  /// \see ctutil_ReleaseSpecificResources
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  statusCode_t ctparts_InitSpecificResources(ctparts_commonMainContext_t * const pstContext,
                                             ctutil_ResourcesSpecific_t * * const ppstResources);

  //------------------------------------------------------------------------------
  /// Prototype description<br>
  /// ======================<br>
  /// Prototype to release application specific application resources.
  ///
  /// \note The function implementation has to be provided application specific.
  ///
  /// \param pstContext
  ///   Application specific context. If unused, NULL is allowed.
  /// \param ppstResources
  ///   Pointer to resource structure pointer. May be pointing to NULL.
  ///
  /// \pre
  ///   Pointer to application specific resource pointer points to NULL or a valid structure.
  ///
  /// \post
  ///   Pointer to application specific resource pointer points to NULL.
  ///
  /// \see ctutil_Resources_t
  /// \see ctutil_ResourcesCommon_t
  /// \see ctutil_ResourcesSpecific_t
  /// \see ctutil_OpenSpecificResources
  //------------------------------------------------------------------------------
  void ctparts_ReleaseSpecificResources(ctparts_commonMainContext_t * const pstContext,
                                        ctutil_ResourcesSpecific_t * * const ppstResources);

  /// Sets default prototypes into function table for \link ctutil_CommonMain \endlink.
  ///
  /// \note The functions have to be provided application specific, so the equipped prototypes leads to open symbols
  ///       which have to be implemented.
  ///
  /// \param pFunctionTable
  ///   Function table for \link ctutil_CommonMain \endlink to equip with default prototypes.
  ///
  /// \see ctutil_CommonMain
  /// \see commonMainFunctions_t
  //------------------------------------------------------------------------------
  static inline void ctparts_SetDefaultCommonMainFunctions(ctparts_commonMainFunctions_t * const pFunctionTable);

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
static inline void ctparts_SetDefaultCommonMainFunctions(ctparts_commonMainFunctions_t * const pFunctionTable)
{
  pFunctionTable->pfPrintInfoText            = &ctparts_PrintInfoText;
  pFunctionTable->pfSetOptionDefaults        = &ctparts_SetOptionDefaults;
  pFunctionTable->pfReleaseSpecificOptions   = &ctparts_ReleaseSpecificOptions;
  pFunctionTable->pfEvaluateEarlyOptions     = &ctparts_EvaluateEarlyOptions;
  pFunctionTable->pfEvaluateOptions          = &ctparts_EvaluateOptions;
  pFunctionTable->pfInitSpecificResources    = &ctparts_InitSpecificResources;
  pFunctionTable->pfReleaseSpecificResources = &ctparts_ReleaseSpecificResources;

  return;
}


#endif /* INC_CTPARTS_COMMON_MAIN_DEFAULTS_H_ */
//---- End of source file ------------------------------------------------------

