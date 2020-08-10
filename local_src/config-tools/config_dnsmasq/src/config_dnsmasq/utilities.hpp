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

#include <cstdint>

typedef struct
{
    uint32_t ip_net_part1;
    uint32_t ip_net_part2;
    uint32_t host_part1;
    uint32_t host_part2;
} dhcpd_range_t;

typedef struct
{
    char *mac;
    uint32_t ip_net_part;
    uint32_t ip_host_part;
} dhcpd_host_t;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Look up str in an string pointer array like argv.
int strv_is_in_set(char **set, const char *str);

// Convert IP-Address from dotted ASCII string to uint32_t.
// Did not use inet_aton() in favour of more strict format checks.
// Set result to NULL for checking only.
int conv_ip_ascii2bin(char *ipaddr, uint32_t *resultp);

// Allocatess memory which the caller has to free with g_free().
int conv_ip_bin2ascii(uint32_t ipaddr, char **resultp);

// Allocs a dhcpd_range_t structure which the caller has to free with g_free().
int conv_range_ascii2bin(char *rangestr, uint32_t netmask, dhcpd_range_t **resultp);

// Converts a dhcpd_range_t structure to the ASCII representation like 192.168.3.100-192.168.3.200
// Allocs memory which the caller has to free with g_free().
int conv_range_bin2ascii(dhcpd_range_t *range, char **resultp);

// Converts a string of the form mac_addr-ip_addr,... into an array of structures of type dhcpd_host_t
// Allocates memory for the array of structures which must be freed by g_free().
int conv_hosts_ascii2bin(char *hostsstr, uint32_t netmask, guint *count, dhcpd_host_t **resultp);

// Allocates memory which must be freed with g_free().
int conv_hosts_bin2ascii(dhcpd_host_t *hosts, guint count, char **resultp);

bool conv_is_valid_domain_name(const char *name);

bool conv_is_valid_mac(const char *mac_in);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // SRC_CONFIG_DNSMASQ_UTILITIES_HPP_
