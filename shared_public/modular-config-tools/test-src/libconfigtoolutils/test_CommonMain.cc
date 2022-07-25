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
///  \file     test_CommonMain.cc
///
///  \brief    Test for config tool common main function.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "ctutil/common_main.h"
#include "ctparts/common_main_defaults.h"
#include "ctparts/ctmain.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Exactly;
using ::testing::AtLeast;
using ::testing::AtMost;
using ::testing::AnyNumber;
using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::Return;

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
class ICommonFunction
{
  public:
    virtual              ~ICommonFunction() {};
    virtual void         PrintInfoText(char const * const szCallString) const = 0;
    virtual statusCode_t SetOptionDefaults(ctutil_Options_t * const pstOptions) const = 0;
    virtual void         ReleaseSpecificOptions(ctutil_Options_t * const pstOptions) const = 0;
    virtual statusCode_t EvaluateEarlyOptions(int const argc,
                                              char * const argv[],
                                              ctutil_Options_t * const pstOptions) const = 0;
    virtual statusCode_t EvaluateOptions(int const argc,
                                         char * const argv[],
                                         ctutil_Options_t * const pstOptions) const = 0;
    virtual statusCode_t InitSpecificResources(ctutil_ResourcesSpecific_t * * const ppstResources) const = 0;
    virtual void         ReleaseSpecificResources(ctutil_ResourcesSpecific_t * * const ppstResources) const = 0;
};

class MockCommonFunctions;

struct ctparts_commonMainContext
{
  MockCommonFunctions * pMock;
};

class MockCommonFunctions : public ICommonFunction
{
  public:
    virtual ~MockCommonFunctions() {};
    MOCK_CONST_METHOD1(SetOptionDefaults, statusCode_t(ctutil_Options_t * const pstOptions));
    statusCode_t SetOptionDefaultsFollowers(ctutil_Options_t * const pstOptions)
    {
      pstOptions->pstSpecificOptions = (ctutil_OptionsSpecific_t *)0x1;
      EXPECT_CALL(*this, ReleaseSpecificOptions(pstOptions))
        .Times(Exactly(1))
        .RetiresOnSaturation();

      return CTUTIL_SUCCESS;
    }
    MOCK_CONST_METHOD1(ReleaseSpecificOptions, void(ctutil_Options_t * const pstOptions));
    MOCK_CONST_METHOD1(PrintInfoText, void(char const * const szCallString));
    MOCK_CONST_METHOD3(EvaluateEarlyOptions, statusCode_t(int const argc,
                                             char * const argv[],
                                             ctutil_Options_t * const pstOptions));
    MOCK_CONST_METHOD3(EvaluateOptions, statusCode_t(int const argc,
                                        char * const argv[],
                                        ctutil_Options_t * const pstOptions));
    MOCK_CONST_METHOD1(InitSpecificResources, statusCode_t(ctutil_ResourcesSpecific_t * * const ppstResources));
    statusCode_t InitSpecificResourcesFollowers(ctutil_ResourcesSpecific_t * * const ppstResources)
    {
      EXPECT_CALL(*this, ReleaseSpecificResources(ppstResources))
        .Times(Exactly(1))
        .RetiresOnSaturation();

      return CTUTIL_SUCCESS;
    }
    MOCK_CONST_METHOD1(ReleaseSpecificResources, void(ctutil_ResourcesSpecific_t * * const ppstResources));
};

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
static statusCode_t SetOptionDefaults(ctparts_commonMainContext_t * const pstContext,
                                      ctutil_Options_t * const pstOptions)
{
  // Ever be quiet to avoid any prints on test
  pstOptions->stCommonOptions.quiet = true;
  return pstContext->pMock->SetOptionDefaults(pstOptions);
}


static void ReleaseSpecificOptions(ctparts_commonMainContext_t * const pstContext,
                                   ctutil_Options_t * const pstOptions)
{
  pstContext->pMock->ReleaseSpecificOptions(pstOptions);
  return;
}


static statusCode_t EvaluateEarlyOptions(ctparts_commonMainContext_t * const pstContext,
                                         int const argc,
                                         char * const argv[],
                                         ctutil_Options_t * const pstOptions)
{
  return pstContext->pMock->EvaluateEarlyOptions(argc, argv, pstOptions);
}


