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
///  \file     test_ResourceHandling.cc
///
///  \brief    Test for helper functions for config tool resource handling.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/resource_handling.h"
#include "ctutil/error_codes.h"
#include <gtest/gtest.h>
#include "testutil/file_operations.hpp"
#include <string.h>


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST(CommonResources, LastErrorFile)
{
  ctutil_ResourcesCommon_t stResources;
  memset(&stResources, 0, sizeof(stResources));

  statusCode_t const result = ctutil_OpenCommonResources(&stResources);

  ASSERT_EQ(CTUTIL_SUCCESS, result);
  EXPECT_STREQ("/tmp/last_error.txt", stResources.szLastErrorFile);

  ctutil_ReleaseCommonResources(&stResources);
}


TEST(ChangeableFile, PrepareChangeableFile)
{
  char const szTestFilePath[] = "/tmp/test_file";
  ctutil_ChangeableFile_t * pFile = NULL;
  statusCode_t const prepareResult = ctutil_PrepareChangeableFile(szTestFilePath, &(pFile));
  ASSERT_EQ(CTUTIL_SUCCESS, prepareResult);
  ASSERT_NE((void *)NULL, pFile);
  char const * const szOriginalPath = ctutil_GetOriginalFilePath(pFile);
  EXPECT_NE((void *)NULL, szOriginalPath);
  EXPECT_STREQ(szTestFilePath, szOriginalPath);
  char const * const szChangablePath = ctutil_GetChangeableFilePath(pFile);
  EXPECT_NE((void *)NULL, szChangablePath);
  EXPECT_STRNE(szTestFilePath, szChangablePath);
  ctutil_ReleaseFileForChanges(&(pFile));
  EXPECT_EQ((void *)NULL, pFile);
}


TEST(ChangeableFile, ReleaseEmptyHandle)
{
  ctutil_ChangeableFile_t * pFile = NULL;
  ctutil_ReleaseFileForChanges(&(pFile));
  EXPECT_EQ((void *)NULL, pFile);
}


TEST(ChangeableFile, CommitChangedFile)
{
  ::std::string lineBuffer;

  // Prepare test file
  std::string tempDir = testutil_CreateTempDir("libconfigtoolutils_");
  std::string testFilePath = tempDir + "/test_file";
  char const * szTestFilePath = testFilePath.c_str();
  char const szTestContentOriginal[] = "Hello World!";
  char const szTestContentReplacement[] = "Unit tests are cool!";
  testutil_CreateFile(szTestFilePath);
  testutil_AppendToFile(szTestFilePath, (uint8_t const *)szTestContentOriginal, strlen(szTestContentOriginal));
  ASSERT_EQ(0, chmod(szTestFilePath, (S_IRUSR | S_IWUSR)));
  sync();
  ASSERT_STREQ(szTestContentOriginal, testutil_GetFirstMatchingLineFromFile(szTestFilePath, "e", lineBuffer));

  // Check initial file rights
  struct stat stOriginalFileStat;
  int const statOriginalResult = lstat(szTestFilePath, &stOriginalFileStat);
  ASSERT_EQ(0, statOriginalResult);

  // Prepare test object
  ctutil_ChangeableFile_t * pFile = NULL;
  statusCode_t const prepareResult = ctutil_PrepareChangeableFile(szTestFilePath, &(pFile));
  ASSERT_EQ(CTUTIL_SUCCESS, prepareResult);
  char const * const szOriginalPath = ctutil_GetOriginalFilePath(pFile);
  ASSERT_STREQ(szTestFilePath, szOriginalPath);
  char const * const szChangablePath = ctutil_GetChangeableFilePath(pFile);

  // Modify file
  testutil_CreateFile(szChangablePath);
  testutil_AppendToFile(szChangablePath, (uint8_t const *)szTestContentReplacement, strlen(szTestContentReplacement));
  ASSERT_STREQ(szTestContentReplacement, testutil_GetFirstMatchingLineFromFile(szChangablePath, "e", lineBuffer));
  ASSERT_EQ(CTUTIL_SUCCESS, ctutil_CommitChangeableFile(pFile));

  // Check for commited result
  EXPECT_STREQ(szTestContentReplacement, testutil_GetFirstMatchingLineFromFile(szTestFilePath, "e", lineBuffer));
  struct stat stModifiedFileStat;
  int const statModifiedResult = lstat(szTestFilePath, &stModifiedFileStat);
  ASSERT_EQ(0, statModifiedResult);
  EXPECT_EQ(stOriginalFileStat.st_mode, stModifiedFileStat.st_mode);

  // Cleanup
  EXPECT_TRUE(testutil_RemoveFile(szTestFilePath));
  EXPECT_TRUE(testutil_RemoveDir(tempDir.c_str()));
  // FIXME: We should clean the temporarily file may be left but if it not exist QEMU will crash when run with Valgrind
//  (void)testutil_RemoveFile(szChangablePath);
  ctutil_ReleaseFileForChanges(&(pFile));
}


//---- End of source file ------------------------------------------------------

