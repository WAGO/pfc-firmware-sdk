// SPDX-License-Identifier: GPL-2.0-or-later

#include <nlohmann/json.hpp>
#include "IPConfig.hpp"

extern "C" {

#include <arpa/inet.h>
#include <syslog.h>
#include "IPConfig.h"

static void netconflog(const char* msg) {
  openlog("Netconf", LOG_NOWAIT, LOG_USER);
  syslog(LOG_DEBUG, "%s", msg);
  closelog();
}

static std::uint8_t GetValue(const nlohmann::json& j, const ::std::string& key) {
  auto iter = j.find(key);
  if (iter != j.end()) {
    ::std::uint8_t value;
    iter.value().get_to(value);
    return value;
  }

  return 0;
}

static int JsonInetPton(const nlohmann::json& j, const ::std::string& key, uint32_t& addr) {
  auto iter = j.find(key);
  if (iter != j.end()) {
    ::std::string value;
    iter.value().get_to(value);
    return inet_pton(AF_INET, value.c_str(), &addr);
  }

  return -1;
}

int NetconfGetDipSwitchConfig(uint32_t* ip_address, uint32_t* netmask, uint8_t* switch_value) {
  try {
    auto ip = netconf::GetDipSwitchConfig();
    auto j  = nlohmann::json::parse(ip);

    auto status = JsonInetPton(j, "ipaddr", *ip_address);
    if (status == 1) {
      status = JsonInetPton(j, "netmask", *netmask);
    }
    if (status == 1) {
      *switch_value = GetValue(j, "value");
    }
    return status == 1 ? 0 : -1;
  } catch (::std::runtime_error& e) {
    netconflog(e.what());
    return -2;
  } catch (::std::exception& e) {
    netconflog(e.what());
    return -3;
  }
  return -4;
}

int NetconfSetDipSwitchConfig(uint32_t ip_address, uint32_t netmask) {
  try {
  char ip_buffer[INET_ADDRSTRLEN];
  char netmask_buffer[INET_ADDRSTRLEN];

  auto result = inet_ntop(AF_INET, &ip_address, static_cast<char*>(ip_buffer), INET_ADDRSTRLEN);
    if (result != nullptr) {
    result = inet_ntop(AF_INET, &netmask, static_cast<char*>(netmask_buffer), INET_ADDRSTRLEN);
  }

  if (result != nullptr) {
    nlohmann::json cfg;

      cfg["ipaddr"]  = ip_buffer;
      cfg["netmask"] = netmask_buffer;

      auto status = netconf::SetDipSwitchConfig(cfg.dump());
      if (status != netconf::Status::OK){ netconflog("failed to set dip switch config");}
    return status == netconf::Status::OK ? 0 : -1;
  }
  } catch (::std::runtime_error& e) {
    netconflog(e.what());
    return -2;
  } catch (::std::exception& e) {
    netconflog(e.what());
    return -3;
  }
  return -4;
}
}
