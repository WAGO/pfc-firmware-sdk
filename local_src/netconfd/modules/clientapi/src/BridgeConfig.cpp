// SPDX-License-Identifier: LGPL-3.0-or-later

#include "BridgeConfig.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm/transform.hpp>
#include "JsonConverter.hpp"
#include "TypesHelper.hpp"
#include "BridgeConfigValidator.hpp"

namespace netconf {
namespace api {

static void RemoveInterface(
    netconf::BridgeConfig &config, const ::std::string &interface_name, const bool remove_bridge_if_empty = false) {
  ::std::string bridge;
  for (auto &cfg_pair : config) {
    auto it = ::std::find(cfg_pair.second.begin(), cfg_pair.second.end(), interface_name);
    if (it != cfg_pair.second.end()) {
      bridge = cfg_pair.first;
      cfg_pair.second.erase(it);
      break;
    }
  }

  if (remove_bridge_if_empty) {
    auto it = config.find(bridge);
    if(it != config.end()){
      if((*it).second.empty()){
        config.erase(it);
      }
    }
  }
}

BridgeConfig::BridgeConfig(const netconf::BridgeConfig &config)
    :
    configs_ { config } {
}

BridgeConfig::~BridgeConfig() = default;

void BridgeConfig::AddBridge(const ::std::string &bridge_name) {
  configs_.insert( { bridge_name, { } });
}

void BridgeConfig::DeleteBridge(const ::std::string &bridge_name) {
  configs_.erase(bridge_name);
}

void BridgeConfig::AssignInterfaceToBridge(
    const ::std::string &interface_name, const ::std::string &bridge_name, const bool remove_bridge_if_empty){
  RemoveInterface(configs_, interface_name, remove_bridge_if_empty);
  AddBridge(bridge_name);

  if (configs_.count(bridge_name) > 0) {
    configs_[bridge_name].push_back(interface_name);
  }

}

void BridgeConfig::DeleteInterface(const ::std::string &interface_name) {
  RemoveInterface(configs_, interface_name);
}

Interfaces BridgeConfig::GetBridgeInterfaces(const ::std::string& bridge_name) const {
  if (configs_.count(bridge_name) > 0) {
    return configs_.at(bridge_name);
  }
  return Interfaces{};
}

Bridge BridgeConfig::GetBridgeOfInterface(const Interface& interface_name) const {
  auto has_interface = [&](auto& entry) {return interface_name == entry;};
  auto entry_it = ::std::find_if(configs_.begin(), configs_.end(), [&](auto& entry){
    return ::std::find_if(entry.second.begin(), entry.second.end(), has_interface) != entry.second.end();});
  if(entry_it != configs_.end())
  {
    return entry_it->first;
  }
  return "";
}

::std::vector<Bridge> BridgeConfig::GetBridges() const {
  ::std::vector<Bridge> bridges;
  ::std::transform(configs_.cbegin(), configs_.cend(), ::std::back_inserter(bridges), [](const auto& entry ) {return entry.first;});
  return bridges;
}

bool BridgeConfig::AreInSameBridge(const ::std::vector<::std::string> &interfaces) const {
  ::std::vector<::std::string> bridges;
  bridges.reserve(4);

  for (auto &itf : interfaces) {
    bridges.push_back(GetBridgeOfInterface(itf));
  }

  if (::std::any_of(bridges.begin(), bridges.end(), [](auto &s) {
    return s.empty();
  })) {
    return false;
  }
  return std::adjacent_find(bridges.begin(), bridges.end(), ::std::not_equal_to<>()) == bridges.end();
}

netconf::BridgeConfig BridgeConfig::GetConfig() const noexcept{
  return configs_;
}

bool BridgeConfig::BridgeIsEmpty(const ::std::string &bridge_name) const {
  return configs_.count(bridge_name) == 0 || configs_.at(bridge_name).empty();
}

::std::string ToJson(const BridgeConfig& config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config.GetConfig());
}

::std::string ToPrettyJson(const BridgeConfig& config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config.GetConfig(), JsonFormat::PRETTY);
}

::std::string ToString(const BridgeConfig& config) noexcept {
  ::std::stringstream ss;
  for(auto& bridge_entry: config.GetConfig()){
    ss << bridge_entry.first << "=";
    ss << boost::algorithm::join(bridge_entry.second, ",") << " ";
  }
  auto brcfg_str = ss.str();
  if(!brcfg_str.empty()){
    brcfg_str.pop_back(); // Remove whithespace at the end
  }
  return brcfg_str;
}

bool operator==(const BridgeConfig &rhs, const BridgeConfig &lhs) {
  return IsEqual(rhs.configs_, lhs.configs_);
}

Status MakeBridgeConfig(const std::string& json_str, BridgeConfig& config)
{
  JsonConverter jc;
  netconf::BridgeConfig c;
  Status status = jc.FromJsonString(json_str, c);
  config = BridgeConfig(c);
  return status;
}


Status ValidateBridgeConfig(const BridgeConfig &config, const InterfaceInformations& interface_information)
{
  netconf::Interfaces available_interfaces;
  using boost::adaptors::filtered;
  boost::transform(interface_information | filtered([](InterfaceInformation const& s) { return s.GetType() == DeviceType::Port; }),
                   std::back_inserter(available_interfaces),
                   [](InterfaceInformation const& i) { return i.GetInterfaceLabel();});

  return BridgeConfigValidator::Validate(config.GetConfig(), available_interfaces);
}

} /* namespace api */
} /* namespace netconf */
