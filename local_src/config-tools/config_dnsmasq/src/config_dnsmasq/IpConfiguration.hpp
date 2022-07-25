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

#ifndef SRC_CONFIG_DNSMASQ_IPCONFIGURATION_HPP_
#define SRC_CONFIG_DNSMASQ_IPCONFIGURATION_HPP_

#include <list>

#include "PortData.hpp"

namespace configdnsmasq {

class IpConfiguration {
 public:
  std::string host_name;
  std::list<std::string> port_name_list;
  std::list<PortData> port_data_list;

  IpConfiguration();
  virtual ~IpConfiguration() {};

  bool portIsAvailable(const std::string &port);
  PortData getPortData(const std::string& port);
};

} // namespace configdnsmasq

#endif /* SRC_CONFIG_DNSMASQ_IPCONFIGURATION_HPP_ */
