//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project common-header (PTXdist package libcommonheader).
//
// Copyright (c) 2019-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file
///
///  \brief    Common log interface and framework to manage log messages.
///            To use log functions it is necessary to implement log level and
///            output functions
///            (\link wc_get_log_level \endlink /\link wc_log_output \endlink).
///
///  \author   MaHe: WAGO GmbH & Co. KG
///  \author   PEn : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_WC_LOG_H_
#define INC_WC_LOG_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "wc/compiler.h"
#include "wc/structuring.h"
#include "wc/std_includes.h"

#ifdef __cplusplus
#include <cstdarg>
#include <cstdio>
#include <cstring>
#else
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#endif

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
/// \def WC_LOG_FORMAT_BUFFER_SIZE
/// Macro defining maximum length of format buffer containing log format strings.
///
/// \note Do not use directly but function
///       \link wc_get_max_log_format_size \endlink instead.
///
/// \see wc_get_max_log_format_size
#define WC_LOG_FORMAT_BUFFER_SIZE                                          (256)

/// \def WC_LOG_BUFFER_SIZE
/// Macro defining maximum length of message buffer containing log strings.
///
/// \note Do not use directly but function
///       \link wc_get_max_log_message_size \endlink instead.
///
/// \see wc_get_max_log_message_size
#define WC_LOG_BUFFER_SIZE                                                (4096)

/// \def WC_LOG_MAX_FILE_LENGTH
/// Macro defining maximum length of logged file names.
///
/// \note For internal use only. Don't use externally.
#define WC_LOG_MAX_FILE_LENGTH                                              (50)

/// \def WC_LOG_MAX_FUNCTION_LENGTH
/// Macro defining maximum length of logged function names.
///
/// \note For internal use only. Don't use externally.
#define WC_LOG_MAX_FUNCTION_LENGTH                                          (50)

/// Defines various log levels.
/// \see <a href="http://www.thejoyofcode.com/Logging_Levels_and_how_to_use_them.aspx">Logging Levels and how to use them</a>
typedef enum log_level
{
  /// Off
  /// Logging is disabled on this log level.
  off = 0,    //!< off

  /// Fatal
  /// Fatal is used to log all unhandled exceptions and handled errors where it is not recommend to continue program
  /// execution. This is typically logged inside a catch block at the boundary of your application.
  fatal = 1,  //!< fatal

  /// Error
  /// Errors are handled 'exceptions' above warnings.
  /// While program operation can safely continue on warnings, errors indicate a critical program state where a master
  /// software part has to consider weather to continue or to stop program execution.
  error = 2,  //!< error

  /// Warning
  /// Warning is often used for handled 'exceptions' or other important log events.
  /// For example, if your application requires a configuration setting but has a default in case the setting is missing,
  /// then the Warning level should be used to log the missing configuration setting.
  warning = 3,//!< warning

  /// Information
  /// The Information level is typically used to output information that is useful to the running and management of your
  /// system. Information would also be the level used to log Entry and Exit points in key areas of your application.
  /// However, you may choose to add more entry and exit points at Debug level for more granularity during development and
  /// testing.
  info = 4,   //!< info

  /// Debug
  /// This is the most verbose logging level (maximum volume setting).
  /// I usually consider Debug to be out-of-bounds for a production system and used it only for development and testing.
  /// I prefer to aim to get my logging levels just right so I have just enough information and endeavour to log this at
  /// the Information level or above.
  debug = 5   //!< debug
} log_level_t;

