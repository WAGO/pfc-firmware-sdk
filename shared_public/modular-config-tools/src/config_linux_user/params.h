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
#include "ctutil/param_handling.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Option structure for config_linux_user config tool specific options.
struct ctutil_OptionsSpecific
{
  /// Username of linux user for password change.
  char const * szUsername;

  /// New password to set for linux user.
  char const * szPassword1;

  /// Password confirmation for linux user.
  char const * szPassword2;

  /// New password hash to set for linux user.
  char const * szHash;
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

