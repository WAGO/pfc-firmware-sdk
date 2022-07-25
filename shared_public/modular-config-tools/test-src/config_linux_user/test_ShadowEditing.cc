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
///  \file     test_ShadowEditing.cc
///
///  \brief    Tests for shadow editing in config tool config_linux_user.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "shadow_functions.h"
#include "resources.h"
#include <gtest/gtest.h>
#include "testutil/file_operations.hpp"
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
class ShadowEditingTest_config_linux_user : public ::testing::Test
{
  private:
    std::string tempDir;
    std::string lastErrorFile;
    std::string randSource;

  protected:
    ctutil_Resources_t stResources;
    char const szShadowLines[3][128] = { "admin:adminpasswordhash:17000:0:99999:7:::",
                                         "root:rootpasswordhash:17000:0:99999:7:::",
                                         "nobody:nobodypasswordhash:17000:0:99999:7:::" };

  protected:
    ShadowEditingTest_config_linux_user() {};
    virtual ~ShadowEditingTest_config_linux_user() {};
    virtual void SetUp()
    {
      statusCode_t status = CTUTIL_SUCCESS;

      // Prepare resources
      memset(&(this->stResources), 0, sizeof(this->stResources));
      this->stResources.pstSpecificResources = (ctutil_ResourcesSpecific_t *)malloc(sizeof(ctutil_ResourcesSpecific_t));
      memset(this->stResources.pstSpecificResources, 0, sizeof(*(this->stResources.pstSpecificResources)));

      tempDir = testutil_CreateTempDir("config_linux_user_");
      lastErrorFile = tempDir + "/test_error.txt";
      this->stResources.stCommonResources.szLastErrorFile = lastErrorFile.c_str();
      randSource = tempDir + "/test_rand_source.txt";
      status = ctutil_PrepareChangeableFile(randSource.c_str(), &(this->stResources.pstSpecificResources->pShadowFile));
      ASSERT_EQ(CTUTIL_SUCCESS, status);

      // Prepare test files
      testutil_CreateFile(this->stResources.stCommonResources.szLastErrorFile);
      char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);
      testutil_CreateFile(szShadowFilePath);
      for(size_t i = 0; i < 3; ++i)
      {
        testutil_AppendToFile(szShadowFilePath, (uint8_t const *)this->szShadowLines[i], strlen(this->szShadowLines[i]));
        testutil_AppendToFile(szShadowFilePath, (uint8_t const *)"\n", 1U);
      }
      ASSERT_EQ(0, chmod(szShadowFilePath, (S_IRUSR | S_IWUSR)));
      sync();
    }
    virtual void TearDown()
    {
      // Clean test files
      EXPECT_TRUE(testutil_RemoveFile(ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile)));
      // FIXME: We should clean the temporarily file may be left but if it not exist QEMU will crash when run with Valgrind
//      (void)testutil_RemoveFile(ctutil_GetChangeableFilePath(this->stResources.pstSpecificResources->pShadowFile));
      EXPECT_TRUE(testutil_RemoveFile(this->stResources.stCommonResources.szLastErrorFile));
      EXPECT_TRUE(testutil_RemoveDir(tempDir.c_str()));

      // Clean resources and options
      ctutil_ReleaseFileForChanges(&(this->stResources.pstSpecificResources->pShadowFile));
      free(this->stResources.pstSpecificResources);
    }
};

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST_F(ShadowEditingTest_config_linux_user, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_F(ShadowEditingTest_config_linux_user, WriteShadowFileWithNewEntry)
{
  // Prepare user entry
  struct spwd stNewUserData;
  memset(&stNewUserData, 0, sizeof(stNewUserData));
  stNewUserData.sp_namp = strdup("nobody");
  stNewUserData.sp_pwdp = strdup("1234");
  stNewUserData.sp_lstchg = 123;
  stNewUserData.sp_min = 0;
  stNewUserData.sp_max = 99999;
  stNewUserData.sp_warn = 7;
  stNewUserData.sp_inact = 0;
  stNewUserData.sp_expire = 0;
  stNewUserData.sp_flag = 0;

  // Write to shadow
  char const szExpectedNobodyLine[] = "nobody:1234:123:0:99999:7:::";
  statusCode_t const result = WriteShadow(&(this->stResources), &stNewUserData);
  EXPECT_EQ(CTUTIL_SUCCESS, result);
  ::std::string lineBuffer;
  char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);
  EXPECT_STREQ(this->szShadowLines[0], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "admin", lineBuffer));
  EXPECT_STREQ(this->szShadowLines[1], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "root", lineBuffer));
  EXPECT_STRNE(this->szShadowLines[2], testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "nobody", lineBuffer));
  EXPECT_STREQ(szExpectedNobodyLine, testutil_GetFirstMatchingLineFromFile(szShadowFilePath, "nobody", lineBuffer));

  // Cleanup
  free(stNewUserData.sp_pwdp);
  free(stNewUserData.sp_namp);
}


TEST_F(ShadowEditingTest_config_linux_user, CheckUnchangedFileRightsAfterWriteShadowFile)
{
  char const * szShadowFilePath = ctutil_GetOriginalFilePath(this->stResources.pstSpecificResources->pShadowFile);

  // Check initial file rights
  struct stat stOriginalFileStat;
  int const statOriginalResult = lstat(szShadowFilePath, &stOriginalFileStat);
  ASSERT_EQ(0, statOriginalResult);

  // Prepare user entry
  struct spwd stNewUserData;
  memset(&stNewUserData, 0, sizeof(stNewUserData));
  stNewUserData.sp_namp = strdup("nobody");
  stNewUserData.sp_pwdp = strdup("1234");
  stNewUserData.sp_lstchg = 123;
  stNewUserData.sp_min = 0;
  stNewUserData.sp_max = 99999;
  stNewUserData.sp_warn = 7;
  stNewUserData.sp_inact = 0;
  stNewUserData.sp_expire = 0;
  stNewUserData.sp_flag = 0;

  // Write to shadow
  statusCode_t const result = WriteShadow(&(this->stResources), &stNewUserData);
  EXPECT_EQ(CTUTIL_SUCCESS, result);
  struct stat stModifiedFileStat;
  int const statModifiedResult = lstat(szShadowFilePath, &stModifiedFileStat);
  ASSERT_EQ(0, statModifiedResult);
  EXPECT_EQ(stOriginalFileStat.st_mode, stModifiedFileStat.st_mode);

  // Cleanup
  free(stNewUserData.sp_pwdp);
  free(stNewUserData.sp_namp);
}


//---- End of source file ------------------------------------------------------