static statusCode_t EvaluateOptions(ctparts_commonMainContext_t * const pstContext,
                                    int const argc,
                                    char * const argv[],
                                    ctutil_Options_t * const pstOptions)
{
  return pstContext->pMock->EvaluateOptions(argc, argv, pstOptions);
}


static void PrintInfoText(ctparts_commonMainContext_t * const pstContext,
                          char const * const szCallString)
{
  pstContext->pMock->PrintInfoText(szCallString);
  return;
}


static statusCode_t InitSpecificResources(ctparts_commonMainContext_t * const pstContext,
                                          ctutil_ResourcesSpecific_t * * const ppstResources)
{
  return pstContext->pMock->InitSpecificResources(ppstResources);
}


static void ReleaseSpecificResources(ctparts_commonMainContext_t * const pstContext,
                                     ctutil_ResourcesSpecific_t * * const ppstResources)
{
  pstContext->pMock->ReleaseSpecificResources(ppstResources);
  return;
}


statusCode_t ctparts_EvaluateEarlyOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                          int const WC_UNUSED_PARAM(argc),
                                          char * const WC_UNUSED_PARAM(argv)[],
                                          ctutil_Options_t * const WC_UNUSED_PARAM(pstOptions))
{
  return CTUTIL_SUCCESS;
}


statusCode_t ctparts_EvaluateOptions(ctparts_commonMainContext_t * const WC_UNUSED_PARAM(pstContext),
                                     int const WC_UNUSED_PARAM(argc),
                                     char * const WC_UNUSED_PARAM(argv)[],
                                     ctutil_Options_t * const WC_UNUSED_PARAM(pstOptions))
{
  return CTUTIL_SUCCESS;
}


exitCode_t ctparts_ctMain(int const WC_UNUSED_PARAM(argc),
                          char * const WC_UNUSED_PARAM(argv)[],
                          ctutil_Options_t const * const WC_UNUSED_PARAM(pstOptions),
                          ctutil_Resources_t const * const WC_UNUSED_PARAM(pstResources))
{
  return CTUTIL_EXIT_SUCCESS;
}


TEST(CommonMain, CheckFlow)
{
  char szArg00[] = "common_main";
  char szArg01[] = "--testArg1";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  // Prepare mock functions
  MockCommonFunctions functionMock;
  ctparts_commonMainContext stContext = { &functionMock };
  ctparts_commonMainFunctions_t stFunctions;
  stFunctions.pfPrintInfoText = &PrintInfoText;
  stFunctions.pfSetOptionDefaults = &SetOptionDefaults;
  stFunctions.pfReleaseSpecificOptions = &ReleaseSpecificOptions;
  stFunctions.pfEvaluateEarlyOptions = &EvaluateEarlyOptions;
  stFunctions.pfEvaluateOptions = &EvaluateOptions;
  stFunctions.pfInitSpecificResources = &InitSpecificResources;
  stFunctions.pfReleaseSpecificResources = &ReleaseSpecificResources;

  // Set call expectation
  EXPECT_CALL(functionMock, SetOptionDefaults(::testing::NotNull()))
    .Times(AtLeast(1))
    .WillRepeatedly(WithArgs<0>(Invoke(&functionMock, &MockCommonFunctions::SetOptionDefaultsFollowers)));
  EXPECT_CALL(functionMock, EvaluateEarlyOptions(argCount, arszArgs, ::testing::_))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_SUCCESS));
  EXPECT_CALL(functionMock, EvaluateOptions(argCount, arszArgs, ::testing::_))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_SUCCESS));
  EXPECT_CALL(functionMock, PrintInfoText(::testing::StrEq(szArg00)))
    .Times(AtMost(1));
  EXPECT_CALL(functionMock, InitSpecificResources(::testing::NotNull()))
    .Times(AtLeast(1))
    .WillRepeatedly(WithArgs<0>(Invoke(&functionMock, &MockCommonFunctions::InitSpecificResourcesFollowers)));

  exitCode_t const result = ctutil_CommonMain(argCount, arszArgs, &stFunctions, &stContext);
  ASSERT_EQ(CTUTIL_SUCCESS, result);
}


