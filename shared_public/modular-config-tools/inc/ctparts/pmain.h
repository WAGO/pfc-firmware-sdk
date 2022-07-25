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
///  \file     pmain.h
///
///  \brief    Function to prepare config tool specific main.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTPARTS_PMAIN_H_
#define INC_CTPARTS_PMAIN_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
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
  /// Prototype for config tool prepare main function.
  ///
  /// \note The function implementation has to be provided application specific.
  /// \note This function has to call the specific config tool main function.
  /// \note As inner implementation \link ctutil_CommonMain \endlink may be used.
  ///       \link ctparts_ctMain \endlink.
  ///
  /// \param argc
  ///   Argument count, directly forwarded from outer main function.
  /// \param argv
  ///   Argument vector, directly forwarded from outer main function.
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
  exitCode_t ctparts_PrepareMain(int const argc,
                                 char * const argv[]);

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


#endif /* INC_CTPARTS_PMAIN_H_ */
//---- End of source file ------------------------------------------------------

