// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include <gtest/gtest.h>
#include <climits> //INT_MAX, INT_MIN
#include <string>  //std::string
#include "testutil/file_operations.hpp"

#include "../gkeyfile_storage.h"
#include "../mdm_config_types.h"
#include "../mdm_parameter_storage.h"

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
constexpr auto MDMD_CONFIG_FILE_GROUP =           "DEFAULT";
constexpr auto MDMD_CONFIG_FILE_SECTION =         "[DEFAULT]";
constexpr auto MDMD_CONFIG_FILE_PROJECT_SOURCE =  "../../../mdmd_init.conf";

constexpr auto KEY_LOG_LEVEL =               "log_level";
constexpr auto KEY_PORT_STATE =              "port_state";
constexpr auto KEY_SIM_ICCID =               "sim_iccid";
constexpr auto KEY_SIM_PIN =                 "sim_pin";
constexpr auto KEY_NET_SELECTION_MODE =      "net_selection_mode";
constexpr auto KEY_NET_MANUAL_OPER =         "net_manual_oper";
constexpr auto KEY_NET_MANUAL_ACT =          "net_manual_act";
constexpr auto KEY_NET_AUTOSELECT_SCANMODE = "net_autoselect_scanmode";
constexpr auto KEY_NET_AUTOSELECT_SCANSEQ =  "net_autoselect_scanseq";
constexpr auto KEY_GPRS_STATE =              "gprs_state";
constexpr auto KEY_GPRS_APN =                "gprs_apn";
constexpr auto KEY_GPRS_AUTH =               "gprs_auth_type";
constexpr auto KEY_GPRS_USER =               "gprs_username";
constexpr auto KEY_GPRS_PASS =               "gprs_password";
constexpr auto KEY_SMS_CNMI_MODE =           "sms_cnmi_mode";
constexpr auto KEY_SMS_CNMI_MT =             "sms_cnmi_mt";
constexpr auto KEY_SMS_CNMI_BM =             "sms_cnmi_bm";
constexpr auto KEY_SMS_CNMI_DS =             "sms_cnmi_ds";
constexpr auto KEY_SMS_CNMI_BFR =            "sms_cnmi_bfr";
constexpr auto KEY_SMS_CPMS_MEM1 =           "sms_cpms_mem1";
constexpr auto KEY_SMS_CPMS_MEM2 =           "sms_cpms_mem2";
constexpr auto KEY_SMS_CPMS_MEM3 =           "sms_cpms_mem3";

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
class MdmParameterStorageTest : public ::testing::Test
{
  public:
    std::string tempDir;
    std::string configFile;
    GKeyFileStorage *gKeyFileStorage;

  protected:
    MdmParameterStorageTest() : gKeyFileStorage(nullptr) {};
    ~MdmParameterStorageTest() override = default;

    void ExpectInFile(std::string key, std::string value)
    {
        std::stringstream resultString;
        resultString << key << "=" << value; 

        std::string lineBuffer;
        EXPECT_STREQ(resultString.str().c_str(), testutil_GetFirstMatchingLineFromFile(configFile.c_str(), key.c_str(), lineBuffer));
    }

    void NotExpectInFile(std::string key)
    {
        std::string lineBuffer;
        EXPECT_EQ(nullptr, testutil_GetFirstMatchingLineFromFile(configFile.c_str(), key.c_str(), lineBuffer));
    }

    void SetUp() override
    {
      tempDir = testutil_CreateTempDir("mdmd_");
      configFile = tempDir + "/mdmd_init.conf";

      // Prepare test files
      testutil_CreateFile(configFile.c_str());
      sync();
      gKeyFileStorage = new GKeyFileStorage(configFile, MDMD_CONFIG_FILE_GROUP);
    }

    void TearDown() override
    {
      // Clean test files
      EXPECT_TRUE(testutil_RemoveFile(configFile.c_str()));
      EXPECT_TRUE(testutil_RemoveDir(tempDir.c_str()));
      delete gKeyFileStorage;
    }
};

class MdmParameterStorageWithPresetConfigTest : public MdmParameterStorageTest
{
  protected:
    MdmParameterStorageWithPresetConfigTest() : MdmParameterStorageTest() {};
    ~MdmParameterStorageWithPresetConfigTest() override{};

