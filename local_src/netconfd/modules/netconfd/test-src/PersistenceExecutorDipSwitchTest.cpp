// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     PersistenceProviderTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TypesHelper.hpp"

#include "BackupRestoreFake.hpp"
#include "PersistenceExecutor.hpp"
#include "StatusPrettyPrint.hpp"
#include "FileEditorFake.hpp"

#include "MockIFileEditor.hpp"
#include "MockIBackupRestore.hpp"
#include "MockIJsonConvert.hpp"
#include "MockIDipSwitch.hpp"
#include "Status.hpp"
#include "TypesTestHelper.hpp"

using namespace testing;

namespace netconf
{

  class ADipSwitchPersistenceExecutor: public Test
  {
    public:

      NiceMock<MockIFileEditor> mock_file_editor_;
      MockIBackupRestore mock_backup_restore_;
      MockIJsonConvert<InterfaceConfigs> mock_port_configs_converter_;
      MockIDipSwitch mock_dip_switch_;
      BackupRestoreFake legacy_restore_fake_;

      InterfaceConfigs interface_configs;
      BridgeConfig persisted_bridge_config;

      IPConfigs persisted_ip_configs;
      DipSwitchIpConfig persisted_dip_switch_ip_config_;

      const ::std::string base_path_ = ".";
      const ::std::string path_persistence_file_ = "./netconfd.json";
      const ::std::string path_interface_config_file_ = "./netconfd_interface_config.json";
      const ::std::string path_network_interfaces_xml_ = "/etc/specific/network-interfaces.xml";

      ::std::unique_ptr<PersistenceExecutor> persistence_executor_;

      void SetUp() override
      {

        ON_CALL(mock_file_editor_, Read(path_interface_config_file_, _)).WillByDefault(
            DoAll(SetArgReferee<1>(interface_config_file_content_), Return(Status{ })));

        persisted_ip_configs = {{"br0", IPSource::STATIC, "192.168.2.17", "255.255.255.0"}, {
            "br1", IPSource::DHCP, "172.29.233.17", "255.255.0.0"}};
        persisted_bridge_config = {{"br0", {"X1"}}, {"br1", {"X2"}}};

        interface_configs.clear();

      }

      void WriteDipIPConfigIfNoDipIpIsPersisted(DipSwitchMode dip_mode,
                                                const ::std::string &persistence_file)
      {
        DipSwitchIpConfig dipConfigToWrite("192.168.1.0", "255.255.255.0");
        DipSwitchIpConfig dipConfigWritten;

        ON_CALL(mock_file_editor_, Read(path_persistence_file_, _)).WillByDefault(
            DoAll(SetArgReferee<1>(persistence_file), Return(Status{ })));

        EXPECT_CALL(mock_dip_switch_, GetMode()).WillRepeatedly(Return(dip_mode));

        persistence_executor_ = ::std::make_unique<PersistenceExecutor>(base_path_,
                                                                        mock_file_editor_, mock_backup_restore_,
                                                                        legacy_restore_fake_, mock_dip_switch_);
        Status status = persistence_executor_->Write(dipConfigToWrite);
        ASSERT_EQ(StatusCode::OK, status.GetStatusCode());

        status = persistence_executor_->Read(dipConfigWritten);
        ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
        ASSERT_EQ(dipConfigToWrite, dipConfigWritten);
      }

