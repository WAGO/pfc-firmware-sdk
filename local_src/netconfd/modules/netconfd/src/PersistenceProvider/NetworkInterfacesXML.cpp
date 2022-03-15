// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetworkInterfacesXML.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>
#include <fstream>
#include <string>

#include "CollectionUtils.hpp"
#include "Logger.hpp"
#include "NetworkHelper.hpp"
#include "TypeUtils.hpp"
#include "Types.hpp"
#include "TypesHelper.hpp"

class EthernetSettingsXml {
 private:
  friend class ::boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;
    ar& BOOST_SERIALIZATION_NVP(port_name);
    ar& BOOST_SERIALIZATION_NVP(autoneg);
    ar& BOOST_SERIALIZATION_NVP(speed);
    ar& BOOST_SERIALIZATION_NVP(duplex);
    ar& BOOST_SERIALIZATION_NVP(mac);
  }
  ::std::string port_name;
  ::std::string autoneg;
  ::std::string speed;
  ::std::string duplex;
  ::std::string mac;

 public:
  EthernetSettingsXml() = default;

  explicit EthernetSettingsXml(netconf::InterfaceConfig port_config)
      : port_name{port_config.device_name_},
        autoneg{port_config.autoneg_ == netconf::Autonegotiation::ON ? "enabled" : "disabled"},
        speed{::std::to_string(port_config.speed_).append("M")},
        duplex{netconf::DuplexToString(port_config.duplex_)},
        mac{""} {
  }
};

class IpSettingsXml {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;
    ar& BOOST_SERIALIZATION_NVP(show_in_wbm);
    ar& BOOST_SERIALIZATION_NVP(port_name);
    ar& BOOST_SERIALIZATION_NVP(type);
    ar& BOOST_SERIALIZATION_NVP(static_ipaddr);
    ar& BOOST_SERIALIZATION_NVP(static_netmask);
    ar& BOOST_SERIALIZATION_NVP(static_netmask_long);
    ar& BOOST_SERIALIZATION_NVP(static_broadcast);
  }
  ::std::string show_in_wbm;
  ::std::string port_name;
  ::std::string type;
  ::std::string static_ipaddr;
  ::std::string static_netmask;
  ::std::string static_netmask_long;
  ::std::string static_broadcast;

 public:
  IpSettingsXml() = default;

  IpSettingsXml(netconf::BridgeConfig bridge_config, netconf::IPConfig ip_config)
      : type{netconf::IPSourceToString(ip_config.source_)},
        static_ipaddr{ip_config.address_},
        static_netmask{netconf::GetPrefix(ip_config.netmask_)},
        static_netmask_long{ip_config.netmask_},
        static_broadcast{netconf::GetBroadcast(ip_config.address_, ip_config.netmask_)} {
    show_in_wbm = "0";

    if (bridge_config.count(ip_config.interface_) > 0) {
      if ("br0" == ip_config.interface_) {
        show_in_wbm = "1";
      } else if (("br1" == ip_config.interface_) && IsIncluded("X2", bridge_config.at("br1"))) {
        show_in_wbm = "1";
      }
    }

    if (ip_config.interface_ == "br0") {
      port_name = "X1";
    }
    if (ip_config.interface_ == "br1") {
      port_name = "X2";
    }
  }
};

class IFaceXml {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;
    ar& BOOST_SERIALIZATION_NVP(device_name);
    ar& BOOST_SERIALIZATION_NVP(state);
    ar& BOOST_SERIALIZATION_NVP(no_dsa_disable);
    ar& BOOST_SERIALIZATION_NVP(promisc);
  }
  ::std::string device_name;
  ::std::string state;
  ::std::string no_dsa_disable;
  ::std::string promisc;

 public:
  IFaceXml() : device_name{"eth0"}, state{"enabled"}, no_dsa_disable{"no"}, promisc{"off"} {
  }
};

