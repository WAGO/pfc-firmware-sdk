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
///  \file     testutil_FileOperations.hpp
///
///  \brief    File operation util functions for google test.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_INC_TESTUTIL_FILE_OPERATIONS_HPP_
#define TEST_INC_TESTUTIL_FILE_OPERATIONS_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <string>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
void testutil_CreateFile(char const * const szFilePath);

std::string testutil_CreateTempDir(char const * szPrefix);

bool testutil_RemoveFile(char const * const szFilePath);

bool testutil_RemoveDir(char const * const szPath);

void testutil_AppendToFile(char const * const szFilePath,
                           uint8_t const * const arByteArray,
                           size_t const bytesToWrite);

char const * testutil_GetFirstMatchingLineFromFile(char const * const szFilePath,
                                                   char const * const szPattern,
                                                   ::std::string &lineBuffer);

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* TEST_INC_TESTUTIL_FILE_OPERATIONS_HPP_ */
//---- End of source file ------------------------------------------------------

