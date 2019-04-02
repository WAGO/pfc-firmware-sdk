//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project mdmd (PTXdist package mdmd).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_Persistent_Storage.cc
///
///  \brief    Test for MDMD persistent storage layer.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <string>
#include "testutil/file_operations.hpp"
#include "../mdmd_log.h"
#include "../persistent_storage.h"

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------
#define MDMD_CONFIG_FILE_PROJECT_SOURCE                "../../../mdmd_init.conf"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

static char const * mdmd_test_szConfigFile = nullptr;

class PersistentStorageTest : public ::testing::Test
{
  public:
    std::string tempDir;
    std::string configFile;
    char const * szConfigFile;
    char const * const szSection;

  protected:
    PersistentStorageTest() : szConfigFile(nullptr), szSection("[" MDMD_CONFIG_FILE_SECTION "]") {};
    ~PersistentStorageTest() override {};

    void SetUp() override
    {
      tempDir = testutil_CreateTempDir("mdmd_");
      configFile = tempDir + "/mdmd_init.conf";
      szConfigFile = configFile.c_str();
      mdmd_test_szConfigFile = szConfigFile;

      // Prepare test files
      testutil_CreateFile(this->szConfigFile);
      sync();
    }

    void TearDown() override
    {
      // Clean test files
      EXPECT_TRUE(testutil_RemoveFile(this->szConfigFile));
      EXPECT_TRUE(testutil_RemoveDir(tempDir.c_str()));
    }
};


class PersistentStorageWithPresetConfigTest : public PersistentStorageTest
{
  protected:
    PersistentStorageWithPresetConfigTest() : PersistentStorageTest() {};
    ~PersistentStorageWithPresetConfigTest() override{};
    void SetUp() override
    {
      // Basic setup
      PersistentStorageTest::SetUp();

      // Add config file content
      std::string file = "";
      char const * const szConfig = testutil_ReadFromFile(MDMD_CONFIG_FILE_PROJECT_SOURCE, file);
      testutil_AppendToFile(this->szConfigFile, (uint8_t const *)szConfig, strlen(szConfig));
      sync();

      // Check config filt to contain at least section header
      std::string lineBuffer;
      ASSERT_STREQ(this->szSection, testutil_GetFirstMatchingLineFromFile(this->szConfigFile, this->szSection, lineBuffer));
    }
};

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------

extern "C"
{
  char const * mdmd_test_GetConfigFileName(void)
  {
    return mdmd_test_szConfigFile;
  }
}

TEST_F(PersistentStorageTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}

TEST_F(PersistentStorageWithPresetConfigTest, CheckSetup_Host)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


static void CreateInstance()
{
  // Create UUT instance
  PersistentStorage storageManager;
  EXPECT_TRUE(true);
}

TEST_F(PersistentStorageTest, CreateInstance)
{
  EXPECT_NO_FATAL_FAILURE(CreateInstance());
}

TEST_F(PersistentStorageWithPresetConfigTest, CreateInstance_Host)
{
  EXPECT_NO_FATAL_FAILURE(CreateInstance());
}


static void Sim(PersistentStorageTest const * const test)
{
  // Create UUT instance
  PersistentStorage storageManager;

  // Write sim attributes
  std::string iccid = "test-ICCID_1";
  std::string pin = "testpin0123";
  storageManager.set_sim(iccid, pin);
  storageManager.save();

  // Check config file for written lines
  {
    std::string key = "sim_iccid";
    std::stringstream resultString;
    resultString << key << "=" << iccid;
    std::string lineBuffer;
    EXPECT_STREQ(resultString.str().c_str(), testutil_GetFirstMatchingLineFromFile(test->szConfigFile, key.c_str(), lineBuffer));
  }
  {
    std::string key = "sim_pin";
    std::stringstream resultString;
    resultString << key << "=" << pin;
    std::string lineBuffer;
    EXPECT_STREQ(resultString.str().c_str(), testutil_GetFirstMatchingLineFromFile(test->szConfigFile, key.c_str(), lineBuffer));
  }

  // Read sim attributes
  std::string readIccid;
  std::string readPin;
  storageManager.get_sim(readIccid, readPin);
  EXPECT_STREQ(iccid.c_str(), readIccid.c_str());
  EXPECT_STREQ(pin.c_str(), readPin.c_str());
}

TEST_F(PersistentStorageTest, Sim)
{
  EXPECT_NO_FATAL_FAILURE(Sim(this));
}

TEST_F(PersistentStorageWithPresetConfigTest, Sim_Host)
{
  EXPECT_NO_FATAL_FAILURE(Sim(this));
}


static void NetSelection(PersistentStorageTest const * const test)
{
  // Create UUT instance
  PersistentStorage storageManager;

  // Write net selection mode
  int const mode = 3;
  storageManager.set_selection(mode);
  storageManager.save();

  // Check config file for written line
  std::string key = "net_selection";
  std::stringstream resultString;
  resultString << key << "=" << mode;
  std::string lineBuffer;
  EXPECT_STREQ(resultString.str().c_str(), testutil_GetFirstMatchingLineFromFile(test->szConfigFile, key.c_str(), lineBuffer));

  // Read net selection mode
  int readMode;
  storageManager.get_selection(readMode);
  EXPECT_EQ(mode, readMode);
}

TEST_F(PersistentStorageTest, NetSelection)
{
  EXPECT_NO_FATAL_FAILURE(NetSelection(this));
}

TEST_F(PersistentStorageWithPresetConfigTest, NetSelection_Host)
{
  EXPECT_NO_FATAL_FAILURE(NetSelection(this));
}


static void OperationContext(PersistentStorageTest const * const test)
{
  // Create UUT instance
  PersistentStorage storageManager;

  // Write operation context
  int const id = 53;
  int const act = 64;
  storageManager.set_oper(id, act);
  storageManager.save();

  // Check config file for written lines
  {
    std::string key = "oper_id";
    std::stringstream resultString;
    resultString << key << "=" << id;
    std::string lineBuffer;
    EXPECT_STREQ(resultString.str().c_str(), testutil_GetFirstMatchingLineFromFile(test->szConfigFile, key.c_str(), lineBuffer));
  }
  {
    std::string key = "oper_act";
    std::stringstream resultString;
    resultString << key << "=" << act;
    std::string lineBuffer;
    EXPECT_STREQ(resultString.str().c_str(), testutil_GetFirstMatchingLineFromFile(test->szConfigFile, key.c_str(), lineBuffer));
  }

  // Read operation context
  int readId;
  int readAct;
  storageManager.get_oper(readId, readAct);
  EXPECT_EQ(id, readId);
  EXPECT_EQ(act, readAct);

  // Clear operation context
  storageManager.remove_oper();
  storageManager.save();

  // Check config file for removed lines
  {
    std::string key = "oper_id";
    std::string lineBuffer;
    EXPECT_STREQ(NULL, testutil_GetFirstMatchingLineFromFile(test->szConfigFile, key.c_str(), lineBuffer));
  }
  {
    std::string key = "oper_act";
    std::string lineBuffer;
    EXPECT_STREQ(NULL, testutil_GetFirstMatchingLineFromFile(test->szConfigFile, key.c_str(), lineBuffer));
  }
}

TEST_F(PersistentStorageTest, OperationContext)
{
  EXPECT_NO_FATAL_FAILURE(OperationContext(this));
}

TEST_F(PersistentStorageWithPresetConfigTest, OperationContext_Host)
{
  EXPECT_NO_FATAL_FAILURE(OperationContext(this));
}


//---- End of source file ------------------------------------------------------

