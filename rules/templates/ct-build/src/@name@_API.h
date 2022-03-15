//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     @name@_API.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_@NAME@_API_H_
#define SRC_@NAME@_API_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define WC_API_EXPORT __attribute__((visibility("default")))

// Tell compiler and linker that the functions declared with this macro should not be removed from the .so API.
// @NAME@_API will be set to WC_API_EXPORT when the implementing project is compiled.
#ifndef @NAME@_API
  #define @NAME@_API
#endif

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  @NAME@_API uint16_t @name_c@_GetVersion(void);
  @NAME@_API uint16_t @name_c@_first_Self(void);
  @NAME@_API uint16_t @name_c@_second_Self(void);

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


#endif /* SRC_@NAME@_API_H_ */
//---- End of source file ------------------------------------------------------

