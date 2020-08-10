// SPDX-License-Identifier: GPL-2.0-or-later

#include <nlohmann/json.hpp>
#include "DipSwitchConfig.hpp"

extern "C" {

#include <arpa/inet.h>
#include <syslog.h>
#include "IPConfig.h"

static void netconflog(const char* msg) {
  openlog("Netconf", LOG_NOWAIT, LOG_USER);
  syslog(LOG_DEBUG, "%s", msg);
  closelog();
}

int NetconfGetDipSwitchConfig(uint32_t* ip_address, uint32_t* netmask, uint8_t* switch_value) {
  try {

    auto dip_switch_config = netconf::api::GetDipSwitchConfig();
    auto ip_config = dip_switch_config.GetDipSwitchIpConfig();

    auto status = inet_pton(AF_INET, ip_config.address_.c_str(), &ip_address);
    if (status == 1) {
      status = inet_pton(AF_INET, ip_config.netmask_.c_str(), &netmask);
    }
    if (status == 1) {
      *switch_value = static_cast<uint8_t>(dip_switch_config.GetDipSwitchValue());
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
      auto ipaddr_ = ::std::string{static_cast<const char*>(ip_buffer)};
      auto netmask_ = ::std::string{static_cast<const char*>(netmask_buffer)};
      netconf::DipSwitchConfig config {netconf::DipSwitchIpConfig{ipaddr_, netmask_}};

      auto status = netconf::api::SetDipSwitchConfig(netconf::api::DipSwitchConfig{config});
      if (status != netconf::api::Status::OK) {netconflog("failed to set dip switch config");}
      return status == netconf::api::Status::OK ? 0 : -1;
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
