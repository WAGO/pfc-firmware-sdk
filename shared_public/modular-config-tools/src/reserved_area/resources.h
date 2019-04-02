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
///  \file     resources.h
///
///  \brief    Config tool specific resource definition and handling for
///            config_linux_user.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_CONFIG_LINUX_USER_RESOURCES_H_
#define SRC_CONFIG_LINUX_USER_RESOURCES_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "ctutil/resource_handling.h"
#include <sys/mount.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Type definition for mount function corresponding to mount() in <sys/mount.h>
/// with additional common option structure pointer and standardized status code.
typedef statusCode_t (*ctparts_Mount_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                          char const * const szSpecialFile,
                                          char const * const szDir,
                                          char const * const szFileSystemType,
                                          unsigned long int const flags,
                                          void const * const pData);

/// Type definition for mount function corresponding to umount2() in <sys/mount.h>
/// with additional common option structure pointer and standardized status code.
typedef statusCode_t (*ctparts_Umount_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                           char const * const szSpecialFile,
                                           int const flags);

//------------------------------------------------------------------------------
/// Type definition for a clear function to use to remove any content from a directory.
///
/// \param pstCommonOptions
///   Common option structure pointer.
/// \param szPath
///   Folder path to clean from any content.
///
/// \return
///   \link CTUTIL_SUCCESS \endlink in case of successfully cleaned folder, an error code otherwise.
///
/// \see CTUTIL_SUCCESS
//------------------------------------------------------------------------------
typedef statusCode_t (*ctparts_ClearFolder_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                char const * const szFolderPath);

//------------------------------------------------------------------------------
/// Type definition for a function to get the free capacity for a given folder.
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
typedef statusCode_t (*ctparts_GetFreeBytes_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                 char const * const szFolderPath,
                                                 uint64_t * const freeBytes);

//------------------------------------------------------------------------------
/// Type definition for a function to get the available capacity for a given folder.
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
typedef statusCode_t (*ctparts_GetAvailableBytes_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                      char const * const szFolderPath,
                                                      uint64_t * const availableBytes);

//------------------------------------------------------------------------------
/// Type definition for a function to get the mount point for a given device path.
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
typedef statusCode_t (*ctparts_GetMountPoint_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                  char const * const szDeviceFilePath,
                                                  char * * const pszMountPoint,
                                                  size_t const mountPointBufferSize);

/// Resource structure for config_linux_user config tool specific resources.
struct ctutil_ResourcesSpecific
{
  /// Device file path to device used for reserved area.
  char const * szDeviceFilePath;

  /// Type of file system, e.g. ext4.
  char const * szFileSystemType;

  /// Target directory path used to mount reserved area.
  char const * szTargetDirectory;

  /// Function pointer for mount function to use.
  ctparts_Mount_ptr pfMount;

  /// Function pointer for unmount function to use.
  ctparts_Umount_ptr pfUmount;

  /// Function pointer for clear function to use to remove any content from a directory.
  ctparts_ClearFolder_ptr pfClearFolder;

  /// Function pointer for a function to get the free capacity for a given folder.
  ctparts_GetFreeBytes_ptr pfGetFreeBytes;

  /// Function pointer for a function to get the available capacity for a given folder.
  ctparts_GetAvailableBytes_ptr pfGetAvailableBytes;

  /// Function pointer for a function to get the available capacity for a given folder.
  ctparts_GetMountPoint_ptr pfGetMountPoint;
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


#endif /* SRC_CONFIG_LINUX_USER_RESOURCES_H_ */
//---- End of source file ------------------------------------------------------

