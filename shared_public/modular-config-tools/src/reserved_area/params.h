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
///  \file     params.h
///
///  \brief    Config tool specific parameter definition and handling for
///            config_linux_user.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_CONFIG_LINUX_USER_PARAMS_H_
#define SRC_CONFIG_LINUX_USER_PARAMS_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "ctutil/param_handling.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Marker for a not mounted partition.
#define MARKER_NOT_MOUNTED    "none"

/// Option structure for reserved_area config tool specific options.
struct ctutil_OptionsSpecific
{
  /// Target directory for mounting reserved area.
  char const * szTargetDirectory;

  /// Compatibility mode for devices without reserved area.
  bool compatibilityMode;

  /// Flag to mount reserved partition.
  bool mount;

  /// Flag to unmount reserved partition.
  bool umount;

  /// Flag to mount reserved partition in readonly mode.
  bool readonly;

  /// Flag to create mount point if necessary.
  bool createMountPoint;

  /// Flag to print free capacity of mounted reserved partition.
  bool printFreeCapacity;

  /// Flag to print user available capacity of mounted reserved partition.
  bool printAvailableCapacity;

  /// Flag to print mount point of mounted reserved partition.
  bool printMountPoint;

  /// Flag to clear reserved partition.
  bool clear;
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


#endif /* SRC_CONFIG_LINUX_USER_PARAMS_H_ */
//---- End of source file ------------------------------------------------------

