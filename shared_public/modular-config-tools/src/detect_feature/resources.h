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
///  \file     resources.h
///
///  \brief    Config tool specific resource definition and handling for
///            detect_feature.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_DETECT_FEATURE_RESOURCES_H_
#define SRC_DETECT_FEATURE_RESOURCES_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "ctutil/resource_handling.h"
#include "ctutil/common_functions.h"
#include <dirent.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Type definition for a function to resolve the real path of a file.
///
/// \param szFilePath
///   File path to resolve.
/// \param szResolvedPath
///   Buffer to store the resolved path, have to be at least PATH_MAX bytes when given.
///   Parameter may be NULL, then malloc is used to allocate an buffer which have to be freed.
///
/// \return
///   Pointer to the resolved path on success, NULL otherwise.
//------------------------------------------------------------------------------
typedef char * (*resolveRealPath_ptr)(char const * const szFilePath,
                                      char * const szResolvedPath);

//------------------------------------------------------------------------------
/// Type definition for a function to check availabilities of feature dependencies.
///
/// \param szFilePath
///   File path to feature file to check dependencies for.
///
/// \return
///   true in case of all dependencies are available, false otherwise.
//------------------------------------------------------------------------------
typedef bool (*isDependencyComplete_ptr)(char const * const szFilePath);

//------------------------------------------------------------------------------
/// Type definition for a function to open a directory.
///
/// \param szName
///   Directory path to open.
///
/// \return
///   Pointer to the directory stream on success, otherwise NULL is returned
///   and errno is set appropriately.
//------------------------------------------------------------------------------
typedef DIR * (*openDir_ptr)(char const * const szName);

//------------------------------------------------------------------------------
/// Type definition for a function to read from an open directory.
///
/// \param pDir
///   Pointer to the directory stream.
///
/// \return
///   Pointer to a dirent structure on success, NULL is returned on end of stream,
///   otherwise NULL is returned and errno is set appropriately.
//------------------------------------------------------------------------------
typedef struct dirent * (*readDir_ptr)(DIR * const pDir);

//------------------------------------------------------------------------------
/// Type definition for a function to close an open directory.
///
/// \param pDir
///   Pointer to the directory stream.
///
/// \return
///   0 on success, otherwise a negative value is returned and errno is set appropriately.
//------------------------------------------------------------------------------
typedef int (*closeDir_ptr)(DIR * const pDir);

/// Resource structure for detect_feature config tool specific resources.
struct ctutil_ResourcesSpecific
{
  /// Device feature path containing feature files.
  char const * szDeviceFeaturePath;

  /// Function pointer for a function to resolve the real path of a file.
  resolveRealPath_ptr pfResolveRealPath;

  /// Function pointer for file availability function to use.
  ctutil_IsFileAvailable_ptr pfIsFileAvailable;

  /// Function pointer for feature dependency check function to use.
  isDependencyComplete_ptr pfIsDependencyComplete;

  /// Function pointer for a function to open a directory.
  openDir_ptr pfOpenDir;

  /// Function pointer for a function to read from an open directory.
  readDir_ptr pfReadDir;

  /// Function pointer for a function to close an open directory.
  closeDir_ptr pfCloseDir;
};

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

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


#endif /* SRC_DETECT_FEATURE_RESOURCES_H_ */
//---- End of source file ------------------------------------------------------

