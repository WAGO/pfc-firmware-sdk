//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2019-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file
///
///  \brief    Header definitions for common parameter check functions.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_INC_TESTUTIL_COMMON_PARAMETER_CHECKS_H_
#define TEST_INC_TESTUTIL_COMMON_PARAMETER_CHECKS_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/param_handling.h"

namespace testutil {
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
void TestCommonEarlyOptionsDefault(int const argCount,
                                   char * const arszArgs[],
                                   char const * const szArg01Copy,
                                   char const * const szArg02Copy,
                                   ctutil_Options_t * const pstOptions);

void TestCommonOptionsEmpty(int const argCount,
                            char * const arszArgs[],
                            ctutil_Options_t * const pstOptions);

void TestCommonOptionsQuiet(int const argCount,
                            char * const arszArgs[],
                            ctutil_Options_t * const pstOptions);

void TestCommonOptionsHelp(int const argCount,
                           char * const arszArgs[],
                           ctutil_Options_t * const pstOptions);

} // Namespace testutil


#endif /* TEST_INC_TESTUTIL_COMMON_PARAMETER_CHECKS_H_ */
//---- End of source file ------------------------------------------------------

