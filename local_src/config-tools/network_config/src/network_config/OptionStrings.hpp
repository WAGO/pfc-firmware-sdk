// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace network_config {

struct Option {
  const char* name = nullptr;
  const char* description = nullptr;
  const char* parameter = nullptr;
};

/**
 * Main program option names and description
 */
struct Options {
  // @formatter:off
  const Option bridge_config
    { "bridge-config", "Set or get bridge configuration." };
  const Option interface_config
    { "interface-config", "Set or get interface configuration." };
  const Option ip_config
    { "ip-config", "Set or get ip configuration." };
  const Option mac_address
    { "mac-address", "Get mac address of an device. The device name must be given using the --device option." };
  const Option device_info
    { "device-info", "Get information about ports and/or bridges." };
  const Option backup_targetversion
    { "backup-targetversion", "Backup made for the specific firmware version [targetversion], e.g. V03.03.02" };
  const Option backup
    { "backup", "Backup network settings to given <file path>." };
  const Option restore
    { "restore", "Restore network settings from given <file path>." };
  const Option get_backup_parameter_count
    { "get-backup-parameter-count", "Get number of backup parameters." };
  const Option dsa_mode
    { "dsa-mode", "Set or Get dsa-mode configuration." };
  const Option fix_ip
    { "fix-ip", "Set fixed ip (192.168.1.17)" };
  const Option dip_switch_config
    { "dip-switch-config", "Set or get DIP switch configuration. Caution: mode and last address byte cannot be changed." };

  // Operations
  const Option get
    { "get", "Get configuration; Optionally specify a value to get only part of the configuration. The \"value\" can only be used together with the --mac-address option." };
  const Option get_current
    { "get-current", "Get a current configuration" };
  const Option set
    { "set", "Set a configuration with the given value" };
  const Option filter
    {"filter", "Displays data of configuration only from given device type. Usable in 'get' operations. <brigde|wwan|virtual|port>" };

  // Format options
  const Option format
    { "format,f", "Choose how to format the output. <text|json|pretty-json>" };

  // Miscellaneous options
  const Option help
    { "help", "Print usage information about this tool" };
  const Option quiet
    { "quiet", "Suppress any outout to the console" };
  const Option error_msg_dst
    { "error-msg-dst", "filename where error messages are put into", "<filename>" };
  const Option dryrun
    { "dryrun", "do not actually write anything just validate the request" };
  // @formatter:on
};

/**
 * Option field names and description
 */
struct Fields {
  // @formatter:off
  const Option field
      { "field","Selects a specific field of the data set, e.g. ipaddr", "<name>" };
  const Option device
    { "device", "Device name <br0|wwan0|..>", "<value>" };
  const Option source
      { "source", "IP address source <static|dhcp|bootp>" ,"<value>"};
  const Option address
    { "ipaddr", "IP address, e.g. 192.168.1.1" ,"<value>"};
  const Option netmask
    { "netmask", "IP netmask, e.g. 255.255.255.0" ,"<value>"};
  const Option bcast
    { "bcast", "Broadcast address, e.g. 192.168.1.255" ,"<value>"};
  const Option prefix
    { "prefix", "IP netmask prefix, e.g. 24" ,"<value>"};
  const Option state
    { "state", "Ethernet-Interface Port-State <up|down>" ,"<value>"};
  const Option autoneg
    { "autoneg", "Autonegotiation activated <on|off>" ,"<value>"};
  const Option speed
    { "speed", "Physical speed <10|100>" ,"<value>"};
  const Option duplex
    { "duplex", "Duplexity <half|full>" ,"<value>"};
  const Option backup_version
    { "backup-targetversion", "Target version, e.g. V03.03.02" ,"<value>"};
  // @formatter:on
};

const Options& GetOptions();
const Fields& GetFields();
}  // namespace netconf
