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
///  \file     shadow_functions.h
///
///  \brief    Shadow modify and cryption functionality for config_linux_user.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_CONFIG_LINUX_USER_SHADOW_FUNCTIONS_H_
#define SRC_CONFIG_LINUX_USER_SHADOW_FUNCTIONS_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "resources.h"
#include <shadow.h>

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

  void GetRandomBytes(ctutil_Resources_t const * const pstResources,
                      unsigned char * const szBuffer,
                      size_t const bufferSize);

  char * CryptPwd(ctutil_Resources_t const * const pstResources,
                  char const * const szPassword);

  statusCode_t FillUserData(struct spwd * const stUserData,
                            ctutil_Resources_t const * const pstResources,
                            char const * const szUsername,
                            char const * const szPassword1,
                            char const * const szPassword2);

  statusCode_t FillUserDataFromHash(struct spwd * const stUserData,
                                    ctutil_Resources_t const * const pstResources,
                                    char const * const szUsername,
                                    char const * const szHash);

  statusCode_t SearchShadowEntry(char const * const szUsername);

  statusCode_t WriteShadow(ctutil_Resources_t const * const pstResources,
                           struct spwd const * const stNewUserData);

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


#endif /* SRC_CONFIG_LINUX_USER_SHADOW_FUNCTIONS_H_ */
//---- End of source file ------------------------------------------------------

