// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceConfig.hpp"

#include <nlohmann/json.hpp>
#include <vector>

#include "ConfigBase.hpp"
#include "NetconfdDbusClient.hpp"
#include "EthernetInterface.hpp"

namespace netconf {

using nlohmann::json;

NLOHMANN_JSON_SERIALIZE_ENUM(Duplex, {{Duplex::UNKNOWN, nullptr}, {Duplex::HALF, "half"}, {Duplex::FULL, "full"}})

NLOHMANN_JSON_SERIALIZE_ENUM(Autonegotiation, { {Autonegotiation::UNKNOWN, nullptr}, {Autonegotiation::OFF, "off"}, {Autonegotiation::ON, "on"}})

NLOHMANN_JSON_SERIALIZE_ENUM(InterfaceState, { {InterfaceState::UNKNOWN, nullptr}, {InterfaceState::DOWN, "down"}, {InterfaceState::UP, "up"}})

constexpr auto PORT_CFG_KEY_DEVICE = "device";
constexpr auto PORT_CFG_KEY_STATE = "state";
constexpr auto PORT_CFG_KEY_AUTONEG = "autonegotiation";
constexpr auto PORT_CFG_KEY_SPEED = "speed";
constexpr auto PORT_CFG_KEY_DUPLEX = "duplex";

class JsonKeyList {
 public:
  JsonKeyList(std::initializer_list<std::string> args)
      : keys { args } {
  }

  bool containsKey(const std::string &value) const {
    return std::find(keys.begin(), keys.end(), value) != keys.end();
  }

  bool matchesJsonObject(const nlohmann::json &tree) const {
    for (const auto &v : tree.items()) {
      if (!containsKey(v.key())) {
        return false;
      }
    }
    return true;
  }

