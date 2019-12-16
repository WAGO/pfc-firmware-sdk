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
#include "PersistenceJsonConfigConverter.hpp"

#include "MockIJsonConfigConverter.hpp"
#include "MockIPersistenceJsonConfigConverter.hpp"
#include "MockIFileEditor.hpp"
#include "MockIBackupRestore.hpp"
#include "MockIJsonConvert.hpp"
#include "TypesTestHelper.hpp"

using namespace testing;

namespace netconfd {

class APersistenceExecutor : public Test {
 public:

  PersistenceJsonConfigConverter json_converter_;
  NiceMock<MockIFileEditor> mock_file_editor_;
  MockIBackupRestore mock_backup_restore_;
  MockIJsonConvert<InterfaceConfigs> mock_port_configs_converter_;
  BackupRestoreFake legacy_restore_fake_;

  InterfaceConfigs interface_configs;
  BridgeConfig persisted_bridge_config;

  IPConfigs persisted_ip_configs;

  const ::std::string base_path_ = ".";
  const ::std::string path_persistence_file_ = "./netconfd.json";
  const ::std::string path_interface_config_file_ = "./netconfd_interface_config.json";
  const ::std::string path_network_interfaces_xml_ = "/etc/specific/network-interfaces.xml";

  ::std::unique_ptr<PersistenceExecutor> persistence_executor_;

  void SetUp() override {

    ON_CALL(mock_file_editor_, Read(path_persistence_file_, _)).WillByDefault(
        DoAll(SetArgReferee<1>(persistence_file_content), Return(Status { })));
    ON_CALL(mock_file_editor_, Read(path_interface_config_file_, _)).WillByDefault(
        DoAll(SetArgReferee<1>(interface_config_file_content_), Return(Status { })));

    persisted_ip_configs = { { "br0", IPSource::STATIC, "192.168.2.17", "255.255.255.0", "192.168.2.255" } , { "br1", IPSource::DHCP, "172.29.233.17", "255.255.0.0", "172.29.255.255" } };
    persisted_bridge_config = { { "br0", { "X1" } }, { "br1", { "X2" } } };

    interface_configs.clear();
    persistence_executor_ = ::std::make_unique<PersistenceExecutor>(base_path_, json_converter_, json_converter_,
                                                                    mock_file_editor_, mock_backup_restore_,
                                                                    legacy_restore_fake_);
  }

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
    }
}
)";

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

TEST_F(APersistenceExecutor, PersistsABridgeConfig) {

  BridgeConfig bridge_config = { { "br0", { "X1" } }, { "br1", { "X2" } } };
  IPConfigs ip_configs = { };
  ::std::string config_str = "{\"br0\": [\"X1\"]}";

  Status status = persistence_executor_->Write(bridge_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(APersistenceExecutor, PersistsIPConfigs) {

  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" } };
  ::std::string config_str = R"({"br0": ["X1"]})";

  Status status = persistence_executor_->Write(ip_configs);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(APersistenceExecutor, ReadsABridgeConfigFromPersistence) {
  BridgeConfig config_read;
  Status status = persistence_executor_->Read(config_read);

  ASSERT_EQ(StatusCode::OK, status.Get());
  EXPECT_TRUE(IsEqual(persisted_bridge_config, config_read));
}

TEST_F(APersistenceExecutor, ReadsIPConfigsFromPersistence) {
  IPConfigs config_read;
  Status status = persistence_executor_->Read(config_read);

  ASSERT_EQ(StatusCode::OK, status.Get());
  EXPECT_EQ(persisted_ip_configs, config_read);
}

TEST_F(APersistenceExecutor, ReadsConfigurationsFromPersistence) {

  BridgeConfig bridge_config_read;
  IPConfigs ip_config_read;
  Status status = persistence_executor_->Read(bridge_config_read, ip_config_read);

  ASSERT_EQ(StatusCode::OK, status.Get());
  EXPECT_TRUE(IsEqual(persisted_bridge_config, bridge_config_read));
  EXPECT_TRUE(IsEqual(persisted_ip_configs, ip_config_read));

}

TEST_F(APersistenceExecutor, BackupsAConfig) {

  ::std::string config_str =
      "{\"bridge-config\":{},\"interface-config\":{\"autonegotiation\":true,\"device\":\"X1\",\"duplex\":\"full\",\"speed\":100,\"state\":\"up\"},\"ip-config\":{}}";

  EXPECT_CALL(mock_backup_restore_, Backup(path_persistence_file_, _ , _)).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = persistence_executor_->Backup(path_persistence_file_);

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(APersistenceExecutor, RestoresAConfig) {

  ::std::string json_config_pretty = "0xxxx";

  EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_)).WillOnce(
      DoAll(SetArgReferee<1>(backup_file_content), SetArgReferee<2>(1), Return(Status(StatusCode::OK))));

  BridgeConfig bridge_config_read;
  IPConfigs ip_configs_read;
  Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                 interface_configs);

  ASSERT_EQ(StatusCode::OK, status.Get());

  EXPECT_THAT(bridge_config_read,
              ContainerEq( BridgeConfig{ {"br0", { "X1" } }, { "br1", { "X2" } }}));
  EXPECT_THAT(interface_configs,
              ElementsAre(InterfaceConfig { "X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL }));
}

TEST_F(APersistenceExecutor, TriesToRestoreAConfigInCaseRestoreFailed) {

  EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_)).WillOnce(
      DoAll(SetArgReferee<1>(""), SetArgReferee<2>(1), Return(Status(StatusCode::FILE_READ_ERROR))));
  legacy_restore_fake_.restore_status = Status(StatusCode::FILE_READ_ERROR);

  BridgeConfig bridge_config_read;
  IPConfigs ip_configs_read;

  Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                 interface_configs);

  EXPECT_EQ(StatusCode::FILE_READ_ERROR, status.Get());
  EXPECT_TRUE(bridge_config_read.empty());
  EXPECT_TRUE(ip_configs_read.empty());
}

TEST_F(APersistenceExecutor, TriesToRestoreABridgeConfigInCasePersitenceWriteFailes) {

  ::std::string config_str = "XXX";

  EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_)).WillOnce(
      DoAll(SetArgReferee<1>(backup_file_content), SetArgReferee<2>(1), Return(Status(StatusCode::OK))));

  EXPECT_CALL(mock_file_editor_, Write(path_persistence_file_, _)).WillOnce(Return(Status{StatusCode::FILE_WRITE_ERROR}));
  EXPECT_CALL(mock_file_editor_, Write(path_network_interfaces_xml_, _)).WillOnce(Return(Status{StatusCode::OK}));

  BridgeConfig bridge_config_read;
  IPConfigs ip_configs_read;
  Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                 interface_configs);

  EXPECT_EQ(StatusCode::FILE_WRITE_ERROR, status.Get());
  EXPECT_TRUE(bridge_config_read.empty());
  EXPECT_TRUE(ip_configs_read.empty());
}