TEST(CommonMain, FailSetOptionDefaults)
{
  char szArg00[] = "common_main_fail_defaults";
  char szArg01[] = "--testArgY";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  // Prepare mock functions
  MockCommonFunctions functionMock;
  ctparts_commonMainContext stContext = { &functionMock };
  ctparts_commonMainFunctions_t stFunctions;
  stFunctions.pfPrintInfoText = &PrintInfoText;
  stFunctions.pfSetOptionDefaults = &SetOptionDefaults;
  stFunctions.pfReleaseSpecificOptions = &ReleaseSpecificOptions;
  stFunctions.pfEvaluateEarlyOptions = &EvaluateEarlyOptions;
  stFunctions.pfEvaluateOptions = &EvaluateOptions;
  stFunctions.pfInitSpecificResources = &InitSpecificResources;
  stFunctions.pfReleaseSpecificResources = &ReleaseSpecificResources;

  // Set call expectation
  EXPECT_CALL(functionMock, SetOptionDefaults(::testing::NotNull()))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_FAILED));
  EXPECT_CALL(functionMock, ReleaseSpecificOptions(::testing::NotNull()))
    .Times(AnyNumber());
  EXPECT_CALL(functionMock, EvaluateEarlyOptions(::testing::_, ::testing::_, ::testing::_))
    .Times(Exactly(0));
  EXPECT_CALL(functionMock, EvaluateOptions(::testing::_, ::testing::_, ::testing::_))
    .Times(Exactly(0));
  EXPECT_CALL(functionMock, PrintInfoText(::testing::_))
    .Times(Exactly(0));
  EXPECT_CALL(functionMock, InitSpecificResources(::testing::NotNull()))
    .Times(AnyNumber())
    .WillRepeatedly(WithArgs<0>(Invoke(&functionMock, &MockCommonFunctions::InitSpecificResourcesFollowers)));

  exitCode_t const result = ctutil_CommonMain(argCount, arszArgs, &stFunctions, &stContext);
  ASSERT_NE(CTUTIL_SUCCESS, result);
}


TEST(CommonMain, FailEvaluateEarlyOptions)
{
  char szArg00[] = "common_main_fail_eOpt";
  char szArg01[] = "--testArgF";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  // Prepare mock functions
  MockCommonFunctions functionMock;
  ctparts_commonMainContext stContext = { &functionMock };
  ctparts_commonMainFunctions_t stFunctions;
  stFunctions.pfPrintInfoText = &PrintInfoText;
  stFunctions.pfSetOptionDefaults = &SetOptionDefaults;
  stFunctions.pfReleaseSpecificOptions = &ReleaseSpecificOptions;
  stFunctions.pfEvaluateEarlyOptions = &EvaluateEarlyOptions;
  stFunctions.pfEvaluateOptions = &EvaluateOptions;
  stFunctions.pfInitSpecificResources = &InitSpecificResources;
  stFunctions.pfReleaseSpecificResources = &ReleaseSpecificResources;

  // Set call expectation
  EXPECT_CALL(functionMock, SetOptionDefaults(::testing::NotNull()))
    .Times(AtLeast(1))
    .WillRepeatedly(WithArgs<0>(Invoke(&functionMock, &MockCommonFunctions::SetOptionDefaultsFollowers)));
  EXPECT_CALL(functionMock, EvaluateEarlyOptions(argCount, arszArgs, ::testing::_))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_FAILED));
  EXPECT_CALL(functionMock, EvaluateOptions(::testing::_, ::testing::_, ::testing::_))
    .Times(Exactly(0));
  EXPECT_CALL(functionMock, PrintInfoText(::testing::_))
    .Times(Exactly(0));
  EXPECT_CALL(functionMock, InitSpecificResources(::testing::NotNull()))
    .Times(AnyNumber())
    .WillRepeatedly(WithArgs<0>(Invoke(&functionMock, &MockCommonFunctions::InitSpecificResourcesFollowers)));

  exitCode_t const result = ctutil_CommonMain(argCount, arszArgs, &stFunctions, &stContext);
  ASSERT_NE(CTUTIL_SUCCESS, result);
}


