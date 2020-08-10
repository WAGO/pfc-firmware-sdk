// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPValidator.hpp"
#include "Helper.hpp"
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/network_v4.hpp>

#include "NetDevManager.hpp"

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

  bool must_be_checked = true;
  if ((ip_config.source_ == IPSource::NONE) || (ip_config.source_ == IPSource::EXTERNAL)) {
    must_be_checked = false;
  } else if (ip_config.address_ == ZeroIP && ip_config.netmask_ == ZeroIP) {
    must_be_checked = false;
  } else if ((ip_config.source_ == IPSource::DHCP || ip_config.source_ == IPSource::BOOTP)) {
    must_be_checked = false;
  }

  return must_be_checked;

}

static uint32_t CheckAddressFormat(const Address &address, const Interface &interface, Status &status) {

  boost_error error_code;
  boost_address boost_ipaddress = boost_address::from_string(address, error_code);
  if (error_code) {
    status.Append(StatusCode::INVALID_PARAMETER,
                  "Interface " + interface + " ip address " + address + " is invalid (" + error_code.message() + "). ");
  } else {
    uint32_t binary_address = boost_ipaddress.to_v4().to_uint();
    if (binary_address == 0) {
      status.Append(StatusCode::INVALID_PARAMETER,
                    "Interface " + interface + " ip-address " + address + " is invalid (address is zero). ");
    } else if (binary_address == ~(0U)) {
      status.Append(
          StatusCode::INVALID_PARAMETER,
          "Interface " + interface + " ip-address " + address + " is invalid (ip-address only contains ones). ");
    }

  }

  return boost_ipaddress.to_v4().to_uint();
}

static uint32_t CheckNetmaskFormat(const Netmask &netmask, const Interface &interface, Status &status) {

  boost_error error_code;
  boost_address binary_netmask = boost_address::from_string(netmask, error_code);
  if (error_code) {
    status.Append(StatusCode::INVALID_PARAMETER,
                  "Interface " + interface + " netmask " + netmask + " is invalid (" + error_code.message() + "). ");
  } else {
    uint32_t mask = binary_netmask.to_v4().to_uint();
    if (mask == 0) {
      status.Append(StatusCode::INVALID_PARAMETER,
                    "Interface " + interface + " netmask " + netmask + " is invalid (netmask is zero). ");
    } else if ((mask & (~mask >> 1)) != 0) {
      status.Append(StatusCode::INVALID_PARAMETER,
                    "Interface " + interface + " netmask " + netmask + " is invalid (netmask contains zeros). ");
    }
  }

  return binary_netmask.to_v4().to_uint();

}

static void CheckIPAddressExistMoreOften(const IPConfigs &ip_configs, Status &status) {

  Addresses checked_adresses;
  for (auto &ip_config : ip_configs) {

    uint32_t address = ToBinaryAddress(ip_config.address_);
    if (IsIncluded(address, checked_adresses)) {
      status.Append(StatusCode::INVALID_PARAMETER,
                    "IP address " + ip_config.address_ + " is included in several ip configs. ");
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
    status.Append(
        StatusCode::INVALID_PARAMETER,
        "Interface " + lhs.interface_ + " address " + lhs.address_ + " netmask " + lhs.netmask_
            + " overlaps an existing network. ");
  }

}

static void CheckIpConfigCombinability(const IPConfig &ip_config, const IPConfigs &other_ip_configs, Status &status) {

  uint32_t address = ToBinaryAddress(ip_config.address_);
  for (auto other_ip_config : other_ip_configs) {

    uint32_t other_address = ToBinaryAddress(other_ip_config.address_);
    if (address == other_address) {
      status.Append(
          StatusCode::INVALID_PARAMETER,
          "Interface " + ip_config.interface_ + " address " + ip_config.address_
              + " is already assigned to an interface. ");
    }

    CheckIPConflictInOverlappingNetwork(ip_config, other_ip_config, status);
    if (status.NotOk()) {
      break;
    }
  }

}

static void CheckOverlappingNetwork(const IPConfigs &ip_configs, Status &status) {

  IPConfigs checked_ip_configs;
  for (auto &ip_config : ip_configs) {
    CheckIpConfigCombinability(ip_config, checked_ip_configs, status);
    if (status.NotOk()) {
      break;
    }
    checked_ip_configs.emplace_back(ip_config);
  }
}

static void CheckIPAddressFormat(const IPConfigs &ip_configs, Status &status) {

  for (auto &ip_config : ip_configs) {
    CheckAddressFormat(ip_config.address_, ip_config.interface_, status);
    CheckNetmaskFormat(ip_config.netmask_, ip_config.interface_, status);

    if (status.NotOk()) {
      break;
    }
  }
}

static void CheckInterfaceIsIncludedSeveralTimes(const IPConfigs &ip_configs, Status &status) {

  Interfaces checked_interfaces;
  for (auto &ip_config : ip_configs) {

    if (IsIncluded(ip_config.interface_, checked_interfaces)) {
      status.Append(StatusCode::INVALID_PARAMETER,
                    "Interface " + ip_config.interface_ + " is included several times. ");
      break;
    }
    checked_interfaces.emplace_back(ip_config.interface_);
  }

}

Status IPValidator::ValidateIPConfigs(const IPConfigs &ip_configs) {

  Status status;
  CheckInterfaceIsIncludedSeveralTimes(ip_configs, status);

  IPConfigs configs = FilterValidStaticAndTemporaryIPConfigs(ip_configs);
  if (status.Ok()) {
    CheckIPAddressFormat(configs, status);
  }
  if (status.Ok()) {
    CheckIPAddressExistMoreOften(configs, status);
  }
  if (status.Ok()) {
    CheckOverlappingNetwork(configs, status);
  }

  if (status.NotOk()) {
    status.Prepend("IP validation error: ");
  }

  return status;
}

Status IPValidator::ValidateCombinabilityOfIPConfigs(const IPConfigs &lhs_ip_configs,
                                                     const IPConfigs &rhs_ip_configs) {

  Status status;
  for (const auto &lhs_config : lhs_ip_configs) {
    CheckIpConfigCombinability(lhs_config, rhs_ip_configs, status);
    if (status.NotOk()) {
      status.Prepend("IP validation error: ");
      break;
    }
  }

  return status;
}

bool IPValidator::IsSameSubnet(IPConfig lhs, IPConfig rhs) {
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

}
