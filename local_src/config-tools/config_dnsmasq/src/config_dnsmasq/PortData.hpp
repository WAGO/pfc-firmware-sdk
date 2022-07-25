//------------------------------------------------------------------------------
/// Copyright (c) 2020-2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Class representation of a port configuration
///
///  \author   MSc : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_PORTDATA_HPP_
#define SRC_CONFIG_DNSMASQ_PORTDATA_HPP_

#include <stdint.h>
#include <string>
#include <BridgeConfig.hpp>
#include <IPConfig.hpp>
#include <InterfaceConfig.hpp>

namespace napi = ::netconf::api;

namespace configdnsmasq {

class PortData {
 public:
  std::string port_name_;
  std::string state_;
  std::string type_;
  std::string ip_addr_;
  std::string netmask_;
  uint32_t ip_addr_bin_;
  uint32_t netmask_bin_;
  uint32_t network_bin_;

  PortData();
  virtual ~PortData() { };

  void setState(const std::string &bridge, const napi::BridgeConfig &bridgeConfig,
                napi::InterfaceConfigs interfaceConfig);
  void setIpAddress(const std::string &address);
  void setNetmask(const std::string &netmask);
  void setType(const netconf::IPConfig &ip_config);
  bool has_valid_ip_config();

 private:
  bool areAllBridgeInterfacesDown(
  const std::string &bridge_name,
  const napi::BridgeConfig& bridge_config,
  napi::InterfaceConfigs& interface_config);
};

} // namespace configdnsmasq
#endif /* SRC_CONFIG_DNSMASQ_PORTDATA_HPP_ */
