// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     AnNetworkInterfacesXML
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <NetworkInterfacesXML.hpp>
#include "MockIFileEditor.hpp"
#include <memory>
#include <string>

using namespace testing;

namespace netconf {

class ANetworkInterfacesXML : public Test {
 public:

  MockIFileEditor mock_file_editor_;

  ANetworkInterfacesXML() {

  }

  void SetUp() override {
  }

  void TearDown() override {

  }
  ::std::string NetworkInterfacesXMLSeparatedContent =
      R"(
<?xml version="1.0" encoding="UTF-8"?>
<!--This file is generated. Do not edit manually! Content will be overwritten. -->
<interfaces>
  <dsa_mode>1</dsa_mode>
  <iface>
    <device_name>eth0</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <promisc>off</promisc>
  </iface>
  <iface>
    <device_name>ethX1</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <ethernet_settings>
      <port_name>X1</port_name>
      <autoneg>enabled</autoneg>
      <speed>100M</speed>
      <duplex>full</duplex>
      <mac></mac>
    </ethernet_settings>
  </iface>
  <iface>
    <device_name>ethX2</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <ethernet_settings>
      <port_name>X2</port_name>
      <autoneg>enabled</autoneg>
      <speed>100M</speed>
      <duplex>full</duplex>
      <mac></mac>
    </ethernet_settings>
  </iface>
  <iface>
    <device_name>br0</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <bridge>
      <dsa_slave>ethX1</dsa_slave>
      <no_dsa_slave>eth0</no_dsa_slave>
    </bridge>
    <ip_settings>
      <show_in_wbm>1</show_in_wbm>
      <port_name>X1</port_name>
      <type>static</type>
      <static_ipaddr>192.168.10.10</static_ipaddr>
      <static_netmask>28</static_netmask>
      <static_netmask_long>255.255.255.240</static_netmask_long>
      <static_broadcast>192.168.10.15</static_broadcast>
    </ip_settings>
  </iface>
  <iface>
    <device_name>br1</device_name>
    <state>enabled</state>
    <no_dsa_disable>yes</no_dsa_disable>
    <bridge>
      <dsa_slave>ethX2</dsa_slave>
      <no_dsa_slave></no_dsa_slave>
    </bridge>
    <ip_settings>
      <show_in_wbm>1</show_in_wbm>
      <port_name>X2</port_name>
      <type>static</type>
      <static_ipaddr>192.168.20.20</static_ipaddr>
      <static_netmask>16</static_netmask>
      <static_netmask_long>255.255.0.0</static_netmask_long>
      <static_broadcast>192.168.255.255</static_broadcast>
    </ip_settings>
  </iface>
</interfaces>
)";

  ::std::string NetworkInterfacesXMLSwitchedContent =
      R"(
<?xml version="1.0" encoding="UTF-8"?>
<!--This file is generated. Do not edit manually! Content will be overwritten.-->
<interfaces>
  <dsa_mode>0</dsa_mode>
  <iface>
    <device_name>eth0</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <promisc>off</promisc>
  </iface>
  <iface>
    <device_name>ethX1</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <ethernet_settings>
      <port_name>X1</port_name>
      <autoneg>enabled</autoneg>
      <speed>100M</speed>
      <duplex>full</duplex>
      <mac></mac>
    </ethernet_settings>
  </iface>
  <iface>
    <device_name>ethX2</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <ethernet_settings>
      <port_name>X2</port_name>
      <autoneg>enabled</autoneg>
      <speed>100M</speed>
      <duplex>full</duplex>
      <mac></mac>
    </ethernet_settings>
  </iface>
  <iface>
    <device_name>br0</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <bridge>
      <dsa_slave>ethX1</dsa_slave>
      <no_dsa_slave>eth0</no_dsa_slave>
    </bridge>
    <ip_settings>
      <show_in_wbm>1</show_in_wbm>
      <port_name>X1</port_name>
      <type>static</type>
      <static_ipaddr>192.168.10.10</static_ipaddr>
      <static_netmask>24</static_netmask>
      <static_netmask_long>255.255.255.0</static_netmask_long>
      <static_broadcast>192.168.10.255</static_broadcast>
    </ip_settings>
  </iface>
  <iface>
    <device_name>br1</device_name>
    <state>enabled</state>
    <no_dsa_disable>yes</no_dsa_disable>
    <bridge>
      <dsa_slave>ethX2</dsa_slave>
      <no_dsa_slave></no_dsa_slave>
    </bridge>
    <ip_settings>
      <show_in_wbm>0</show_in_wbm>
      <port_name>X2</port_name>
      <type>static</type>
      <static_ipaddr>192.168.20.20</static_ipaddr>
      <static_netmask>16</static_netmask>
      <static_netmask_long>255.255.0.0</static_netmask_long>
      <static_broadcast>192.168.255.255</static_broadcast>
   </ip_settings>
 </iface>
</interfaces>
)";

  ::std::string NetworkInterfacesXMLStartup =
      R"(
