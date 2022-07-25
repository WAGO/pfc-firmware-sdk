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
///  \file     resources.h
///
///  \brief    Config tool specific resource definition and handling for
///            config_linux_user.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_CONFIG_LINUX_USER_RESOURCES_H_
#define SRC_CONFIG_LINUX_USER_RESOURCES_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "ctutil/resource_handling.h"
#include "ctutil/common_functions.h"
#include <sys/mount.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

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
  ctutil_Mount_ptr pfMount;

  /// Function pointer for unmount function to use.
  ctutil_Umount_ptr pfUmount;

  /// Function pointer for clear function to use to remove any content from a directory.
  ctutil_ClearFolder_ptr pfClearFolder;

  /// Function pointer for a function to get the free capacity for a given folder.
  ctutil_GetFreeBytes_ptr pfGetFreeBytes;

  /// Function pointer for a function to get the available capacity for a given folder.
  ctutil_GetAvailableBytes_ptr pfGetAvailableBytes;

  /// Function pointer for a function to get the mount point for a given device.
  ctutil_GetMountPointForDevice_ptr pfGetMountPointForDevice;

  /// Function pointer for file availability function to use.
  ctutil_IsFileAvailable_ptr pfIsFileAvailable;

  /// Function pointer for folder accessibility function to use.
  ctutil_IsFolderAccessible_ptr pfIsFolderAccessible;

  /// Function pointer for a function to create a folder hierarchy.
  ctutil_CreateFolder_ptr pfCreateFolder;
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

