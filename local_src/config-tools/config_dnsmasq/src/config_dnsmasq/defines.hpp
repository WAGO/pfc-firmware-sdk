//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
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
///  \author   MSc : WAGO GmbH & Co. KG
///  \author   MOe : WAGO GmbH & Co. KG
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
typedef enum
{
  NONE = 0,
  GENERATE,
  GET_CONFIG,
  GET_JSON_CONFIG,
  SET_CONFIG,
  RESTORE_CONFIG,
  STORE_CONFIG,
  HELP
} command_t;

// Type of service of config_dnsmasq.
typedef enum
{
  UNDEF = 0,
  DHCPD,
  DNS
} service_t;

// Program configuration data.
typedef struct prgconf
{
    command_t command;                  // Function to perform.
    ::std::string dbg_root;             // Root directory for testing and debugging.
    ::std::string interfaces_xml_path;  // Path to network-interfaces.xml.
    ::std::string services_xml_path;    // Path to network-services.xml.
    ::std::string etchosts;             // Path to /etc/hosts.
    ::std::string dnsmasq_conf;         // Path to /etc/dnsmasq.conf.
    ::std::string dnsmasq_conf_tmp;     // Path to temporary /etc/dnsmasq.conf.
    ::std::string restart_command;      // Path to /etc/init.d/dnsmasq.
    ::std::string lease_file;           // Path to /var/lib/misc/dnsmasq.leases.
    ::std::string selected_port;        // Port selected for getting/setting DHCPD parameters.
    service_t service_type;             // Configuration type DNS or DHCPD.
    bool dnsmasq_restart;               // Do not call dnsmasq init script after config setting.
    ::std::string bridge_config;
    ::std::string ip_config;
    ::std::string interface_config;
    ::std::string backup_file_path;     //Path to backup file.

    prgconf() :
            command{NONE},
            dbg_root{""},
            interfaces_xml_path{""},
            services_xml_path{""},
            etchosts{""},
            dnsmasq_conf{""},
            dnsmasq_conf_tmp{""},
            restart_command{""},
            lease_file{""},
            selected_port{""},
            service_type{UNDEF},
            dnsmasq_restart{true},
            bridge_config{""},
            ip_config{""},
            interface_config{""},
            backup_file_path{""}
    {
    }
    ;
} prgconf_t;

#endif /* SRC_CONFIG_DNSMASQ_DEFINES_HPP_ */

