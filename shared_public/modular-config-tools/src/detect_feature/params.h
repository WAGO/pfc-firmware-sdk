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
///  \file     params.h
///
///  \brief    Config tool specific parameter definition and handling for
///            detect_feature.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_DETECT_FEATURE_PARAMS_H_
#define SRC_DETECT_FEATURE_PARAMS_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "ctutil/param_handling.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Option structure for detect_feature config tool specific options.
struct ctutil_OptionsSpecific
{
  /// Feature to check.
  char const * szFeature;

  /// Print only value without name
  bool onlyValue;

  /// List mode to return all currently active features.
  bool listMode;
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


#endif /* SRC_DETECT_FEATURE_PARAMS_H_ */
//---- End of source file ------------------------------------------------------

