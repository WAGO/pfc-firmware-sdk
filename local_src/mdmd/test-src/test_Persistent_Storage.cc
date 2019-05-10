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

    void ExpectInFile(std::string key, std::string value)
    {
        std::stringstream resultString;
        resultString << key << "=" << value; 

        std::string lineBuffer;
        EXPECT_STREQ(resultString.str().c_str(), testutil_GetFirstMatchingLineFromFile(szConfigFile, key.c_str(), lineBuffer));
    }

    void SetUp() override
    {
      tempDir = testutil_CreateTempDir("mdmd_");
      configFile = tempDir + "/mdmd_init.conf";
      szConfigFile = configFile.c_str();
      mdmd_test_szConfigFile = szConfigFile;

      // Prepare test files
      testutil_CreateFile(szConfigFile);
      sync();
    }

    void TearDown() override
    {
      // Clean test files
      EXPECT_TRUE(testutil_RemoveFile(szConfigFile));
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
      testutil_AppendToFile(szConfigFile, (uint8_t const *)szConfig, strlen(szConfig));
      sync();

      // Check config filt to contain at least section header
      std::string lineBuffer;
      ASSERT_STREQ(szSection, testutil_GetFirstMatchingLineFromFile(szConfigFile, szSection, lineBuffer));
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

TEST_F(PersistentStorageTest, GprsAccessConfig)
{
    PersistentStorage storageManager;
    
    GprsAccessConfig gprs_access(1, "testapn", 1, "testuser", "testpw");
    GprsAccessConfig read_gprs_access;

    storageManager.set_gprs_access(gprs_access);
    storageManager.save();

    EXPECT_TRUE(storageManager.get_gprs_access(read_gprs_access));
    EXPECT_EQ(read_gprs_access.get_state(), gprs_access.get_state());
    EXPECT_EQ(read_gprs_access.get_apn(), gprs_access.get_apn());
    EXPECT_EQ(read_gprs_access.get_auth(), gprs_access.get_auth());
    EXPECT_EQ(read_gprs_access.get_user(), gprs_access.get_user());
    EXPECT_EQ(read_gprs_access.get_pass(), gprs_access.get_pass());

    ExpectInFile("gprs_state", std::to_string(gprs_access.get_state()));
    ExpectInFile("gprs_apn", gprs_access.get_apn());
    ExpectInFile("gprs_auth_type", std::to_string(gprs_access.get_auth()));
    ExpectInFile("gprs_username", gprs_access.get_user());
    ExpectInFile("gprs_password", gprs_access.get_pass());
}

TEST_F(PersistentStorageTest, CreateInstance)
{
  EXPECT_NO_FATAL_FAILURE(CreateInstance());
}

TEST_F(PersistentStorageWithPresetConfigTest, CreateInstance_Host)
{
  EXPECT_NO_FATAL_FAILURE(CreateInstance());
}

TEST_F(PersistentStorageTest, Sim)
{
  // Create UUT instance
  PersistentStorage storageManager;

  // Write sim attributes
  std::string iccid = "test-ICCID_1";
  std::string pin = "testpin0123";
  storageManager.set_sim(iccid, pin);
  storageManager.save();

  ExpectInFile("sim_iccid", iccid);
  ExpectInFile("sim_pin", pin);
  
  // Read sim attributes
  std::string readIccid;
  std::string readPin;
  storageManager.get_sim(readIccid, readPin);
  EXPECT_STREQ(iccid.c_str(), readIccid.c_str());
  EXPECT_STREQ(pin.c_str(), readPin.c_str());
}

TEST_F(PersistentStorageTest, NetSelection)
{
  // Create UUT instance
  PersistentStorage storageManager;

  // Write net selection mode
  int const mode = 3;
  storageManager.set_selection(mode);
  storageManager.save();

  ExpectInFile("net_selection_mode", std::to_string(mode));

  // Read net selection mode
  int readMode;
  storageManager.get_selection(readMode);
  EXPECT_EQ(mode, readMode);
}

TEST_F(PersistentStorageTest, OperationContext)
{
  // Create UUT instance
  PersistentStorage storageManager;

  // Write operation context
  int const id = 53;
  int const act = 64;
  storageManager.set_oper(id, act);
  storageManager.save();

  ExpectInFile("net_manual_oper", std::to_string(id));
  ExpectInFile("net_manual_act", std::to_string(act));
  
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
    std::string key = "net_manual_oper";
    std::string lineBuffer;
    EXPECT_STREQ(NULL, testutil_GetFirstMatchingLineFromFile(szConfigFile, key.c_str(), lineBuffer));
  }
  {
    std::string key = "net_manual_act";
    std::string lineBuffer;
    EXPECT_STREQ(NULL, testutil_GetFirstMatchingLineFromFile(szConfigFile, key.c_str(), lineBuffer));
  }
}

TEST_F(PersistentStorageTest, SmsEventReportingConfig)
{
  PersistentStorage storageManager;

  SmsEventReportingConfig sms_config(2, 1, 2, 1, 1);

  storageManager.set_sms_reporting_config(sms_config);
  storageManager.save();

  ExpectInFile("sms_cnmi_mode", std::to_string(sms_config.get_mode()));
  ExpectInFile("sms_cnmi_mt", std::to_string(sms_config.get_mt()));
  ExpectInFile("sms_cnmi_bm", std::to_string(sms_config.get_bm()));
  ExpectInFile("sms_cnmi_ds", std::to_string(sms_config.get_ds()));
  ExpectInFile("sms_cnmi_bfr", std::to_string(sms_config.get_bfr()));

  SmsEventReportingConfig read_sms_config;
  ASSERT_TRUE(storageManager.get_sms_reporting_config(read_sms_config));
  EXPECT_EQ(sms_config.get_mode(), read_sms_config.get_mode());
  EXPECT_EQ(sms_config.get_mt(), read_sms_config.get_mt());
  EXPECT_EQ(sms_config.get_bm(), read_sms_config.get_bm());
  EXPECT_EQ(sms_config.get_ds(), read_sms_config.get_ds());
  EXPECT_EQ(sms_config.get_bfr(), read_sms_config.get_bfr());
}

TEST_F(PersistentStorageTest, SmsStorageConfig)
{
  PersistentStorage storageManager;
  SmsStorageConfig sms_storage_config;

  storageManager.set_sms_storage_config(sms_storage_config);
  storageManager.save();

  ExpectInFile("sms_cpms_mem1", sms_storage_config.get_mem1());
  ExpectInFile("sms_cpms_mem2", sms_storage_config.get_mem2());
  ExpectInFile("sms_cpms_mem3", sms_storage_config.get_mem3());

  SmsStorageConfig read_sms_storage_config;
  ASSERT_TRUE(storageManager.get_sms_storage_config(read_sms_storage_config));
  EXPECT_EQ(sms_storage_config.get_mem1(), read_sms_storage_config.get_mem1());
  EXPECT_EQ(sms_storage_config.get_mem2(), read_sms_storage_config.get_mem2());
  EXPECT_EQ(sms_storage_config.get_mem3(), read_sms_storage_config.get_mem3());
}

//---- End of source file ------------------------------------------------------