class IFaceEthernetXml {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;
    ar& BOOST_SERIALIZATION_NVP(device_name);
    ar& BOOST_SERIALIZATION_NVP(state);
    ar& BOOST_SERIALIZATION_NVP(no_dsa_disable);
    ar& BOOST_SERIALIZATION_NVP(ethernet_settings);
  }
  ::std::string device_name;
  ::std::string state;
  ::std::string no_dsa_disable;
  EthernetSettingsXml ethernet_settings;

 public:
  IFaceEthernetXml() = default;

  explicit IFaceEthernetXml(netconf::InterfaceConfig port_config)
      : device_name{"eth" + port_config.device_name_}, no_dsa_disable{"no"}, ethernet_settings{port_config} {
    if (port_config.state_ == netconf::InterfaceState::UP) {
      state = "enabled";
    } else if (port_config.state_ == netconf::InterfaceState::DOWN) {
      state = "disabled";
    } else {
      state = "unknown";
    }
  }
};

class BridgeXml {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;
    ar& BOOST_SERIALIZATION_NVP(dsa_slave);
    ar& BOOST_SERIALIZATION_NVP(no_dsa_slave);
  }
  ::std::string dsa_slave;
  ::std::string no_dsa_slave;

 public:
  BridgeXml() = default;

  explicit BridgeXml(netconf::InterfaceConfig port_config) : dsa_slave{"eth" + port_config.device_name_} {
    if (port_config.device_name_ == "X1") {
      no_dsa_slave = "eth0";
    }
    if (port_config.device_name_ == "X2") {
      no_dsa_slave = "";
    }
  }
};

class IFaceIpXml {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;
    ar& BOOST_SERIALIZATION_NVP(device_name);
    ar& BOOST_SERIALIZATION_NVP(state);
    ar& BOOST_SERIALIZATION_NVP(no_dsa_disable);
    ar& BOOST_SERIALIZATION_NVP(bridge);
    ar& BOOST_SERIALIZATION_NVP(ip_settings);
  }
  ::std::string device_name;
  ::std::string state;
  ::std::string no_dsa_disable;
  BridgeXml bridge;
  IpSettingsXml ip_settings;

 public:
  IFaceIpXml() = default;
  IFaceIpXml(const netconf::BridgeConfig& bridge_config, const netconf::IPConfig& ip_config,
             const netconf::InterfaceConfig& port_config)
      : device_name{ip_config.interface_}, bridge{port_config}, ip_settings{bridge_config, ip_config} {
    if (port_config.state_ == netconf::InterfaceState::UP) {
      state = "enabled";
    } else if (port_config.state_ == netconf::InterfaceState::DOWN) {
      state = "disabled";
    } else {
      state = "unknown";
    }

    if (ip_config.interface_ == "br0") {
      no_dsa_disable = "no";
    }
    if (ip_config.interface_ == "br1") {
      no_dsa_disable = "yes";
    }
  }
};

