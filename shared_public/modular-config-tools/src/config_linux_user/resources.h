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
#ifndef SRC_CONFIG_LINUX_USER_RESOURCES_H_NEW_
#define SRC_CONFIG_LINUX_USER_RESOURCES_H_NEW_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/resource_handling.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Resource structure for config_linux_user config tool specific resources.
struct ctutil_ResourcesSpecific
{
  /// Changeable shadow file.
  ctutil_ChangeableFile_t * pShadowFile;

  /// File path for primary random source.
  char const * szRandomSource1;

  /// File path for secondary random source.
  char const * szRandomSource2;
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


#endif /* SRC_CONFIG_LINUX_USER_RESOURCES_H_NEW_ */
//---- End of source file ------------------------------------------------------