      ::std::string persistence_file_content_no_dip =
          R"({
    "bridge-config" : {
      "br0": ["X1"],
      "br1": ["X2"]
    },
    "ip-config" : {
      "br0" : {
        "source" : "static",
        "ipaddr" : "192.168.2.17",
        "netmask" : "255.255.255.0",
        "bcast" : "192.168.2.255"
      },
    "br1" : {
        "source" : "dhcp",
        "ipaddr" : "172.29.233.17",
        "netmask" : "255.255.0.0",
        "bcast" : "172.29.255.255"
      }
    }
  })";

      ::std::string persistence_file_content =
          R"({
    "bridge-config" : {
      "br0": ["X1"],
      "br1": ["X2"]
    },
    "ip-config" : {
      "br0" : {
        "source" : "static",
        "ipaddr" : "192.168.2.17",
        "netmask" : "255.255.255.0",
        "bcast" : "192.168.2.255"
      },
    "br1" : {
        "source" : "dhcp",
        "ipaddr" : "172.29.233.17",
        "netmask" : "255.255.0.0",
        "bcast" : "172.29.255.255"
      }
    },
    "dip-ip-config" : {
        "ipaddr" : "192.169.42.0",
        "netmask" : "255.255.255.0"
    }
  })";

      ::std::string interface_config_file_content_ =
          R"([
          {"autonegotiation":true,"device":"X1","duplex":"full","speed":100,"state":"up"}, 
          {"autonegotiation":false,"device":"X2","duplex":"half","speed":10,"state":"up"}
         ])";

      ::std::string backup_file_content =
          R"({
      "bridge-config" : {
        "br0": ["X1"],
        "br1": ["X2"]
      },
      "ip-config" : {
        "br0" : {
          "source" : "static",
          "ipaddr" : "192.168.2.17",
          "netmask" : "255.255.255.0",
          "bcast" : "192.168.2.255"
        },
      "br1" : {
          "source" : "dhcp",
          "ipaddr" : "172.29.233.17",
          "netmask" : "255.255.0.0",
          "bcast" : "172.29.255.255"
        }
      },
      "interface-config":{"autonegotiation":true,"device":"X1","duplex":"full","speed":100,"state":"up"}
  })";

  };

  TEST_F(ADipSwitchPersistenceExecutor, ReadADipIPConfigIfDipIpIsPersisted)
  {
    DipSwitchIpConfig dipConfigWritten;

    ON_CALL(mock_file_editor_, Read(path_persistence_file_, _)).WillByDefault(
        DoAll(SetArgReferee<1>(persistence_file_content), Return(Status{ })));

    EXPECT_CALL(mock_dip_switch_, GetMode()).WillRepeatedly(Return(DipSwitchMode::OFF));

    persistence_executor_ = ::std::make_unique<PersistenceExecutor>(base_path_,
                                                                    mock_file_editor_, mock_backup_restore_,
                                                                    legacy_restore_fake_, mock_dip_switch_);

    Status status = persistence_executor_->Read(dipConfigWritten);
    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
    ASSERT_EQ(DipSwitchIpConfig("192.169.42.0", "255.255.255.0"), dipConfigWritten);
  }

  TEST_F(ADipSwitchPersistenceExecutor, ReadADipIPConfigIfNoDipIpIsPersisted)
  {
    DipSwitchIpConfig dipConfigWritten;

    ON_CALL(mock_file_editor_, Read(path_persistence_file_, _)).WillByDefault(
        DoAll(SetArgReferee<1>(persistence_file_content_no_dip), Return(Status{ })));

    EXPECT_CALL(mock_dip_switch_, GetMode()).WillRepeatedly(Return(DipSwitchMode::OFF));

    persistence_executor_ = ::std::make_unique<PersistenceExecutor>(base_path_,
                                                                    mock_file_editor_, mock_backup_restore_,
                                                                    legacy_restore_fake_, mock_dip_switch_);

    Status status = persistence_executor_->Read(dipConfigWritten);
    ASSERT_EQ(StatusCode::PERSISTENCE_READ, status.GetStatusCode());
    ASSERT_EQ(DipSwitchIpConfig(), dipConfigWritten);
  }

  TEST_F(ADipSwitchPersistenceExecutor, PersistsADipIPConfigIfDipSwitchIsOff)
  {
    WriteDipIPConfigIfNoDipIpIsPersisted(DipSwitchMode::OFF, persistence_file_content);
  }

  TEST_F(ADipSwitchPersistenceExecutor, PersistsADipIPConfigIfDipSwitchIsOffAndNoDipIpIsPersisted)
  {
    WriteDipIPConfigIfNoDipIpIsPersisted(DipSwitchMode::OFF, persistence_file_content_no_dip);
  }

  TEST_F(ADipSwitchPersistenceExecutor, PersistsADipIPConfigIfDipSwitchIsSetToDhcpAndNoDipIpIsPersisted)
  {
    WriteDipIPConfigIfNoDipIpIsPersisted(DipSwitchMode::DHCP, persistence_file_content_no_dip);
  }

  TEST_F(ADipSwitchPersistenceExecutor, PersistsADipIPConfigIfDipSwitchIsSetToStaticAndNoDipIpIsPersisted)
  {
    WriteDipIPConfigIfNoDipIpIsPersisted(DipSwitchMode::STATIC, persistence_file_content_no_dip);
  }

} /* namespace netconf */