 private:
  const std::vector<std::string> keys;
};

static json PortConfigToNJson(const InterfaceConfig &interface_config) {
  json j;

  j[PORT_CFG_KEY_DEVICE] = interface_config.interface_;

  if (interface_config.state_ != InterfaceState::UNKNOWN) {
    j[PORT_CFG_KEY_STATE] = interface_config.state_;
  }

  if (interface_config.autoneg_ != Autonegotiation::UNKNOWN) {
    j[PORT_CFG_KEY_AUTONEG] = interface_config.autoneg_;
  }

  if (interface_config.speed_ > 0) {
    j[PORT_CFG_KEY_SPEED] = interface_config.speed_;
  }

  if (interface_config.duplex_ != Duplex::UNKNOWN) {
    j[PORT_CFG_KEY_DUPLEX] = interface_config.duplex_;
  }
  return j;
}

static InterfaceConfig PortConfigFromJson(const json &config) {
  InterfaceConfig p { config.at(PORT_CFG_KEY_DEVICE).get<::std::string>() };

  auto autoneg_opt = config.find(PORT_CFG_KEY_AUTONEG) != config.end();
  if (autoneg_opt) {
    p.autoneg_ = config.at(PORT_CFG_KEY_AUTONEG).get<Autonegotiation>();
    if(p.autoneg_ == Autonegotiation::UNKNOWN){
      throw ::std::runtime_error("Failed to parse json, invalid autonegotiation parameter.");
    }
  }

  auto duplex_opt = config.find(PORT_CFG_KEY_DUPLEX) != config.end();
  if (duplex_opt) {
    p.duplex_ = config.at(PORT_CFG_KEY_DUPLEX).get<Duplex>();
    if(p.duplex_ == Duplex::UNKNOWN){
      throw ::std::runtime_error("Failed to parse json, invalid duplex parameter.");
    }
  }

  auto state_opt = config.find(PORT_CFG_KEY_STATE) != config.end();
  if (state_opt) {
    p.state_ = config.at(PORT_CFG_KEY_STATE).get<InterfaceState>();
    if(p.state_ == InterfaceState::UNKNOWN){
      throw ::std::runtime_error("Failed to parse json, invalid state parameter.");
    }
  }

  auto speed_opt = config.find(PORT_CFG_KEY_SPEED) != config.end();
  if (speed_opt) {
    p.speed_ = config.at(PORT_CFG_KEY_SPEED).get<int>();
  }
  return p;
}

static json InterfaceConfigsToNJson(const ::std::vector<InterfaceConfig> &configs) {
  if (configs.size() > 1) {
    nlohmann::json jarray { };
    for (const auto &config : configs) {
      jarray.push_back(PortConfigToNJson(config));
    }
    return jarray;
  }

  return configs.size() == 1 ? PortConfigToNJson(configs.at(0)) : nlohmann::json( { });
}

static Status NJsonToInterfaceConfigs(const nlohmann::json &j, ::std::vector<InterfaceConfig> &configs) {
  JsonKeyList interface_config_keys { PORT_CFG_KEY_DEVICE, PORT_CFG_KEY_STATE, PORT_CFG_KEY_AUTONEG, PORT_CFG_KEY_SPEED,
      PORT_CFG_KEY_DUPLEX };

  if (j.empty()) {
    return Status::JSON_CONVERT_ERROR;
  }
  if (j.is_array()) {
    for (auto &jentry : j) {
      if (!interface_config_keys.matchesJsonObject(jentry)) {
        return Status::JSON_CONVERT_ERROR;
      }
      configs.push_back(PortConfigFromJson(jentry));
    }
  } else {
    if (!interface_config_keys.matchesJsonObject(j)) {
      return Status::JSON_CONVERT_ERROR;
    }
    configs.push_back(PortConfigFromJson(j));
  }
  return Status::OK;
}

MacAddress GetMacAddress(const ::std::string &interface_name) {
  EthernetInterface interface { interface_name };
  return interface.GetMac();
}

class InterfaceConfigs::Impl : public ConfigBase<InterfaceConfig> {
 public:
  Impl() = default;
 private:
};

InterfaceConfigs::InterfaceConfigs()
    : impl_ { new Impl() } {
}

InterfaceConfigs::InterfaceConfigs(const ::std::string &json_str)
    : impl_ { new Impl() } {
  auto j = json::parse(json_str);
  NJsonToInterfaceConfigs(j, impl_->configs_);
}

InterfaceConfigs::InterfaceConfigs(const InterfaceConfigs &other) noexcept
    : impl_ { new Impl() } {
  impl_->configs_ = other.impl_->configs_;
}

InterfaceConfigs::InterfaceConfigs(InterfaceConfigs &&other) noexcept
    : impl_ { ::std::move(other.impl_) } {
  other.impl_ = ::std::make_unique<Impl>();
}

InterfaceConfigs::~InterfaceConfigs() = default;

InterfaceConfigs& InterfaceConfigs::operator=(const InterfaceConfigs &assign) {
  impl_->configs_ = assign.impl_->configs_;
  return *this;
}

void InterfaceConfigs::AddInterfaceConfig(InterfaceConfig config) {
  impl_->AddConfig(config);
}

void InterfaceConfigs::RemoveInterfaceConfig(const ::std::string &interface_name) {
  impl_->RemoveConfig(interface_name);
}

boost::optional<InterfaceConfig> InterfaceConfigs::GetInterfaceConfig(const ::std::string &interface_name) {
  return impl_->GetConfig(interface_name);
}

::std::string InterfaceConfigs::ToJson() const {
  auto j = InterfaceConfigsToNJson(impl_->configs_);
  return j.dump();
}

::std::string InterfaceConfigs::ToString() const {
  ::std::string ret;
  for (auto &config : impl_->configs_) {
    ret.append("device=" + config.interface_ + " ");

    if (config.state_ != InterfaceState::UNKNOWN) {
      ::std::string value = config.state_ == InterfaceState::UP ? "up" : "down";
      ret.append("state=" + value + " ");
    }
    if (config.autoneg_ != Autonegotiation::UNKNOWN) {
      ::std::string value = config.autoneg_ == Autonegotiation::ON ? "on" : "off";
      ret.append("autonegotiation=" + value + " ");
    }
    if (config.duplex_ != Duplex::UNKNOWN) {
      ::std::string value = (config.duplex_ == Duplex::FULL) ? "full" : "half";  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
      ret.append("duplex=" + value + " ");
    }
    if (config.speed_ != InterfaceConfig::UNKNOWN_SPEED) {
      ret.append("speed=" + std::to_string(config.speed_) + " ");
    }

  }

  return ret;
}

class InterfaceInfo::Impl {
 public:
  explicit Impl(::std::string json_str)
      : json_str_ { ::std::move(json_str) } {
  }
  ::std::string json_str_;
};

InterfaceInfo::InterfaceInfo()
    : impl_ { new Impl("") } {
}

InterfaceInfo::InterfaceInfo(const InterfaceInfo &other) noexcept
    : impl_ { new Impl(other.impl_->json_str_) } {
}

InterfaceInfo::InterfaceInfo(InterfaceInfo &&other) noexcept
    : impl_ { ::std::move(other.impl_) } {
  other.impl_ = ::std::make_unique < Impl > ("");
}

InterfaceInfo::~InterfaceInfo() = default;

InterfaceInfo::InterfaceInfo(const ::std::string &json_str)
    : impl_ { new Impl(json_str) } {
}

::std::string InterfaceInfo::ToJson() const {
  return impl_->json_str_;
}

InterfaceConfigs GetInterfaceConfigs() {
  NetconfdDbusClient client;
  return InterfaceConfigs { client.GetInterfaceConfigs() };
}

Status SetInterfaceConfigs(const InterfaceConfigs &config) {
  NetconfdDbusClient client;
  ::std::string json_config = config.ToJson();
  if (client.SetInterfaceConfigs(json_config)) {
    return Status::OK;
  }
  return Status::ERROR;
}

InterfaceInfo GetInterfaceInfo() {
  NetconfdDbusClient client;
  return InterfaceInfo { client.GetDeviceInterfaces() };
}

}  // namespace netconf
