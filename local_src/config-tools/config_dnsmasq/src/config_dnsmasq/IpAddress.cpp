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
///  \brief    Class representation of a port configuration
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "IpAddress.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>

#include "error_handling.hpp"
#include "utilities.hpp"

IpAddress::IpAddress(const std::string &address) {
  string_ = address;
  auto status = utilities::conv_ip_ascii2bin(binary_, address);
  erh_assert(status == SUCCESS, INVALID_PARAMETER,
    boost::format("The ip address %s cannot be parsed to its binary representation") % address);
}

IpAddress::IpAddress(uint32_t address) {
  binary_ = address;
  string_ = utilities::conv_ip_bin2ascii(address);
}

std::string IpAddress::asString() const { return string_; }

uint32_t IpAddress::asBinary() const { return binary_; }

bool IpAddress::isZero() const { return binary_ == 0; }

