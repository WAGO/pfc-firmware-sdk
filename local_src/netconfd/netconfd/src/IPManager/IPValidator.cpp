// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPValidator.hpp"
#include "Helper.hpp"
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/network_v4.hpp>

#include "NetDevManager.hpp"

namespace netconfd {
using namespace std::literals;

using boost_address = boost::asio::ip::address;
using boost_address_v4 = boost::asio::ip::address_v4;
using boost_net = boost::asio::ip::network_v4;
using boost_error = boost::system::error_code;

using Addresses = ::std::vector<uint32_t>;

IPValidator::IPValidator(const IIPController &ip_controller)
    :
    ip_controller_ { ip_controller } {
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

  //TODO (s.b.) check if address is broadcast (eg 192.168.1.255/24 192.168.1.0/24)

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

static uint32_t CheckBroadcastFormat(const Broadcast &broadcast, const Interface &interface, Status &status) {

  if (not broadcast.empty()) {
    boost_error error_code;
    boost_address binary_broadcast = boost_address::from_string(broadcast, error_code);
    if (error_code) {
      status.Append(
          StatusCode::INVALID_PARAMETER,
          "Interface " + interface + " broadcast " + broadcast + " is invalid (" + error_code.message() + "). ");
    }

    return binary_broadcast.to_v4().to_uint();
  }

  return 0;

}

static void CheckInterfaceIsIncludedSeveralTimes(const Interface &interface, Interfaces &interfaces, Status &status) {

  if (IsIncluded(interface, interfaces)) {
    status.Append(StatusCode::INVALID_PARAMETER, "Interface " + interface + " is included several times. ");
  } else {
    interfaces.push_back(interface);
  }

}

static void CheckIPAddressExistMoreOften(uint32_t binary_address, Addresses &binary_addresses, Status &status) {

  if (IsIncluded(binary_address, binary_addresses)) {
    boost::asio::ip::address_v4 address(binary_address);
    status.Append(StatusCode::INVALID_PARAMETER,
                  "IP address " + address.to_string() + " is included in several ip configs. ");
  } else {
    binary_addresses.push_back(binary_address);
  }
}

static void CheckNetworkIncludedSeveralTimes(uint32_t binary_address, uint32_t binary_netmask,
                                             Addresses &binary_networks, Status &status) {

  uint32_t binary_network = binary_address & binary_netmask;
  if (IsIncluded(binary_network, binary_networks)) {
    boost::asio::ip::address_v4 network(binary_network);
    status.Append(StatusCode::INVALID_PARAMETER,
                  "Network " + network.to_string() + " is included in several ip configs. ");
  } else {
    binary_networks.push_back(binary_network);
  }

}

static void CheckBroadcast(const Interface &interface, uint32_t binary_broadcast, uint32_t binary_address,
                           uint32_t binary_netmask, Status &status) {

  if (binary_broadcast != 0) {

    uint32_t binary_network = binary_address & binary_netmask;
    uint32_t calculated_binary_broadcast = binary_network | (~binary_netmask);

    if (status.Ok()) {

      if (calculated_binary_broadcast != binary_broadcast) {
        boost::asio::ip::address_v4 broadcast(binary_broadcast);
        status.Append(StatusCode::INVALID_PARAMETER,
                      "Interface " + interface + " Broadcast " + broadcast.to_string() + " is invalid. ");
      }
    }
  }

}

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

static bool IPConfigsInterferenceMustBeChecked(const IPConfig &ip_config) {
  return (ip_config.source_ == IPSource::STATIC) || (ip_config.source_ == IPSource::TEMPORARY);
}

Status IPValidator::ValidateIPConfigs(const IPConfigs &ip_configs, const bool interference_has_to_be_checked) const {

  Status status;

  Addresses binary_networks;
  Addresses binary_addresses;
  Interfaces ip_config_interfaces;
  for (const auto &ip_config : ip_configs) {

    const Interface &interface = ip_config.interface_;

    CheckInterfaceIsIncludedSeveralTimes(interface, ip_config_interfaces, status);

    if (IPConfigParametersMustBeChecked(ip_config)) {

      uint32_t binary_address = CheckAddressFormat(ip_config.address_, interface, status);

      uint32_t binary_netmask = CheckNetmaskFormat(ip_config.netmask_, interface, status);

      uint32_t binary_broadcast = CheckBroadcastFormat(ip_config.broadcast_, interface, status);
      CheckBroadcast(interface, binary_broadcast, binary_address, binary_netmask, status);

      if (interference_has_to_be_checked && IPConfigsInterferenceMustBeChecked(ip_config)) {
        CheckIPAddressExistMoreOften(binary_address, binary_addresses, status);

        CheckNetworkIncludedSeveralTimes(binary_address, binary_netmask, binary_networks, status);
      }
    }

  }

  if (status.NotOk()) {
    status.Prepend("IP validation error: ");
  }

  return status;
}

bool IPValidator::IsInterfaceApplicableToSystem(const IPConfigs &ip_configs, const NetDevs &netdevs,
                                                const Interfaces &not_assignable_interface) const {
  for (const auto &ip_config : ip_configs) {
    const Interface &interface = ip_config.interface_;

    if (NetDevManager::DoesNotExistByName(interface, netdevs) || IsIncluded(interface, not_assignable_interface)) {
      return false;
    }
  }

  return true;
}

Status IPValidator::ValidateIPConfigIsApplicableToSystem(const IPConfigs &ip_configs, const NetDevs &netdevs) const {

  Status status;
  Addresses binary_existing_networks;
  Addresses binary_existing_addresses;

  Interfaces ip_config_interfaces;
  ::std::transform(ip_configs.begin(), ip_configs.end(), ::std::back_inserter(ip_config_interfaces),
                   [](IPConfig c) -> Interface {
                     return c.interface_;
                   });

  for (const auto &netdev : netdevs) {

    if (IsNotIncluded(netdev->GetName(), ip_config_interfaces)) {

      IPConfig ip_config;
      ip_controller_.GetIPConfig(netdev->GetName(), ip_config);

      uint32_t netmask = ToBinaryAddress(ip_config.netmask_);
      uint32_t address = ToBinaryAddress(ip_config.address_);
      uint32_t network = netmask & address;

      binary_existing_addresses.push_back(address);
      binary_existing_networks.push_back(network);
    }

  }

  for (const auto &ip_config : ip_configs) {

    if (IPConfigParametersMustBeChecked(ip_config)) {

      uint32_t netmask = ToBinaryAddress(ip_config.netmask_);
      uint32_t address = ToBinaryAddress(ip_config.address_);
      uint32_t network = netmask & address;

      if (IsIncluded(address, binary_existing_addresses)) {
        status.Append(
            StatusCode::INVALID_PARAMETER,
            "Interface " + ip_config.interface_ + " address " + ip_config.address_
                + " is already assigned to an interface. ");
      }

      if (IsIncluded(network, binary_existing_networks)) {
        boost::asio::ip::address_v4 boost_network(network);
        status.Append(
            StatusCode::INVALID_PARAMETER,
            "Interface " + ip_config.interface_ + " network " + boost_network.to_string() + " already exists. ");
      }
    }
  }

  if (status.NotOk()) {
    status.Prepend("IP validation error: ");
  }

  return status;

}

bool IPValidator::IsSameSubnet(IPConfig lhs, IPConfig rhs) const {
  auto lhs_addr = ToBinaryAddress(lhs.address_);
  auto lhs_mask = ToBinaryAddress(lhs.netmask_);
  auto rhs_addr = ToBinaryAddress(rhs.address_);
  auto rhs_mask = ToBinaryAddress(rhs.netmask_);

  return (lhs_addr & lhs_mask) == (rhs_addr & rhs_mask);
}

} /* namespace netconfd */
