//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2019 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_MainFunctionality.cc
///
///  \brief    Tests for config tool detect_feature main functionality.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctparts/common_main_defaults.h"
#include "ctparts/ctmain.h"
#include "params.h"
#include "resources.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <cstring>

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define MAX_TESTDIR_ENTRIES                                                    8

class MainFunctionalityTest_detect_feature : public ::testing::Test
{
  private:

  protected:
    ctutil_Options_t stOptions;
    ctutil_Resources_t stResources;

  protected:
    MainFunctionalityTest_detect_feature() {};
    virtual ~MainFunctionalityTest_detect_feature() {};
    virtual void SetUp()
    {
      statusCode_t status = CTUTIL_SUCCESS;

      // Prepare options
      memset(&(this->stOptions), 0, sizeof(this->stOptions));
      status = ctparts_SetOptionDefaults(NULL, &(this->stOptions));
      ASSERT_EQ(CTUTIL_SUCCESS, status);

      // Prepare resources
      memset(&(this->stResources), 0, sizeof(this->stResources));
      this->stResources.pstSpecificResources = (ctutil_ResourcesSpecific_t *)malloc(sizeof(ctutil_ResourcesSpecific_t));
      memset(this->stResources.pstSpecificResources, 0, sizeof(*(this->stResources.pstSpecificResources)));
    }
    virtual void TearDown()
    {
      // Clean resources and options
      free(this->stResources.pstSpecificResources);
      ctparts_ReleaseSpecificOptions(NULL, &(this->stOptions));
    }
};

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
static struct dirent arTestDirectoryContent[MAX_TESTDIR_ENTRIES];
static unsigned currentDirectoryEntry = 0;

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST_F(MainFunctionalityTest_detect_feature, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_F(MainFunctionalityTest_detect_feature, NoOptionQuiet)
{
  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


static char * ResolveRealPathDummy(char const * const szFilePath,
                                   char * const szResolvedPath)
{
  return strcpy(szResolvedPath, szFilePath);
}


static char * ResolveRealPathFail(char const *,
                                  char *)
{
  return nullptr;
}


static char * ResolveRealPathRoot(char const * const szFilePath,
                                  char * const szResolvedPath)
{
  char * result = nullptr;

  std::string filePath = szFilePath;
  size_t const lastPart = filePath.rfind('/');
  if(lastPart == std::string::npos)
  {
    result = strcpy(szResolvedPath, szFilePath);
  }
  else
  {
    result = strcpy(szResolvedPath, filePath.substr(lastPart).c_str());
  }

  return result;
}


static bool IsFileAvailableTrue(ctutil_OptionsCommon_t const *,
                                char const *,
                                char const *)
{
  return true;
}


static bool IsFileAvailableFalse(ctutil_OptionsCommon_t const *,
                                 char const *,
                                 char const *)
{
  return false;
}


static bool IsDependencyCompleteTrue(char const *)
{
  return true;
}


static bool IsDependencyCompleteFalse(char const *)
{
  return false;
}


TEST_F(MainFunctionalityTest_detect_feature, DetectSpecificFeature)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const featureNameToCheck = "feature-to-check";
  std::string const expectedTextResult = featureNameToCheck + "=true";

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.textOutput = true;
  this->stOptions.pstSpecificOptions->szFeature = featureNameToCheck.c_str();
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathDummy;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ(expectedTextResult.c_str(), testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


TEST_F(MainFunctionalityTest_detect_feature, DetectSpecificFeatureValueOnly)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const featureNameToCheck = "feature-to-check";
  std::string const expectedTextResult = "true";

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.textOutput = true;
  this->stOptions.pstSpecificOptions->onlyValue = true;
  this->stOptions.pstSpecificOptions->szFeature = featureNameToCheck.c_str();
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathDummy;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ(expectedTextResult.c_str(), testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


TEST_F(MainFunctionalityTest_detect_feature, DetectSpecificFeatureJson)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const featureNameToCheck = "feature-to-check";
  std::string const expectedJsonResult = ("{\"" + featureNameToCheck + "\":true}");

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.jsonOutput = true;
  this->stOptions.pstSpecificOptions->szFeature = featureNameToCheck.c_str();
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathDummy;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  std::string capturedOutput = testing::internal::GetCapturedStdout();
  capturedOutput.erase(remove_if(capturedOutput.begin(), capturedOutput.end(), isspace), capturedOutput.end());
  EXPECT_STREQ(expectedJsonResult.c_str(), capturedOutput.c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


TEST_F(MainFunctionalityTest_detect_feature, DetectSpecificFeatureFailNoFile)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const featureNameToCheck = "feature-to-check";
  std::string const expectedTextResult = featureNameToCheck + "=false";

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.textOutput = true;
  this->stOptions.pstSpecificOptions->szFeature = featureNameToCheck.c_str();
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathDummy;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableFalse;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ(expectedTextResult.c_str(), testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


TEST_F(MainFunctionalityTest_detect_feature, DetectSpecificFeatureFailDependencyIncomplete)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const featureNameToCheck = "feature-to-check";
  std::string const expectedTextResult = featureNameToCheck + "=false";

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.textOutput = true;
  this->stOptions.pstSpecificOptions->szFeature = featureNameToCheck.c_str();
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathDummy;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteFalse;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ(expectedTextResult.c_str(), testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


TEST_F(MainFunctionalityTest_detect_feature, DetectSpecificFeatureFailResolveError)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const featureNameToCheck    = "feature-to-check";
  std::string const expectedTextResult    = featureNameToCheck + "=false";

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.textOutput = true;
  this->stOptions.pstSpecificOptions->szFeature = featureNameToCheck.c_str();
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathFail;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ(expectedTextResult.c_str(), testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


TEST_F(MainFunctionalityTest_detect_feature, DetectSpecificFeatureFailEvilFeature)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const featureNameToCheck    = "../evil-feature";
  std::string const expectedTextResult    = featureNameToCheck + "=false";

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.textOutput = true;
  this->stOptions.pstSpecificOptions->szFeature = featureNameToCheck.c_str();
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathRoot;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ(expectedTextResult.c_str(), testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


static DIR * OpenDirTest(char const * const)
{
  currentDirectoryEntry = 0;
  return (DIR *)arTestDirectoryContent;
}

static struct dirent * ReadDirTest(DIR * const pDir)
{
  struct dirent * resultEntry = &(arTestDirectoryContent[currentDirectoryEntry++]);
  if(    (pDir != (DIR *)arTestDirectoryContent)
      || (currentDirectoryEntry > MAX_TESTDIR_ENTRIES)
      || (resultEntry->d_name[0] == '\0'))
  {
    resultEntry = nullptr;
  }

  return resultEntry;
}

static int CloseDirTest(DIR * const pDir)
{
  int result = -1;

  if(pDir == (DIR *)arTestDirectoryContent)
  {
    result = 0;
  }

  return result;
}


TEST_F(MainFunctionalityTest_detect_feature, FetchFeatureListEmpty)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const expectedTextResult    = "";
  memset(arTestDirectoryContent, 0, sizeof(arTestDirectoryContent));
  strcpy(arTestDirectoryContent[0].d_name, ".");
  strcpy(arTestDirectoryContent[1].d_name, "..");

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.textOutput = true;
  this->stOptions.pstSpecificOptions->listMode = true;
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathDummy;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;
  this->stResources.pstSpecificResources->pfOpenDir = &OpenDirTest;
  this->stResources.pstSpecificResources->pfReadDir = &ReadDirTest;
  this->stResources.pstSpecificResources->pfCloseDir = &CloseDirTest;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ(expectedTextResult.c_str(), testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


TEST_F(MainFunctionalityTest_detect_feature, FetchFeatureList)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const feature_1 = "test-feature-1";
  std::string const feature_2 = "test-feature-2";
  std::string const expectedFeatureText_1 = feature_1 + "=true";
  std::string const expectedFeatureText_2 = feature_2 + "=true";
  std::string const expectedTextResult    = expectedFeatureText_1 + '\n' + expectedFeatureText_2 + '\n';
  memset(arTestDirectoryContent, 0, sizeof(arTestDirectoryContent));
  strcpy(arTestDirectoryContent[0].d_name, ".");
  strcpy(arTestDirectoryContent[1].d_name, "..");
  strcpy(arTestDirectoryContent[2].d_name, feature_1.c_str());
  strcpy(arTestDirectoryContent[3].d_name, feature_2.c_str());

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.textOutput = true;
  this->stOptions.pstSpecificOptions->listMode = true;
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathDummy;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;
  this->stResources.pstSpecificResources->pfOpenDir = &OpenDirTest;
  this->stResources.pstSpecificResources->pfReadDir = &ReadDirTest;
  this->stResources.pstSpecificResources->pfCloseDir = &CloseDirTest;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ(expectedTextResult.c_str(), testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


TEST_F(MainFunctionalityTest_detect_feature, FetchFeatureListJson)
{
  std::string const deviceFeatureTestPath = "/testpath";
  std::string const feature_1 = "test-feature-1";
  std::string const feature_2 = "test-feature-2";
  std::string const expectedJsonResult    = "{\"features\":[\"" + feature_1 + "\",\"" + feature_2 + "\"]}";
  memset(arTestDirectoryContent, 0, sizeof(arTestDirectoryContent));
  strcpy(arTestDirectoryContent[0].d_name, ".");
  strcpy(arTestDirectoryContent[1].d_name, "..");
  strcpy(arTestDirectoryContent[2].d_name, feature_1.c_str());
  strcpy(arTestDirectoryContent[3].d_name, feature_2.c_str());

  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;
  this->stOptions.stCommonOptions.jsonOutput = true;
  this->stOptions.pstSpecificOptions->listMode = true;
  this->stResources.pstSpecificResources->szDeviceFeaturePath = deviceFeatureTestPath.c_str();
  this->stResources.pstSpecificResources->pfResolveRealPath = &ResolveRealPathDummy;
  this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableTrue;
  this->stResources.pstSpecificResources->pfIsDependencyComplete = &IsDependencyCompleteTrue;
  this->stResources.pstSpecificResources->pfOpenDir = &OpenDirTest;
  this->stResources.pstSpecificResources->pfReadDir = &ReadDirTest;
  this->stResources.pstSpecificResources->pfCloseDir = &CloseDirTest;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  std::string capturedOutput = testing::internal::GetCapturedStdout();
  capturedOutput.erase(remove_if(capturedOutput.begin(), capturedOutput.end(), isspace), capturedOutput.end());
  EXPECT_STREQ(expectedJsonResult.c_str(), capturedOutput.c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


//---- End of source file ------------------------------------------------------
