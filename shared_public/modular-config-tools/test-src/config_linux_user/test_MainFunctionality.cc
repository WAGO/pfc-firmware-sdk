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
///  \file     test_MainFunctionality.cc
///
///  \brief    Tests for config tool config_linux_user main functionality.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctparts/common_main_defaults.h"
#include "ctparts/ctmain.h"
#include "params.h"
#include "resources.h"
#include <gtest/gtest.h>
#include "testutil/file_operations.hpp"
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <string>

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------
#define TEST_RANDOM_BYTES_1_SIZE                                            (4U)
#define TEST_RANDOM_BYTES_1_VALUE                                           (1U)

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
class MainFunctionalityTest_config_linux_user : public ::testing::Test
{
  private:
    std::string tempDir;
    std::string lastErrorFile;
    std::string randSource;
    std::string testShadow;

  protected:
    ctutil_Options_t stOptions;
    ctutil_Resources_t stResources;
    char const szShadowLines[3][128] = { "admin:adminpasswordhash:17000:0:99999:7:::",
                                         "root:rootpasswordhash:17000:0:99999:7:::",
                                         "nobody:nobodypasswordhash:17000:0:99999:7:::" };

  protected:
    MainFunctionalityTest_config_linux_user() {};
    virtual ~MainFunctionalityTest_config_linux_user() {};
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

      tempDir = testutil_CreateTempDir("config_linux_user_");
      lastErrorFile = tempDir + "/test_error.txt";
      this->stResources.stCommonResources.szLastErrorFile = lastErrorFile.c_str();
      randSource = tempDir + "/test_rand_source.txt";
      this->stResources.pstSpecificResources->szRandomSource1 = randSource.c_str();
      this->stResources.pstSpecificResources->szRandomSource2 = "/dev/zero";
      testShadow = tempDir + "/test_shadow";
      status = ctutil_PrepareChangeableFile(testShadow.c_str(), &(this->stResources.pstSpecificResources->pShadowFile));
      ASSERT_EQ(CTUTIL_SUCCESS, status);

      // Prepare test files
      testutil_CreateFile(this->stResources.stCommonResources.szLastErrorFile);
      testutil_CreateFile(this->stResources.pstSpecificResources->szRandomSource1);
      uint8_t const arTestBytes[TEST_RANDOM_BYTES_1_SIZE] = { TEST_RANDOM_BYTES_1_VALUE,
                                                              TEST_RANDOM_BYTES_1_VALUE,
                                                              TEST_RANDOM_BYTES_1_VALUE,
                                                              TEST_RANDOM_BYTES_1_VALUE};
      size_t const random1Bytes = sizeof(arTestBytes);
      testutil_AppendToFile(this->stResources.pstSpecificResources->szRandomSource1, arTestBytes, random1Bytes);
      char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);
      testutil_CreateFile(szShadowFilePath);
      for(size_t i = 0; i < 3; ++i)
      {
        testutil_AppendToFile(szShadowFilePath, (uint8_t const *)this->szShadowLines[i], strlen(this->szShadowLines[i]));
        testutil_AppendToFile(szShadowFilePath, (uint8_t const *)"\n", 1U);
      }
      sync();
    }
    virtual void TearDown()
    {
      // Clean test files
      EXPECT_TRUE(testutil_RemoveFile(ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile)));
      // FIXME: We should clean the temporarily file may be left but if it not exist QEMU will crash when run with Valgrind
//      (void)testutil_RemoveFile(ctutil_GetChangeableFilePath(this->stResources.pstSpecificResources->pShadowFile));
      EXPECT_TRUE(testutil_RemoveFile(this->stResources.pstSpecificResources->szRandomSource1));
      EXPECT_TRUE(testutil_RemoveFile(this->stResources.stCommonResources.szLastErrorFile));
      EXPECT_TRUE(testutil_RemoveDir(tempDir.c_str()));

      // Clean resources and options
      ctutil_ReleaseFileForChanges(&(this->stResources.pstSpecificResources->pShadowFile));
      free(this->stResources.pstSpecificResources);
      ctparts_ReleaseSpecificOptions(NULL, &(this->stOptions));
    }
};

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST_F(MainFunctionalityTest_config_linux_user, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_F(MainFunctionalityTest_config_linux_user, ChangePasswordOfUserAdmin)
{
  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.pstSpecificOptions->szUsername = "admin";
  this->stOptions.pstSpecificOptions->szPassword1 = "1234";
  this->stOptions.pstSpecificOptions->szPassword2 = "1234";

  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
  ::std::string lineBuffer;
  char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);
  EXPECT_STRNE(this->szShadowLines[0], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "admin", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[1], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "root", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[2], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "nobody", lineBuffer));
}


TEST_F(MainFunctionalityTest_config_linux_user, FailChangePasswordOfUserRoot)
{
  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.pstSpecificOptions->szUsername = "root";
  this->stOptions.pstSpecificOptions->szPassword1 = "1234";
  this->stOptions.pstSpecificOptions->szPassword2 = "1234";

  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_NE(CTUTIL_EXIT_SUCCESS, result);
  ::std::string lineBuffer;
  char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);
  EXPECT_STREQ(this->szShadowLines[0], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "admin", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[1], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "root", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[2], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "nobody", lineBuffer));
}


TEST_F(MainFunctionalityTest_config_linux_user, FailChangePasswordOfUserNobody)
{
  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.pstSpecificOptions->szUsername = "nobody";
  this->stOptions.pstSpecificOptions->szPassword1 = "1234";
  this->stOptions.pstSpecificOptions->szPassword2 = "1234";

  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_NE(CTUTIL_EXIT_SUCCESS, result);
  ::std::string lineBuffer;
  char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);
  EXPECT_STREQ(this->szShadowLines[0], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "admin", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[1], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "root", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[2], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "nobody", lineBuffer));
}


TEST_F(MainFunctionalityTest_config_linux_user, ChangeHashOfUserAdmin)
{
  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.pstSpecificOptions->szUsername = "admin";
  this->stOptions.pstSpecificOptions->szHash = "fakehash:17000:0:99999";

  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
  ::std::string lineBuffer;
  char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);
  EXPECT_STRNE(this->szShadowLines[0], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "admin", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[1], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "root", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[2], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "nobody", lineBuffer));
  char const * const szChangedLine = testutil_GetFirstMatchingLineFromFile(szShadowFilePath, this->stOptions.pstSpecificOptions->szHash, lineBuffer);
  EXPECT_NE((void*)NULL, szChangedLine);
  if(szChangedLine != NULL)
  {
    EXPECT_NE((void*)NULL, strstr(szChangedLine, this->stOptions.pstSpecificOptions->szUsername));
  }
}


TEST_F(MainFunctionalityTest_config_linux_user, FailChangeHashOfUserNobody)
{
  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.pstSpecificOptions->szUsername = "nobody";
  this->stOptions.pstSpecificOptions->szHash = "fakehash:17000:0:99999";

  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_NE(CTUTIL_EXIT_SUCCESS, result);
  ::std::string lineBuffer;
  char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);
  EXPECT_STREQ(this->szShadowLines[0], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "admin", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[1], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "root", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[2], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "nobody", lineBuffer));
}


//---- End of source file ------------------------------------------------------

