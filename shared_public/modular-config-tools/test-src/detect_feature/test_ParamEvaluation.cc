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
///  \brief    Tests for parameter evaluation of config tool detect_feature.
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
class OptionEvaluationTest_detect_feature : public ::testing::Test
{
  protected:
    ctutil_Options_t stOptions;

  protected:
    OptionEvaluationTest_detect_feature() {};
    virtual ~OptionEvaluationTest_detect_feature() {};
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
TEST(OptionEvaluationBasicTest_detect_feature, SetOptionDefaultsAndRelease)
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


TEST_F(OptionEvaluationTest_detect_feature, CheckSetup)
{
  // Nothing else to do here, everything is done in SetUp/TearDown
}


TEST_F(OptionEvaluationTest_detect_feature, EvaluateEarlyOptions)
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


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsEmpty)
{
  char szArg00[] = "test";
  char * const arszArgs[]
  {
    szArg00,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  testutil::TestCommonOptionsEmpty(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsQuiet)
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


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsQuietShort)
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


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsHelp)
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


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsHelpShort)
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


static void TestOptionsFeatureRequest(int const argCount,
                                      char * const arszArgs[],
                                      ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  ASSERT_LT(2, argCount);
  EXPECT_STREQ(arszArgs[2], pstOptions->pstSpecificOptions->szFeature);
  EXPECT_FALSE(pstOptions->pstSpecificOptions->listMode);
}


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsFeatureRequest)
{
  char szArg00[] = "test";
  char szArg01[] = "--feature";
  char szArg02[] = "test-feature";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  TestOptionsFeatureRequest(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsFeatureRequestShort)
{
  char szArg00[] = "test";
  char szArg01[] = "-f";
  char szArg02[] = "test-feature";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  TestOptionsFeatureRequest(argCount, arszArgs, &(this->stOptions));
}


static void TestOptionsFeatureList(int const argCount,
                                   char * const arszArgs[],
                                   ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_TRUE(pstOptions->pstSpecificOptions->listMode);
  EXPECT_EQ(nullptr, pstOptions->pstSpecificOptions->szFeature);
}


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsFeatureList)
{
  char szArg00[] = "test";
  char szArg01[] = "--list";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  TestOptionsFeatureList(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsFeatureListShort)
{
  char szArg00[] = "test";
  char szArg01[] = "-l";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  TestOptionsFeatureList(argCount, arszArgs, &(this->stOptions));
}


static void TestOptionsOnlyValue(int const argCount,
                                 char * const arszArgs[],
                                 ctutil_Options_t * const pstOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, pstOptions);
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_NE(nullptr, pstOptions->pstSpecificOptions->szFeature);
  EXPECT_TRUE(pstOptions->pstSpecificOptions->onlyValue);
}


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsOnlyValue)
{
  char szArg00[] = "test";
  char szArg01[] = "-f";
  char szArg02[] = "testfeaturename";
  char szArg03[] = "--only-value";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
    szArg03,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  TestOptionsOnlyValue(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsOnlyValueShort)
{
  char szArg00[] = "test";
  char szArg01[] = "-f";
  char szArg02[] = "testfeaturename";
  char szArg03[] = "-o";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
    szArg03,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  TestOptionsOnlyValue(argCount, arszArgs, &(this->stOptions));
}



TEST_F(OptionEvaluationTest_detect_feature, EvaluateOptionsOnlyValueFeatureDependency)
{
  char szArg00[] = "test";
  char szArg01[] = "-o";
  char szArg02[] = "-l";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_STRNE("", testing::internal::GetCapturedStderr().c_str()); // Expected no ASSERT before GetCapturedStderr!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_EQ(nullptr, this->stOptions.pstSpecificOptions->szFeature);
  EXPECT_FALSE(this->stOptions.pstSpecificOptions->onlyValue);
}


//---- End of source file ------------------------------------------------------

