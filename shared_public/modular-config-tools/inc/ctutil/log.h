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
///  \file     log.h
///
///  \brief    Log helper functions for modular config tools.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTUTIL_LOG_H_
#define INC_CTUTIL_LOG_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "wc/structuring.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Maximum size for message buffer.
#define MAX_MESSAGE_BUFFER_SIZE   256

/// Log level type
/// \see <a href="http://www.thejoyofcode.com/Logging_Levels_and_how_to_use_them.aspx">
///        http://www.thejoyofcode.com/Logging_Levels_and_how_to_use_them.aspx
///      </a>
///
///   # Off
///   # Logging is disabled on this log level.
///
///   # Fatal
///   # Fatal is used to log all unhandled exceptions and handled errors where it is not recommend to continue program
///   # execution.
///   # This is typically logged inside a catch block at the boundary of your application.
///
///   # Error
///   # Errors are handled 'exceptions' above warnings.
///   # While program operation can safely continue on warnings, errors indicate a critical program state where a master
///   # software part has to consider weather to continue or to stop program execution.
///
///   # Warning
///   # Warning is often used for handled 'exceptions' or other important log events.
///   # For example, if your application requires a configuration setting but has a default in case the setting is missing,
///   # then the Warning level should be used to log the missing configuration setting.
///
///   # Information
///   # The Information level is typically used to output information that is useful to the running and management of your
///   # system. Information would also be the level used to log Entry and Exit points in key areas of your application.
///   # However, you may choose to add more entry and exit points at Debug level for more granularity during development and
///   # testing.
///
///   # Debug
///   # This is the most verbose logging level (maximum volume setting).
///   # I usually consider Debug to be out-of-bounds for a production system and used it only for development and testing.
///   # I prefer to aim to get my logging levels just right so I have just enough information and endeavour to log this at
///   # the Information level or above.
typedef enum
{
  ctutil_LogLevelEnum_off,     //!< ctutil_LogLevelEnum_off
  ctutil_LogLevelEnum_fatal,   //!< ctutil_LogLevelEnum_fatal
  ctutil_LogLevelEnum_error,   //!< ctutil_LogLevelEnum_error
  ctutil_LogLevelEnum_warning, //!< ctutil_LogLevelEnum_warning
  ctutil_LogLevelEnum_info,    //!< ctutil_LogLevelEnum_info
  ctutil_LogLevelEnum_debug,   //!< ctutil_LogLevelEnum_debug
} ctutil_LogLevelEnum_t;

/// Timestamp definition based on a single number.
typedef uint64_t ctutil_Timestamp_t;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  // This function is normally not used directly, but instead of the log macros.
  // TODO: Move this log functionality to a common library
  void ctutil_Log(ctutil_LogLevelEnum_t const logLevel,
                  ctutil_Timestamp_t const * const WC_UNUSED_PARAM(pTimestamp),
                  char const * const szFile,
                  char const * const szFunction,
                  int const line,
                  char const * const szMessage);

  // This function is normally not used directly, but instead of the log macros.
  // TODO: Move this log functionality to a common library
  void ctutil_LogErrno(ctutil_LogLevelEnum_t const logLevel,
                       ctutil_Timestamp_t const * const WC_UNUSED_PARAM(pTimestamp),
                       char const * const szFile,
                       char const * const szFunction,
                       int const line,
                       int const errorNumber,
                       char const * const szPreText);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

/// Internal log macro to add additional information and insert status code to message.
#define CTUTIL_LOG_INTERNAL_STATUS(enable, logLevel, status, message) \
  if(enable) \
  { \
    char const * szMessage = message; \
    char szMessageBuffer[MAX_MESSAGE_BUFFER_SIZE]; \
    if(status != 0) \
    { \
      int const formatResult = snprintf(szMessageBuffer, MAX_MESSAGE_BUFFER_SIZE, "%s (%s %i)", message, "status code", status); \
      if(formatResult < 0) \
      { \
        ctutil_Log(ctutil_LogLevelEnum_warning, NULL, __FILE__, __func__, __LINE__, "Format error"); \
      } \
      else if(formatResult >= MAX_MESSAGE_BUFFER_SIZE) \
      { \
        ctutil_Log(ctutil_LogLevelEnum_warning, NULL, __FILE__, __func__, __LINE__, "Message text too long to add status code"); \
      } \
      else \
      { \
        szMessage = szMessageBuffer; \
      } \
    } \
    ctutil_Log(logLevel, NULL, __FILE__, __func__, __LINE__, szMessage); \
  }

/// Internal log macro to add additional information and errno information.
#define CTUTIL_LOG_INTERNAL_ERRNO(enable, logLevel, errorNumber, pretext) \
  if(enable) \
  { \
    ctutil_LogErrno(logLevel, NULL, __FILE__, __func__, __LINE__, errorNumber, pretext); \
  }

