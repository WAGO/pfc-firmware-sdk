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
///  \file     testutil_common_parameter_checks.cc
///
///  \brief    Implementation for common parameter check functions.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "testutil/common_parameter_checks.h"
#include "ctparts/common_main_defaults.h"
#include <gtest/gtest.h>

namespace testutil {
//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
void TestCommonEarlyOptionsDefault(int const argCount,
                                   char * const arszArgs[],
                                   char const * const szArg01Copy,
                                   char const * const szArg02Copy,
                                   ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  ASSERT_LT(2, argCount);
  EXPECT_STREQ(szArg01Copy, arszArgs[1]);
  EXPECT_STREQ(szArg02Copy, arszArgs[2]);
}

void TestCommonOptionsEmpty(int const argCount,
                            char * const arszArgs[],
                            ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_TRUE(pstOptions->stCommonOptions.infoPrint);
}

void TestCommonOptionsQuiet(int const argCount,
                            char * const arszArgs[],
                            ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_TRUE(pstOptions->stCommonOptions.quiet);
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_EQ(CTUTIL_SUCCESS, status);
}


void TestCommonOptionsHelp(int const argCount,
                           char * const arszArgs[],
                           ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_STRNE("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_TRUE(pstOptions->stCommonOptions.earlyExit);
}


} // Namespace testutil


//---- End of source file ------------------------------------------------------
