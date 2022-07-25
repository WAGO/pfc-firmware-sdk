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
///  \file     test_ParamEvaluation.cc
///
///  \brief    Tests for parameter evaluation of config tool reserved_area.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctparts/common_main_defaults.h"
#include "params.h"
#include "testutil/common_parameter_checks.h"
#include <gtest/gtest.h>
#include <string.h>

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
class OptionEvaluationTest_reserved_area : public ::testing::Test
{
  protected:
    ctutil_Options_t stOptions;

  protected:
    OptionEvaluationTest_reserved_area() {};
    virtual ~OptionEvaluationTest_reserved_area() {};
    virtual void SetUp()
    {
      memset(&stOptions, 0, sizeof(stOptions));
      statusCode_t status = CTUTIL_SUCCESS;
      status = ctparts_SetOptionDefaults(NULL, &(this->stOptions));
      ASSERT_EQ(CTUTIL_SUCCESS, status);
    }
    virtual void TearDown()
    {
      ctparts_ReleaseSpecificOptions(NULL, &(this->stOptions));
    }
};

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST(OptionEvaluationBasicTest_reserved_area, SetOptionDefaultsAndRelease)
{
  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  stOptions.stCommonOptions.infoPrint = false;
  stOptions.stCommonOptions.quiet = false;
  stOptions.stCommonOptions.extractFromUri = false;
  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_SetOptionDefaults(NULL, &stOptions);
  ASSERT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_TRUE(stOptions.stCommonOptions.infoPrint); // Default for config tools (print if no other parameter given)
  EXPECT_FALSE(stOptions.stCommonOptions.quiet); // Default for new config tools
  EXPECT_FALSE(stOptions.stCommonOptions.extractFromUri); // Default for new config tools
  EXPECT_NE((void *)NULL, stOptions.pstSpecificOptions); // Needed because specific options possible
  ctparts_ReleaseSpecificOptions(NULL, &stOptions);
  EXPECT_EQ((void *)NULL, stOptions.pstSpecificOptions); // Pointer have to be reseted to NULL
}


TEST_F(OptionEvaluationTest_reserved_area, CheckSetup)
{
  // Nothing else to do here, everything is done in SetUp/TearDown
}


TEST_F(OptionEvaluationTest_reserved_area, EvaluateEarlyOptions)
{
  char szArg00[] = "test";
  char szArg01[] = "otherOptionsShouldBeIgnored";
  char szArg02[] = "encoded%28Options%29MustNotBeDecodedByDefault";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  // Don't use szArg01, Arguments decoded in place!
  char const * const szArg01Copy = "otherOptionsShouldBeIgnored";
  // Don't use szArg02, Arguments decoded in place!
  char const * const szArg02Copy = "encoded%28Options%29MustNotBeDecodedByDefault";
  testutil::TestCommonEarlyOptionsDefault(argCount, arszArgs, szArg01Copy, szArg02Copy, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_reserved_area, EvaluateOptionsEmpty)
{
  char szArg00[] = "test";
  char * const arszArgs[]
  {
    szArg00,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  testutil::TestCommonOptionsEmpty(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_reserved_area, EvaluateOptionsQuiet)
{
  char szArg00[] = "test";
  char szArg01[] = "--quiet";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  testutil::TestCommonOptionsQuiet(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_reserved_area, EvaluateOptionsQuietShort)
{
  char szArg00[] = "test";
  char szArg01[] = "-q";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  testutil::TestCommonOptionsQuiet(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_reserved_area, EvaluateOptionsHelp)
{
  char szArg00[] = "test";
  char szArg01[] = "--help";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  testutil::TestCommonOptionsHelp(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_reserved_area, EvaluateOptionsHelpShort)
{
  char szArg00[] = "test";
  char szArg01[] = "-h";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  testutil::TestCommonOptionsHelp(argCount, arszArgs, &(this->stOptions));
}


//---- End of source file ------------------------------------------------------

