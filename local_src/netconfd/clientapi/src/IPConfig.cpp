// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPConfig.hpp"
#include "JsonParse.hpp"
#include <nlohmann/json.hpp>

#include "ConfigBase.hpp"
#include "NetconfdDbusClient.hpp"

namespace netconf {

using nlohmann::json;

NLOHMANN_JSON_SERIALIZE_ENUM(IPSource, { {IPSource::UNKNOWN, nullptr}, {IPSource::NONE, "none"}, {IPSource::STATIC,
        "static"}, {IPSource::DHCP, "dhcp"}, {IPSource::BOOTP, "bootp"}, {IPSource::TEMPORARY,
        "temporary"}, {IPSource::EXTERNAL, "external"}});

static json IPConfigsToNJson(const ::std::vector<IPConfig> &ip_configs) {
  json json_out( { });
  for (auto ip_config : ip_configs) {
    json inner = json { { "ipaddr", ip_config.address_ }, { "netmask", ip_config.netmask_ }, { "bcast", ip_config
        .broadcast_ } };
    to_json(inner["source"], ip_config.source_);
    json_out[ip_config.interface_] = inner;
  }
  return json_out;
}

static void NJsonToIPConfigs(const json &json_object, ::std::vector<IPConfig> &ip_configs) {
  auto get_to_if_exists = [](const ::std::string &key, const nlohmann::json &inner_json, auto &to) {
    auto iter = inner_json.find(key);
    if (iter != inner_json.end()) {
      iter.value().get_to(to);
    }
  };

  for (const auto &ip_config : json_object.items()) {
    IPConfig c;
    c.interface_ = ip_config.key();
    auto inner = ip_config.value();

    get_to_if_exists("ipaddr", inner, c.address_);
    get_to_if_exists("netmask", inner, c.netmask_);
    get_to_if_exists("bcast", inner, c.broadcast_);
    get_to_if_exists("source", inner, c.source_);

    ip_configs.push_back(::std::move(c));
  }
}

class IPConfigs::Impl : public ConfigBase<IPConfig> {
 public:
  Impl() = default;
};

IPConfigs::IPConfigs(IPConfigs &&other) noexcept
    : impl_ { ::std::move(other.impl_) } {
  other.impl_ = ::std::make_unique<Impl>();
}

IPConfigs::IPConfigs(const IPConfigs &other) noexcept
    : impl_ { new Impl() } {
  impl_->configs_ = other.impl_->configs_;
}

IPConfigs::IPConfigs()
    : impl_ { new Impl() } {
}

IPConfigs::IPConfigs(const ::std::string &jsonstr)
    : impl_ { new Impl() } {
  auto j = JsonToNJson(jsonstr);
  NJsonToIPConfigs(j, impl_->configs_);
}

IPConfigs::~IPConfigs() = default;

void IPConfigs::AddIPConfig(IPConfig config) {
  impl_->AddConfig(config);
}

void IPConfigs::RemoveIPConfig(const ::std::string &interface_name) {
  impl_->RemoveConfig(interface_name);
}

boost::optional<IPConfig> IPConfigs::GetIPConfig(const ::std::string &interface_name) const {
  return impl_->GetConfig(interface_name);
}

::std::vector<IPConfig> IPConfigs::Get() const {
  return impl_->configs_;
}

::std::string IPConfigs::ToJson() const {
  auto j = IPConfigsToNJson(impl_->configs_);
  return j.dump();
}

IPConfigs GetIPConfigs() {
  NetconfdDbusClient client;
  return IPConfigs { client.GetIpConfigs() };
}

IPConfigs GetCurrentIPConfigs() {
  NetconfdDbusClient client;
  return IPConfigs { client.GetCurrentIpConfigs() };
}

Status SetIPConfigs(const IPConfigs &config) {
  NetconfdDbusClient client;
  ::std::string json_config = config.ToJson();
  if (client.SetIpConfigs(json_config)) {
    return Status::OK;
  }
  return Status::ERROR;
}

void DeleteIPConfig(::std::string interface_name) {
  IPConfigs ip_configs;
  IPConfig ip_config { ::std::move(interface_name), IPSource::NONE, "", "", "" };
  ip_configs.AddIPConfig(ip_config);
  SetIPConfigs(ip_configs);
}

void SetTempFixIp() {
  NetconfdDbusClient client;
  client.SetTemporaryFixedIpAddress();
}

::std::string GetDipSwitchConfig() {
  NetconfdDbusClient client;
  return client.GetDipSwitchConfig();
}

Status SetDipSwitchConfig(const ::std::string &config) {
  NetconfdDbusClient client;
  auto success = client.SetDipSwitchConfig(config);
  return success ? Status::OK : Status::ERROR;
}

} /* namespace netconf */