<?xml version="1.0" encoding="UTF-8"?>
<!--This file is generated. Do not edit manually! Content will be overwritten.-->
<interfaces>
  <dsa_mode>0</dsa_mode>
  <iface>
    <device_name>eth0</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <promisc>off</promisc>
  </iface>
  <iface>
    <device_name>ethX1</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <ethernet_settings>
      <port_name>X1</port_name>
      <autoneg>enabled</autoneg>
      <speed>100M</speed>
      <duplex>full</duplex>
      <mac></mac>
    </ethernet_settings>
  </iface>
  <iface>
    <device_name>ethX2</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <ethernet_settings>
      <port_name>X2</port_name>
      <autoneg>enabled</autoneg>
      <speed>100M</speed>
      <duplex>full</duplex>
      <mac></mac>
    </ethernet_settings>
  </iface>
  <iface>
    <device_name>br0</device_name>
    <state>enabled</state>
    <no_dsa_disable>no</no_dsa_disable>
    <bridge>
      <dsa_slave>ethX1</dsa_slave>
      <no_dsa_slave>eth0</no_dsa_slave>
    </bridge>
    <ip_settings>
      <show_in_wbm>1</show_in_wbm>
      <port_name>X1</port_name>
      <type>dhcp</type>
      <static_ipaddr>0.0.0.0</static_ipaddr>
      <static_netmask>0</static_netmask>
      <static_netmask_long>0.0.0.0</static_netmask_long>
      <static_broadcast>0.0.0.0</static_broadcast>
    </ip_settings>
  </iface>
  <iface>
    <device_name>br1</device_name>
    <state>enabled</state>
    <no_dsa_disable>yes</no_dsa_disable>
    <bridge>
      <dsa_slave>ethX2</dsa_slave>
      <no_dsa_slave></no_dsa_slave>
    </bridge>
    <ip_settings>
      <show_in_wbm>0</show_in_wbm>
      <port_name>X2</port_name>
      <type>static</type>
      <static_ipaddr>0.0.0.0</static_ipaddr>
      <static_netmask>0</static_netmask>
      <static_netmask_long>0.0.0.0</static_netmask_long>
      <static_broadcast>0.0.0.0</static_broadcast>
   </ip_settings>
 </iface>
</interfaces>
)";

};

static void ExpectStringEqIgnoreNewlineAndBlank(::std::string expected, ::std::string actual) {

  expected.erase(std::remove(expected.begin(), expected.end(), '\n'), expected.end());
  expected.erase(std::remove(expected.begin(), expected.end(), ' '), expected.end());
  expected.erase(std::remove(expected.begin(), expected.end(), '\t'), expected.end());

  actual.erase(std::remove(actual.begin(), actual.end(), '\n'), actual.end());
  actual.erase(std::remove(actual.begin(), actual.end(), ' '), actual.end());
  actual.erase(std::remove(actual.begin(), actual.end(), '\t'), actual.end());

  EXPECT_EQ(expected, actual);
}

struct StrArg {
  Error Write(const ::std::string& file_path, const ::std::string& data) {
    file_path_ = file_path;
    data_ = data;
    return Error();
  }

  ::std::string file_path_;
  ::std::string data_;
};

TEST_F(ANetworkInterfacesXML, PersistASeperatedBridgeConfig) {

  StrArg actual_content;

  EXPECT_CALL(mock_file_editor_, Write(_,_)).WillOnce(Invoke(&actual_content, &StrArg::Write));

  BridgeConfig bridge_config { { "br0", { "X1" } }, { "br1", { "X2" } } };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.10.10", "255.255.255.240" }, { "br1",
      IPSource::STATIC, "192.168.20.20", "255.255.0.0" } };
  InterfaceConfigs port_configs = { InterfaceConfig("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL), InterfaceConfig(
      "X2", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL) };

  WriteNetworkInterfacesXML(mock_file_editor_, bridge_config, ip_configs, port_configs);

  ExpectStringEqIgnoreNewlineAndBlank(NetworkInterfacesXMLSeparatedContent, actual_content.data_);
}

TEST_F(ANetworkInterfacesXML, PersistASwitchedBridgeConfig) {

  StrArg actual_content;

  EXPECT_CALL(mock_file_editor_, Write(_,_)).WillOnce(Invoke(&actual_content, &StrArg::Write));

  BridgeConfig bridge_config { { "br0", { "X1", "X2" } }, { "br1", { } } };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.10.10", "255.255.255.0" }, { "br1",
      IPSource::STATIC, "192.168.20.20", "255.255.0.0" } };
  InterfaceConfigs port_configs = { InterfaceConfig("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL), InterfaceConfig(
      "X2", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL) };
  WriteNetworkInterfacesXML(mock_file_editor_, bridge_config, ip_configs, port_configs);

  ExpectStringEqIgnoreNewlineAndBlank(NetworkInterfacesXMLSwitchedContent, actual_content.data_);
}

TEST_F(ANetworkInterfacesXML, PersistsAStartupConfig) {

  StrArg actual_content;

  EXPECT_CALL(mock_file_editor_, Write(_,_)).WillOnce(Invoke(&actual_content, &StrArg::Write));

  BridgeConfig bridge_config { { "br0", { "X1", "X2" } }};
  IPConfigs ip_configs = { { "br0", IPSource::DHCP, ZeroIP, ZeroIP } };
  InterfaceConfigs port_configs = { InterfaceConfig("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL), InterfaceConfig(
      "X2", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL) };
  auto write_result = WriteNetworkInterfacesXML(mock_file_editor_, bridge_config, ip_configs, port_configs);
  EXPECT_EQ(Error::Ok(), write_result);
  ExpectStringEqIgnoreNewlineAndBlank(NetworkInterfacesXMLStartup, actual_content.data_);
}

TEST_F(ANetworkInterfacesXML, PersistsAnZeroIPAndBridgeConfig) {

  StrArg actual_content;

  EXPECT_CALL(mock_file_editor_, Write(_,_)).WillOnce(Invoke(&actual_content, &StrArg::Write));

  BridgeConfig bridge_config {};
  IPConfigs ip_configs = {};
  InterfaceConfigs port_configs = { InterfaceConfig("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL), InterfaceConfig(
      "X2", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL) };
  auto write_result = WriteNetworkInterfacesXML(mock_file_editor_, bridge_config, ip_configs, port_configs);
  EXPECT_EQ(Error{}, write_result);
}

} /* namespace netconf */
