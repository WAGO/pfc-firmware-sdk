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
///  \file     error_codes.h
///
///  \brief    Common exit/error codes and helper functions for modular config tools.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTUTIL_ERROR_CODES_H_
#define INC_CTUTIL_ERROR_CODES_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "ctutil/api.h"
#include <stdlib.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Exit code type for program exit.
/// \see CTUTIL_EXIT_MIN
/// \see CTUTIL_EXIT_MAX
/// \see CTUTIL_EXIT_SUCCESS
/// \see CTUTIL_EXIT_GENERAL_ERROR
typedef int exitCode_t;

/// Minimum allowed exit code.
#define CTUTIL_EXIT_MIN                                      ((exitCode_t)    0)

/// Maximum allowed exit code.
#define CTUTIL_EXIT_MAX                                      ((exitCode_t)  125)

// Common exit codes
// Note: There should be an corresponding status code for every exit code
//------------------------------------------------------------------------------
/// Exit code for success.
#define CTUTIL_EXIT_SUCCESS                           ((exitCode_t)EXIT_SUCCESS)

// FIXME: Solve conflicts for 1 (reserved error code for general error)
//       - WBM: UnknownArgument
//       - config_tool_lib (C): MISSING_PARAMETER
//       - config_tool_lib (BASH): MISSING_CONFIG_FILE
//       - sudo: Error code for any problem invoking executable
/// Exit code to indicate a general (unspecified) error.
//#define CTUTIL_EXIT_GENERAL_ERROR                     ((exitCode_t)EXIT_FAILURE)
#define CTUTIL_EXIT_GENERAL_ERROR                              ((exitCode_t) 64)

// FIXME: Temporarily value for compatibility with config_tool_lib (C)
/// Exit code to indicate a missing parameter.
#define CTUTIL_EXIT_MISSING_PARAMETER                          ((exitCode_t)  1)

// FIXME: Solve conflicts for 2 (reserved for misuse of shell builtins)
//       - WBM: MissingArgument
//       - config_tool_lib (C): INVALID_PARAMETER
//       - config_tool_lib (BASH): MISSING_PARAMETER
// Do not use exit code 2: Normally reserved for misuse of shell builtins

// FIXME: Temporarily value for compatibility with config_tool_lib (C)
/// Exit code to indicate an invalid parameter.
#define CTUTIL_EXIT_INVALID_PARAMETER                          ((exitCode_t)  2)

// FIXME: Solve conflicts for 3
//       - WBM: InvalidArgument
//       - config_tool_lib (BASH): INVALID_PARAMETER
/// Exit code to indicate a file open error.
#define CTUTIL_EXIT_FILE_OPEN_ERROR                            ((exitCode_t)  3)

// FIXME: Solve conflicts for 4
//       - WBM: ConfigFileMissing
//       - config_tool_lib (BASH): CONFIG_FILE_MISSING
/// Exit code to indicate a file read error.
#define CTUTIL_EXIT_FILE_READ_ERROR                            ((exitCode_t)  4)

// FIXME: Solve conflicts for 5
//       - WBM: ConfigFileInconstistent
//       - config_tool_lib (BASH): CONFIG_FILE_INCONSISTENT
/// Exit code to indicate a file close error.
#define CTUTIL_EXIT_FILE_CLOSE_ERROR                           ((exitCode_t)  5)

// FIXME: Solve conflicts for 6
//       - WBM: ConfigDataError
//       - config_tool_lib (BASH): CONFIG_DATA_ERROR
/// Exit code to indicate a not found error.
#define CTUTIL_EXIT_NOT_FOUND                                  ((exitCode_t)  6)

// FIXME: Solve conflicts for 7
//       - WBM: InternalError
//       - config_tool_lib (BASH): INTERNAL_ERROR
/// Exit code to indicate a system call error.
#define CTUTIL_EXIT_SYSTEM_CALL_ERROR                          ((exitCode_t)  7)

// FIXME: Solve conflicts for 8
//       - WBM: ShellError
//       - config_tool_lib (BASH): SHELL_ERROR
/// Exit code to indicate an inconsistent config file.
#define CTUTIL_EXIT_CONFIG_FILE_INCONSISTENT                   ((exitCode_t)  8)

// FIXME: Solve conflicts for 9
//       - WBM: ExecutableError
//       - config_tool_lib (BASH): EXECUTABLE_ERROR
/// Exit code to indicate a timeout.
#define CTUTIL_EXIT_TIMEOUT                                    ((exitCode_t)  9)

