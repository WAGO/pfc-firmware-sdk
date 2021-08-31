// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef __CT_LIB_NET_H__
#define __CT_LIB_NET_H__

#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define NETWORK_INTERFACES_XML  "/etc/specific/network-interfaces.xml"
#define SWCONFIG_SWITCH_NAME "switch0"

typedef struct libnetSession libnetSession_t;

int ct_libnet_start_session(const char *xmlConfigFile, libnetSession_t **sessionHandle);
int ct_libnet_finish_session(libnetSession_t *sessionHandle);

void ct_libnet_permanent_close(void);


int ct_libnet_generate_network_config(const char * const outFilename);
int ct_libnet_commit_changes_ip(const char *port, libnetSession_t *sessionHandle) __attribute_deprecated__;
int ct_libnet_commit_changes_eth(const char *port, libnetSession_t *sessionHandle) __attribute_deprecated__;
int ct_libnet_commit_changes_all(libnetSession_t *sessionHandle) __attribute_deprecated__;
int ct_libnet_save_xml_file(libnetSession_t *sessionHandle) __attribute_deprecated__;

//TODO: add/delete instead of set in combination with ip/netmask synthax (1.2.3.4/24)  would make multiple ips possible

///////////////////////////////////////////
// stateless functions -> no session needed
int ct_libnet_netmask_to_classic(const char *from, char *res, size_t resLen);
int ct_libnet_netmask_to_cidr(const char *from, char *res, size_t resLen);
int ct_libnet_calculate_broadcast(const char *ipAddr,
                                  const char *netmask,
                                  char *broadcast,
                                  size_t broadcastLen);

///////////////////////////////////////////

enum ipAddrFormatModes_t
{
    IP_FORMAT_MODE_NO_NETMASK, // 1.2.3.4/16
    IP_FORMAT_MODE_NETMASK,    // 1.2.3.4
};

enum netmaskFormatModes_t
{
    NETMASK_FORMAT_MODE_CLASSIC, // 255.255.0.0
    NETMASK_FORMAT_MODE_CIDR,    // 16
};

int ct_libnet_get_ip_addr_from_hw(const char *port,
                                  char *ip,
                                  size_t ipLen,
                                  enum ipAddrFormatModes_t mode,
                                  libnetSession_t *sessionHandle);

int ct_libnet_get_netmask_from_hw(const char *port,
                                  char *netmask,
                                  size_t netmaskLen,
                                  enum netmaskFormatModes_t mode,
                                  libnetSession_t *sessionHandle);

int ct_libnet_get_mac_addr_from_hw(const char *port,
                                   char *mac,
                                   size_t macLen,
                                   libnetSession_t *sessionHandle);

int ct_libnet_get_eth_port_state_from_hw(const char *port,
                                         char *state,
                                         size_t stateLen,
                                         libnetSession_t *sessionHandle);

int ct_libnet_get_default_via_from_hw(const char *port,
                                      char *strDefaultVia,
                                      size_t strDefaultViaLen);

int ct_libnet_get_ip_addr_from_config(const char *port, char *ip, size_t ipLen, libnetSession_t *sessionHandle);

int ct_libnet_get_netmask_from_config(const char *port,
                                      char *netmask,
                                      size_t netmaskLen,
                                      enum netmaskFormatModes_t mode,
                                      libnetSession_t *sessionHandle);

int ct_libnet_get_broadcast_from_config(const char *port,
                                        char *broadcast,
                                        size_t broadcastLen,
                                        libnetSession_t *sessionHandle);

int ct_libnet_get_ports_list(char *result, size_t resultLen, const char *delim, libnetSession_t *sessionHandle);
int ct_libnet_get_all_ports_list(char *result, size_t resultLen, const char *delim, libnetSession_t *sessionHandle);

int ct_libnet_set_ip_addr_to_config(const char *port, const char *ip, libnetSession_t *sessionHandle);
int ct_libnet_set_netmask_to_config(const char *port, const char *netmask, libnetSession_t *sessionHandle);
int ct_libnet_set_broadcast_to_config(const char *port, const char *broadcast, libnetSession_t *sessionHandle);