TEST_F(APersistenceExecutor, TriesToRestoreABridgeConfigInCaseInterfaceConfigWriteFailes) {

  ::std::string config_str = "XXX";

  EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_)).WillOnce(
      DoAll(SetArgReferee<1>(backup_file_content), SetArgReferee<2>(1), Return(Status(StatusCode::OK))));

  EXPECT_CALL(mock_file_editor_, Write(path_persistence_file_, _)).WillOnce(Return(Status{StatusCode::OK}));
  EXPECT_CALL(mock_file_editor_, Write(path_interface_config_file_, _)).WillOnce(Return(Status{StatusCode::FILE_WRITE_ERROR}));
  EXPECT_CALL(mock_file_editor_, Write(path_network_interfaces_xml_, _)).WillOnce(Return(Status{StatusCode::OK}));

  BridgeConfig bridge_config_read;
  IPConfigs ip_configs_read;
  Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                 interface_configs);

  EXPECT_EQ(StatusCode::FILE_WRITE_ERROR, status.Get());
  EXPECT_TRUE(bridge_config_read.empty());
  EXPECT_TRUE(ip_configs_read.empty());
}

TEST_F(APersistenceExecutor, RestoreZeroVersionAndFails) {

  ::std::string config_str = "XXX";
  ::std::uint32_t version = 0;

  EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_)).WillOnce(
      DoAll(SetArgReferee<1>(config_str), SetArgReferee<2>(version), Return(Status(StatusCode::OK))));

  BridgeConfig bridge_config_read;
  IPConfigs ip_configs_read;
  Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                 interface_configs);

  EXPECT_EQ(StatusCode::BACKUP_FILE_ERROR, status.Get());
  EXPECT_TRUE(bridge_config_read.empty());
  EXPECT_TRUE(ip_configs_read.empty());
}

TEST_F(APersistenceExecutor, TriesToRestoreAHigherVersionAndFails) {

  ::std::string config_str = "XXX";
  ::std::uint32_t higher_version = 42;

  EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_)).WillOnce(
      DoAll(SetArgReferee<1>(config_str), SetArgReferee<2>(higher_version), Return(Status(StatusCode::OK))));

  BridgeConfig bridge_config_read;
  IPConfigs ip_configs_read;
  Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                 interface_configs);

  EXPECT_EQ(StatusCode::BACKUP_FILE_ERROR, status.Get());
  EXPECT_TRUE(bridge_config_read.empty());
  EXPECT_TRUE(ip_configs_read.empty());
}

} /* namespace netconfd */
