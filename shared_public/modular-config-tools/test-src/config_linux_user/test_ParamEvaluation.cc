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
///  \file     test_ParamEvaluation.cc
///
///  \brief    Tests for parameter evaluation of config tool config_linux_user.
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
class OptionEvaluationTest_config_linux_user : public ::testing::Test
{
  protected:
    ctutil_Options_t stOptions;

  protected:
    OptionEvaluationTest_config_linux_user() {};
    virtual ~OptionEvaluationTest_config_linux_user() {};
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
TEST(OptionEvaluationBasicTest_config_linux_user, SetOptionDefaultsAndRelease)
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
  EXPECT_TRUE(stOptions.stCommonOptions.quiet); // Needed because quiet option not available in first public interface
  EXPECT_TRUE(stOptions.stCommonOptions.extractFromUri); // Needed because extract option not available in first public interface
  EXPECT_NE((void *)NULL, stOptions.pstSpecificOptions); // Needed because specific options possible
  ctparts_ReleaseSpecificOptions(NULL, &stOptions);
  EXPECT_EQ((void *)NULL, stOptions.pstSpecificOptions); // Pointer have to be reseted to NULL
}


TEST_F(OptionEvaluationTest_config_linux_user, CheckSetup)
{
  // Nothing else to do here, everything is done in SetUp/TearDown
}


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateEarlyOptions)
{
  char szArg00[] = "test";
  char szArg01[] = "otherOptionsShouldBeIgnored";
  char szArg02[] = "encoded%28Options%29MustBeDecoded";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  // Other default behavior expected than in new config tools by TestCommonEarlyOptionsDefault
  statusCode_t status = CTUTIL_SUCCESS;
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_STREQ("encoded(Options)MustBeDecoded", szArg02);
}


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateOptionsEmpty)
{
  char szArg00[] = "test";
  char * const arszArgs[]
  {
    szArg00,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  // Other default behavior expected than in new config tools by TestCommonOptionsEmpty
  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_NE(CTUTIL_SUCCESS, status);
}


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateOptionsQuiet)
{
  char szArg00[] = "test";
  char szArg01[] = "--quiet";
  char szArg02[] = "user=universe";
  char szArg03[] = "hash=1234";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
    szArg03,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  testutil::TestCommonOptionsQuiet(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateOptionsQuietShort)
{
  char szArg00[] = "test";
  char szArg01[] = "-q";
  char szArg02[] = "user=universe";
  char szArg03[] = "hash=1234";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
    szArg03,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  testutil::TestCommonOptionsQuiet(argCount, arszArgs, &(this->stOptions));
}


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateOptionsHelp)
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


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateOptionsHelpShort)
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


TEST_F(OptionEvaluationTest_config_linux_user, FailEvaluateOptionsUserOnly)
{
  char szArg00[] = "test";
  char szArg01[] = "user=universe";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_NE(CTUTIL_SUCCESS, status);
}


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateOptionsNewPassword)
{
  char szArg00[] = "test";
  char szArg01[] = "user=universe";
  char szArg02[] = "new-password=secret%2Bstrong";
  char szArg03[] = "-q";
  char szArg04[] = "confirm-password=secret%2Bstrong";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
    szArg03,
    szArg04,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
}


TEST_F(OptionEvaluationTest_config_linux_user, FailEvaluateOptionsNewPasswordWithoutUser)
{
  char szArg00[] = "test";
  char szArg01[] = "new-password=secret%2Bstrong";
  char szArg02[] = "confirm-password=secret%2Bstrong";
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
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_NE(CTUTIL_SUCCESS, status);
}


TEST_F(OptionEvaluationTest_config_linux_user, FailEvaluateOptionsNewPasswordMissingConfirmation)
{
  char szArg00[] = "test";
  char szArg01[] = "user=universe";
  char szArg02[] = "new-password=secret%2Bstrong";
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
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_NE(CTUTIL_SUCCESS, status);
}


TEST_F(OptionEvaluationTest_config_linux_user, FailEvaluateOptionsNewPasswordWrongConfirmation)
{
  char szArg00[] = "test";
  char szArg01[] = "user=universe";
  char szArg02[] = "new-password=secret%2Bstrong";
  char szArg03[] = "confirm-password=secret%2BstrongWrong";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
    szArg03,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_NE(CTUTIL_SUCCESS, status);
}


TEST_F(OptionEvaluationTest_config_linux_user, FailEvaluateOptionsNewPasswordAndHash)
{
  char szArg00[] = "test";
  char szArg01[] = "user=universe";
  char szArg02[] = "new-password=secret%2Bstrong";
  char szArg03[] = "hash=1234";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
    szArg03,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_NE(CTUTIL_SUCCESS, status);
}


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateOptionsNewPasswordHash)
{
  char szArg00[] = "test";
  char szArg01[] = "user=universe";
  char szArg02[] = "hash=1234";
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
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
}


TEST_F(OptionEvaluationTest_config_linux_user, EvaluateOptionsNewPasswordHashUri)
{
  char szArg00[] = "test";
  char szArg01[] = "user=universe";
  char szArg02[] = "hash=$6$1234567887654321$n5%2FK%5c6%2d";
  char decodedHash[] = "hash=$6$1234567887654321$n5/K\\6-";
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
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  EXPECT_STREQ(decodedHash, szArg02);
}


TEST_F(OptionEvaluationTest_config_linux_user, FailEvaluateOptionsNewPasswordHashWithoutUser)
{
  char szArg00[] = "test";
  char szArg01[] = "hash=1234";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  statusCode_t status = CTUTIL_SUCCESS;
  status = ctparts_EvaluateEarlyOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_EQ(CTUTIL_SUCCESS, status);
  testing::internal::CaptureStdout();
  status = ctparts_EvaluateOptions(NULL, argCount, arszArgs, &(this->stOptions));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_NE(CTUTIL_SUCCESS, status);
}


//---- End of source file ------------------------------------------------------

