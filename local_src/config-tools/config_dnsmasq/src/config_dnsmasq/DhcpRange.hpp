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

#ifndef SRC_CONFIG_DNSMASQ_DHCPRANGE_HPP_
#define SRC_CONFIG_DNSMASQ_DHCPRANGE_HPP_

#include <stdint.h>
#include <string>

#include "IpAddress.hpp"

class DhcpRange {
 public:
  IpAddress lower_ip_;
  IpAddress upper_ip_;
  IpAddress netmask_;

  static DhcpRange fromRangeString(const std::string &range, uint32_t netmask);
  static DhcpRange fromRangeString(const std::string &range, const std::string &netmask);

  DhcpRange(const IpAddress &lowerIp, const IpAddress &upperIp, uint32_t netmask);
  DhcpRange(uint32_t lowerIp, uint32_t upperIp, uint32_t netmask);
  DhcpRange(const std::string &lower_ip, const std::string &upper_ip, const std::string &netmask);
  DhcpRange() : DhcpRange(0, 0, 0) {};
  virtual ~DhcpRange() { };

  uint32_t getLowerNetwork() const;
  uint32_t getLowerHost() const;
  uint32_t getUpperNetwork() const;
  uint32_t getUpperHost() const;

  void resetNetwork(const uint32_t network);

  bool isValid() const;
  std::string toString() const;
 private:
  void validate() const;
};

#endif /* SRC_CONFIG_DNSMASQ_DHCPRANGE_HPP_ */
