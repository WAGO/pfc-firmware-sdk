/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var CreateDeviceparams = (function()
{

  /*
  * Convert ebtables original configuration
  */
  var ConvertEbtablesOriginalConfig = function(originalConfig)
  {
    var ebtables = originalConfig;

    // convert interface list (ordered by sequential number) to "associative" array with access via device id
    if(ebtables.ethernet.interfaces.interface)
    {
      $.each(ebtables.ethernet.interfaces.interface, function(index, iFace)
      {
        var iFaceId = iFace['if'] || index;
        ebtables.ethernet.interfaces[iFaceId] = iFace;
      });

      delete ebtables.ethernet.interfaces.interface;
    }

    return ebtables;
  };

  /*
  var ConvertIptablesOriginalConfig = function(originalConfig)
  {
    if(originalConfig.ipv4.input.filter.rule && (0 === originalConfig.ipv4.input.filter.rule.length))
    {
      originalConfig.ipv4.input.filter[0] = originalConfig.ipv4.input.filter.rule;
    }

    return iptables;
  };
  */

  /*
  * Convert firewall interface state configuration
  */
  var ConvertFirewallInterfaceOriginalConfig = function(originalConfig)
  {
    var ifConfig = { iFace: {} };

    if(originalConfig.ipv4.service.interfaces.interface)
    {
      $.each(originalConfig.ipv4.service.interfaces.interface, function(index, iFace)
      {
        var iFaceId = iFace['if'] || index;
        ifConfig.iFace[iFaceId] =
        {
          iFaceId:  iFace['if'],
          state:    iFace.state
        }
      });
    }

    return ifConfig;
  };

  /*
  * Convert interfaces XML configuration
  */
  var ConvertInterfacesOriginalConfig = function(originalConfig)
  {
    var interfaceConfig = { ifaces: {} };

    interfaceConfig.dsaMode = originalConfig.dsa_mode;

    $.each(originalConfig.iface, function(index, iFace)
    {
      var iFaceId = iFace.device_name;

      if(iFace.ip_settings && ('1' === iFace.ip_settings.show_in_wbm))
      {
        interfaceConfig.ifaces[iFaceId] =
        {
          deviceName:       iFace.device_name,
          state:            iFace.state,
          ipSettings:
          {
            portName:         iFace.ip_settings.port_name,
            type:             iFace.ip_settings.type,
            staticIpAddr:     iFace.ip_settings.static_ipaddr,
            staticNetmask:    iFace.ip_settings.static_netmask,
            staticBroadcast:  iFace.ip_settings.static_broadcast,
            // gateway removed after restructuring to own configtool
            gateway:
            {
              ip:             undefined, //iFace.ip_settings.gw.ip,
              state:          undefined  //iFace.ip_settings.gw.state
            }
          }
        };
      }

      else if(iFace.ethernet_settings)
      {
        interfaceConfig.ifaces[iFaceId] =
        {
          deviceName:       iFace.device_name,
          state:            iFace.state,
          ethernetSettings:
          {
            portName:       iFace.ethernet_settings.port_name,
            autoneg:        iFace.ethernet_settings.autoneg,
            speed:          iFace.ethernet_settings.speed,
            duplex:         iFace.ethernet_settings.duplex,
            mac:            iFace.ethernet_settings.mac
          }
        };
      }
    });

    return interfaceConfig;
  };


  /*---------------------------------------------------------------------------
  * state area
  * ---------------------------------------------------------------------------
  */
  var CreateStateAreaParams = (function()
  {
    // time_local - defined in clock
    // date_local - defined in clock

    deviceParams.Add(
    {
      id                    : 'run_stop_switch_value',
      configtoolReadParams  :
      {
        name            : 'get_run_stop_switch_value',
        parameter       : [ 'run-stop-reset-state' ],
        sudo            : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'complete_led_states',
      configtoolReadParams  :
      {
        name            : 'get_led_config',
        parameter       : [ 'all-states' ],
        multiline       : true
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * device details
  * ---------------------------------------------------------------------------
  */
  CreateDeviceDetailsParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'product_description',
      exampleValue          : 'WAGO 750-8206 PFC200 CS 2ETH RS CAN DPS',
      invariant             : true,
      configtoolReadParams  :
      {
        name      : 'get_typelabel_value',
        parameter : [ 'SYSDESC' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'order_number',
      exampleValue          : '750-8206',
      invariant             : true,
      configtoolReadParams  :
      {
        name      : 'get_typelabel_value',
        parameter : [ 'ORDER' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'rts_license_information',
      exampleValue          : 'Codesys-Runtime-License',
      invariant             : true,
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'license-information' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'firmware_revision',
      exampleValue          : '01.10.16(00)',
      invariant             : true,
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'firmware-revision' ]
      }
    });

  })();



  /*---------------------------------------------------------------------------
  * CODESYS
  * ---------------------------------------------------------------------------
  */
  var CreateCodesysParams = (function()
  {
    deviceParams.Add(
    {
      id                   : 'codesys_version',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_runtime_config',
        parameter : [ 'running-version' ]
      },
      configtoolWriteParams:
      {
        name      : 'config_runtime',
        parameter : [ '--wait', 'runtime-version=$codesysVersion', 'restart-server=NO', ],
        sudo      : true
      }
    });

    // TODO after implementation of configtool!
    deviceParams.Add(
    {
      id                   : 'codesys_skill_version_3',
      value                : '1',
      initialized          : true,
      invariant            : true,
      configtoolReadParams :
      {
        name      : 'get_plc_config',
        parameter : [ ]
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_webserver_version',
      exampleValue         : '',
      invariant            : true,
      configtoolReadParams :
      {
        name      : 'get_coupler_details',
        parameter : [ 'codesys-webserver-version' ]
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_2_webserver_port_state',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_runtime_config',
        parameter : [ 'cfg-version=2', 'webserver-state' ]
      },
      configtoolWriteParams:
      {
        name      : 'config_runtime',
        parameter :  [ 'cfg-version=2', 'webserver-state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_3_webserver_port_state',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_runtime_config',
        parameter : [ 'cfg-version=3', 'webserver-state' ]
      },
      configtoolWriteParams:
      {
        name      : 'config_runtime',
        parameter :  [ 'cfg-version=3', 'webserver-state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_2_communication_state',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_runtime_config',
        parameter : [ 'service-state' ]
      },
      configtoolWriteParams:
      {
        name      : 'config_runtime',
        parameter : [ 'cfg-version=2', 'service-state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_communication_port',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_runtime_config',
        parameter : [ 'cfg-version=2', 'comm-port' ]
      },
      configtoolWriteParams:
      {
        name      : 'config_runtime',
        parameter : [ 'cfg-version=2', 'comm-port=$portNumber' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_2_port_2455_auth_state',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_runtime_config',
        parameter : [ 'cfg-version=2', 'authentication' ]
      },
      configtoolWriteParams:
      {
        name      : 'config_runtime',
        parameter : [ 'cfg-version=2', 'authentication=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_3_port_2455_auth_state',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_runtime_config',
        parameter : [ 'cfg-version=3', 'authentication' ]
      },
      configtoolWriteParams:
      {
        name      : 'config_runtime',
        parameter : [ 'cfg-version=3', 'authentication=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_port_auth_password',
      configtoolWriteParams:
      {
        name      : 'config_linux_user',
        parameter : [ 'user=admin', 'new-password=$newPassword', 'confirm-password=$confirmPassword' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_state',
      exampleValue         : 'STOP',
      configtoolReadParams :
      {
        name      : 'get_rts_info',
        parameter : [ 'state' ]
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_task_count',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_rts_info',
        parameter : [ 'task', '0', 'number' ]
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_project_date',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_rts_info',
        parameter : [ 'project', 'date' ]
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_project_title',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_rts_info',
        parameter : [ 'project', 'title' ]
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_project_version',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_rts_info',
        parameter : [ 'project', 'version' ]
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_project_author',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_rts_info',
        parameter : [ 'project', 'author' ]
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_project_description',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_rts_info',
        parameter : [ 'project', 'description' ],
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                   : 'codesys_project_checksum',
      exampleValue         : '',
      configtoolReadParams :
      {
        name      : 'get_rts_info',
        parameter : [ 'project', 'checksum' ],
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'rts_task_list',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_rts_info',
        parameter : [ 'task-list-json' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'rts_bootproject_location',
      configtoolReadParams  :
      {
        name      : 'get_runtime_config',
        parameter : [ 'boot-project' ],
      },
      configtoolWriteParams :
      {
        name      : 'config_runtime',
        parameter : [ 'boot-project=$bootprojectLocation' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'rts_available_runtimes',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_possible_runtimes',
        parameter : [ '-j' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'rts_home_directory_on_sd_card',
      configtoolReadParams  :
      {
        name      : 'get_runtime_config',
        parameter : [ 'homedir-on-sdcard' ],
      },
      configtoolWriteParams :
      {
        name      : 'config_runtime',
        parameter : [ 'homedir-on-sdcard=$homedirOnSdCardState' ],
        sudo      : true
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * hostname
  * ---------------------------------------------------------------------------
  */
  var CreateHostnameParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'hostname',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'hostname' ]
      },
      configtoolWriteParams :
      {
        name          : 'change_hostname',
        parameter     : [ 'hostname=$hostname' ],
        sudo          : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'actual_hostname',
      exampleValue          : 'PFC200-FF004F',
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'actual-hostname' ]
      }
    });
  })();


  /*---------------------------------------------------------------------------
  * domain name
  * ---------------------------------------------------------------------------
  */
  var CreateDomainNameConfigParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'domain_name',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'domain-name' ]
      },
      configtoolWriteParams :
      {
        name          : 'edit_dns_server',
        parameter     : [ 'domain-name=$domainName' ],
        sudo          : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'actual_domain_name',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'actual-domain-name' ]
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * interface configuration Eth0
  * ---------------------------------------------------------------------------
  */
  var CreateInterfaceParamsEth0 = (function()
  {
    deviceParams.Add(
    {
      id                    : 'interface_state_eth0',
      exampleValue          : 'enabled',
      configtoolReadParams  :
      {
        name      : 'get_eth_config',
        parameter : [ 'eth0', 'state' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'actual_mac_address_eth0',
      exampleValue          : '00:30:DE:FF:00:4F',
      configtoolReadParams  :
      {
        name      : 'get_actual_eth_config',
        parameter : [ 'eth0', 'mac-address' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'actual_ip_address_eth0',
      exampleValue          : '192.168.1.87',
      configtoolReadParams  :
      {
        name      : 'get_actual_eth_config',
        parameter : [ 'eth0', 'ip-address' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'actual_subnet_mask_address_eth0',
      exampleValue          : '255.255.255.0',
      configtoolReadParams  :
      {
        name      : 'get_actual_eth_config',
        parameter : [ 'eth0', 'subnet-mask' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'ip_config_type_eth0',
      exampleValue          : 'static',
      configtoolReadParams  :
      {
        name      : 'get_eth_config',
        parameter : [ 'eth0', 'config-type' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'device_id_eth0',
      exampleValue          : 'X2',
      configtoolReadParams  :
      {
        name      : 'get_eth_config',
        parameter : [ 'eth0', 'device-id' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'if_state_eth0',
      exampleValue          : 'enabled',
      configtoolReadParams  :
      {
        name      : 'get_eth_config',
        parameter : [ 'eth0', 'state' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'static_ip_address_eth0',
      exampleValue          : '192.168.1.87',
      configtoolReadParams  :
      {
        name      : 'get_eth_config',
        parameter : [ 'eth0', 'ip-address' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'static_subnet_mask_eth0',
      exampleValue          : '255.255.255.0',
      configtoolReadParams  :
      {
        name      : 'get_eth_config',
        parameter : [ 'eth0', 'subnet-mask' ]
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * general interface configuration
  * ---------------------------------------------------------------------------
  */
  var CreateGeneralInterfaceParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'interface_config_xml',
      format                : 'xml',
      convertFkt            : ConvertInterfacesOriginalConfig,
      configtoolReadParams :
      {
        name          : 'get_eth_config',
        parameter     : [ '--xml' ],
        multiline     : true
        //sudo          : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'config_ip_address',
      configtoolWriteParams :
      {
        name          : 'config_interfaces',
        parameter     : [ 'interface=$interfaceId', 'state=$state', 'config-type=$configType', 'ip-address=$ipAddress', 'subnet-mask=$subnetMask' ],
        sudo          : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'dsa_mode',
      configtoolWriteParams :
      {
        name          : 'set_dsa_mode',
        parameter     : [ '-v', '$dsaMode' ],
        sudo          : true,
        timeout       : (1000 * 60 * 2)  // 2 minutes
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * switch configuration
  * ---------------------------------------------------------------------------
  */
  var CreateSwitchParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'switch_broadcast_protection',
      configtoolReadParams :
      {
        name          : 'get_switch_settings',
        parameter     : [ 'bcast-protect' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'switch_rate_limit',
      configtoolReadParams :
      {
        name          : 'get_switch_settings',
        parameter     : [ 'rate-limit' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'switch_port_mirror',
      configtoolReadParams :
      {
        name          : 'get_switch_settings',
        parameter     : [ 'port-mirror' ],
        sudo          : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'switch_fast_aging_state',
      configtoolReadParams :
      {
        name          : 'get_switch_settings',
        parameter     : [ 'fast-aging' ]
      }
    });

    // already initialized with values, but prepared to be read by configtool when it is existing
    deviceParams.Add(
    {
      id                    : 'switch_valid_rate_limits',
      value                 : [ 'off',
                                '64.kbps', '128.kbps', '192.kbps', '256.kbps', '320.kbps', '384.kbps', '448.kbps', '512.kbps', '576.kbps', '640.kbps', '704.kbps', '768.kbps', '832.kbps', '896.kbps', '960.kbps',
                                '1.mbps', '2.mbps', '3.mbps', '4.mbps', '5.mbps', '6.mbps', '7.mbps', '8.mbps', '9.mbps', '10.mbps',
                                '11.mbps', '12.mbps', '13.mbps', '14.mbps', '15.mbps', '16.mbps', '17.mbps', '18.mbps', '19.mbps', '20.mbps',
                                '21.mbps', '22.mbps', '23.mbps', '24.mbps', '25.mbps', '26.mbps', '27.mbps', '28.mbps', '29.mbps', '30.mbps',
                                '31.mbps', '32.mbps', '33.mbps', '34.mbps', '35.mbps', '36.mbps', '37.mbps', '38.mbps', '39.mbps', '40.mbps',
                                '41.mbps', '42.mbps', '43.mbps', '44.mbps', '45.mbps', '46.mbps', '47.mbps', '48.mbps', '49.mbps', '50.mbps',
                                '51.mbps', '52.mbps', '53.mbps', '54.mbps', '55.mbps', '56.mbps', '57.mbps', '58.mbps', '59.mbps', '60.mbps',
                                '61.mbps', '62.mbps', '63.mbps', '64.mbps', '65.mbps', '66.mbps', '67.mbps', '68.mbps', '69.mbps', '70.mbps',
                                '71.mbps', '72.mbps', '73.mbps', '74.mbps', '75.mbps', '76.mbps', '77.mbps', '78.mbps', '79.mbps', '80.mbps',
                                '81.mbps', '82.mbps', '83.mbps', '84.mbps', '85.mbps', '86.mbps', '87.mbps', '88.mbps', '89.mbps', '90.mbps',
                                '91.mbps', '92.mbps', '93.mbps', '94.mbps', '95.mbps', '96.mbps', '97.mbps', '98.mbps', '99.mbps' ],
      initialized           : true,
      invariant             : true
      /*
      configtoolReadParams :
      {
        name          : 'get_switch_settings',
        parameter     : [ '' ]
      }
      */
    });

    deviceParams.Add(
    {
      id                    : 'switch_config',
      configtoolWriteParams :
      {
        name          : 'config_switch',
        parameter     : [ '-b', '$broadcastProtection', '-r', '$rateLimit', '-p', '$portMirror', '-f', '$fastAging' ],
        sudo          : true
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * interface configuration X1/X2
  * ---------------------------------------------------------------------------
  */
  var CreateInterfaceParamsForPort = function(portName)
  {
    deviceParams.Add(
    {
      id                    : 'actual_mac_address_' + portName,
      exampleValue          : '00:30:DE:FF:00:4F',
      configtoolReadParams  :
      {
        name      : 'get_actual_eth_config',
        parameter : [ portName, 'mac-address' ]
        //parameter : [ 'eth0', 'mac-address' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'actual_ip_address_' + portName,
      exampleValue          : '192.168.1.87',
      configtoolReadParams  :
      {
        name      : 'get_actual_eth_config',
        parameter : [ portName, 'ip-address' ]
        //parameter : [ 'eth0', 'ip-address' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'actual_subnet_mask_address_' + portName,
      exampleValue          : '255.255.255.0',
      configtoolReadParams  :
      {
        name      : 'get_actual_eth_config',
        parameter : [ portName, 'subnet-mask' ]
        //parameter : [ 'eth0', 'subnet-mask' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'actual_config_type_' + portName,
      configtoolReadParams  :
      {
        name      : 'get_actual_eth_config',
        parameter : [ portName, 'config-type' ]
      }
    });

  };

  CreateInterfaceParamsForPort('X1');
  CreateInterfaceParamsForPort('X2');


  /*---------------------------------------------------------------------------
  * default gateway
  * ---------------------------------------------------------------------------
  */
  var CreateDefaultGatewayParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'default_gateway_config',
      exampleValue          : 'none',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_default_gateway_config',
        parameter : [ '-j' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'config_default_gateway',
      configtoolWriteParams  :
      {
        name      : 'config_default_gateway',
        parameter : [ 'number=$gatewayNo', 'state=$state', 'value=$value', 'metric=$metric', 'destination=$destination', 'dest_mask=$dest_mask' ],
        sudo      : true

      }
    });

  })();


  /*---------------------------------------------------------------------------
  * Ethernet
  * ---------------------------------------------------------------------------
  */
  var CreateEthernetParams = (function()
  {
    deviceParams.Add(
    {
      id                      : 'eth_state_X1',
      configtoolReadParams   :
      {
        name        : 'get_eth_config',
        parameter   : ['X1', 'state' ]
      }
    });

    deviceParams.Add(
    {
      id                      : 'eth_state_X2',
      configtoolReadParams   :
      {
        name        : 'get_eth_config',
        parameter   : ['X2', 'state' ]
      }
    });

    deviceParams.Add(
    {
      id                      : 'eth_autoneg_X1',
      configtoolReadParams   :
      {
        name        : 'get_eth_config',
        parameter   : ['X1', 'autoneg' ]
      }
    });

    deviceParams.Add(
    {
      id                      : 'eth_autoneg_X2',
      configtoolReadParams   :
      {
        name        : 'get_eth_config',
        parameter   : ['X2', 'autoneg' ]
      }
    });

    deviceParams.Add(
    {
      id                      : 'eth_speed_duplex_X1',
      configtoolReadParams   :
      {
        name        : 'get_eth_config',
        parameter   : ['X1', 'speed-duplex' ]
      }
    });

    deviceParams.Add(
    {
      id                      : 'eth_speed_duplex_X2',
      configtoolReadParams   :
      {
        name        : 'get_eth_config',
        parameter   : ['X2', 'speed-duplex' ]
      }
    });

    deviceParams.Add(
    {
      id                      : 'config_ethernet',
      configtoolWriteParams   :
      {
        name        : 'config_ethernet',
        parameter   : ['interface=eth0', 'port=$portName', 'state=$state', 'autoneg=$autoneg', 'speed=$speed', 'duplex=$duplex' ],
        sudo        : true
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * DNS server
  * ---------------------------------------------------------------------------
  */
  var CreateDnsServerParams = (function()
  {
    deviceParams.Add(
    {
      id                      : 'add_dns_server',
      configtoolWriteParams   :
      {
        name        : 'edit_dns_server',
        parameter   : ['add=add', 'dns-server-name=$dnsServerName' ],
        sudo        : true
      }
    });

    deviceParams.Add(
    {
      id                      : 'delete_dns_server',
      configtoolWriteParams   :
      {
        name        : 'edit_dns_server',
        parameter   : ['delete=DELETE', 'dns-server-nr=$dnsServerNo', 'dns-server-name=$dnsServerName' ],
        sudo        : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'dnsServerConfig',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_dns_server',
        parameter : [ 'all' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'dns_server_currently_used',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_dns_server',
        parameter : [ 'get-complete' ]
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * webserver
  * ---------------------------------------------------------------------------
  */
  var CreateWebserverParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'default_webserver',
      exampleValue          : 'Webserver',
      configtoolReadParams  :
      {
        name      : 'get_runtime_config',
        parameter : [ 'default-webpage' ]
      },
      configtoolWriteParams :
      {
        name          : 'config_runtime',
        parameter     : [ 'default-webpage=$defaultWebpage', 'restart-server=NO' ],
        sudo          : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'webvisu_port',
      exampleValue          : '8080',
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'webvisu-port' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'restart_webserver',
      configtoolWriteParams :
      {
        name      : 'restart_webserver',
        sudo      : true
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * firewall - ebtables, iptables
  * ---------------------------------------------------------------------------
  */
  var CreateFirewallParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'firewall_state',
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'firewall', '--is-enabled' ],
        sudo      : true
      },
      configtoolWriteParams :
      {
        name      : 'firewall',
        parameter : [ 'firewall', '--$state' ],
        sudo      : true,
        timeout   : (1000 * 60 * 2)  // 2 min
      }
    });

    deviceParams.Add(
    {
      id                    : 'ebtables_config',
      format                : 'xml',
      convertFkt            : ConvertEbtablesOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'ebtables', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'iptables_config',
      format                : 'xml',
      //convertFkt            : ConvertIptablesOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'iptables', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'icmp_echo_broadcast_protection',
      configtoolWriteParams :
      {
        name      : 'firewall',
        parameter : [ 'iptables', '--set-echo', 'drop', '-', '-', '$state', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'firewall_limits',
      configtoolWriteParams :
      {
        name      : 'firewall',
        parameter : [ 'iptables', '--set-climits', '-', '-', '-', '$tcpLimit', '$udpLimit', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'interface_ip_firewall_rule_state',
      configtoolWriteParams :
      {
        name      : 'firewall',
        parameter : [ 'iptables', '--set-open-if', '$state', '$iFaceId', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'interface_ip_firewall_rule_remove',
      configtoolWriteParams :
      {
        name      : 'firewall',
        parameter : [ 'iptables', '--rem-open-if', '$iFaceId', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'interface_firewall_state',
      configtoolWriteParams :
      {
        name      : 'firewall',
        parameter : [ 'ebtables', '--set-if', '$state', '$iFaceId', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'icmp_echo_interface_config',
      configtoolWriteParams :
      {
        name      : 'firewall',
        parameter : [ 'iptables', '--set-echo-if', '$policy', '$iFaceId', '$limit', '$burstlimit', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'mac_address_filter_mode',
      configtoolWriteParams :
      {
        name      : 'firewall' ,
        parameter : [ 'ebtables', '--set-mode', '$mode', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'mac_address_filter_state',
      configtoolWriteParams :
      {
        name      : 'firewall' ,
        parameter : [ 'ebtables', '--upd-host', '$hostNumber', '$state', '$macAddress', '$macMask', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'mac_address_filter_delete',
      configtoolWriteParams :
      {
        name      : 'firewall' ,
        parameter : [ 'ebtables', '--rem-host', '$hostNumber', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'mac_address_filter_add',
      configtoolWriteParams :
      {
        name      : 'firewall' ,
        parameter : [ 'ebtables', '--add-host', '$state', '$macAddress', '$macMask', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'firewall_user_filter_delete',
      configtoolWriteParams :
      {
        name      : 'firewall' ,
        parameter : [ 'iptables', '--rem-filter', '$ruleNo', '--apply' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'firewall_user_filter_add',
      configtoolWriteParams :
      {
        name      : 'firewall' ,
        parameter : [ 'iptables', '--add-filter', 'on', '$interfaceId', '$protocol', '$sourceIpAddr', '$sourceNetmask', '$sourcePort', '$destIpAddr', '$destSubnetMask', '$destPort', '$policy', '--apply' ],
        sudo      : true
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * firewall - ports and services
  * ---------------------------------------------------------------------------
  */
  var CreateFirewallServicesParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'firewall_service_config',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'services', '--get-ifs-status', 'json' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'telnet_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'telnet', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'ftp_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'ftp', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'ftps_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'ftps', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'http_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'http', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'https_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'https', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'snmp', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'ssh_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'ssh', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'tftp_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'tftp', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'codesysr_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'codesysr', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'codesysw_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'codesysw', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'dhcpd_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'dhcpd', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'iocheck_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'iocheck', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'dns_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'dns', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'modbus_tcp_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'modbus_tcp', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'modbus_udp_firewall_config',
      format                : 'xml',
      convertFkt            : ConvertFirewallInterfaceOriginalConfig,
      configtoolReadParams  :
      {
        name      : 'firewall' ,
        parameter : [ 'modbus_udp', '--get-xml' ],
        sudo      : true,
        multiline : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'set_service_firewall_interface_state',
      configtoolWriteParams :
      {
        name      : 'firewall_apply.sh' ,
        parameter : [ '$service', '--set-if', '$state', '$iFaceId' ]
      }
    });


  })();


  /*---------------------------------------------------------------------------
  * clock
  * ---------------------------------------------------------------------------
  */
  var CreateClockParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'date_local',
      exampleValue          : '09.12.2013',
      configtoolReadParams  :
      {
        name      : 'get_clock_data',
        parameter : [ 'date-local' ]
      },
      configtoolWriteParams :
      {
        name          : 'config_clock',
        parameter     : [ 'type=local', 'date=$date' ],
        sudo          : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'time_local',
      exampleValue          : '09:11:49',
      configtoolReadParams  :
      {
        name      : 'get_clock_data',
        parameter : [ 'time-local' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_clock',
        parameter : [ 'type=local', 'time=$time' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'time_utc',
      exampleValue          : '08:11:49',
      configtoolReadParams  :
      {
        name      : 'get_clock_data',
        parameter : [ 'time-utc' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_clock',
        parameter : [ 'type=utc', 'time=$time' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'clock_display_mode',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_clock_data',
        parameter : [ 'display-mode' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_clock_display_mode',
        parameter : [ 'display-mode=$displayMode' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'tz_string',
      exampleValue          : 'CET-1CEST,M3.5.0/2,M10.5.0/3',
      configtoolReadParams  :
      {
        name      : 'get_clock_data',
        parameter : [ 'tz-string' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_timezone',
        parameter : [ 'tz-string=$tzString' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'timezone',
      exampleValue          : 'CET/CEST: Central European Time, B, DK, D, F, I, CRO, NL, ...',
      configtoolReadParams  :
      {
        name      : 'get_clock_data',
        parameter : [ 'timezone-string' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_timezone',
        parameter : [ 'timezone=$timezone' ]
      }
    });

	deviceParams.Add(
    {
      id                    : 'all-timezone-strings',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_clock_data',
        parameter : [ 'all-timezone-strings' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'timezone-list',
      exampleValue          : '',
      invariant             : true,
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_clock_data',
        parameter : [ 'timezone-strings-json' ]
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * user management
  * ---------------------------------------------------------------------------
  */
  var CreateUsersParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'config_user',
      configtoolWriteParams :
      {
        name          : 'config_user',
        parameter     : [ 'user=$user', 'new-password=$newPassword', 'confirm-password=$confirmPassword', 'old-password=$oldPassword' ]
       }
    });

    deviceParams.Add(
    {
      id                    : 'default_password_flag_admin',
      configtoolReadParams :
      {
        name          : 'get_user_info',
        parameter     : [ '--is-default-pwd', 'admin' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'default_password_flag_user',
      configtoolReadParams :
      {
        name          : 'get_user_info',
        parameter     : [ '--is-default-pwd', 'user' ]
      }
    });

})();


  /*---------------------------------------------------------------------------
  * serial interfaces
  * ---------------------------------------------------------------------------
  */
  var CreateSerialInterfacesParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'current_rs232_owner',
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'RS232-owner' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'configured_rs232_owner',
      configtoolReadParams  :
      {
        name      : 'get_coupler_details',
        parameter : [ 'RS232-owner-after-reboot' ],
        sudo      : true
      },
      configtoolWriteParams :
      {
        name      : 'config_RS232',
        parameter : [ 'owner=$owner' ],
        sudo      : true
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * filesystem
  * ---------------------------------------------------------------------------
  */
  var CreateFilesystemParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'active_partition',
      configtoolReadParams  :
      {
        name      : 'get_filesystem_data',
        parameter : [ 'active-partition' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'active_partition_medium',
      configtoolReadParams  :
      {
        name      : 'get_filesystem_data',
        parameter : [ 'active-partition-medium' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'active_device',
      configtoolReadParams  :
      {
        name      : 'get_filesystem_data',
        parameter : [ 'active-device' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'device_data_list',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_filesystem_data',
        parameter : [ 'device-data-list-json' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'device_medium_list',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_filesystem_data',
        parameter : [ 'medium-list-json' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'copy_firmware_image',
      configtoolWriteParams :
      {
        name      : 'copy_firmware_image',
        parameter : [ 'source-medium=$sourceMedium', 'destination-medium=$destinationMedium', 'action=stop-codesys', 'size=$size' ],
        sudo      : true,
        timeout   : (1000 * 60 * 10)
      }
    });

    deviceParams.Add(
    {
      id                    : 'fw_copy_nand_size_check',
      configtoolReadParams :
      {
        name      : 'copy_firmware_image',
        parameter : [ 'source-medium=$sourceMedium', 'destination-medium=$destinationMedium', 'action=nand-check-size' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'fw_copy_in_use_check',
      configtoolReadParams :
      {
        name      : 'copy_firmware_image',
        parameter : [ 'source-medium=$sourceMedium', 'destination-medium=$destinationMedium', 'action=check-in-use' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'format_medium',
      configtoolWriteParams :
      {
        name      : 'format_medium',
        parameter : [ 'device=$deviceName', 'volume-name=$volumeName', 'fs-type=$fsType' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'start_reboot',
      configtoolWriteParams :
      {
        name      : 'start_reboot',
        // don't send parameter "web" (causes a timeout befor reboot), it was only necessary for earlier wbm versions
        parameter : [ ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'switch_bootloader',
      configtoolWriteParams :
      {
        name      : 'switch_bootloader',
        parameter : [ ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'memory_card_image_min_size_mb',
      configtoolReadParams :
      {
        name      : 'get_min_sd_card_size',
        parameter : [ ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'memory_card_size_kb',
      configtoolReadParams :
      {
        name      : 'get_device_data',
        parameter : [ 'size', '$sdCardDeviceName' ]
        //sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'memory_card_device_name',
      configtoolReadParams :
      {
        name      : 'get_device_data',
        parameter : [ 'name', 'sd-card' ]
        //sudo      : true
      }
    });

    deviceParams.Add(
    {
      id:                   'systeminfo',
      format:               'json',
      configtoolReadParams:
      {
        name:       'get_systeminfo',
        parameter:  [ '-j' ]
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * backup and restore
  * ---------------------------------------------------------------------------
  */
  var CreateBackupAndRestoreParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'firmware_restore_packages',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_firmware_restore_packages',
        sudo      : true,
        multiline : true,
        timeout   : 20000
      }
    });

    deviceParams.Add(
    {
      id                    : 'process_firmware_restore',
      configtoolWriteParams  :
      {
        name      : 'firmware_restore',
        parameter : [ 'device-medium=$sourceDeviceMedium', 'upload-dir=$uploadDir',
                      'package-codesys=$packageCodesysFlag',
                      'package-settings=$packageSettingsFlag',
                      'package-system=$packageSystemFlag',
                      'passphrase=$encPassphrase' ],
        sudo      : true,
        timeout   : (1000 * 60 * 10)
      }
    });

    deviceParams.Add(
    {
      id                    : 'process_firmware_backup',
      configtoolWriteParams  :
      {
        name      : 'firmware_backup',
        parameter : [ 'device-medium=$destinationDeviceMedium', 'download-dir=$downloadDir', 'auto-update=$autoUpdateFlag',
                      'package-codesys=$packageCodesysFlag',
                      'package-settings=$packageSettingsFlag',
                      'package-system=$packageSystemFlag',
                      'passphrase=$encPassphrase' ],
        sudo      : true,
        timeout   : (1000 * 60 * 5) //0 //20000
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * package/software services
  * ---------------------------------------------------------------------------
  */
  var CreateSoftwarePackageParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'software_upload_update_script',
      exampleValue          : 'lighttpd_1.4.32_arm.ipk',
      configtoolReadParams  :
      {
        name      : 'get_wbm_uploads',
        parameter : [ 'update-script' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'activate_software_package',
      configtoolWriteParams :
      {
        name      : 'activate_download',
        parameter : [ 'download-type=$packageType' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'force_software_package',
      configtoolWriteParams :
      {
        name      : 'activate_download',
        parameter : [ 'download-type=$packageType', 'action=force' ]
      }
    });
    deviceParams.Add(
    {
      id                    : 'delete_software_package',
      configtoolWriteParams :
      {
        name      : 'activate_download',
        parameter : [ 'download-type=$packageType', 'action=delete' ]
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * network services
  * ---------------------------------------------------------------------------
  */
  var CreateNetworkServicesParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'telnet_state',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_port_state',
        parameter : [ 'telnet' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_port',
        parameter : [ 'port=telnet', 'state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'ftp_state',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'config_ssl',
        parameter : [ 'ftp-status' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_port',
        parameter : [ 'port=ftp', 'state=$state' ],
        sudo      : true
      }
    });


    deviceParams.Add(
    {
      id                    : 'ftps_state',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'config_ssl',
        parameter : [ 'ftps-status' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_port',
        parameter : [ 'port=ftps', 'state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'http_state',
      exampleValue          : 'enabled',
      configtoolReadParams  :
      {
        name      : 'config_ssl',
        parameter : [ 'http-status' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_port',
        parameter : [ 'port=http', 'state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'https_state',
      exampleValue          : 'enabled',
      configtoolReadParams  :
      {
        name      : 'config_ssl',
        parameter : [ 'https-status' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_port',
        parameter : [ 'port=https', 'state=$state' ],
        sudo      : true
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * NTP client
  * ---------------------------------------------------------------------------
  */
  var CreateNtpClientParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'ntp_client_state',
      exampleValue          : 'enabled',
      configtoolReadParams  :
      {
        name      : 'get_ntp_config',
        parameter : [ 'state' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_sntp',
        parameter : [ 'state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'ntp_client_active_state',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_ntp_config',
        parameter : [ 'active-state' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'ntp_client_port',
      exampleValue          : '123',
      configtoolReadParams  :
      {
        name      : 'get_ntp_config',
        parameter : [ 'port' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_sntp',
        parameter : [ 'port=$port' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'ntp_client_time_server',
      exampleValue          : '192.168.1.50',
      configtoolReadParams  :
      {
        name      : 'get_ntp_config',
        parameter : [ 'time-server' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_sntp',
        parameter : [ 'time-server=$timeServer' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'ntp_client_update_time',
      exampleValue          : 'enabled',
      configtoolReadParams  :
      {
        name      : 'get_ntp_config',
        parameter : [ 'update-time' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_sntp',
        parameter : [ 'update-time=$updateTime' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      // set only config parameters - not the state of service
      id                    : 'ntp_client_parameter',
      configtoolWriteParams :
      {
        name      : 'config_sntp',
        //parameter : [ 'state=$state', 'time-server=$timeServer', 'update-time=$updateTime' ],
        parameter : [ 'port=$port', 'time-server=$timeServer', 'update-time=$updateTime' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      // for parameters AND service state
      id                    : 'ntp_client_configuration',
      format                : 'json',
      configtoolReadParams:
      {
        name      : 'get_ntp_config',
        parameter : [ '-j' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_sntp',
        parameter : [ 'state=$state', 'update-time=$updateTime', 'time-server-1=$timeServer1', 'time-server-2=$timeServer2', 'time-server-3=$timeServer3', 'time-server-4=$timeServer4' ],
        sudo      : true,
        timeout   : 20000
      }
    });

    deviceParams.Add(
    {
      id                    : 'ntp_client_time_servers_currently_used',
      format                : 'json',
      configtoolReadParams:
      {
        name      : 'get_ntp_config',
        parameter : [ 'get-complete' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'ntp_client_trigger',
      configtoolWriteParams :
      {
        name      : 'config_sntp',
        parameter : [ 'update' ],
        sudo      : true,
        timeout   : 20000
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * SSH
  * ---------------------------------------------------------------------------
  */
  var CreateSshParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'ssh_state',
      configtoolReadParams  :
      {
        name      : 'get_ssh_config',
        parameter : [ 'state' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_ssh',
        parameter : [ 'state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'ssh_root_access_state',
      configtoolReadParams  :
      {
        name      : 'get_ssh_config',
        parameter : [ 'root-access-state' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'ssh_password_request_state',
      configtoolReadParams  :
      {
        name      : 'get_ssh_config',
        parameter : [ 'password-request-state' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'ssh_port_number',
      configtoolReadParams  :
      {
        name      : 'get_ssh_config',
        parameter : [ 'port-number' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'ssh_parameter',
      configtoolWriteParams :
      {
        name      : 'config_ssh',
        parameter : [ 'state=$state', 'root-access-state=$rootAccessState', 'password-request-state=$passwordRequestState', 'port-number=$portNumber' ],
        //parameter : [ 'root-access-state=$rootAccessState', 'password-request-state=$passwordRequestState', 'port-number=$portNumber' ],
        sudo      : true
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * TFTP
  * ---------------------------------------------------------------------------
  */
  var CreateTftpParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'tftp_state',
      configtoolReadParams  :
      {
        name      : 'get_tftp_config',
        parameter : [ 'state' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_tftp',
        parameter : [ 'state=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'tftp_download_dir',
      configtoolReadParams  :
      {
        name      : 'get_tftp_config',
        parameter : [ 'download-dir' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_tftp',
        parameter : [ 'download-dir=$downloadDir' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'tftp_config',
      configtoolWriteParams :
      {
        name      : 'config_tftp',
        parameter : [ 'state=$state', 'download-dir=$downloadDir' ],
        sudo      : true
      }
    });

  })();




  /*---------------------------------------------------------------------------
  * DHCPD
  * ---------------------------------------------------------------------------
  */
  var CreateDhcpdParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'dhcpd_config',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_dhcpd_config',
        parameter : [ '-j' ]
      },
      configtoolWriteParams  :
      {
        name      : 'config_dhcpd',
        parameter : [ '--port=$port', 'dhcpd-state=$state', 'dhcpd-range=$range', 'dhcpd-lease-time=$leaseTime', 'dhcpd-fix-host=$fixHost' ]
        // no sudo necessary
      }
    });

    deviceParams.Add(
    {
      id                    : 'dhcpd_basic_config',
      configtoolWriteParams  :
      {
        name      : 'config_dhcpd',
        parameter : [ '--port=$port', 'dhcpd-state=$state', 'dhcpd-range=$range', 'dhcpd-lease-time=$leaseTime' ]
                // no sudo necessary
      }
    });

    deviceParams.Add(
    {
      id                    : 'dhcpd_fix_hosts',
      configtoolWriteParams  :
      {
        name      : 'config_dhcpd',
        parameter : [ '--port=$port', 'dhcpd-fix-host=$fixHost' ]
        // no sudo necessary
      }
    });

  })();

  /*---------------------------------------------------------------------------
  * DNS service
  * ---------------------------------------------------------------------------
  */
  var CreateDnsServiceParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'dns_service_config',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_dns_service_config',
        parameter : [ '-j' ]
      },
      configtoolWriteParams  :
      {
        name      : 'config_dns_service',
        parameter : [ 'dns-state=$state', 'dns-mode=$mode', 'dns-fix-host=$fixHost' ]
        // no sudo necessary
      }
    });

    deviceParams.Add(
    {
      id                    : 'dns_service_basic_config',
      configtoolWriteParams  :
      {
        name      : 'config_dns_service',
        parameter : [ 'dns-state=$state', 'dns-mode=$mode' ]
        // no sudo necessary
      }
    });

    deviceParams.Add(
    {
      id                    : 'dns_fix_hosts',
      configtoolWriteParams  :
      {
        name      : 'config_dns_service',
        parameter : [ 'dns-fix-host=$fixHost' ]
        // no sudo necessary
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * I/O-Check
  * ---------------------------------------------------------------------------
  */
  var CreateIocheckParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'iocheck_state',
      configtoolReadParams  :
      {
        name      : 'get_iocheckport_config',
        parameter : [ 'state' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_iocheckport',
        parameter : [ 'state=$state' ],
        sudo      : true
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * MODBUS services
  * ---------------------------------------------------------------------------
  */
  var CreateModbusServicesParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'modbus_tcp_service_state',
      configtoolReadParams  :
      {
        name      : 'modbus_config',
        parameter : [ 'get', 'tcp', 'enabled' ],
        sudo      : true
      },
      configtoolWriteParams :
      {
        name      : 'modbus_config',
        parameter : [ 'set', 'tcp', 'enabled', '$bEnabled' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'modbus_udp_service_state',
      configtoolReadParams  :
      {
        name      : 'modbus_config',
        parameter : [ 'get', 'udp', 'enabled' ],
        sudo      : true
      },
      configtoolWriteParams :
      {
        name      : 'modbus_config',
        parameter : [ 'set', 'udp', 'enabled', '$bEnabled' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'modbus_service_state',
      configtoolWriteParams :
      {
        name      : 'modbus_config',
        parameter : [ 'set', '$modbusType', 'enabled', '$bEnabled' ],
        sudo      : true
      }
    });

})();


  /*---------------------------------------------------------------------------
  * SNMP
  * ---------------------------------------------------------------------------
  */
  var CreateSnmpParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'snmp_device_name',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'device-name' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'device-name=$deviceName' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_state',
      configtoolReadParams  :
      {
        name      : 'get_port_state',
        parameter : [ 'snmp' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter : [ 'set-snmp=$state' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_description',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'description' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'description=$description' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_physical_location',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'physical-location' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'physical-location=$physicalLocation' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_contact',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'contact' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'contact=$contact' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_objectID',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'objectID' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'objectID=$objectID' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_general_config',
      exampleValue          : '',
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'set-snmp=$state', 'device-name=$deviceName', 'description=$description', 'physical-location=$physicalLocation', 'contact=$contact', 'objectID=$objectID' ],
        sudo      : true
      }
    });


    deviceParams.Add(
    {
      id                    : 'snmp_v1_v2c_state',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'v1-v2c-state' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_v1_v2c_community_name',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'v1-v2c-community-name' ]
      },
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'v1-v2c-community-name=$communityName' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_v1_v2c_configuration',
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'v1-v2c-state=$state', 'v1-v2c-community-name=$communityName' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_v1_v2c_trap_receiver_list',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'v1-v2c-trap-receiver-list-json' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_edit_v1_v2c_trap_receiver',
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'v1-v2c-trap-receiver-edit=$action', 'v1-v2c-trap-receiver-address=$address',
                      'v1-v2c-trap-receiver-community-name=$communityName', 'v1-v2c-trap-receiver-version=$version' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_v3_user_list',
      format                : 'json',
      configtoolReadParams  :
      {
        name      : 'get_snmp_data',
        parameter : [ 'v3-user-list-json' ],
        timeout   : 200000
      }
    });

    deviceParams.Add(
    {
      id                    : 'snmp_edit_v3_user',
      configtoolWriteParams :
      {
        name      : 'config_snmp',
        parameter:  [ 'v3-edit=$action', 'v3-auth-name=$authName', 'v3-auth-type=$authType', 'v3-auth-key=$authKey',
                      'v3-privacy=$privacy', 'v3-privacy-key=$privacyKey', 'v3-notification-receiver=$notificationReceiver' ],
        sudo      : true
      }
    });

  })();


  /*---------------------------------------------------------------------------
  * PROFIBUS
  * ---------------------------------------------------------------------------
  */
  var CreateProfibusParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'pbdp_ssa_user_released',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'pbdp_config',
        parameter : [ 'get', 'ssa.user', 'ssa_released' ],
        sudo      : true
      },
      configtoolWriteParams :
      {
        name      : 'pbdp_config',
        parameter : [ 'set', 'ssa.user', 'ssa_released', '$ssaReleased' ],
        sudo      : true
      }
    });

  deviceParams.Add(
    {
      id                    : 'pbdp_ssa_user_slave_address',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'pbdp_config',
        parameter : [ 'get', 'ssa.user', 'slave_address' ],
        sudo      : true
      },
      configtoolWriteParams :
      {
        name      : 'pbdp_config',
        parameter : [ 'set', 'ssa.user', 'slave_address', '$slaveAddress'],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'pbdp_ssa_user_slave_address_by_plc',
      exampleValue          : '',
      configtoolReadParams  :
      {
        name      : 'pbdp_config',
        parameter : [ 'get', 'plc.user', 'slave_address' ],
        sudo      : true
      },
      configtoolWriteParams :
      {
        name      : 'pbdp_config',
        parameter : [ 'set', 'plc.user', 'slave_address', '$slaveAddress'],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'pbdp_ssa_factory_released',
      exampleValue          : '',
      invariant             : true,
      configtoolReadParams  :
      {
        name      : 'pbdp_config',
        parameter : [ 'get', 'ssa.factory', 'ssa_released' ],
        sudo      : true
      }
    });

    deviceParams.Add(
    {
      id                    : 'pbdp_ssa_factory_slave_address',
      exampleValue          : '',
      invariant             : true,
      configtoolReadParams  :
      {
        name      : 'pbdp_config',
        parameter : [ 'get', 'ssa.factory', 'slave_address' ],
        sudo      : true
      }
    });

  })();
  
  /*---------------------------------------------------------------------------
  * diagnostic
  * ---------------------------------------------------------------------------
  */
  var CreateDiagnosticParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'diaglist',
      configtoolReadParams  :
      {
        name      : 'get_wbm_diaglist',
        parameter : [ '$diag_count' ],
        multiline : true,
        sudo      : true
      }
    });
  })();

  /*---------------------------------------------------------------------------
   * modem
   * ---------------------------------------------------------------------------
   */
   var CreateModemParams = (function()
   {
     deviceParams.Add(
     {
       id                    : 'modem_info',
       format                : 'json',
       configtoolReadParams  :
       {
         name      : 'config_mdmd',
         parameter : [ '-j', '-d' ]
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_sim_config',
       format                : 'json',
       configtoolReadParams  :
       {
         name      : 'config_mdmd',
         parameter : [ '-j', '-S' ]
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_sim_pin',
       configtoolWriteParams :
       {
         name      : 'config_mdmd',
         parameter : [ '--set', '--SIM', 'SimPin=$pin' ]
         //sudo      : true
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_sim_puk',
       configtoolWriteParams :
       {
         name      : 'config_mdmd',
         parameter : [ '--set', '--SIM', 'SimPuk=$puk', 'SimPin=$pin' ]
         //sudo      : true
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_providerlist',
       format                : 'json',
       configtoolReadParams  :
       {
         name      : 'config_mdmd',
         parameter : [ '-j', '-l' ],
         timeout   : (1000 * 60 * 4)  // 4 minutes
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_gprs_config',
       format                : 'json',
       configtoolReadParams  :
       {
         name      : 'config_mdmd',
         parameter : [ '-j', '-G' ]
       },
       configtoolWriteParams :
       {
         name       : 'config_mdmd',
         parameter  : [ '--set', '--gprsaccess', 'GprsAccessPointName=$apn', 'GprsAuthType=$authType', 'GprsUserName=$userName', 'GprsPassword=$password', 'GprsConnectivity=$connectivity']
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_network_config',
       format                : 'json',
       configtoolReadParams  :
       {
         name      : 'config_mdmd',
         parameter : [ '-j', '-n' ]
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_network_selection_mode',
       configtoolWriteParams  :
       {
         name      : 'config_mdmd',
         parameter : [ '--set', '--network', 'NetworkSelection=$selectionMode' ]
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_network_provider',
       configtoolWriteParams  :
       {
         name      : 'config_mdmd',
         parameter : [ '--set', '--network', 'NetworkSelection=MANUAL', 'NetworkId=$networkId', 'NetworkType=$networkType' ]
       }
     });

     deviceParams.Add(
     {
       id                    : 'modem_firmware',
       configtoolWriteParams :
       {
         name      : 'update_modem_firmware',
         parameter : [ '$modemFirmwarePath', '$logfilePath' ],
         sudo      : true,
         timeout   : (1000 * 60 * 15)  // 15 minutes
       }
     });
     
     deviceParams.Add(
     {
       id          : 'wwan_status',
       configtoolReadParams:
       {
         name      : 'config_wwan',
         parameter : [ '-s' ]
       }
     });

   })();
   
   
   /*---------------------------------------------------------------------------
    * BACnet
    * ---------------------------------------------------------------------------
    */
    var CreateBacnetParams = (function()
    {
    
      deviceParams.Add(
      {
        id                    : 'bacnet_device_id',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-g', 'device-id' ],
          sudo      : true
        }
      });

      deviceParams.Add(
      {
        id                    : 'bacnet_udp_port',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-g', 'udp-port' ],
          sudo      : true
        },
        configtoolWriteParams :
        {
          name      : 'bacnet_config',
          parameter : [ '-s', 'udp-port', '-v', '$udpPort' ],
          sudo      : true
        }
      });
  
      deviceParams.Add(
      {
        id                    : 'bacnet_who_is_online_interval',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-g', 'who-is-online-interval' ],
          sudo      : true
        },
        configtoolWriteParams :
        {
          name      : 'bacnet_config',
          parameter : [ '-s', 'who-is-online-interval', '-v', '$whoIsOnlineInterval' ],
          sudo      : true
        }
      });
      
      deviceParams.Add(
      {
        id                    : 'bacnet_persistence_delete',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-g', 'persistence-delete' ],
          sudo      : true
        },
        configtoolWriteParams :
        {
          name      : 'bacnet_config',
          parameter : [ '-s', 'persistence-delete', '-v', '$persDelState' ],
          sudo      : true
        }
      });
      
      deviceParams.Add(
      {
        id                    : 'bacnet_delete_all',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-g', 'delete-all' ],
          sudo      : true
        },
        configtoolWriteParams :
        {
          name      : 'bacnet_config',
          parameter : [ '-s', 'delete-all', '-v', '$delAllState' ],
          sudo      : true
        }
      });
      
      deviceParams.Add(
      {
        id                    : 'bacnet_wbm_upload',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-u', 'info' ],
          sudo      : true
        },
        configtoolWriteParams :
        {
          name      : 'bacnet_config',
          parameter : [ '-u', '$uploadFunc' ],
          sudo      : true
        }
      });
  
      deviceParams.Add(
      {
        id                    : 'bacnet_persistence_storage',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-g', 'persistence-storage' ],
          sudo      : true
        },
        configtoolWriteParams :
        {
          name      : 'bacnet_config',
          parameter : [ '-s', 'persistence-storage', '-v', '$persistenceStorage' ],
          sudo      : true
        }
      });
  
      deviceParams.Add(
      {
        id                    : 'bacnet_trendlog_storage',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-g', 'trendlog-storage' ],
          sudo      : true
        },
        configtoolWriteParams :
        {
          name      : 'bacnet_config',
          parameter : [ '-s', 'trendlog-storage', '-v', '$trendlogStorage' ],
          sudo      : true
        }
      });
      
      deviceParams.Add(
      {
        id                    : 'bacnet_eventlog_storage',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-g', 'eventlog-storage' ],
          sudo      : true
        },
        configtoolWriteParams :
        {
          name      : 'bacnet_config',
          parameter : [ '-s', 'eventlog-storage', '-v', '$eventlogStorage' ],
          sudo      : true
        }
      });
      
      deviceParams.Add(
      {
        id                    : 'bacnet_storage_state',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'bacnet_config',
          parameter : [ '-i' ],
          sudo      : true
        }
      });
      
      deviceParams.Add(
      {
        id                    : 'bacnet_diaglist',
        configtoolReadParams  :
        {
          name      : 'bacnet_wbm_diaglist',
          parameter : [ '$diag_count' ],
          multiline : true,
          sudo      : true
        }
      });

    })();
   

   /*---------------------------------------------------------------------------
    * OpenVPN / IpSec
    * ---------------------------------------------------------------------------
    */
   var CreateVpnParams = (function()
   {
     deviceParams.Add(
     {
       id                    : 'vpn_config',
       format                : 'json',
       configtoolReadParams  :
       {
         name      : 'vpncfg',
         multiline : true,
         parameter : [ 'vpn', '--get-full-status' ],
         sudo      : true
       }
     });

     deviceParams.Add(
     {
       id                    : 'vpn_openvpn_state',
       configtoolWriteParams  :
       {
         name      : 'vpncfg',
         parameter : [ 'ovpn', '$state' ],
         sudo      : true
       }
     });

     deviceParams.Add(
     {
       id                    : 'vpn_ipsec_state',
       configtoolWriteParams  :
       {
         name      : 'vpncfg',
         parameter : [ 'ipsec', '$state' ],
         sudo      : true
       }
     });

     deviceParams.Add(
     {
       id                    : 'vpn_store_config_file',
       configtoolWriteParams  :
       {
         name      : 'vpncfg',
         parameter : [ 'file', '--store', '$configFileType', '$filename' ],
         sudo      : true
       }
     });

     deviceParams.Add(
     {
       id                    : 'vpn_copy_config_file',
       configtoolWriteParams :
       {
         name      : 'vpncfg',
         parameter : [ 'file', '--copy', '$configFileType', 'www', 'www' ],
         sudo      : true
       }
     });

     deviceParams.Add(
     {
       id                    : 'vpn_delete_certificate',
       configtoolWriteParams  :
       {
         name      : 'vpncfg',
         parameter : [ 'file', '--remove', '$fileType', '$filename' ],
         sudo      : true
       }
     });

   })();

   /*---------------------------------------------------------------------------
    * service interface
    * ---------------------------------------------------------------------------
    */
   var CreateServiceInterfaceParams = (function()
   {
     deviceParams.Add(
     {
       id                    : 'service_interface_config',
       format                : 'json',
       configtoolReadParams  :
       {
         name      : 'get_service_interface_config',
         parameter : [ '-j', 'mode' ],
         sudo            : true
       }
     });

     deviceParams.Add(
     {
       id                    : 'service_interface_configured_mode',
       configtoolWriteParams :
       {
         name      : 'config_service_interface',
         parameter : [ 'mode=$mode' ]
       }
     });

   })();

   /*---------------------------------------------------------------------------
    * TLS
    * ---------------------------------------------------------------------------
    */
   var CreateTlsParams = (function()
   {
     deviceParams.Add(
     {
       id                    : 'tls_cipher_list_class',
       configtoolReadParams  :
       {
         name      : 'get_https_tls_config',
         parameter : [ 'cipher-list' ]
       },
       configtoolWriteParams :
       {
         name      : 'config_https_tls',
         parameter : [ 'cipher-list=$cipherListClass', 'restart-webserver=NO' ]
       }
     });

   })();

  /* -----------------------------------------------------------------------------
   * Integrity
   * ---------------------------------------------------------------------------
   */
  var CreateIntegrityParams = (function()
  {
    deviceParams.Add(
    {
      id                    : 'integrity_check_state',
      configtoolReadParams  :
      {
        name      : 'manage_integrity',
        parameter : [ 'status' ],
        multiline : false
      },
      configtoolWriteParams :
      {
        name      : 'manage_integrity',
        parameter : [ '$integrity_check_state' ]
      }
    });
    deviceParams.Add(
    {
      id                    : 'integrity_log',
      configtoolReadParams  :
      {
        name      : 'manage_integrity',
        parameter : [ '$integrity_action', '$integrity_count' ],
        multiline : true,
        sudo      : true,
        timeout   : (1000 * 60 * 5)  // 5 minutes
      }
    });

  })();

   /*---------------------------------------------------------------------------
    * DNP3-Service Availability (for Telecontrol Devices)
    * ---------------------------------------------------------------------------
    */
    var CreateTelecontrolParams = (function()
    {
      deviceParams.Add(
      {
        id                    : 'dnp3_state',
        configtoolReadParams  :
        {
          name      : 'get_telecontrol_state',
          parameter : [ 'service=dnp3' ]
        }
      });
      deviceParams.Add(
      {
        id                    : 'iec60870_5_104_state',
        configtoolReadParams  :
        {
          name      : 'get_telecontrol_state',
          parameter : [ 'service=iec60870_5_104' ]
        }
      });
      deviceParams.Add(
      {
        id                    : 'iec61850_mms_state',
        configtoolReadParams  :
        {
          name      : 'get_telecontrol_state',
          parameter : [ 'service=iec61850_mms' ]
        }
      });
    })();


    /*---------------------------------------------------------------------------
    * opc ua server - network service
    * ---------------------------------------------------------------------------
    */
    var CreateOpcUaServerParams = (function()
    {
      deviceParams.Add(
      {
        id                    : 'opcua_state',
        exampleValue          : '',
        configtoolReadParams  :
        {
          name      : 'config_opcua',
          parameter : [ 'state' ]
        },
        configtoolWriteParams :
        {
          name      : 'config_opcua',
          parameter : [ 'state=$state' ]
        }
      });
    })();


    /*---------------------------------------------------------------------------
    * routing
    * ---------------------------------------------------------------------------
    */
    var CreateOpcUaServerParams = (function()
    {
      deviceParams.Add(
        {
          id                    : 'routing_config',
          exampleValue          : '',
          configtoolReadParams  :
          {
            name      : 'config_routing',
            parameter : [ '-g', 'all', '-j' ]
          }
        });
      deviceParams.Add(
        {
          id                    : 'routing_state',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-c', 'general', 'state=$state' ]
          }
        });
      deviceParams.Add(
        {
          id                    : 'routing_add_static_route',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-a', 'static', 'state=$state', 'dest=$destination', 'dest-mask=$destinationMask', 'gw=$gateway', 'metric=$gatewayMetric' ]
          }
        });
      deviceParams.Add(
          {
            id                    : 'routing_change_static_route',
            exampleValue          : '',
            configtoolWriteParams :
            {
              name      : 'config_routing',
              parameter : [ '-c', 'static', 'index=$index', 'state=$state', 'dest=$destination', 'dest-mask=$destinationMask', 'gw=$gateway', 'metric=$gatewayMetric' ]
            }
          });
      deviceParams.Add(
        {
          id                    : 'routing_delete_static_route',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-d', 'static', 'index=$index' ]
          }
        });
      deviceParams.Add(
        {
          id                    : 'routing_add_masquerading',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-a', 'masquerading', 'interface=$interface', 'state=$state' ]
          }
        });
      deviceParams.Add(
        {
          id                    : 'routing_delete_masquerading',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-d', 'masquerading', 'index=$index' ]
          }
        });
      deviceParams.Add(
        {
          id                    : 'routing_change_masquerading',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-c', 'masquerading', 'index=$index', 'state=$state' ]
          }
        });
      deviceParams.Add(
        {
          id                    : 'routing_add_forwarding',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-a', 'forwarding', 'state=$state', 'interface=$interface', 'port=$port', 'protocol=$protocol', 'forward-ip=$forwardIp', 'forward-port=$forwardPort' ]
          }
        });
      deviceParams.Add(
        {
          id                    : 'routing_delete_forwarding',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-d', 'forwarding', 'index=$index' ]
          }
        });
      deviceParams.Add(
        {
          id                    : 'routing_change_forwarding',
          exampleValue          : '',
          configtoolWriteParams :
          {
            name      : 'config_routing',
            parameter : [ '-c', 'forwarding', 'index=$index', 'state=$state' ]
          }
        });

    })();

    /*---------------------------------------------------------------------------
    * Feature Detection
    * ---------------------------------------------------------------------------
    */
    var CreateFeatureDetectionParam = function(paramId, featureId)
    {
      deviceParams.Add(
      {
        id                    : paramId,
        invariant             : true,
        configtoolReadParams:
        {
          name      : 'detectfeature',
          parameter : [ featureId ]
        }
      });
    };


    /*---------------------------------------------------------------------------
    * Optional OPC UA IPKG settings
    * ---------------------------------------------------------------------------
    */
    CreateFeatureDetectionParam('opcua_feature', 'opcua-server');

    var CreateOpcUaIpkParams = (function()
    {
      deviceParams.Add(
      {
        id                    : 'opcua_available',
        invariant             : true,
        configtoolReadParams:
        {
          name      : 'detectfeature',
          parameter : [ 'opcua-server' ]
        }
      });

      deviceParams.Add(
      {
        id                    : 'opcua_config',
        format                : 'json',
        configtoolReadParams:
        {
          name      : 'config-opcua',
          parameter : [ '-G' ]
        },
        configtoolWriteParams:
        {
          name      : 'config-opcua',
          parameter : [
            '-S=$configOpcua',
            '-r',
            '--url'
          ]
        }
      });
    })();


    /*---------------------------------------------------------------------------
    * OPKG
    * ---------------------------------------------------------------------------
    */
    CreateFeatureDetectionParam('opkg_feature','feature_opkglist');

    var CreateOpkgPArams = (function()
    {
      deviceParams.Add(
        {
          id		 	  :  'opkglist',
	  configtoolReadParams  :
	  {
	    name	:  'get_wbm_opkglist',
	    parameter	:  [ '$opkg_list_val' ],
	    multiline	:  true,
	    sudo	:  true
	  }
        });

      deviceParams.Add(
        {
	  id		 	  :  'opkgfkt',
	  configtoolReadParams  :
	  {
	    name	:  'get_wbm_opkglist',
	    parameter	:  [ '$opkg_fkt_val','$opkg_pkg_val' ],
	    multiline	:  true,
	    sudo	:  true
	  }
        });
    })();

})();
