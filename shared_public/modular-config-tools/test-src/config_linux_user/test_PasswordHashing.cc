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
///  \file     test_PasswordHashing.cc
///
///  \brief    Tests for password hashing in config tool config_linux_user.
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

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------
#define TEST_RANDOM_BYTES_1_SIZE                                            (4U)
#define TEST_RANDOM_BYTES_1_VALUE                                           (1U)

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
class PasswordHashingTest_config_linux_user : public ::testing::Test
{
  private:
    std::string tempDir;
    std::string lastErrorFile;
    std::string randSource;

  protected:
    ctutil_Resources_t stResources;

  protected:
    PasswordHashingTest_config_linux_user() {};
    virtual ~PasswordHashingTest_config_linux_user() {};
    virtual void SetUp()
    {
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

      // Prepare test files
      testutil_CreateFile(this->stResources.stCommonResources.szLastErrorFile);
      testutil_CreateFile(this->stResources.pstSpecificResources->szRandomSource1);
      uint8_t const arTestBytes[TEST_RANDOM_BYTES_1_SIZE] = { TEST_RANDOM_BYTES_1_VALUE,
                                                              TEST_RANDOM_BYTES_1_VALUE,
                                                              TEST_RANDOM_BYTES_1_VALUE,
                                                              TEST_RANDOM_BYTES_1_VALUE};
      size_t const random1Bytes = sizeof(arTestBytes);
      testutil_AppendToFile(this->stResources.pstSpecificResources->szRandomSource1, arTestBytes, random1Bytes);
      sync();
    }
    virtual void TearDown()
    {
      // Clean test files
      EXPECT_TRUE(testutil_RemoveFile(this->stResources.pstSpecificResources->szRandomSource1));
      EXPECT_TRUE(testutil_RemoveFile(this->stResources.stCommonResources.szLastErrorFile));
      EXPECT_TRUE(testutil_RemoveDir(tempDir.c_str()));

      // Clean resources and options
      free(this->stResources.pstSpecificResources);
    }
};

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST_F(PasswordHashingTest_config_linux_user, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_F(PasswordHashingTest_config_linux_user, GetBytesFromRandomSource)
{
  unsigned char arRandomBuffer[128];
  size_t const bufferSize = sizeof(arRandomBuffer);
  GetRandomBytes(&(this->stResources), arRandomBuffer, bufferSize);
  ASSERT_LT(TEST_RANDOM_BYTES_1_SIZE, bufferSize);
  for(size_t i = 0; i < TEST_RANDOM_BYTES_1_SIZE; ++i)
  {
    EXPECT_EQ(TEST_RANDOM_BYTES_1_VALUE, arRandomBuffer[i]);
  }
  ASSERT_LE(TEST_RANDOM_BYTES_1_SIZE * 2, bufferSize);
  for(size_t i = TEST_RANDOM_BYTES_1_SIZE; i < (TEST_RANDOM_BYTES_1_SIZE * 2); ++i)
  {
    EXPECT_EQ(0U, arRandomBuffer[i]);
  }
  for(size_t i = (TEST_RANDOM_BYTES_1_SIZE * 2); i < bufferSize; ++i)
  {
    ASSERT_EQ(0U, arRandomBuffer[i]);
  }
}


TEST_F(PasswordHashingTest_config_linux_user, HashPassword)
{
  char szPassword[] = "secret";
  char const * const szHashedPassword = CryptPwd(&(this->stResources), szPassword);
  ASSERT_NE((char const *)NULL, szHashedPassword);
  // Expected format: $<ID>$<salt>$<hashed>, ID: 1 char, salt: 8..16 char, encrypted: 86 char in case of SHA512
  ASSERT_EQ('$', szHashedPassword[0]);
  ASSERT_NE('\0', szHashedPassword[1]);
  EXPECT_EQ('6', szHashedPassword[1]);
  ASSERT_EQ('$', szHashedPassword[2]);
  // Random source isn't random in this test => salt is equal for every call
  char const expectedSalt[17U] = { 47, 47, 47, 47, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, '$' };
  size_t saltIndex = 0;
  while(    (szHashedPassword[3 + saltIndex] != '\0')
         && (szHashedPassword[3 + saltIndex] != '$'))
  {
    ASSERT_EQ(expectedSalt[saltIndex], szHashedPassword[3 + saltIndex]);
    ++saltIndex;
  }
  ASSERT_GE(saltIndex, 8U);
  ASSERT_EQ('$', szHashedPassword[3 + saltIndex]);
  EXPECT_LE(86U, strlen(&(szHashedPassword[4 + saltIndex])));
  // SHA512 for given password is known and in every test the same because salt isn't random in test
  char const szExpectedHash[] = "65JlJIiNBA.vHagukTzD1/QrJkQlr2jloQMwd2ugZ2Sk6RW5FdQWp1IGOVfuTfDmu/pLByQATgRwP2F6edJr4/";
  EXPECT_STREQ(szExpectedHash, &(szHashedPassword[4 + saltIndex]));
}


//---- End of source file ------------------------------------------------------

