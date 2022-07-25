//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     error_codes.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_CPULOADMONITOR_ERROR_CODES_H_
#define SRC_CPULOADMONITOR_ERROR_CODES_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Status code type for program status (e. g. on function return).
/// Positive values (including 0) indicate an OK status,
/// negative values indicate an error.
/// \see CPULOADMONITOR_SUCCESS
/// \see CPULOADMONITOR_FAILED
typedef int statusCode_t;

// Common status codes
// Note: There should be an corresponding status code for every exit code
// TODO: Corresponding status codes for every exit code
//------------------------------------------------------------------------------
// Positive status codes count as OK
/// Status code to indicate success in general.
#define CPULOADMONITOR_SUCCESS                                    ((statusCode_t)     0)
// Ideal area for custom positive status codes: 1 to 16000

// Negative status codes count as FAILURE
/// Status code to indicate failure in general.
#define CPULOADMONITOR_FAILED                                     ((statusCode_t)    -1)

/// Status code to indicate a file open error.
#define CPULOADMONITOR_FILE_OPEN_ERROR                            ((statusCode_t)    -3)

/// Status code to indicate a not found error.
#define CPULOADMONITOR_NOT_FOUND                                  ((statusCode_t)    -6)

/// Status code to indicate a system call error.
#define CPULOADMONITOR_SYSTEM_CALL_ERROR                          ((statusCode_t)    -7)

/// Status code to indicate an file write error.
#define CPULOADMONITOR_FILE_WRITE_ERROR                           ((statusCode_t)   -10)

/// Status code to indicate a missing parameter.
#define CPULOADMONITOR_MISSING_PARAMETER                          ((statusCode_t)   -11)

/// Status code to indicate an invalid parameter.
#define CPULOADMONITOR_INVALID_PARAMETER                          ((statusCode_t)   -12)
// Ideal area for custom error codes: -256 to -16000


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


#endif /* SRC_CPULOADMONITOR_ERROR_CODES_H_ */
//---- End of source file ------------------------------------------------------

