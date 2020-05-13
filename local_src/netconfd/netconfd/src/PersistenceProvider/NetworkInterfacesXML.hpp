// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include "Types.hpp"
#include "IFileEditor.hpp"

namespace netconfd {

#define NETWORKINTERFACESPATH "/etc/specific/network-interfaces.xml"

struct BaseInterface {
  ::std::string device_name;
  ::std::string state;
  ::std::string no_dsa_disable;
};

struct eth0Interface {
  BaseInterface base_if;
  ::std::string promisc;
};

struct PortInterface {
  BaseInterface base_if;
  ::std::string port_name;
  ::std::string autoneg;
  ::std::string speed;
  ::std::string duplex;
};

struct BridgeInterface {
  BaseInterface base_if;
  ::std::string dsa_slave;
  ::std::string no_dsa_slave;
  ::std::string show_in_wbm;
  ::std::string port_name;
  ::std::string type;
  ::std::string static_ipaddr;
  ::std::string static_netmask;
  ::std::string static_netmask_long;
  ::std::string static_broadcast;
};

enum DeviceInterface : uint {
  ETH0,
  ETHX11,
  ETHX12,
  BR0,
  BR1
};

class State {
 public:
  enum Value : uint {
    ENABLED,
    DISABLED
  };

  State(Value value) {
    value_ = value;
  }

  ::std::string ToString() {
    switch (value_) {
      case ENABLED:
        return "enabled";
      case DISABLED:
        return "disabled";
      default:
        return "";
    }
  }

 private:
  Value value_;
};

Status WriteNetworkInterfacesXML(IFileEditor &file_editor_, const BridgeConfig& bridge_config, const IPConfigs& ip_configs,
                                 const InterfaceConfigs& port_configs);

} /* namespace netconfd */