    void SetUp() override
    {
      // Basic setup
      MdmParameterStorageTest::SetUp();

      // Add config file content
      std::string file = "";
      char const * const szConfig = testutil_ReadFromFile(MDMD_CONFIG_FILE_PROJECT_SOURCE, file);
      testutil_AppendToFile(configFile.c_str(), (uint8_t const *)szConfig, strlen(szConfig));
      sync();

      // Check config filt to contain at least section header
      std::string lineBuffer;
      ASSERT_STREQ(MDMD_CONFIG_FILE_SECTION, testutil_GetFirstMatchingLineFromFile(configFile.c_str(), MDMD_CONFIG_FILE_SECTION, lineBuffer));
      gKeyFileStorage = new GKeyFileStorage(configFile.c_str(), MDMD_CONFIG_FILE_GROUP);
    }
};


//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST_F(MdmParameterStorageTest, CreateInstance_Target)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    EXPECT_TRUE(true);
}

TEST_F(MdmParameterStorageWithPresetConfigTest, CreateInstance)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    EXPECT_TRUE(true);
}

TEST_F(MdmParameterStorageTest, ModemManagementConfig_Target)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    ModemManagementConfig defaultConfig = mdmParameterStorage.get_modem_management_config();
    
    NotExpectInFile(KEY_LOG_LEVEL);
    NotExpectInFile(KEY_PORT_STATE);

    ASSERT_NO_THROW(mdmParameterStorage.set_modem_management_config(defaultConfig));
    ExpectInFile(KEY_LOG_LEVEL,  std::to_string(defaultConfig.get_log_level()));
    ExpectInFile(KEY_PORT_STATE, std::to_string(defaultConfig.get_port_state()));
}

TEST_F(MdmParameterStorageWithPresetConfigTest, ModemManagementConfig)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    ModemManagementConfig defaultConfig = mdmParameterStorage.get_modem_management_config();
    
    ExpectInFile(KEY_LOG_LEVEL,  std::to_string(defaultConfig.get_log_level()));
    ExpectInFile(KEY_PORT_STATE, std::to_string(defaultConfig.get_port_state()));

    ModemManagementConfig newConfig(INT_MAX, INT_MIN);
    ASSERT_NO_THROW(mdmParameterStorage.set_modem_management_config(newConfig));
    ExpectInFile(KEY_LOG_LEVEL,  std::to_string(newConfig.get_log_level()));
    ExpectInFile(KEY_PORT_STATE, std::to_string(newConfig.get_port_state()));
}

TEST_F(MdmParameterStorageTest, SimAuthentication_Target)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    SimAutoActivation defaultConfig = mdmParameterStorage.get_sim_autoactivation();
    
    NotExpectInFile(KEY_SIM_ICCID);
    NotExpectInFile(KEY_SIM_PIN);

    ASSERT_NO_THROW(mdmParameterStorage.set_sim_autoactivation(defaultConfig));
    ExpectInFile(KEY_SIM_ICCID, defaultConfig.get_iccid());
    ExpectInFile(KEY_SIM_PIN,   defaultConfig.get_pin());
}

TEST_F(MdmParameterStorageWithPresetConfigTest, SimAutoActivation)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    SimAutoActivation defaultConfig = mdmParameterStorage.get_sim_autoactivation();
    
    ExpectInFile(KEY_SIM_ICCID, defaultConfig.get_iccid());
    ExpectInFile(KEY_SIM_PIN,   defaultConfig.get_pin());

    SimAutoActivation newConfig("TEST_ICCID", "TEST_PIN");
    ASSERT_NO_THROW(mdmParameterStorage.set_sim_autoactivation(newConfig));
    ExpectInFile(KEY_SIM_ICCID, newConfig.get_iccid());
    ExpectInFile(KEY_SIM_PIN,   newConfig.get_pin());
}

TEST_F(MdmParameterStorageTest, NetworkAccessConfig_Target)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    NetworkAccessConfig defaultConfig = mdmParameterStorage.get_network_access_config();
    
    NotExpectInFile(KEY_NET_SELECTION_MODE);
    NotExpectInFile(KEY_NET_MANUAL_OPER);
    NotExpectInFile(KEY_NET_MANUAL_ACT);
    NotExpectInFile(KEY_NET_AUTOSELECT_SCANMODE);
    NotExpectInFile(KEY_NET_AUTOSELECT_SCANSEQ);

    ASSERT_NO_THROW(mdmParameterStorage.set_network_access_config(defaultConfig));
    ExpectInFile(KEY_NET_SELECTION_MODE, std::to_string(defaultConfig.get_selection_mode()));
    ExpectInFile(KEY_NET_MANUAL_OPER,    std::to_string(defaultConfig.get_manual_oper()));
    ExpectInFile(KEY_NET_MANUAL_ACT,     std::to_string(defaultConfig.get_manual_act()));
    ExpectInFile(KEY_NET_AUTOSELECT_SCANMODE,      std::to_string(defaultConfig.get_autoselect_scanmode()));
    ExpectInFile(KEY_NET_AUTOSELECT_SCANSEQ, std::to_string(defaultConfig.get_autoselect_scanseq()));
}