// FIXME: Solve conflicts for 10
//       - WBM: FileNotWritable
//       - config_tool_lib (BASH): FILE_NOT_WRITEABLE
/// Exit code to indicate a file write error.
#define CTUTIL_EXIT_FILE_WRITE_ERROR                           ((exitCode_t) 10)

// FIXME: Solve conflicts for 11
//       - WBM: FileNotExecutable
//       - config_tool_lib (BASH): FILE_NOT_EXECUTABLE
//#define CTUTIL_EXIT_MISSING_PARAMETER                          ((exitCode_t) 11)

// FIXME: Solve conflicts for 12
//       - WBM: FileWriteError
//       - config_tool_lib (BASH): WRITE_FILE_ERROR
//#define CTUTIL_EXIT_INVALID_PARAMETER                          ((exitCode_t) 12)

/// Exit code to indicate a narrow space warning.
#define CTUTIL_EXIT_NARROW_SPACE_WARNING                       ((exitCode_t) 13)

/// Exit code to indicate an error issued by insufficient space.
#define CTUTIL_EXIT_NOT_ENOUGH_SPACE_ERROR                     ((exitCode_t) 14)

/// Exit code to indicate a mount error.
#define CTUTIL_EXIT_MOUNT_ERROR                                ((exitCode_t) 15)

// Ideal area for custom exit codes: 79 to 113
// Do not use codes above 125: Special meanings on shells (like bash)
// See also
//   http://tldp.org/LDP/abs/html/exitcodes.html
//   https://www.gnu.org/software/libc/manual/html_node/Exit-Status.html


/// Status code type for program status (e. g. on function return).
/// Positive values (including 0) indicate an OK status,
/// negative values indicate an error.
/// \see CTUTIL_SUCCESS
/// \see CTUTIL_FAILED
typedef int statusCode_t;

// Common status codes
// Note: There should be an corresponding status code for every exit code
// TODO: Corresponding status codes for every exit code
//------------------------------------------------------------------------------
// Positive status codes count as OK
/// Status code to indicate success in general.
#define CTUTIL_SUCCESS                                    ((statusCode_t)     0)
// Ideal area for custom positiv status codes: 1 to 16000

// Negative status codes count as FAILURE
/// Status code to indicate failure in general.
#define CTUTIL_FAILED                                     ((statusCode_t)    -1)

/// Status code to indicate a file open error.
#define CTUTIL_FILE_OPEN_ERROR                            ((statusCode_t)    -3)

/// Status code to indicate a not found error.
#define CTUTIL_NOT_FOUND                                  ((statusCode_t)    -6)

/// Status code to indicate a system call error.
#define CTUTIL_SYSTEM_CALL_ERROR                          ((statusCode_t)    -7)

/// Status code to indicate an file write error.
#define CTUTIL_FILE_WRITE_ERROR                           ((statusCode_t)   -10)

/// Status code to indicate a missing parameter.
#define CTUTIL_MISSING_PARAMETER                          ((statusCode_t)   -11)

/// Status code to indicate an invalid parameter.
#define CTUTIL_INVALID_PARAMETER                          ((statusCode_t)   -12)
// Ideal area for custom error codes: -256 to -16000

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  //------------------------------------------------------------------------------
  /// Checks status codes for OK.
  ///
  /// \param status
  ///   Status code to check.
  ///
  /// \return
  ///   True, if result is OK.
  ///
  /// \see ctutil_IsStatusFailure
  //------------------------------------------------------------------------------
  extern CTUTIL_API bool ctutil_IsStatusOk(statusCode_t const status);

  //------------------------------------------------------------------------------
  /// Checks status codes for FAILURE.
  ///
  /// \param status
  ///   Status code to check.
  ///
  /// \return
  ///   True, if result is a failure.
  ///
  /// \see ctutil_IsStatusOk
  //------------------------------------------------------------------------------
  extern CTUTIL_API bool ctutil_IsStatusFailure(statusCode_t const status);

  //------------------------------------------------------------------------------
  /// Gets corresponding exit code to a given status code.
  ///
  /// \param status
  ///   Status code to translate into corresponding exit code.
  ///
  /// \return
  ///   Corresponding exit code (if available) or \link CTUTIL_EXIT_GENERAL_ERROR\endlink otherwise.
  ///
  /// \see CTUTIL_EXIT_GENERAL_ERROR
  //------------------------------------------------------------------------------
  extern CTUTIL_API exitCode_t ctutil_GetExitCodeFromStatus(statusCode_t const status);

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


#endif /* INC_CTUTIL_ERROR_CODES_H_ */
//---- End of source file ------------------------------------------------------