/// Internal log macro to add one additional log parameter into message.
#define CTUTIL_LOG_INTERNAL1(enable, logLevel, format, parameter1) \
  if(enable) \
  { \
    char const * szFormat = format; \
    char szMessageBuffer[MAX_MESSAGE_BUFFER_SIZE]; \
    int const formatResult = snprintf(szMessageBuffer, MAX_MESSAGE_BUFFER_SIZE, szFormat, parameter1); \
    if(formatResult < 0) \
    { \
      ctutil_Log(ctutil_LogLevelEnum_error, NULL, __FILE__, __func__, __LINE__, "Format error"); \
    } \
    else if(formatResult >= MAX_MESSAGE_BUFFER_SIZE) \
    { \
      ctutil_Log(ctutil_LogLevelEnum_error, NULL, __FILE__, __func__, __LINE__, "Message text too long to add parameter"); \
    } \
    else \
    { \
      ctutil_Log(logLevel, NULL, __FILE__, __func__, __LINE__, szMessageBuffer); \
    } \
  }

// Log macros
//------------------------------------------------------------------------------
/// Log macro to log fatal errors.
#define CTUTIL_LOG_FATAL(quiet, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_fatal, 0, message);

/// Log macro to log errors.
#define CTUTIL_LOG_ERROR(quiet, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_error, 0, message);

/// Log macro to log warnings.
#define CTUTIL_LOG_WARN(quiet, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_warning, 0, message);

/// Log macro to log information texts.
#define CTUTIL_LOG_INFO(quiet, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_info, 0, message);

/// Log macro to log debug output.
#define CTUTIL_LOG_DEBUG(quiet, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_debug, 0, message);

/// Log macro to log debug output.
#define CTUTIL_LOG_DEBUG1(quiet, format, parameter1) \
  CTUTIL_LOG_INTERNAL1(!(quiet), ctutil_LogLevelEnum_debug, format, parameter1);

// Log macros with status code integration
//------------------------------------------------------------------------------
/// Log macro to log fatal errors with well known status code from \link ctutil/error_codes.h \endlink.
#define CTUTIL_LOG_STATUS_FATAL(quiet, status, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_fatal, status, message);

/// Log macro to log errors with well known status code from \link ctutil/error_codes.h \endlink.
#define CTUTIL_LOG_STATUS_ERROR(quiet, status, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_error, status, message);

/// Log macro to log warnings with well known status code from \link ctutil/error_codes.h \endlink.
#define CTUTIL_LOG_STATUS_WARN(quiet, status, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_warning, status, message);

/// Log macro to log information texts with well known status code from \link ctutil/error_codes.h \endlink.
#define CTUTIL_LOG_STATUS_INFO(quiet, status, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_info, status, message);

/// Log macro to log debug output with well known status code from \link ctutil/error_codes.h \endlink.
#define CTUTIL_LOG_STATUS_DEBUG(quiet, status, message) \
  CTUTIL_LOG_INTERNAL_STATUS(!(quiet), ctutil_LogLevelEnum_debug, status, message);

// Log macros with errno error code text expansion
//------------------------------------------------------------------------------
/// Log macro to log fatal errors with errno code.
#define CTUTIL_LOG_ERRNO_FATAL(quiet, errorNumber, pretext) \
  CTUTIL_LOG_INTERNAL_ERRNO(!(quiet), ctutil_LogLevelEnum_fatal, errorNumber, pretext);

/// Log macro to log errors with errno code.
#define CTUTIL_LOG_ERRNO_ERROR(quiet, errorNumber, pretext) \
  CTUTIL_LOG_INTERNAL_ERRNO(!(quiet), ctutil_LogLevelEnum_error, errorNumber, pretext);

/// Log macro to log warnings with errno code.
#define CTUTIL_LOG_ERRNO_WARN(quiet, errorNumber, pretext) \
  CTUTIL_LOG_INTERNAL_ERRNO(!(quiet), ctutil_LogLevelEnum_warning, errorNumber, pretext);

/// Log macro to log information texts with errno code.
#define CTUTIL_LOG_ERRNO_INFO(quiet, errorNumber, pretext) \
  CTUTIL_LOG_INTERNAL_ERRNO(!(quiet), ctutil_LogLevelEnum_info, errorNumber, pretext);

/// Log macro to log debug output with errno code.
#define CTUTIL_LOG_ERRNO_DEBUG(quiet, errorNumber, pretext) \
  CTUTIL_LOG_INTERNAL_ERRNO(!(quiet), ctutil_LogLevelEnum_debug, errorNumber, pretext);

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* INC_CTUTIL_LOG_H_ */
//---- End of source file ------------------------------------------------------