TEST_F(MdmParameterStorageWithPresetConfigTest, NetworkAccessConfig)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    NetworkAccessConfig defaultConfig = mdmParameterStorage.get_network_access_config();
    
    ExpectInFile(KEY_NET_SELECTION_MODE, std::to_string(defaultConfig.get_selection_mode()));
    ExpectInFile(KEY_NET_MANUAL_OPER,    std::to_string(defaultConfig.get_manual_oper()));
    ExpectInFile(KEY_NET_MANUAL_ACT,     std::to_string(defaultConfig.get_manual_act()));
    ExpectInFile(KEY_NET_AUTOSELECT_SCANMODE, std::to_string(defaultConfig.get_autoselect_scanmode()));
    ExpectInFile(KEY_NET_AUTOSELECT_SCANSEQ, std::to_string(defaultConfig.get_autoselect_scanseq()));

    NetworkAccessConfig newConfig(INT_MAX, INT_MIN, -1, 750, 42);
    ASSERT_NO_THROW(mdmParameterStorage.set_network_access_config(newConfig));
    ExpectInFile(KEY_NET_SELECTION_MODE, std::to_string(newConfig.get_selection_mode()));
    ExpectInFile(KEY_NET_MANUAL_OPER,    std::to_string(newConfig.get_manual_oper()));
    ExpectInFile(KEY_NET_MANUAL_ACT,     std::to_string(newConfig.get_manual_act()));
    ExpectInFile(KEY_NET_AUTOSELECT_SCANMODE, std::to_string(newConfig.get_autoselect_scanmode()));
    ExpectInFile(KEY_NET_AUTOSELECT_SCANSEQ, std::to_string(newConfig.get_autoselect_scanseq()));
}

TEST_F(MdmParameterStorageTest, GprsAccessConfig_Target)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    GprsAccessConfig defaultConfig = mdmParameterStorage.get_gprs_access_config();
    
    NotExpectInFile(KEY_GPRS_STATE);
    NotExpectInFile(KEY_GPRS_APN);
    NotExpectInFile(KEY_GPRS_AUTH);
    NotExpectInFile(KEY_GPRS_USER);
    NotExpectInFile(KEY_GPRS_PASS);

    ASSERT_NO_THROW(mdmParameterStorage.set_gprs_access_config(defaultConfig));
    ExpectInFile(KEY_GPRS_STATE, std::to_string(defaultConfig.get_state()));
    ExpectInFile(KEY_GPRS_APN,                  defaultConfig.get_apn());
    ExpectInFile(KEY_GPRS_AUTH,  std::to_string(defaultConfig.get_auth()));
    ExpectInFile(KEY_GPRS_USER,                 defaultConfig.get_user());
    ExpectInFile(KEY_GPRS_PASS,                 defaultConfig.get_pass());
}

TEST_F(MdmParameterStorageWithPresetConfigTest, GprsAccessConfig)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    GprsAccessConfig defaultConfig = mdmParameterStorage.get_gprs_access_config();
    
    ExpectInFile(KEY_GPRS_STATE, std::to_string(defaultConfig.get_state()));
    ExpectInFile(KEY_GPRS_APN,                  defaultConfig.get_apn());
    ExpectInFile(KEY_GPRS_AUTH,  std::to_string(defaultConfig.get_auth()));
    ExpectInFile(KEY_GPRS_USER,                 defaultConfig.get_user());
    ExpectInFile(KEY_GPRS_PASS,                 defaultConfig.get_pass());

    GprsAccessConfig newConfig(INT_MAX, "TEST_APN", INT_MIN, "TEST_USER", "TEST_PASS");
    ASSERT_NO_THROW(mdmParameterStorage.set_gprs_access_config(newConfig));
    ExpectInFile(KEY_GPRS_STATE, std::to_string(newConfig.get_state()));
    ExpectInFile(KEY_GPRS_APN,                  newConfig.get_apn());
    ExpectInFile(KEY_GPRS_AUTH,  std::to_string(newConfig.get_auth()));
    ExpectInFile(KEY_GPRS_USER,                 newConfig.get_user());
    ExpectInFile(KEY_GPRS_PASS,                 newConfig.get_pass());
}

