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
///  \file     test_ParamHandling.cc
///
///  \brief    Test for helper functions for config tool parameter handling.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/param_handling.h"
#include "ctutil/error_codes.h"
#include <gtest/gtest.h>
#include <string.h>

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
namespace
{
  struct UriTestArgs
  {
    char const * const szUriEncodedString;
    char const * const szUriDecodedString;
  };

  class ParamHandlingUriChar: public ::testing::TestWithParam<UriTestArgs>
  {

  };

  class ParamHandlingUriExample: public ::testing::TestWithParam<UriTestArgs>
  {

  };
}

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST_P(ParamHandlingUriChar, DecodeUriChar)
{
  UriTestArgs const & testArgs = GetParam();
  {
    statusCode_t result = CTUTIL_SUCCESS;
    char szTestArg[4];
    strncpy(szTestArg, testArgs.szUriEncodedString, sizeof(szTestArg));
    szTestArg[3] = '\0';
    result = ctutil_DecodeUriArgument(szTestArg);
    EXPECT_TRUE(ctutil_IsStatusOk(result));
    EXPECT_STREQ(testArgs.szUriDecodedString, szTestArg);
  }
}


TEST_P(ParamHandlingUriExample, DecodeUriExample)
{
  UriTestArgs const & testArgs = GetParam();
  {
    statusCode_t result = CTUTIL_SUCCESS;
    size_t const inputLength = strlen(testArgs.szUriEncodedString);
    char * szTestArg = new char[inputLength + 1];
    strncpy(szTestArg, testArgs.szUriEncodedString, inputLength);
    szTestArg[inputLength] = '\0';
    result = ctutil_DecodeUriArgument(szTestArg);
    EXPECT_TRUE(ctutil_IsStatusOk(result));
    EXPECT_STREQ(testArgs.szUriDecodedString, szTestArg);
    delete[] szTestArg;
  }
}


UriTestArgs const arTestUriChars[] =
{
  { "%20", " " },
  { "%21", "!" },
  { "%22", "\"" },
  { "%23", "#" },
  { "%24", "$" },
  { "%25", "%" },
  { "%26", "&" },
  { "%27", "'" },
  { "%28", "(" },
  { "%29", ")" },
  { "%2A", "*" },
  { "%2B", "+" },
  { "%2C", "," },
  { "%2D", "-" },
  { "%2E", "." },
  { "%2F", "/" },
  { "%2a", "*" },
  { "%2b", "+" },
  { "%2c", "," },
  { "%2d", "-" },
  { "%2e", "." },
  { "%2f", "/" },
  { "%3A", ":" },
  { "%3B", ";" },
  { "%3C", "<" },
  { "%3D", "=" },
  { "%3E", ">" },
  { "%3F", "?" },
  { "%3a", ":" },
  { "%3b", ";" },
  { "%3c", "<" },
  { "%3d", "=" },
  { "%3e", ">" },
  { "%3f", "?" },
  { "%40", "@" },
  { "%5B", "[" },
  { "%5C", "\\" },
  { "%5D", "]" },
  { "%5b", "[" },
  { "%5c", "\\" },
  { "%5d", "]" },
  { "%7B", "{" },
  { "%7C", "|" },
  { "%7D", "}" },
  { "%7b", "{" },
  { "%7c", "|" },
  { "%7d", "}" },
};
INSTANTIATE_TEST_CASE_P(ParamHandling, ParamHandlingUriChar, ::testing::ValuesIn(arTestUriChars));


