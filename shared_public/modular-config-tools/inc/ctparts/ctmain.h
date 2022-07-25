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
///  \file     ctmain.h
///
///  \brief    Config tool main function for modular config tools.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTPARTS_CTMAIN_H_
#define INC_CTPARTS_CTMAIN_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/param_handling.h"
#include "ctutil/resource_handling.h"
#include "ctutil/error_codes.h"

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
  /// Prototype description<br>
  /// ======================<br>
  /// Prototype for specific config tool main function.
  ///
  /// \note The function implementation has to be provided application specific.
  ///
  /// \param argc
  ///   Argument count, directly forwarded from outer main function.
  /// \param argv
  ///   Argument vector, directly forwarded from outer main function.
  /// \param pstOptions
  ///   Option structure pointer with common and specific application options.
  /// \param pstResources
  ///   Resource structure pointer with common and specific application resources.
  ///
  /// \return
  ///   Application exit code, \link CTUTIL_EXIT_SUCCESS \endlink on success, error code otherwise.
  ///   The provided exit code should be positive and never a forward from a status code.
  ///   The provided exit code should be between \link CTUTIL_EXIT_MIN \endlink and \link CTUTIL_EXIT_MAX \endlink.
  ///   The provided exit code should honor the common exit code defines and avoid any conflict.
  ///
  /// \see exit_codes.h
  /// \see CTUTIL_EXIT_MIN
  /// \see CTUTIL_EXIT_MAX
  /// \see CTUTIL_EXIT_SUCCESS
  /// \see CTUTIL_EXIT_GENERAL_ERROR
  //------------------------------------------------------------------------------
  exitCode_t ctparts_ctMain(int const argc,
                            char * const argv[],
                            ctutil_Options_t const * const pstOptions,
                            ctutil_Resources_t const * const pstResources);

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


#endif /* INC_CTPARTS_CTMAIN_H_ */
//---- End of source file ------------------------------------------------------

