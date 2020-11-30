//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Class representation of a dhcp ip range
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "DhcpRange.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <regex>

#include "error_handling.hpp"
#include "utilities.hpp"

DhcpRange DhcpRange::fromRangeString(const std::string &range, uint32_t netmask) {
  const std::regex IpRangePattern = std::regex("(.*)_(.*)");
  std::smatch match;

  erh_assert(std::regex_search(range, match, IpRangePattern), INVALID_PARAMETER,
    boost::format("\"%s\" is no valid ip range string because it does not have the syntax <lower-ip>_<upper-ip>") % range);

  return DhcpRange { match[1].str(), match[2].str(), netmask };
}

DhcpRange DhcpRange::fromRangeString(const std::string &range, const std::string &netmask) {
  uint32_t binary_netmask;
  utilities::conv_ip_ascii2bin(binary_netmask, netmask);
  return fromRangeString(range, binary_netmask);
}

DhcpRange::DhcpRange(const IpAddress &lower_ip, const IpAddress &upper_ip, uint32_t netmask)
    :
    lower_ip_ { lower_ip },
    upper_ip_ { upper_ip },
    netmask_ { netmask } {
      validate();
}

DhcpRange::DhcpRange(const std::string &lower_ip, const std::string &upper_ip, const std::string &netmask)
    :
    lower_ip_ { lower_ip },
    upper_ip_ { upper_ip },
    netmask_ { netmask } {
      validate();
}

DhcpRange::DhcpRange(uint32_t lower_ip, uint32_t upper_ip, uint32_t netmask)
    :
    lower_ip_ { lower_ip },
    upper_ip_ { upper_ip },
    netmask_ { netmask } {
      validate();
}

uint32_t DhcpRange::getLowerNetwork() const {
  return lower_ip_.asBinary() & netmask_.asBinary();
}

uint32_t DhcpRange::getLowerHost() const {
  return lower_ip_.asBinary() & ~netmask_.asBinary();
}

uint32_t DhcpRange::getUpperNetwork() const {
  return upper_ip_.asBinary() & netmask_.asBinary();
}

uint32_t DhcpRange::getUpperHost() const {
  return upper_ip_.asBinary() & ~netmask_.asBinary();
}

bool DhcpRange::isValid() const {
  return !(lower_ip_.isZero() || upper_ip_.isZero() || netmask_.isZero());
}

std::string DhcpRange::toString() const {
  if (lower_ip_.isZero() || upper_ip_.isZero()) {
    return "";
  }

  return lower_ip_.asString() + "_" + upper_ip_.asString();
}

void DhcpRange::resetNetwork(const uint32_t network) {
  lower_ip_ = IpAddress(network | getLowerHost());
  upper_ip_ = IpAddress(network | getUpperHost());
}

void DhcpRange::validate() const {
  auto sameSubnet = (lower_ip_.asBinary() & netmask_.asBinary()) == (upper_ip_.asBinary() & netmask_.asBinary());
  erh_assert(sameSubnet, INVALID_PARAMETER,
    boost::format("The range %s is invalid because its addresses are not in the same subnet") % toString());

  auto orderCorrect = lower_ip_.asBinary() <= upper_ip_.asBinary();
  erh_assert(orderCorrect, INVALID_PARAMETER,
    boost::format("The range %s is invalid because the first ip address must be smaller "
        "than or equal to the second so that both outline a valid ip range") % toString());
}
