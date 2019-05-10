<?php

function AccessAllowed($configtool, $username = "")
{
  // userlevel => configtool names
  $accessRights = array(
    USERLEVEL_ADMIN => array(
      'get_user_info',
      'get_wbm_uploads',
      'get_snmp_data',
      
      'config_snmp',
      
      'start_reboot',
      'config_user',
      'config_RS232',
      'copy_firmware_image',
      'switch_bootloader',
      'format_medium',
      'firmware_backup',
      'firmware_restore',
      'activate_download',
      'config_service_interface',
      'get_service_interface_config',
      'config_opcua',
      'config-opcua',
      'get_dataagent_config',
      'config_dataagent',
      'bacnet_config',
      'bacnet_wbm_diaglist',
      'get_wbm_opkglist'
    ),
    USERLEVEL_USER  => array(
      'get_telecontrol_state',
      'get_rts3scfg_value',
      'get_default_gateway_config',
      'get_dns_server',
      'get_dns_service_config',
      'get_rts3scfg_value',
      'get_filesystem_data',
      'get_firmware_restore_packages',
      'get_port_state',
      'get_iocheckport_config',
      'config_iocheckport',
      'config_ssl',
      'get_ntp_config',
      'get_ssh_config',
      'get_tftp_config',
      'get_dhcpd_config',
      'pbdp_config',
      'get_switch_settings',
      
      'config_plc',
      'config_port',
      'change_rts_config',
      'config_dns_service',
      'config_runtime',
      'config_linux_user',
      'config_interfaces',
      'config_routing',
      'set_dsa_mode',
      'config_default_gateway',
      'config_switch',
      'config_ethernet',
      'edit_dns_server',
      'change_hostname',
      'firewall',
      'firewall_apply.sh',
      'config_clock',
      'config_clock_display_mode',
      'config_timezone',
      'config_port',
      'config_sntp',
      'config_ssh',
      'config_tftp',
      'config_dhcpd',
      'config_mdmd',
      'modbus_config',
      'config_wwan',
      
      'restart_webserver'
    ),
    USERLEVEL_GUEST => array(
      'get_coupler_details',
      'get_typelabel_value',
      'get_runtime_config',
      'get_clock_data',
      'get_eth_config',
      'get_actual_eth_config',
      'get_plc_config',
      'get_run_stop_switch_value',
      'get_led_config',
      'get_rts_info',
      'get_port_state',
      'get_wbm_diaglist',
      'get_possible_runtimes',
      'detectfeature'
    )
  );

  $accessAllowed = false;

  switch($username)
  {
    case 'user':   $userlevel = USERLEVEL_USER; break;
    case 'admin':  $userlevel = USERLEVEL_ADMIN; break;
    default:       $userlevel = USERLEVEL_GUEST; break;
  }

  if((USERLEVEL_GUEST <= $userlevel) && in_array($configtool, $accessRights[USERLEVEL_GUEST]))
  {
    $accessAllowed = true;
  }
  else if((USERLEVEL_USER <= $userlevel) && in_array($configtool, $accessRights[USERLEVEL_USER]))
  {
    $accessAllowed = true;
  }

  // access for admin always allowed
  else if((USERLEVEL_ADMIN <= $userlevel)) // && in_array($configtool, $accessRights[USERLEVEL_ADMIN]))
  {
    $accessAllowed = true;
  }

  return $accessAllowed;
}

?>
