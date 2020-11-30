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

#ifndef SRC_CONFIG_DNSMASQ_IPADDRESS_HPP_
#define SRC_CONFIG_DNSMASQ_IPADDRESS_HPP_

#include <string>

class IpAddress {
 public:
  IpAddress(const std::string &address);
  IpAddress(const uint32_t address);
  virtual ~IpAddress() {};

  std::string asString() const;
  uint32_t asBinary() const;
  bool isZero() const;

 private:
  std::string string_;
  uint32_t binary_;
};

#endif /* SRC_CONFIG_DNSMASQ_IPADDRESS_HPP_ */
