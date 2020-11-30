// SPDX-License-Identifier: GPL-2.0-or-later
#include <gtest/gtest.h>
#include <MockIDeviceProperties.hpp>
#include "RestoreLegacy.hpp"
#include "FileEditorFake.hpp"
#include <limits>
#include <memory>

namespace netconf {

using ::std::string;
using ::std::unique_ptr;
using ::std::uint32_t;
using ::std::size_t;

class ALegacyRestore : public ::testing::Test {
 public:
  FileEditorFake mock_file_editor_;
  MockIDeviceProperties mock_properties_provider_;
  uint32_t restored_version;
  string data;
  string path;
  unique_ptr<RestoreLegacy> backup_restore_;

  void SetUp() {
    restored_version = ::std::numeric_limits<uint32_t>::max();
    backup_restore_ = ::std::make_unique<RestoreLegacy>(mock_file_editor_, mock_properties_provider_);
  }
};

static string StringRemoveNewlineAndBlank(string str) {

  str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
  str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

  return str;
}


TEST_F(ALegacyRestore, RestoresABackupOfAPreviousFirmwareInSwitchedMode) {

  mock_file_editor_.content_ =
      R"(
XXX=123
XXX=123
dsa-mode=0
ip-config-type-eth0=static
ip-address-eth0=192.168.1.1
subnet-mask-eth0=255.255.255.0
ip-config-type-eth1=dhcp
ip-address-eth1=172.29.233.1
subnet-mask-eth1=255.255.0.0
eth0-state=enabled
eth0-autoneg=enabled
eth0-speed=100
eth0-duplex=full
X1-dhcpd-state=disabled
X1-dhcpd-range=192.168.1.18_192.168.1.254
X1-dhcpd-lease-time=120
X1-dhcpd-fix-host=
ip-state-eth1=enabled
eth1-state=enabled
eth1-autoneg=enabled
eth1-speed=10
eth1-duplex=half
XXX=123
   )";

  std::string expected_config =
R"( {
"bridge-config" : {
"br0":["X1","X2"],
"br1":[]
},
"ip-config" : {
"br0": {"source": "static","ipaddr": "192.168.1.1","netmask": "255.255.255.0","bcast": "192.168.1.255"},
"br1": {"source": "dhcp","ipaddr": "172.29.233.1","netmask": "255.255.0.0","bcast": "172.29.255.255"}
},
"interface-config": 
[
{"device":"X1","autonegotiation":"on","state":"up","speed":100,"duplex":"full"},
{"device":"X2","autonegotiation":"on","state":"up","speed":10,"duplex":"half"}
]
})";



  ::std::string network_data;
  ::std::string dipswitch_data;
  Error status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ(StringRemoveNewlineAndBlank(expected_config),
            StringRemoveNewlineAndBlank(network_data));
  ASSERT_EQ(ErrorCode::OK, status.GetErrorCode());
}

TEST_F(ALegacyRestore, RestoresABackupOfAPreviousFirmwareInSeperatedMode) {

  mock_file_editor_.content_ =
      R"(
XXX=123
XXX=123
dsa-mode=1
ip-config-type-eth0=static
ip-address-eth0=192.168.1.1
subnet-mask-eth0=255.255.255.0
ip-config-type-eth1=dhcp
ip-address-eth1=172.29.233.1
subnet-mask-eth1=255.255.0.0
eth0-state=enabled
eth0-autoneg=enabled
eth0-speed=100
eth0-duplex=full
X1-dhcpd-state=disabled
X1-dhcpd-range=192.168.1.18_192.168.1.254
X1-dhcpd-lease-time=120
X1-dhcpd-fix-host=
eth1-state=enabled
eth1-autoneg=enabled
eth1-speed=10
eth1-duplex=half
XXX=123
   )";

  std::string expected_config =
R"( {
"bridge-config" : {
"br0":["X1"],"br1":["X2"]
},
"ip-config" : {
"br0": {"source": "static","ipaddr": "192.168.1.1","netmask": "255.255.255.0","bcast": "192.168.1.255"},
"br1": {"source": "dhcp","ipaddr": "172.29.233.1","netmask": "255.255.0.0","bcast": "172.29.255.255"}
},
"interface-config": [{"device":"X1","autonegotiation":"on","state":"up","speed":100,"duplex":"full"},{"device":"X2","autonegotiation":"on","state":"up","speed":10,"duplex":"half"}]
})";


  ::std::string network_data;
  ::std::string dipswitch_data;
  Error status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ(StringRemoveNewlineAndBlank(expected_config),
            StringRemoveNewlineAndBlank(network_data));
  ASSERT_EQ(ErrorCode::OK, status.GetErrorCode());
}

TEST_F(ALegacyRestore, RestoresABackupOfAPreviousFirmwareInvalidDsaTag) {

  mock_file_editor_.content_ =
      R"(
XXX=123
XXX=123
dsa-mode=10
ip-config-type-eth0=static
ip-address-eth0=192.168.1.1
subnet-mask-eth0=255.255.255.0
ip-config-type-eth1=dhcp
ip-address-eth1=172.29.233.1
subnet-mask-eth1=255.255.0.0
XXX=123
   )";


  ::std::string network_data;
  ::std::string dipswitch_data;
  Error status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ(ErrorCode::BACKUP_CONTENT_INVALID, status.GetErrorCode()) << status.ToString();
}


TEST_F(ALegacyRestore, TryToRestoresABackupOfAPreviousFirmwareInSeperatedMissingEth1Settings) {

  mock_file_editor_.content_ =
      R"(
XXX=123
XXX=123
dsa-mode=1
ip-config-type-eth0=static
ip-address-eth0=192.168.1.1
subnet-mask-eth0=255.255.255.0
XXX=123
   )";



  ::std::string network_data;
  ::std::string dipswitch_data;
  Error status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_TRUE(network_data.empty());
  EXPECT_EQ(ErrorCode::BACKUP_CONTENT_MISSING, status.GetErrorCode());
}

}  // namespace netconf

