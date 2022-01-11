// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPValidator.hpp"

#include "Helper.hpp"
#include "BaseTypes.hpp"
#include "TypesHelper.hpp"

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/network_v4.hpp>

namespace netconf {
using namespace std::literals;

using boost_address = boost::asio::ip::address;
using boost_address_v4 = boost::asio::ip::address_v4;
using boost_net = boost::asio::ip::network_v4;
using boost_error = boost::system::error_code;

using Addresses = ::std::vector<uint32_t>;

static uint32_t ToBinaryAddress(const Address &address) {
  boost_error error_code;
  boost_address binary_address = boost_address::from_string(address, error_code);
  return binary_address.to_v4().to_uint();
}

static bool IPConfigParametersMustBeChecked(const IPConfig &ip_config) {
  return IPConfig::SourceIsAnyOf(ip_config, IPSource::STATIC, IPSource::TEMPORARY);
}

static uint32_t CheckAddressFormat(const Address &address, const Interface &interface, Status &status) {

  boost_error error_code;
  boost_address boost_ipaddress = boost_address::from_string(address, error_code);
  if (error_code) {
    status.Set(StatusCode::IP_INVALID, interface, address, error_code.message());
  } else {
    uint32_t binary_address = boost_ipaddress.to_v4().to_uint();
    if (binary_address == 0 || binary_address == ~(0U)) {
      status.Set(StatusCode::IP_INVALID, interface, address);
    }
  }

  return boost_ipaddress.to_v4().to_uint();
}

static uint32_t CheckNetmaskFormat(const Netmask &netmask, const Interface &interface, Status &status) {

  boost_error error_code;
  boost_address binary_netmask = boost_address::from_string(netmask, error_code);
  if (error_code) {
    status.Set(StatusCode::NETMASK_INVALID, interface, netmask, error_code.message());
  } else {
    uint32_t mask = binary_netmask.to_v4().to_uint();
    if (mask == 0 || (mask & (~mask >> 1)) != 0) {
      status.Set(StatusCode::NETMASK_INVALID, interface, netmask);
    }
  }

  return binary_netmask.to_v4().to_uint();
}

static void CheckIPAddressExistMoreOften(const IPConfigs &ip_configs, Status &status) {

  Addresses checked_adresses;
  for (auto &ip_config : ip_configs) {

    uint32_t address = ToBinaryAddress(ip_config.address_);
    if (IsIncluded(address, checked_adresses)) {
      status.Set(StatusCode::IP_DISTRIBUTED_MULTIPLE_TIMES, ip_config.address_);
      break;
    }
    checked_adresses.emplace_back(address);
  }

}

static void CheckIPConflictInOverlappingNetwork(const IPConfig &lhs, const IPConfig &rhs, Status &status) {

  uint32_t lhs_netmask = ToBinaryAddress(lhs.netmask_);
  uint32_t rhs_netmask = ToBinaryAddress(rhs.netmask_);
  uint32_t lhs_address = ToBinaryAddress(lhs.address_);
  uint32_t rhs_address = ToBinaryAddress(rhs.address_);

  uint32_t overlapping_netmask = lhs_netmask & rhs_netmask;

  if ((overlapping_netmask & lhs_address) == (overlapping_netmask & rhs_address)) {
    status.Set(StatusCode::NETWORK_CONFLICT, lhs.interface_, rhs.interface_);
  }

}

static bool CheckForFlushIp(const IPConfig &ip_config){
  return ip_config.source_ == IPSource::TEMPORARY && ip_config.address_== ZeroIP && ip_config.netmask_ == ZeroIP;
}

static void CheckIpConfigCombinability(const IPConfig &ip_config_to_check, const IPConfigs &other_ip_configs, Status &status) {
  if( not IPConfig::SourceIsAnyOf(ip_config_to_check, IPSource::STATIC)) {
    return;
  }

  uint32_t address = ToBinaryAddress(ip_config_to_check.address_);
  for (auto other_ip_config : other_ip_configs) {
    // Do not check against the same interface and empty/zero IP configs
    if((ip_config_to_check.interface_ == other_ip_config.interface_) || (IsEmptyOrZeroIP(other_ip_config.address_))){
      continue;
    }
    uint32_t other_address = ToBinaryAddress(other_ip_config.address_);
    if (address == other_address) {
      status.Set(StatusCode::IP_DISTRIBUTED_MULTIPLE_TIMES, ip_config_to_check.address_);
    }

    CheckIPConflictInOverlappingNetwork(ip_config_to_check, other_ip_config, status);
    if (status.IsNotOk()) {
      break;
    }
  }
}

static void CheckOverlappingNetwork(const IPConfigs &ip_configs, Status &status) {

  IPConfigs checked_ip_configs;
  for (auto &ip_config : ip_configs) {
    if(CheckForFlushIp(ip_config)) {
      // Allow dhcp client to flush ip address.
      continue;
    }
    CheckIpConfigCombinability(ip_config, checked_ip_configs, status);
    if (status.IsNotOk()) {
      break;
    }
    checked_ip_configs.emplace_back(ip_config);
  }
}

static void CheckIPAddressFormat(const IPConfigs &ip_configs, Status &status) {

  for (auto &ip_config : ip_configs) {

    if(CheckForFlushIp(ip_config)) {
      // Allow dhcp client to flush ip address.
      continue;
    }
    CheckAddressFormat(ip_config.address_, ip_config.interface_, status);
    if (status.IsNotOk()) {
      break;
    }
    CheckNetmaskFormat(ip_config.netmask_, ip_config.interface_, status);
    if (status.IsNotOk()) {
      break;
    }
  }
}

static void CheckInterfaceIsIncludedSeveralTimes(const IPConfigs &ip_configs, Status &status) {

  Interfaces checked_interfaces;
  for (auto &ip_config : ip_configs) {

    if (IsIncluded(ip_config.interface_, checked_interfaces)) {
      status.Set(StatusCode::ENTRY_DUPLICATE, ip_config.interface_);
      break;
    }
    checked_interfaces.emplace_back(ip_config.interface_);
  }

}

Status IPValidator::ValidateIPConfigs(const IPConfigs &ip_configs) {

  Status status;
  CheckInterfaceIsIncludedSeveralTimes(ip_configs, status);

  IPConfigs configs = FilterValidStaticAndTemporaryIPConfigs(ip_configs);
  if (status.IsOk()) {
    CheckIPAddressFormat(configs, status);
  }
  if (status.IsOk()) {
    CheckIPAddressExistMoreOften(configs, status);
  }
  if (status.IsOk()) {
    CheckOverlappingNetwork(configs, status);
  }

  return status;
}

Status IPValidator::ValidateExistenceAndAccess(const IPConfigs &ip_configs, const InterfaceInformations interface_information) {
  auto status = Status { };
  for (const auto &ip_config : ip_configs) {
    const Interface &interface = ip_config.interface_;
    const auto [exists, info] = GetFirstOf(interface_information, byName(interface));

    if (exists) {
      if (info->IsIpConfigurationReadonly()) {
        status.Set(StatusCode::IP_CONFIG_READONLY, interface);
      }
    } else {
      status.Set(StatusCode::INTERFACE_NOT_EXISTING, interface);
    }
  }

  return status;

}
Status IPValidator::ValidateCombinabilityOfIPConfigs(const IPConfigs &lhs_ip_configs, const IPConfigs &rhs_ip_configs) {

  Status status;
  for (const auto &lhs_config : lhs_ip_configs) {
    CheckIpConfigCombinability(lhs_config, rhs_ip_configs, status);
    if (status.IsNotOk()) {
      break;
    }
  }

  return status;
}

bool IPValidator::IsSameSubnet(const IPConfig& lhs, const IPConfig& rhs) {
  auto lhs_addr = ToBinaryAddress(lhs.address_);
  auto lhs_mask = ToBinaryAddress(lhs.netmask_);
  auto rhs_addr = ToBinaryAddress(rhs.address_);
  auto rhs_mask = ToBinaryAddress(rhs.netmask_);

  return (lhs_addr & lhs_mask) == (rhs_addr & rhs_mask);
}

IPConfigs IPValidator::FilterValidStaticAndTemporaryIPConfigs(const IPConfigs &ip_configs) {

  IPConfigs configs;
  for (auto &ip_config : ip_configs) {
    if (IPConfigParametersMustBeChecked(ip_config)) {
      configs.emplace_back(ip_config);
    }
  }
  return configs;
}

void inline AppendIfNotOk(Status &status, const char* text) {
  if (status.IsNotOk()) {
    status.Append(text);
  }
}

Status IPValidator::Validate(IPConfigs candidate, IPConfigs current, const InterfaceInformations& interface_information ) {
  ComplementNetmasks(candidate);
  SortByInterface(candidate);
  SortByInterface(current);
  current = GetIpConfigsDifferenceByInterface(current, candidate);

  Status status = IPValidator::ValidateIPConfigs(candidate);
  AppendIfNotOk(status, "candidate itself");

  if (status.IsOk()) {
    status = IPValidator::ValidateExistenceAndAccess(candidate, interface_information);
    AppendIfNotOk(status, "against interface information");
  }

  if (status.IsOk()) {
    status = IPValidator::ValidateCombinabilityOfIPConfigs(candidate, current);
    AppendIfNotOk(status, "candidate against current");
  }
  return status;
}

} // namespace netconf
