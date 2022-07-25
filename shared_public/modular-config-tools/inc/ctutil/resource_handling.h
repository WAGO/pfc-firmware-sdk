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
///  \file     resource_handling.h
///
///  \brief    Helper functions for config tool resource handling.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTUTIL_RESOURCE_HANDLING_H_
#define INC_CTUTIL_RESOURCE_HANDLING_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/api.h"
#include "ctutil/error_codes.h"
#include <stdio.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Common config tool resources structure.
/// \see ctutil_Resources
/// \see ctutil_ResourcesSpecific_t
typedef struct ctutil_ResourcesCommon
{
  /// File path of "last error" file.
  char const * szLastErrorFile;
  // TODO: Add stream for "last error" file
} ctutil_ResourcesCommon_t;

/// Specific config tool resources structure.
/// \see ctutil_Resources
/// \see ctutil_ResourcesCommon_t
typedef struct ctutil_ResourcesSpecific ctutil_ResourcesSpecific_t;

/// Config tool resources structure with common and specific part.
/// \see ctutil_ResourcesCommon_t
/// \see ctutil_ResourcesSpecific_t
typedef struct ctutil_Resources
{
  /// Common resources.
  ctutil_ResourcesCommon_t stCommonResources;

  /// Program specific resources.
  ctutil_ResourcesSpecific_t * pstSpecificResources;
} ctutil_Resources_t;

/// Type definition for a changeable file handle (implementation hided).
typedef struct ctutil_ChangeableFile ctutil_ChangeableFile_t;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  //------------------------------------------------------------------------------
  /// Initializes a given \link ctutil_Resources_t \endlink structure.
  ///
  /// \param pstResources
  ///   Resources structure to initialize.
  //------------------------------------------------------------------------------
  extern CTUTIL_API void ctutil_InitResources(ctutil_Resources_t * const pstResources);

  //------------------------------------------------------------------------------
  /// Opens common config tool resources.
  ///
  /// \param pstResources
  ///   Pointer to common resource structure.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully acquired resources, an error code otherwise.
  ///
  /// \pre
  ///   No common resources acquired.
  ///
  /// \post
  ///   All common resources acquired in case of success.
  ///   No common resources acquired in case of an error.
  ///
  /// \see ctutil_Resources_t
  /// \see ctutil_ResourcesCommon_t
  /// \see ctutil_ResourcesSpecific_t
  /// \see ctutil_ReleaseSpecificResources
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_OpenCommonResources(ctutil_ResourcesCommon_t * const pstResources);

  //------------------------------------------------------------------------------
  /// Releases common config tool resources.
  ///
  /// \param pstResources
  ///   Pointer to common resource structure.
  ///
  /// \pre
  ///   All common resources acquired.
  ///
  /// \post
  ///   No common resources acquired.
  ///
  /// \see ctutil_Resources_t
  /// \see ctutil_ResourcesCommon_t
  /// \see ctutil_ResourcesSpecific_t
  /// \see ctutil_ReleaseSpecificResources
  //------------------------------------------------------------------------------
  extern CTUTIL_API void ctutil_ReleaseCommonResources(ctutil_ResourcesCommon_t * const pstResources);

  //------------------------------------------------------------------------------
  /// Prepares a file for safe write access.
  ///
  /// \param szFilePath
  ///   File path to file to prepare for write access.
  /// \param ppFileHandle
  ///   File handle to prepared file for write access.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully acquired resources, an error code otherwise.
  ///
  /// \pre
  ///   File handle \link ctutil_ChangeableFile_t \endlink points to NULL.
  ///
  /// \post
  ///   File handle \link ctutil_ChangeableFile_t \endlink points to a valid structure in case of success.
  ///   File handle \link ctutil_ChangeableFile_t \endlink points to NULL in case of an error.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_PrepareChangeableFile(char const * const szFilePath,
                                                              ctutil_ChangeableFile_t * * const ppFileHandle);

  //------------------------------------------------------------------------------
  /// Releases a file handle to a prepared file for write access.
  ///
  /// \param ppFileHandle
  ///   File handle to prepared file for write access.
  ///
  /// \pre
  ///   File handle \link ctutil_ChangeableFile_t \endlink points to NULL or a valid structure.
  ///
  /// \post
  ///   File handle \link ctutil_ChangeableFile_t \endlink points to NULL.
  //------------------------------------------------------------------------------
  extern CTUTIL_API void ctutil_ReleaseFileForChanges(ctutil_ChangeableFile_t * * const ppFileHandle);

  //------------------------------------------------------------------------------
  /// Gets file path for original file, which is unchanged until changes not commited.
  ///
  /// \param pFileHandle
  ///   File handle to prepared file for write access.
  ///
  /// \return
  ///   File path for original file.
  //------------------------------------------------------------------------------
  extern CTUTIL_API char const * ctutil_GetOriginalFilePath(ctutil_ChangeableFile_t const * const pFileHandle);

  //------------------------------------------------------------------------------
  /// Gets file path for writable (copied) file.
  ///
  /// \param pFileHandle
  ///   File handle to prepared file for write access.
  ///
  /// \return
  ///   File path for writable (copied) file.
  //------------------------------------------------------------------------------
  extern CTUTIL_API char const * ctutil_GetChangeableFilePath(ctutil_ChangeableFile_t const * const pFileHandle);

  //------------------------------------------------------------------------------
  /// Commits changed file and replace original file.
  ///
  /// \pre
  ///   Changeable file is closed.
  ///
  /// \post
  ///   Original file is replaced by changeable file.
  ///   File mode of original file is kept.
  ///
  /// \param pFileHandle
  ///   File handle to prepared file for write access.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully commited changeable file.
  ///
  /// \see CTUTIL_SUCCESS
  /// \see ctutil_GetOriginalFilePath
  /// \see ctutil_GetChangeableFilePath
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_CommitChangeableFile(ctutil_ChangeableFile_t const * const pFileHandle);

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


#endif /* INC_CTUTIL_RESOURCE_HANDLING_H_ */
//---- End of source file ------------------------------------------------------