TEST(CommonMain, FailEvaluateOptions)
{
  char szArg00[] = "common_main_fail_opt";
  char szArg01[] = "--testArgA";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  // Prepare mock functions
  MockCommonFunctions functionMock;
  ctparts_commonMainContext stContext = { &functionMock };
  ctparts_commonMainFunctions_t stFunctions;
  stFunctions.pfPrintInfoText = &PrintInfoText;
  stFunctions.pfSetOptionDefaults = &SetOptionDefaults;
  stFunctions.pfReleaseSpecificOptions = &ReleaseSpecificOptions;
  stFunctions.pfEvaluateEarlyOptions = &EvaluateEarlyOptions;
  stFunctions.pfEvaluateOptions = &EvaluateOptions;
  stFunctions.pfInitSpecificResources = &InitSpecificResources;
  stFunctions.pfReleaseSpecificResources = &ReleaseSpecificResources;

  // Set call expectation
  EXPECT_CALL(functionMock, SetOptionDefaults(::testing::NotNull()))
    .Times(AtLeast(1))
    .WillRepeatedly(WithArgs<0>(Invoke(&functionMock, &MockCommonFunctions::SetOptionDefaultsFollowers)));
  EXPECT_CALL(functionMock, EvaluateEarlyOptions(argCount, arszArgs, ::testing::_))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_SUCCESS));
  EXPECT_CALL(functionMock, EvaluateOptions(argCount, arszArgs, ::testing::_))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_FAILED));
  EXPECT_CALL(functionMock, PrintInfoText(::testing::_))
    .Times(Exactly(0));
  EXPECT_CALL(functionMock, InitSpecificResources(::testing::NotNull()))
    .Times(AnyNumber())
    .WillRepeatedly(WithArgs<0>(Invoke(&functionMock, &MockCommonFunctions::InitSpecificResourcesFollowers)));

  exitCode_t const result = ctutil_CommonMain(argCount, arszArgs, &stFunctions, &stContext);
  ASSERT_NE(CTUTIL_SUCCESS, result);
}


TEST(CommonMain, FailInitSpecificResources)
{
  char szArg00[] = "common_main_fail_res";
  char szArg01[] = "--testArgR";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);

  // Prepare mock functions
  MockCommonFunctions functionMock;
  ctparts_commonMainContext stContext = { &functionMock };
  ctparts_commonMainFunctions_t stFunctions;
  stFunctions.pfPrintInfoText = &PrintInfoText;
  stFunctions.pfSetOptionDefaults = &SetOptionDefaults;
  stFunctions.pfReleaseSpecificOptions = &ReleaseSpecificOptions;
  stFunctions.pfEvaluateEarlyOptions = &EvaluateEarlyOptions;
  stFunctions.pfEvaluateOptions = &EvaluateOptions;
  stFunctions.pfInitSpecificResources = &InitSpecificResources;
  stFunctions.pfReleaseSpecificResources = &ReleaseSpecificResources;

  // Set call expectation
  EXPECT_CALL(functionMock, SetOptionDefaults(::testing::NotNull()))
    .Times(AtLeast(1))
    .WillRepeatedly(WithArgs<0>(Invoke(&functionMock, &MockCommonFunctions::SetOptionDefaultsFollowers)));
  EXPECT_CALL(functionMock, EvaluateEarlyOptions(argCount, arszArgs, ::testing::_))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_SUCCESS));
  EXPECT_CALL(functionMock, EvaluateOptions(argCount, arszArgs, ::testing::_))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_SUCCESS));
  EXPECT_CALL(functionMock, PrintInfoText(::testing::StrEq(szArg00)))
    .Times(AtMost(1));
  EXPECT_CALL(functionMock, InitSpecificResources(::testing::NotNull()))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(CTUTIL_FAILED));
  EXPECT_CALL(functionMock, ReleaseSpecificResources(::testing::_))
    .Times(Exactly(0));

  exitCode_t const result = ctutil_CommonMain(argCount, arszArgs, &stFunctions, &stContext);
  ASSERT_NE(CTUTIL_SUCCESS, result);
}


//---- End of source file ------------------------------------------------------