UriTestArgs const arTestUriExamples[] =
{
  {
    "%2B%23%23%23%2A This is a typical example of a %27%25%27%2Dencoded %28URI%29 string%2E%20"
    "%22Respect%22 %26 decode it%21 Is this %3E%24VAR%3C a variable%2C do you know%3F %2A%23%23%23%2B",
    "+###* This is a typical example of a '%'-encoded (URI) string. "
    "\"Respect\" & decode it! Is this >$VAR< a variable, do you know? *###+"
  },
  {
    "%2b%23%23%23%2a This is a typical example of a %27%25%27%2Dencoded %28URI%29 string%2e%20"
    "%22Respect%22 %26 decode it%21 Is this %3e%24VAR%3c a variable%2c do you know%3f %2a%23%23%23%2b",
    "+###* This is a typical example of a '%'-encoded (URI) string. "
    "\"Respect\" & decode it! Is this >$VAR< a variable, do you know? *###+"
  },
  {
    "sudo /etc/config-tools/config_linux_user user=admin new-password=123%22%25abc%2b%27() confirm-password=123%22%25abc%2b%27()",
    "sudo /etc/config-tools/config_linux_user user=admin new-password=123\"%abc+'() confirm-password=123\"%abc+'()"
  },
};
INSTANTIATE_TEST_CASE_P(ParamHandling, ParamHandlingUriExample, ::testing::ValuesIn(arTestUriExamples));


TEST(EarlyOptionEvaluation, EvaluateShortOption_q)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "-q";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_TRUE(stOptions.stCommonOptions.quiet);
}


TEST(EarlyOptionEvaluation, EvaluateShortOption_qMix)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "-kfqcls";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_TRUE(stOptions.stCommonOptions.quiet);
}


TEST(EarlyOptionEvaluation, EvaluateShortOption_x)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "-x";
  char szArg02[] = "%28t%29";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_STREQ("-x", szArg01);
  EXPECT_STREQ("(t)", szArg02);
}


TEST(EarlyOptionEvaluation, EvaluateShortOption_xMix)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "-ltxyav";
  char szArg02[] = "%28t%29";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_STREQ("-ltxyav", szArg01);
  EXPECT_STREQ("(t)", szArg02);
}


TEST(EarlyOptionEvaluation, EvaluateShortOption_xqMix)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "-jftxqyz";
  char szArg02[] = "%28t%29";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_TRUE(stOptions.stCommonOptions.quiet);
  EXPECT_STREQ("-jftxqyz", szArg01);
  EXPECT_STREQ("(t)", szArg02);
}


TEST(EarlyOptionEvaluation, EvaluateLongOption_q)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "--quiet";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_TRUE(stOptions.stCommonOptions.quiet);
}


TEST(EarlyOptionEvaluation, EvaluateLongOption_x)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "--extract-from-uri-encoding";
  char szArg02[] = "%28t%29";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_STREQ("--extract-from-uri-encoding", szArg01);
  EXPECT_STREQ("(t)", szArg02);
}


TEST(EarlyOptionEvaluation, EvaluateLongOption_xqMix)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "--quiet";
  char szArg02[] = "--otherOption";
  char szArg03[] = "%28t%29";
  char szArg04[] = "-mdl";
  char szArg05[] = "--extract-from-uri-encoding";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
    szArg02,
    szArg03,
    szArg04,
    szArg05,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_TRUE(stOptions.stCommonOptions.quiet);
  EXPECT_STREQ("--quiet", szArg01);
  EXPECT_STREQ("--otherOption", szArg02);
  EXPECT_STREQ("(t)", szArg03);
  EXPECT_STREQ("-mdl", szArg04);
  EXPECT_STREQ("--extract-from-uri-encoding", szArg05);
}


TEST(EarlyOptionEvaluation, DecodeUriIfPredefined)
{
  char szArg00[] = "somethingOther";
  char szArg01[] = "%28t%29";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  ctutil_Options_t stOptions;
  memset(&stOptions, 0, sizeof(stOptions));
  stOptions.stCommonOptions.extractFromUri = true;
  statusCode_t const result = ctutil_EvaluateEarlyOptions(argCount, arszArgs, &stOptions);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("somethingOther", szArg00);
  EXPECT_STREQ("(t)", szArg01);
}


//---- End of source file ------------------------------------------------------

