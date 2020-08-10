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
///  \brief    Reading of network information (ethernet, ip and interfaces).
///
///  \author   MSc, WAGO Kontakttechnik GmbH & Co. KG.
///  \author   MOe, WAGO Kontakttechnik GmbH & Co. KG.
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_
#define SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_

#include <cstdint>

#include "defines.hpp"

typedef struct {
    char *port_name;
    char *state;
    char *type;
    char *ip_addr;
    char *netmask;
    uint32_t ip_addr_bin;
    uint32_t netmask_bin;
    uint32_t network_bin;
} port_data_t;

typedef struct {
    char *host_name;
    int  no_of_ports;
    char **port_name_list;
    port_data_t **port_data_list;
} ip_config_t;


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void netcfg_read_settings(ip_config_t *data, char ***legal_ports, prgconf_t const * const prgconf, int debugmode);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_ */