int ct_libnet_set_mac_addr(const char *port, const char *mac, libnetSession_t *sessionHandle);

int ct_libnet_get_config_type(const char *port,
                              char *configType,
                              size_t configTypeLen,
                              libnetSession_t *sessionHandle);

int ct_libnet_set_config_type(const char *port, const char *configType, libnetSession_t *sessionHandle);

int ct_libnet_port_state_to_config(const char *port, const char *config);

int ct_libnet_get_port_state_from_config(const char *port, char *state, size_t stateLen, libnetSession_t *sessionHandle);

enum cableState_t
{
    CT_CABLE_STATE_DISCONNECTED = 0,
    CT_CABLE_STATE_CONNECTED,
    CT_CABLE_STATE_UNKNOWN,
};
int ct_libnet_get_cable_state(const char *port,
                              enum cableState_t *state,
                              libnetSession_t *sessionHandle);

// ethernet-related

int ct_libnet_get_speed_from_hw(const char *port,
                                char *speed,
                                size_t speedLen,
                                libnetSession_t *sessionHandle);

int ct_libnet_get_speed_from_config(const char *port,
                                    char *speed,
                                    size_t speedLen,
                                    libnetSession_t *sessionHandle);

int ct_libnet_get_duplex_from_hw(const char *port,
                                 char *duplex,
                                 size_t duplexLen,
                                 libnetSession_t *sessionHandle);

int ct_libnet_get_duplex_from_config(const char *port,
                                     char *duplex,
                                     size_t duplexLen,
                                     libnetSession_t *sessionHandle);

int ct_libnet_get_autoneg_from_hw(const char *port,
                                  char *autoneg,
                                  size_t autonegLen,
                                  libnetSession_t *sessionHandle);

int ct_libnet_get_autoneg_from_config(const char *port,
                                      char *autoneg,
                                      size_t autonegLen,
                                      libnetSession_t *sessionHandle);

int ct_libnet_set_port_params_to_hw(const char *port,
                                     const char *autoneg,
                                     const char *speed,
                                     const char *duplex,
                                     libnetSession_t *sessionHandle);

int ct_libnet_set_port_state_to_hw(const char *port,
                                   const char *state,
                                   libnetSession_t *sessionHandle);

int ct_libnet_set_dsa_state(const char *value, libnetSession_t *sessionHandle);
int ct_libnet_get_dsa_state(char *value, size_t valueLen, libnetSession_t *sessionHandle);
int ct_libnet_get_actual_dsa_state(char * const value, size_t const valueLen);

const char * ct_libnet_get_last_error(void);

int ct_libnet_set_bcast_protect(const char *dev, const char *value);
int ct_libnet_get_bcast_protect(const char *dev, char *value, size_t valueLen);
void ct_libnet_print_bcast_protect_args(void);

int ct_libnet_set_port_mirror(const char *dev, const char *value);
int ct_libnet_get_port_mirror(const char *dev, char *value, size_t valueLen);
void ct_libnet_print_port_mirror_args(void);

int ct_libnet_set_rate_limit(const char *dev, const char *value);
int ct_libnet_get_rate_limit(const char *dev, char *value, size_t valueLen);
void ct_libnet_print_rate_limit_args(void);
const char * ct_libnet_get_rate_limit_by_index (size_t value);

int ct_libnet_set_fast_aging(const char *dev, const char *value);
int ct_libnet_get_fast_aging(const char *dev, char *value, size_t valueLen);
void ct_libnet_print_fast_aging_args(void);

typedef struct {
    const char *bcastProtect;
    const char *rateLimit;
    const char *portMirror;
    const char *fastAging;
} ct_switch_settings_t;

int ct_libnet_save_switch_settings_to_config(const char *configFile, ct_switch_settings_t *settings);

__END_DECLS

#endif