TEST_F(MdmParameterStorageTest, SmsEventReportingConfig_Target)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    SmsEventReportingConfig defaultConfig = mdmParameterStorage.get_sms_event_reporting_config();
    
    NotExpectInFile(KEY_SMS_CNMI_MODE);
    NotExpectInFile(KEY_SMS_CNMI_MT);
    NotExpectInFile(KEY_SMS_CNMI_BM);
    NotExpectInFile(KEY_SMS_CNMI_DS);
    NotExpectInFile(KEY_SMS_CNMI_BFR);

    ASSERT_NO_THROW(mdmParameterStorage.set_sms_event_reporting_config(defaultConfig));
    ExpectInFile(KEY_SMS_CNMI_MODE, std::to_string(defaultConfig.get_mode()));
    ExpectInFile(KEY_SMS_CNMI_MT,   std::to_string(defaultConfig.get_mt()));
    ExpectInFile(KEY_SMS_CNMI_BM,   std::to_string(defaultConfig.get_bm()));
    ExpectInFile(KEY_SMS_CNMI_DS,   std::to_string(defaultConfig.get_ds()));
    ExpectInFile(KEY_SMS_CNMI_BFR,  std::to_string(defaultConfig.get_bfr()));
}

TEST_F(MdmParameterStorageWithPresetConfigTest, SmsEventReportingConfig)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    SmsEventReportingConfig defaultConfig = mdmParameterStorage.get_sms_event_reporting_config();
    
    ExpectInFile(KEY_SMS_CNMI_MODE, std::to_string(defaultConfig.get_mode()));
    ExpectInFile(KEY_SMS_CNMI_MT,   std::to_string(defaultConfig.get_mt()));
    ExpectInFile(KEY_SMS_CNMI_BM,   std::to_string(defaultConfig.get_bm()));
    ExpectInFile(KEY_SMS_CNMI_DS,   std::to_string(defaultConfig.get_ds()));
    ExpectInFile(KEY_SMS_CNMI_BFR,  std::to_string(defaultConfig.get_bfr()));

    SmsEventReportingConfig newConfig(INT_MAX, INT_MIN, -1, 42, 100);
    ASSERT_NO_THROW(mdmParameterStorage.set_sms_event_reporting_config(newConfig));
    ExpectInFile(KEY_SMS_CNMI_MODE, std::to_string(newConfig.get_mode()));
    ExpectInFile(KEY_SMS_CNMI_MT,   std::to_string(newConfig.get_mt()));
    ExpectInFile(KEY_SMS_CNMI_BM,   std::to_string(newConfig.get_bm()));
    ExpectInFile(KEY_SMS_CNMI_DS,   std::to_string(newConfig.get_ds()));
    ExpectInFile(KEY_SMS_CNMI_BFR,  std::to_string(newConfig.get_bfr()));
}

TEST_F(MdmParameterStorageTest, SmsStorageConfig_Target)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    SmsStorageConfig defaultConfig = mdmParameterStorage.get_sms_storage_config();
    
    NotExpectInFile(KEY_SMS_CPMS_MEM1);
    NotExpectInFile(KEY_SMS_CPMS_MEM2);
    NotExpectInFile(KEY_SMS_CPMS_MEM3);

    ASSERT_NO_THROW(mdmParameterStorage.set_sms_storage_config(defaultConfig));
    ExpectInFile(KEY_SMS_CPMS_MEM1, defaultConfig.get_mem1());
    ExpectInFile(KEY_SMS_CPMS_MEM2, defaultConfig.get_mem2());
    ExpectInFile(KEY_SMS_CPMS_MEM3, defaultConfig.get_mem3());
}

TEST_F(MdmParameterStorageWithPresetConfigTest, SmsStorageConfig)
{
    MdmParameterStorage mdmParameterStorage(*gKeyFileStorage);
    SmsStorageConfig defaultConfig = mdmParameterStorage.get_sms_storage_config();
    
    ExpectInFile(KEY_SMS_CPMS_MEM1, defaultConfig.get_mem1());
    ExpectInFile(KEY_SMS_CPMS_MEM2, defaultConfig.get_mem2());
    ExpectInFile(KEY_SMS_CPMS_MEM3, defaultConfig.get_mem3());

    SmsStorageConfig newConfig("TEST_MEM1", "TEST_MEM2", "TEST_MEM3");
    ASSERT_NO_THROW(mdmParameterStorage.set_sms_storage_config(newConfig));
    ExpectInFile(KEY_SMS_CPMS_MEM1, newConfig.get_mem1());
    ExpectInFile(KEY_SMS_CPMS_MEM2, newConfig.get_mem2());
    ExpectInFile(KEY_SMS_CPMS_MEM3, newConfig.get_mem3());
}
//---- End of source file ------------------------------------------------------

