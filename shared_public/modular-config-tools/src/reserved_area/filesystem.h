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
///  \file     filesystem.h
///
///  \brief    Filesystem tools.
///            TODO: Should be moved to a library.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_RESERVED_AREA_FILESYSTEM_H_
#define SRC_RESERVED_AREA_FILESYSTEM_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "ctutil/resource_handling.h"
#include "ctutil/param_handling.h"
#include <sys/stat.h>
#include <sys/statvfs.h>

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

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t GetParentFolderString(char const * const szPath,
                                     size_t const pathLength,
                                     char * * const pszTarget,
                                     size_t const targetBufferSize,
                                     bool allowAllocation);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  bool IsFolderAccessable(char const * const szPath);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t CreateFolderHierarchy(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                     char const * const szPath);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t CreateFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                            char const * const szPath,
                            bool createParentsAlso);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t ClearFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                           char const * const szPath);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t RemoveFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                            char const * const szPath);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  // statusCode_t GetMountPointForPath(ctutil_OptionsCommon_t const * const pstCommonOptions,
  //                                   char const * const szPath,
  //                                   char * * const pszMountPoint,
  //                                   size_t const mountPointBufferSize);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t GetMountPointForDevice(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                      char const * const szDeviceFilePath,
                                      char * * const pszMountPoint,
                                      size_t const mountPointBufferSize);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t CheckForMountPoint(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                  char const * const szPath);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  // fileType: b = block, c = character, d = directory/folder, f = fifo, l = link, s = socket, - = regular file
  bool IsFileTypeMatch(struct stat * pstStats,
                       char const fileType);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  // szFileTypes: b = block, c = character, d = directory/folder, f = fifo, l = link, s = socket, - = regular file
  // szFileTypes may be NULL
  bool IsFileAvailable(ctutil_OptionsCommon_t const * const pstCommonOptions,
                       char const * const szFilePath,
                       char const * const szFileTypes);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t Mount(ctutil_OptionsCommon_t const * const pstCommonOptions,
                     char const * const szSpecialFile,
                     char const * const szDir,
                     char const * const szFileSystemType,
                     unsigned long int const flags,
                     void const * const pData);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t Umount(ctutil_OptionsCommon_t const * const pstCommonOptions,
                      char const * const szSpecialFile,
                      int const flags);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t GetFileSystemStatistics(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                       char const * const szPath,
                                       struct statvfs * const pstFileSystemStats);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t GetFreeBytes(ctutil_OptionsCommon_t const * const pstCommonOptions,
                            char const * const szPath,
                            uint64_t * const freeBytes);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t GetAvailableBytes(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                 char const * const szPath,
                                 uint64_t * const availableBytes);

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


#endif /* SRC_RESERVED_AREA_FILESYSTEM_H_ */
//---- End of source file ------------------------------------------------------