/// \def WC_DEBUG_LOG
/// Defines an advanced debug log call, fully optimized out on release builds.
//lint -estring(961, WC_DEBUG_LOG) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                 function-like macro defined
//lint -estring(960, WC_DEBUG_LOG) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                 disallowed definition for macro
#ifndef NDEBUG
#define WC_DEBUG_LOG(message)                              wc_log_advanced(__FILE__, WC_ARRAY_TO_PTR(__func__), __LINE__, debug, message)
#else
#define WC_DEBUG_LOG(message)                                                                             (void)message
#endif

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  /// Prototype for application specific output function to write log output.
  ///
  /// \note The function implementation has to be provided application specific.
  ///
  /// \param log_level
  ///   Log level of message to write, may be part of written message or used to
  ///   categorize log message.
  /// \param message
  ///   Log message to write to an output channel.
  //------------------------------------------------------------------------------
  void wc_log_output(log_level_t const  log_level,
                     char const * const message);

  /// Prototype for application specific source of log level setting.
  ///
  /// \note The function implementation has to be provided application specific.
  ///
  /// \return
  ///   Currently active log level, used to forward or discard log messages.
  //------------------------------------------------------------------------------
  log_level_t wc_get_log_level(void);

  /// Simple log function to log static messages.
  ///
  /// \param log_level
  ///   Log level for message, used to forward or discard message dependent on
  ///   currently active application log level.
  /// \param message
  ///   Log message to compute.
  ///
  /// \see wc_get_log_level
  /// \see wc_log_advanced
  /// \see wc_log_format
  //------------------------------------------------------------------------------
  static inline void wc_log(log_level_t const  log_level,
                            char const * const message);

  /// Log function to log formatted messages.
  ///
  /// \param log_level
  ///   Log level for message, used to forward or discard message dependent on
  ///   currently active application log level.
  /// \param format
  ///   Log message format in printf-style to generate log message.
  ///   Format arguments follow in next parameters like on printf.
  ///
  /// \see wc_get_log_level
  /// \see wc_vlog_format
  /// \see wc_log
  /// \see wc_log_advanced
  //------------------------------------------------------------------------------
  //lint -printf(2, wc_log_format)
  static inline void wc_log_format(log_level_t const  log_level,
                                   char const * const format,
                                   ...) GNUC_ATTRIBUTE(format (gnu_printf, 2, 3));  //lint !e960 !e1916 Variable number of arguments needed

  /// Log function to log formatted messages.
  ///
  /// \param log_level
  ///   Log level for message, used to forward or discard message dependent on
  ///   currently active application log level.
  /// \param format
  ///   Log message format in printf-style to generate log message.
  ///   Format arguments follow in next va_list parameter like on vprintf.
  /// \param args
  ///   Variable argument list with format arguments.
  ///
  /// \see wc_get_log_level
  /// \see wc_log_format
  /// \see wc_vlog_format_advanced
  //------------------------------------------------------------------------------
  static inline void wc_vlog_format(log_level_t const  log_level,
                                    char const * const format,
                                    va_list * const    args) GNUC_ATTRIBUTE(format (gnu_printf, 2, 0));

  /// Log function to log static messages with source (file, function, line)
  /// information.
  /// Source information is only computed when currently used application log level
  /// is "debug", determined by function \link wc_get_log_level \endlink.
  ///
  /// \param file
  ///   File name as source information for log message, normally compiler auto
  ///   values used like "__FILE__". When NULL source information is omitted.
  /// \param function
  ///   Function name as source information for log message, normally compiler
  ///   auto values used like "__func__". This source information is optional.
  ///   Source information is completely omitted when file parameter is NULL.
  /// \param line
  ///   Line number as source information for log message, normally compiler
  ///   auto values used like "__LINE__".
  ///   Source information is completely omitted when file parameter is NULL.
  /// \param log_level
  ///   Log level for message, used to forward or discard message dependent on
  ///   currently active application log level.
  /// \param message
  ///   Log message to compute.
  ///
  /// \see wc_get_log_level
  /// \see wc_log
  /// \see wc_log_format_advanced
  //------------------------------------------------------------------------------
  static inline void wc_log_advanced(char const * const file,
                                     char const * const function,
                                     size_t const       line,
                                     log_level_t const  log_level, 
                                     char const * const message);

  /// Log function to log formatted messages with source (file, function, line)
  /// information.
  /// Source information is only computed when currently used application log level
  /// is "debug", determined by function \link wc_get_log_level \endlink.
  ///
  /// \param file
  ///   File name as source information for log message, normally compiler auto
  ///   values used like "__FILE__". When NULL source information is omitted.
  /// \param function
  ///   Function name as source information for log message, normally compiler
  ///   auto values used like "__func__". This source information is optional.
  ///   Source information is completely omitted when file parameter is NULL.
  /// \param line
  ///   Line number as source information for log message, normally compiler
  ///   auto values used like "__LINE__".
  ///   Source information is completely omitted when file parameter is NULL.
  /// \param log_level
  ///   Log level for message, used to forward or discard message dependent on
  ///   currently active application log level.
  /// \param format
  ///   Log message format in printf-style to generate log message.
  ///   Format arguments follow in next parameters like on printf.
  ///
  /// \see wc_get_log_level
  /// \see wc_vlog_format_advanced
  /// \see wc_log_format
  /// \see wc_log_advanced
  //------------------------------------------------------------------------------
  //lint -printf(5, wc_log_format_advanced)
  static inline void wc_log_format_advanced(char const * const file,
                                            char const * const function,
                                            size_t const       line,
                                            log_level_t const  log_level, 
                                            char const * const format,
                                            ...) GNUC_ATTRIBUTE(format (gnu_printf, 5, 6));  //lint !e960 !e1916 Variable number of arguments needed 

  /// Log function to log formatted messages with source (file, function, line)
  /// information.
  /// Source information is only computed when currently used application log level
  /// is "debug", determined by function \link wc_get_log_level \endlink.
  ///
  /// \param file
  ///   File name as source information for log message, normally compiler auto
  ///   values used like "__FILE__". When NULL source information is omitted.
  /// \param function
  ///   Function name as source information for log message, normally compiler
  ///   auto values used like "__func__". This source information is optional.
  ///   Source information is completely omitted when file parameter is NULL.
  /// \param line
  ///   Line number as source information for log message, normally compiler
  ///   auto values used like "__LINE__".
  ///   Source information is completely omitted when file parameter is NULL.
  /// \param log_level
  ///   Log level for message, used to forward or discard message dependent on
  ///   currently active application log level.
  /// \param format
  ///   Log message format in printf-style to generate log message.
  ///   Format arguments follow in next va_list parameter like on vprintf.
  /// \param args
  ///   Variable argument list with format arguments.
  ///
  /// \see wc_get_log_level
  /// \see wc_log_format_advanced
  /// \see wc_log_format
  /// \see wc_log_advanced
  //------------------------------------------------------------------------------
  static inline void wc_vlog_format_advanced(char const * const file,
                                             char const * const function,
                                             size_t const       line,
                                             log_level_t const  log_level, 
                                             char const * const format,
                                             va_list * const    args) GNUC_ATTRIBUTE(format (gnu_printf, 5, 0));