class InterfacesXml {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;
    ar& BOOST_SERIALIZATION_NVP(dsa_mode);
    {
      auto iface = iface_eth0;
      ar& BOOST_SERIALIZATION_NVP(iface);
    }
    {
      auto iface = iface_ethernet_X1;
      ar& BOOST_SERIALIZATION_NVP(iface);
    }
    {
      auto iface = iface_ethernet_X2;
      ar& BOOST_SERIALIZATION_NVP(iface);
    }
    {
      auto iface = iface_ip_X1;
      ar& BOOST_SERIALIZATION_NVP(iface);
    }
    {
      auto iface = iface_ip_X2;
      ar& BOOST_SERIALIZATION_NVP(iface);
    }
  }

  bool IsSwitched(const netconf::BridgeConfig& bridgeConfig) {
    if ((bridgeConfig.count("br1") == 0) || (bridgeConfig.count("br0") == 0)) {
      return true;
    }
    auto interfaces_br0 = bridgeConfig.at("br0");
    auto interfaces_br1 = bridgeConfig.at("br1");
    return IsIncluded("X1", interfaces_br0) && IsIncluded("X2", interfaces_br0);
  }

  int dsa_mode;
  IFaceXml iface_eth0;
  IFaceEthernetXml iface_ethernet_X1;
  IFaceEthernetXml iface_ethernet_X2;
  IFaceIpXml iface_ip_X1;
  IFaceIpXml iface_ip_X2;

 public:
  InterfacesXml(const netconf::BridgeConfig& bridge_config, const netconf::IPConfigs& ip_configs,
                const netconf::InterfaceConfigs& port_configs) {
    dsa_mode = IsSwitched(bridge_config) ? 0 : 1;

    auto ip_config_br0 = ::std::find_if(ip_configs.begin(), ip_configs.end(),
                                        [](const netconf::IPConfig& c) { return c.interface_ == "br0"; });
    auto br0_ip        = (ip_config_br0 == ip_configs.end()) ? netconf::IPConfig::CreateDefault("br0") : *ip_config_br0;

    auto ip_config_br1 =
        ::std::find_if(ip_configs.begin(), ip_configs.end(), [](const auto& c) { return c.interface_ == "br1"; });
    auto br1_ip = (ip_config_br1 == ip_configs.end()) ? netconf::IPConfig::CreateDefault("br1") : *ip_config_br1;

    auto port_config_X1 =
        ::std::find_if(port_configs.begin(), port_configs.end(), [](const auto& c) { return c.device_name_ == "X1"; });
    auto x1 = port_config_X1 == port_configs.end() ? netconf::InterfaceConfig::DefaultConfig("X1") : *port_config_X1;

    auto port_config_X2 =
        ::std::find_if(port_configs.begin(), port_configs.end(), [](const auto& c) { return c.device_name_ == "X2"; });
    auto x2 = port_config_X2 == port_configs.end() ? netconf::InterfaceConfig::DefaultConfig("X2") : *port_config_X2;

    x1.FillUpDefaults();
    x2.FillUpDefaults();

    iface_ethernet_X1 = IFaceEthernetXml(x1);
    iface_ethernet_X2 = IFaceEthernetXml(x2);
    iface_ip_X1       = IFaceIpXml(bridge_config, br0_ip, x1);
    iface_ip_X2       = IFaceIpXml(bridge_config, br1_ip, x2);
  }
};

BOOST_CLASS_IMPLEMENTATION(InterfacesXml, object_serializable);
BOOST_CLASS_IMPLEMENTATION(EthernetSettingsXml, object_serializable);
BOOST_CLASS_IMPLEMENTATION(IpSettingsXml, object_serializable);
BOOST_CLASS_IMPLEMENTATION(IFaceXml, object_serializable);
BOOST_CLASS_IMPLEMENTATION(IFaceIpXml, object_serializable);
BOOST_CLASS_IMPLEMENTATION(IFaceEthernetXml, object_serializable);
BOOST_CLASS_IMPLEMENTATION(BridgeXml, object_serializable);

namespace netconf {

Status WriteNetworkInterfacesXML(IFileEditor& file_editor_, const BridgeConfig& bridge_config,
                                 const IPConfigs& ip_configs, const InterfaceConfigs& port_configs) {
  auto interfaces = InterfacesXml{bridge_config, ip_configs, port_configs};

  auto xml_content = std::ostringstream(std::ios::binary);

  xml_content << R"(<?xml version="1.0" encoding="UTF-8"?>)"
              << "\n";
  xml_content << R"(<!-- This file is generated. Do not edit manually! Content will be overwritten. -->)"
              << "\n";

  unsigned int flags = boost::archive::no_header;
  boost::archive::xml_oarchive oa(xml_content, flags);
  oa << BOOST_SERIALIZATION_NVP(interfaces);

  return file_editor_.WriteAndReplace(NETWORKINTERFACESPATH, xml_content.str());
}

} /* namespace netconf */
