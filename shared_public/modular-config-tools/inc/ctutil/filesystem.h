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
///  \file     filesystem.h
///
///  \brief    Filesystem tools.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTUTIL_FILESYSTEM_H_
#define INC_CTUTIL_FILESYSTEM_H_

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

  //------------------------------------------------------------------------------
  /// Get string for parent folder path.
  ///
  /// \param szPath
  ///   Path to get parent folder for.
  /// \param pathLength
  ///   Length for szPath string.
  /// \param pszTarget
  ///   Pointer to buffer for resulting parent folder string.
  ///   May be NULL, targetBufferSize has to be 0 in this case.
  /// \param targetBufferSize
  ///   Usable size of pszTarget.
  /// \param allowAllocation
  ///   When true buffer for resulting parent folder string is allocated if pszTarget is to short for result.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case string for parent folder got, an error code otherwise.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_GetParentFolderString(char const * const szPath,
                                                              size_t const pathLength,
                                                              char * * const pszTarget,
                                                              size_t const targetBufferSize,
                                                              bool allowAllocation);

  //------------------------------------------------------------------------------
  /// Check if a given folder is accessible.
  ///
  /// \param szPath
  ///   Folder path to determine accessibility for.
  ///
  /// \return
  ///   true in case of folder is accessible, false otherwise.
  //------------------------------------------------------------------------------
  extern CTUTIL_API bool ctutil_IsFolderAccessible(char const * const szPath);

  //------------------------------------------------------------------------------
  /// Create a given folder.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szPath
  ///   Folder path to create.
  /// \param createParentsAlso
  ///   If true parent folders will be also created if needed.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case folder was created, an error code otherwise.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_CreateFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                     char const * const szPath,
                                                     bool createParentsAlso);

  //------------------------------------------------------------------------------
  /// Clear content of a given folder.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szPath
  ///   Folder path to clear.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case folder was cleared, an error code otherwise.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_ClearFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                    char const * const szPath);

  //------------------------------------------------------------------------------
  /// Remove a given folder and its content.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szPath
  ///   Folder path to remove.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case folder was removed, an error code otherwise.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_RemoveFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                     char const * const szPath);

  //------------------------------------------------------------------------------
  // TODO: Provide an implementation and create doxygen description
  //------------------------------------------------------------------------------
//  extern CTUTIL_API statusCode_t ctutil_GetMountPointForPath(ctutil_OptionsCommon_t const * const pstCommonOptions,
//                                                             char const * const szPath,
//                                                             char * * const pszMountPoint,
//                                                             size_t const mountPointBufferSize);

  //------------------------------------------------------------------------------
  /// Get the mount point for a given device.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szDeviceFilePath
  ///   Device to determine mount point for.
  /// \param pszMountPoint
  ///   Output parameter to store mount point for given path.
  ///   Pointer may point to NULL or a given buffer.
  ///   In case of NULL or a too small buffer a new buffer is allocated, which have to be freed by the caller.
  /// \param mountPointBufferSize
  ///   Size of optionally given mount point buffer. Zero (0) if no buffer given.
  ///
  /// \pre
  ///   mountPointBufferSize is zero (0) and pszMountPoint points to NULL OR
  ///   mountPointBufferSize is >0 and pszMountPoint points to a valid buffer of at least mountPointBufferSize size.
  ///
  /// \post
  ///   Returned status code is \link CTUTIL_SUCCESS \endlink and pszMountPoint points to NULL,
  ///   which means the given device is not mounted.
  ///   Returned status code is \link CTUTIL_SUCCESS \endlink and pszMountPoint points to previously given buffer,
  ///   which means mount point was determined and stored into given buffer.
  ///   Returned status code is \link CTUTIL_SUCCESS \endlink and pszMountPoint points NOT previously given buffer,
  ///   which means mount point was determined and stored into an allocated buffer, which have to be freed by the caller.
  ///   Returned status code is NOT \link CTUTIL_SUCCESS \endlink, then pszMountPoint points to NULL.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully determined mount point or a not mounted device.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_GetMountPointForDevice(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                               char const * const szDeviceFilePath,
                                                               char * * const pszMountPoint,
                                                               size_t const mountPointBufferSize);

  //------------------------------------------------------------------------------
  /// Check if given file path is a mount point.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szPath
  ///   File path to check for mount point.
  ///
  /// \return
  ///   true in case of file path is a mount point, false otherwise.
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_CheckForMountPoint(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                           char const * const szPath);

  //------------------------------------------------------------------------------
  /// Check file stats to match given file types.
  ///
  /// \param pstStats
  ///   File stats to check file type for.
  /// \param fileType
  ///   File type character to check for.
  ///   b = block, c = character, d = directory/folder, f = fifo, l = link, s = socket, - = regular file
  ///   Parameter may be NULL
  ///
  /// \return
  ///   true in case of file match a requested file type, false otherwise.
  //------------------------------------------------------------------------------
  extern CTUTIL_API bool ctutil_IsFileTypeMatch(struct stat * pstStats,
                                                char const fileType);

  //------------------------------------------------------------------------------
  /// Check file availability.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szFilePath
  ///   File path to determine availability for.
  /// \param szFileTypes
  ///   Null terminated character array to check for special file type.
  ///   b = block, c = character, d = directory/folder, f = fifo, l = link, s = socket, - = regular file
  ///   Parameter may be NULL
  ///
  /// \return
  ///   true in case of file is available as requested file type, false otherwise.
  //------------------------------------------------------------------------------
  extern CTUTIL_API bool ctutil_IsFileAvailable(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                char const * const szFilePath,
                                                char const * const szFileTypes);

  //------------------------------------------------------------------------------
  /// Mount function corresponding to mount() in <sys/mount.h>
  /// with additional common option structure pointer and standardized status code.
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_Mount(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                              char const * const szSpecialFile,
                                              char const * const szDir,
                                              char const * const szFileSystemType,
                                              unsigned long int const flags,
                                              void const * const pData);

  //------------------------------------------------------------------------------
  /// Umount function corresponding to umount2() in <sys/mount.h>
  /// with additional common option structure pointer and standardized status code.
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_Umount(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                               char const * const szSpecialFile,
                                               int const flags);

  //------------------------------------------------------------------------------
  /// Get file system statistics for a given file.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szPath
  ///   File path to determine statistics for.
  /// \param pstFileSystemStats
  ///   Output structure to store statistics.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully determined statistics.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_GetFileSystemStatistics(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                                char const * const szPath,
                                                                struct statvfs * const pstFileSystemStats);

  //------------------------------------------------------------------------------
  /// Get the free capacity for a given folder.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szFolderPath
  ///   Folder path to determine free bytes for.
  /// \param freeBytes
  ///   Output parameter to store free bytes determined for given folder.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully determined free bytes.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_GetFreeBytes(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                     char const * const szFolderPath,
                                                     uint64_t * const freeBytes);

  //------------------------------------------------------------------------------
  /// Get the available capacity for a given folder.
  ///
  /// \param pstCommonOptions
  ///   Common option structure pointer.
  /// \param szFolderPath
  ///   Folder path to determine available bytes for.
  /// \param availableBytes
  ///   Output parameter to store available bytes determined for given folder.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case of successfully determined available bytes.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  extern CTUTIL_API statusCode_t ctutil_GetAvailableBytes(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                          char const * const szFolderPath,
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


#endif /* INC_CTUTIL_FILESYSTEM_H_ */
//---- End of source file ------------------------------------------------------