#ifndef __cplusplus

  /// Function (C variant) to determine maximum log message size.
  ///
  /// \return
  ///   Maximum log message size in bytes.
  //------------------------------------------------------------------------------
  static inline size_t wc_get_max_log_message_size() 
  {
    return WC_LOG_BUFFER_SIZE;
  }

  /// Function (C variant) to determine maximum log format size.
  ///
  /// \return
  ///   Maximum log format size in bytes.
  //------------------------------------------------------------------------------
  static inline size_t wc_get_max_log_format_size()
  {
    return WC_LOG_FORMAT_BUFFER_SIZE;
  }

#else

  /// Function (C++ variant: constexpr) to determine maximum log message size.
  ///
  /// \return
  ///   Maximum log message size in bytes.
  //------------------------------------------------------------------------------
  static inline constexpr size_t wc_get_max_log_message_size()
  {
    return WC_LOG_BUFFER_SIZE;
  }

  /// Function (C++ variant: constexpr) to determine maximum log format size.
  ///
  /// \return
  ///   Maximum log format size in bytes.
  //------------------------------------------------------------------------------
  static inline constexpr size_t wc_get_max_log_format_size()
  {
    return WC_LOG_FORMAT_BUFFER_SIZE;
  }

#endif // __cplusplus

  static inline void wc_log(log_level_t const  log_level,
                            char const * const message)
  {
    wc_log_advanced(NULL, NULL, 0, log_level, message);
  }

  static inline void wc_log_format(log_level_t const  log_level,
                                   char const * const format,
                                   ...)
  {
    va_list args;
    va_start(args, format); //lint !e530 Macro initializes list
    wc_vlog_format(log_level, format, &args);
    va_end(args);
  }

  static inline void wc_vlog_format(log_level_t const  log_level,
                                    char const * const format,
                                    va_list * const    args)
  {
    wc_vlog_format_advanced(NULL, NULL, 0, log_level, format, args);
  }

  static inline void wc_log_advanced(char const * const file,
                                     char const * const function,
                                     size_t const       line,
                                     log_level_t const  log_level, 
                                     char const * const message) 
  {
    wc_log_format_advanced(file, function, line, log_level, "%s", message);
  }

  static inline void wc_log_format_advanced(char const * const file,
                                            char const * const function,
                                            size_t const       line,
                                            log_level_t const  log_level, 
                                            char const * const format,
                                            ...)
  {
    va_list args;
    va_start(args, format); //lint !e530 Macro initializes list
    wc_vlog_format_advanced(file, function, line, log_level, format, &args);
    va_end(args);
  }

  static inline void wc_vlog_format_advanced(char const * const file,
                                             char const * const function,
                                             size_t const       line,
                                             log_level_t const  log_level, 
                                             char const * const format,
                                             va_list * const    args)
  {
    if ( (log_level > off) && (wc_get_log_level() >= log_level) ) 
    {
      char const * format_buffer = format;
      char buffer[WC_LOG_FORMAT_BUFFER_SIZE] = { 0 };
      if ( (wc_get_log_level() == debug) && (file != NULL) ) {

        char const * function_name = function;
        if (function_name == NULL) {
          function_name = "<unknown>";
        }

        char const *file_shortend = file;
        size_t const file_length = strlen(file);
        char file_buffer[WC_LOG_MAX_FILE_LENGTH + 1] = { 0 };
        if (file_length > WC_LOG_MAX_FILE_LENGTH) 
        {
          strncpy(file_buffer + (file_length - WC_LOG_MAX_FILE_LENGTH), file, WC_LOG_MAX_FILE_LENGTH);
          file_shortend = file_buffer;
        }

        char const *function_shortend = function_name;
        size_t const function_length = strlen(function_name);
        char function_buffer[WC_LOG_MAX_FUNCTION_LENGTH + 1] = { 0 };
        if (function_length > WC_LOG_MAX_FUNCTION_LENGTH) 
        {
          strncpy(function_buffer + (function_length - WC_LOG_MAX_FUNCTION_LENGTH), function_name, WC_LOG_MAX_FUNCTION_LENGTH);
          function_shortend = function_buffer;
        }

        int const print_result = snprintf(buffer, WC_LOG_FORMAT_BUFFER_SIZE, "%s @%s, %u: ", file_shortend, function_shortend, line);
        WC_ASSERT(print_result >= 0);

        strncat(buffer, format, (WC_LOG_FORMAT_BUFFER_SIZE - 1));
        buffer[WC_LOG_FORMAT_BUFFER_SIZE - 1] = '\0';
        format_buffer = buffer;
      }

      char out_buffer[WC_LOG_BUFFER_SIZE] = { 0 };
      int const print_result = vsnprintf(out_buffer, WC_LOG_BUFFER_SIZE, format_buffer, *args);
      out_buffer[WC_LOG_BUFFER_SIZE - 1] = '\0';
      WC_ASSERT(print_result >= 0);

      if(print_result > 0)
      {
        if(print_result > WC_LOG_BUFFER_SIZE)
        {
            wc_log_output(log_level, "Log message too long, next log line shortened.");
        }
        wc_log_output(log_level, out_buffer);
      }
    }
  }

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


#endif /* INC_WC_LOG_H_ */
//---- End of source file ------------------------------------------------------

