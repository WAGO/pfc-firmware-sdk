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
///  \brief    Collection of utility functions.
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
///  \author   MOe : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_UTILITIES_HPP_
#define SRC_CONFIG_DNSMASQ_UTILITIES_HPP_

#include <ct_error_handling.h>

namespace utilities {

typedef struct {
  ::std::string mac;
  uint32_t ip_net_part;
  uint32_t ip_host_part;

} dhcpd_host_t;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/**
 * Convert IP-Address from dotted ASCII string to uint32_t.
 * Did not use inet_aton() in favour of more strict format checks.
 * Set result to NULL for checking only.
 *
 * @param address
 * @return
 */
eStatusCode conv_ip_ascii2bin(uint32_t &bin_address, const ::std::string &address);

bool is_valid_ip_string (const ::std::string &address);

::std::string conv_ip_bin2ascii(const uint32_t address);

/**
 * Converts a string of the form mac_addr-ip_addr,... into an array of structures of type dhcpd_host_t
 *
 * @param hostsstr
 * @param netmask
 * @param hosts
 * @return status
 */
eStatusCode conv_hosts_ascii2bin(const ::std::string &hostsstr, const uint32_t netmask,
                                 ::std::vector<dhcpd_host_t> &hosts);

::std::string conv_hosts_bin2ascii(const ::std::vector<dhcpd_host_t> &hosts);

bool conv_is_valid_domain_name(const ::std::string &name);

bool conv_is_valid_mac(const ::std::string &mac_in);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

}  // namespace utilities

#endif // SRC_CONFIG_DNSMASQ_UTILITIES_HPP_
