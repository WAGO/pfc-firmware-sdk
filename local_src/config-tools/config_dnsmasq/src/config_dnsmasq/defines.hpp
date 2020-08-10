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
///  \file     defines.h
///
///  \brief    Various defines.
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
///  \author   MOe : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_CONFIG_DNSMASQ_DEFINES_HPP_
#define SRC_CONFIG_DNSMASQ_DEFINES_HPP_

#define SIZEOF_IPADDR   16
#define SIZEOF_MACADDR  18

// States of the domain name check routine.
typedef enum
{
    ERR = -1,
    STOP,
    START,
    INLABEL,
    HYPHEN                         // Input character was a hyphen
} conv_state_t;

// Main function command codes of config_dnsmasq.
typedef enum {
    NONE = 0,
    GENERATE,
    GET_CONFIG,
    GET_JSON_CONFIG,
    SET_CONFIG,
    HELP
} command_t;

// Type of service of config_dnsmasq.
typedef enum {
    UNDEF = 0,
    DHCPD,
    DNS
} service_t;

// Program configuration data.
typedef struct {
    command_t command;          // Function to perform.
    char *dbg_root;             // Root directory for testing and debugging.
    char *interfaces_xml_path;  // Path to network-interfaces.xml.
    char *services_xml_path;    // Path to network-services.xml.
    char *etchosts;             // Path to /etc/hosts.
    char *dnsmasq_conf;         // Path to /etc/dnsmasq.conf.
    char *dnsmasq_conf_tmp;     // Path to temporary /etc/dnsmasq.conf.
    char *restart_command;      // Path to /etc/init.d/dnsmasq.
    char *lease_file;           // Path to /var/lib/misc/dnsmasq.leases.
    char const *selected_port;  // Port selected for getting/setting DHCPD parameters.
    service_t service_type;     // Configuration type DNS or DHCPD.
    int nodmasqrestart;         // Do not call dnsmasq init script after config setting.
    char const *bridge_config;
    char const *ip_config;
    char const *interface_config;
} prgconf_t;

#endif /* SRC_CONFIG_DNSMASQ_DEFINES_HPP_ */

