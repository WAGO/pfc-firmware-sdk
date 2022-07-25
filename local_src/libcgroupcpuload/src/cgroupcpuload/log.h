//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef LOG_H_
#define LOG_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
enum
{
  LogLevel_Quiet,
  LogLevel_Error,
  LogLevel_Info,
  LogLevel_Debug,
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
#define LOG_LEVEL         LogLevel_Quiet

#define DBG(X, ...)       do                                        \
                          {                                         \
                            if (LOG_LEVEL >= LogLevel_Debug)        \
                              fprintf(stderr, X "\n", __VA_ARGS__); \
                          } while (0)

#define INFO(X, ...)      do                                        \
                          {                                         \
                            if (LOG_LEVEL >= LogLevel_Info)         \
                              fprintf(stderr, X "\n", __VA_ARGS__); \
                          } while (0)

#define ERROR(X, ...)     do                                        \
                          {                                         \
                            if (LOG_LEVEL >= LogLevel_Error)        \
                              fprintf(stderr, X "\n", __VA_ARGS__); \
                          } while (0)

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------



#endif /* LOG_H_ */



//---- End of source file ------------------------------------------------------
