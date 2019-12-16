// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DSAJsonConverter.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "DSAJsonConverter.hpp"
#include "Helper.hpp"
#include "TypesHelper.hpp"
#include <sstream>

namespace netconfd {


Status DSAJsonConverter::JsonToDSA(const std::string& json,
                                   std::uint32_t& dsa_flag) const {

  BridgeConfig bridge_config;
  Status status = json_config_converter_.JsonToBridgeConfig(json, bridge_config);

  if (status.Ok()) {

    if (ConfigContainsBridgeWithSpecificInterfaces(bridge_config, "br0",
                                                   { "X1", "X2" })) {
      //switched mode (DSAtag == 0)
      dsa_flag = 0;

    } else if (ConfigContainsBridgeWithSpecificInterfaces(bridge_config, "br0", { "X1" })
        && ConfigContainsBridgeWithSpecificInterfaces(bridge_config, "br1", { "X2" })) {

      //separate mode (DSAtag == 1)
      dsa_flag = 1;

    } else {
      status.Append(StatusCode::ERROR,
                     "Failed to determine the dsa flag (unsupported switch mode). ");
    }

  }

  return status;
}

static void AddInterfacesToBridgeInterfaces(const Interfaces& interfaces,
                                            Interfaces& bridge_interfaces) {

  for (auto& interface : interfaces) {
    if (IsNotIncluded(interface, bridge_interfaces)) {
      bridge_interfaces.push_back(interface);
    }
  }

}

static void RemoveInterfacesformBridgeInterfaces(const Interfaces& interfaces,
                                                 Interfaces& bridge_interfaces) {

  for (auto& interface : interfaces) {
    if (IsIncluded(interface, bridge_interfaces)) {
      bridge_interfaces.erase(
          std::remove(bridge_interfaces.begin(), bridge_interfaces.end(), interface),
          bridge_interfaces.end());
    }
  }

}

static void ModifyBridgeWithSpecificInterfaces(BridgeConfig& bridge_config,
                                               const Bridge& bridge,
                                               const Interfaces& interfaces) {

  //modify existing bridges
  for (auto& bridge_config_pair : bridge_config) {

    auto& bridge_interfaces = bridge_config_pair.second;

    if (bridge_config_pair.first == bridge) {
      AddInterfacesToBridgeInterfaces(interfaces, bridge_interfaces);
    } else {
      RemoveInterfacesformBridgeInterfaces(interfaces, bridge_interfaces);
    }
  }

  //add if bridge does not exist
  auto it = bridge_config.find(bridge);
  if (it == bridge_config.cend()) {
    bridge_config.insert(BridgePair(bridge, interfaces));
  }

}

std::string DSAJsonConverter::GetActualBrigeConfigFromNetconfd() const{

  const ::std::string command =
      "dbus-send --system --print-reply --reply-timeout=15000 --type=method_call --dest=de.wago.netconfd /de/wago/netconfd/interface_config de.wago.netconfd1.interface_config.get";

  ::std::string return_value;
  Status status = command_executor_.Execute(command, return_value);

  //get the last blank separated string
  ::std::string line;
  ::std::stringstream ss(return_value);
  while (::std::getline(ss, line, ' ')) {
  }

  //remove \n
  line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

  //remove firt and last "
  line.erase(0, 1);
  line.erase(line.size() - 1, 1);

  return line;
}

Status DSAJsonConverter::DSAToJson(const std::uint32_t mode, std::string& json) const {

  ::std::string json_bridge_config = GetActualBrigeConfigFromNetconfd();

  BridgeConfig bridge_config;
  Status status = json_config_converter_.JsonToBridgeConfig(json_bridge_config, bridge_config);

  if (status.Ok()) {

    if (mode == 0) {
      //switched mode (DSAtag == 0)
      ModifyBridgeWithSpecificInterfaces(bridge_config, "br0", { "X1", "X2" });
    } else if (mode == 1) {
      //separate mode (DSAtag == 1)
      ModifyBridgeWithSpecificInterfaces(bridge_config, "br0", { "X1" });
      ModifyBridgeWithSpecificInterfaces(bridge_config, "br1", { "X2" });
    } else {
      status.Append(StatusCode::ERROR,
                     "Unsupported dsa mode (supported: 0 == switched, 1 == separated)");
    }
  }

  if (status.Ok()) {
    status = json_config_converter_.BridgeConfigToJson(bridge_config, json);
  }

  return status;

}

} /* namespace netconfd */
