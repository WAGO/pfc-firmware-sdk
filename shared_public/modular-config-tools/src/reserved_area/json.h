//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     json.h
///
///  \brief    JSON format tools.
///            TODO: Should be moved to a library.
///            TODO: Maybe replace by common library already present?
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_RESERVED_AREA_JSON_H_
#define SRC_RESERVED_AREA_JSON_H_

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

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  void EscapeJsonStringValue(char * const szDestination,
                             size_t const freeBufferSize,
                             char const * const szSource,
                             size_t * const pWrittenCharacters);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t JsonObjectStringOpen(char * const szObjectString,
                                    size_t const freeBufferSize,
                                    size_t * const pAddedCharacters);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t JsonObjectStringClose(char * const szObjectString,
                                     size_t const freeBufferSize,
                                     size_t * const pAddedCharacters);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t JsonObjectStringWriteMemberSeparator(char * const szObjectString,
                                                   size_t const freeBufferSize,
                                                   size_t * const pAddedCharacters);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t JsonObjectStringWriteNameValueSeparator(char * const szObjectString,
                                                       size_t const freeBufferSize,
                                                       size_t * const pAddedCharacters);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t JsonObjectStringWriteName(char * const szObjectString,
                                         char const * const szName,
                                         size_t const freeBufferSize,
                                         size_t * const pAddedCharacters);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t JsonObjectStringWriteStringValue(char * const szObjectString,
                                                char const * const szName,
                                                char const * const szValue,
                                                size_t const freeBufferSize,
                                                size_t * const pAddedCharacters);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t JsonObjectStringWriteSizeValue(char * const szObjectString,
                                              char const * const szName,
                                              size_t const value,
                                              size_t const freeBufferSize,
                                              size_t * const pAddedCharacters);

  // TODO: Move to libconfigtoolutils
  // TODO: Create doxygen description
  statusCode_t JsonObjectStringWriteUInt64Value(char * const szObjectString,
                                                char const * const szName,
                                                uint64_t const value,
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


#endif /* SRC_RESERVED_AREA_JSON_H_ */
//---- End of source file ------------------------------------------------------

