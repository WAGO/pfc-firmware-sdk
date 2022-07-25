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
///  \file     json.h
///
///  \brief    JSON format tools.
///            TODO: Maybe replace by common library already present?
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTUTIL_JSON_H_
#define INC_CTUTIL_JSON_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
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

  // TODO: Create doxygen description
  extern CTUTIL_API void ctutil_EscapeJsonStringValue(char * const szDestination,
                                                      size_t const freeBufferSize,
                                                      char const * const szSource,
                                                      size_t * const pWrittenCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringOpen(char * const szObjectString,
                                                             size_t const freeBufferSize,
                                                             size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringClose(char * const szObjectString,
                                                              size_t const freeBufferSize,
                                                              size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteMemberSeparator(char * const szObjectString,
                                                                             size_t const freeBufferSize,
                                                                             size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteNameValueSeparator(char * const szObjectString,
                                                                                size_t const freeBufferSize,
                                                                                size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteName(char * const szObjectString,
                                                                  char const * const szName,
                                                                  size_t const freeBufferSize,
                                                                  size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteStringValue(char * const szObjectString,
                                                                         char const * const szName,
                                                                         char const * const szValue,
                                                                         size_t const freeBufferSize,
                                                                         size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteBoolValue(char * const szObjectString,
                                                                       char const * const szName,
                                                                       bool const value,
                                                                       size_t const freeBufferSize,
                                                                       size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteSizeValue(char * const szObjectString,
                                                                       char const * const szName,
                                                                       size_t const value,
                                                                       size_t const freeBufferSize,
                                                                       size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteUInt64Value(char * const szObjectString,
                                                                         char const * const szName,
                                                                         uint64_t const value,
                                                                         size_t const freeBufferSize,
                                                                         size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteArrayStart(char * const szObjectString,
                                                                        char const * const szName,
                                                                        size_t const freeBufferSize,
                                                                        size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteArrayEnd(char * const szObjectString,
                                                                      size_t const freeBufferSize,
                                                                      size_t * const pAddedCharacters);

  // TODO: Create doxygen description
  extern CTUTIL_API statusCode_t ctutil_JsonObjectStringWriteArrayStringValue(char * const szObjectString,
                                                                              char const * const szValue,
                                                                              size_t const freeBufferSize,
                                                                              size_t * const pAddedCharacters);

  // TODO: Add more common types when needed

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


#endif /* INC_CTUTIL_JSON_H_ */
//---- End of source file ------------------------------------------------------

