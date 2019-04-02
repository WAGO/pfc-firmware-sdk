/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function FirewallGeneralConfigContent(id)
{
  var firewallGeneralConfigContent = this;

  firewallGeneralConfigContent.id = id || 'firewall_general_configuration';

  firewallGeneralConfigContent.serviceList = [
    { id: 'telnet',           outputText: 'Telnet' },
    { id: 'ftp',              outputText: 'FTP' },
    { id: 'ftps',             outputText: 'FTPS' },
    { id: 'http',             outputText: 'HTTP' },
    { id: 'https',            outputText: 'HTTPS' },
    { id: 'iocheck',          outputText: 'I/O-Check' },
    { id: 'codesysr',         outputText: 'PLC Runtime' },
    { id: 'codesysw',         outputText: 'PLC WebVisu - direct link (port 8080)' },
    { id: 'ssh',              outputText: 'SSH' },
    { id: 'tftp',             outputText: 'TFTP' },
    { id: 'dhcpd',            outputText: 'BootP/DHCP' },
    { id: 'dns',              outputText: 'DNS' },
    { id: 'modbus_tcp',       outputText: 'Modbus® TCP' },
    { id: 'modbus_udp',       outputText: 'Modbus® UDP' },
    { id: 'snmp',             outputText: 'SNMP' },
    { id: 'opcua',            outputText: 'OPC UA' }
  ];
  firewallGeneralConfigContent.telecontrolServiceList = [
    { id: 'dnp3',             outputText: 'DNP3 (port 20000)' },
    { id: 'iec60870_5_104',   outputText: 'IEC60870-5-104 (port 2404)' },
    { id: 'iec61850_mms',     outputText: 'IEC61850 (port 102)' }
  ];


  var CreatefirewallGeneralConfigurationParamView = (function()
  {
    firewallGeneralConfigContent.paramView = new DeviceParamView();

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'ebtables_config'
    });

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'iptables_config'
    });

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'interface_config_xml'
    });

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'firewall_state'
    });

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'firewall_service_config'
    });

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'dnp3_state'
    });

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'iec61850_mms_state'
    });

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'iec60870_5_104_state'
    });
    

    firewallGeneralConfigContent.paramView.Add(
    {
      paramId         : 'wwan_status'
    });

  })();


  //------------------------------------------------------------------------------
  //first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(firewallGeneralConfigContent.id + '_create', function(event)
  {
    // change global firewall state
    $('#' + firewallGeneralConfigContent.id + '_content #form_firewall_state').bind('submit', function(event)
    {
      firewallGeneralConfigContent.ProcessChangeState(event);
    });

    // change icmp echo broadcast protection
    $('#' + firewallGeneralConfigContent.id + '_content #form_firewall_icmp_echo_broadcast_protection').bind('submit', function(event)
    {
      firewallGeneralConfigContent.ProcessIcmpBroadcastProtectionSubmit(event);
    });

    // change limits
    $('#' + firewallGeneralConfigContent.id + '_content #form_firewall_limit_configuration').bind('submit', function(event)
    {
      firewallGeneralConfigContent.ProcessChangeLimitsSubmit(event);
    });

    // change some of the configurations for (unknown count) of single interface
    $('#' + firewallGeneralConfigContent.id + '_content #firewall_interface_general_config_area').bind('submit', function(event)
    {
      // change interface firewall state
      if($(event.target).attr('id').match('form_firewall_interface_state'))
      {
        firewallGeneralConfigContent.ProcessChangeInterfaceStateSubmit(event);
      }

      // change interface icmp echo configuration
      else if($(event.target).attr('id').match('form_firewall_interface_icmp_config'))
      {
        firewallGeneralConfigContent.ProcessChangeInterfaceIcmpConfigSubmit(event);
      }

      // change service state
      else if($(event.target).attr('id').match('form_firewall_interface_service_config'))
      {
        firewallGeneralConfigContent.ProcessChangeInterfaceServiceStateSubmit(event);
      }
    });

  });


  //------------------------------------------------------------------------------
  //refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(firewallGeneralConfigContent.id + '_refresh', function(event)
  {
    firewallGeneralConfigContent.Refresh();
  });

};


FirewallGeneralConfigContent.prototype.Refresh = function()
{
  var firewallGeneralConfigContent = this;

  deviceParams.ReadValueGroup(firewallGeneralConfigContent.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(firewallGeneralConfigContent.paramView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(firewallGeneralConfigContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading firewall configuration.', errorString ] );
    }
    else
    {
      // show firewall state (only value which is not includes in xml)
      $('#' + firewallGeneralConfigContent.id + '_content #firewall_state').removeAttr('checked');
      if('enabled' === deviceParams.list['firewall_state'].value)
      {
        $('#' + firewallGeneralConfigContent.id + '_content #firewall_state').prop('checked', 'checked');
      }

      // remove old content
      //$("#firewall_interface_state_area .config_form_box").html('');
      $('#firewall_interface_general_config_area').html('');

      // create html areas for all configured interfaces
      var interfaceConfig = deviceParams.list['interface_config_xml'].value;
      var ebtables        = deviceParams.list['ebtables_config'].value;
      var iptables        = deviceParams.list['iptables_config'].value;

      var telecontrolStates = [
        deviceParams.list['dnp3_state'].value,
        deviceParams.list['iec60870_5_104_state'].value,
        deviceParams.list['iec61850_mms_state'].value
      ];

      firewallGeneralConfigContent.ShowIcmpBroadcastProtectionConfig(iptables);
      firewallGeneralConfigContent.ShowLimitsConfig(iptables);

      // create html areas for all configured eth interfaces
      $.each(interfaceConfig.ifaces, function(deviceId, iFace)
      {
        if(iFace.ipSettings) // && iFace.ipSettings.showInWbm)
        {
          var iFaceId = iFace.ipSettings.portName;

          $.each(firewallGeneralConfigContent.telecontrolServiceList, function(index, telecontrolService) {
            // indexOf is not working as expected in IE here... 
            //var service_index = firewallGeneralConfigContent.serviceList.indexOf(telecontrolService);
            var alreadyExists = $.grep(firewallGeneralConfigContent.serviceList, function (service) {
              return service.id == telecontrolService.id;
            }).length > 0;

            if (telecontrolStates[index] === 'enabled') {
              if (!alreadyExists) {
                firewallGeneralConfigContent.serviceList.push(telecontrolService)
              }
            } else if (alreadyExists) {
              firewallGeneralConfigContent.serviceList.splice(service_index, 1);
            }
          })

          var iFaceHtmlFrame = firewallGeneralConfigContent.InterfaceHtmlFrame(iFace.ipSettings.portName);
          $('#firewall_interface_general_config_area').append(iFaceHtmlFrame);

          firewallGeneralConfigContent.ShowFirewallStateForInterface(iFaceId, iptables);
          firewallGeneralConfigContent.ShowIcmpConfigForInterface(iFaceId, iptables);
          firewallGeneralConfigContent.ShowServiceConfigForInterface(iFaceId);
        }
      });

      // in case of only one eth interface, change interface id to X1/X2
      if(1 == $('#' + firewallGeneralConfigContent.id + '_content form[id^=form_firewall_interface_state_]').length)
      {
        $('#' + firewallGeneralConfigContent.id + '_content .firewall_ip_interface_config_port_id').text('X1/X2');
      }

      firewallGeneralConfigContent.CreateSpecialInterfaceArea('VPN', iptables);
      
      if(deviceParams.list['wwan_status'].value !== 'NOT_CONFIGURED')
      {
        firewallGeneralConfigContent.CreateSpecialInterfaceArea('WAN', iptables);
      }

      // show general firewall state additionally in note text
      var globalStateNote = ('enabled' === deviceParams.list['firewall_state'].value) ? '(currently effective)' : '(currently non-effective)';
      $('#' + firewallGeneralConfigContent.id + '_content #firewall_enabled_interface_global_state_note').text(globalStateNote);
    }

  });

};


FirewallGeneralConfigContent.prototype.CreateSpecialInterfaceArea = function(interfaceName, iptables)
{
  var firewallGeneralConfigContent = this;

  var iFaceHtmlFrame = firewallGeneralConfigContent.InterfaceHtmlFrame(interfaceName);
  $('#firewall_interface_general_config_area').append(iFaceHtmlFrame);

  firewallGeneralConfigContent.ShowFirewallStateForInterface(interfaceName, iptables);
  firewallGeneralConfigContent.ShowIcmpConfigForInterface(interfaceName, iptables);
  firewallGeneralConfigContent.ShowServiceConfigForInterface(interfaceName);
};


FirewallGeneralConfigContent.prototype.InterfaceHtmlFrame = function(iFaceId)
{
  var firewallGeneralConfigContent = this;
  var html = '';

  //html += '<div id="firewall_general_config_area_' + iFaceId + '">';
  html +=   '<h3>Firewall Parameter Interface <span class="firewall_ip_interface_config_port_id">' + iFaceId + '</span></h3>';

  html +=   '<div class="config_form_box group" >';
  html +=     '<form id="form_firewall_interface_state_' + iFaceId + '" action="javascript_requested.php" method="POST">';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="firewall_enabled_interface_' + iFaceId + '">Firewall enabled for Interface:</label></div>';
  html +=           '<div class="value_field" >';
  html +=             '<input id="firewall_enabled_interface_' + iFaceId + '" type="checkbox" name="firewall_enabled">';
  html +=             ' <span id="firewall_enabled_interface_global_state_note"></span>';
  html +=           '</div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area" >';
  html +=         '<input id="change_state_button_firewall_interface_state_' + iFaceId + '" class="button" type="submit" value="Submit" name="change_state">';
  html +=       '</p>';
  html +=     '</form>';
  html +=   '</div>';

  html +=   '<div class="config_form_box group" >';
  html +=     '<form id="form_firewall_interface_icmp_config_' + iFaceId + '" action="javascript_requested.php" method="POST">';
  html +=       '<input type="hidden" name="interface_id" value="'+ iFaceId + '">';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="icmp_echo_protection_' + iFaceId + '">ICMP echo protection:</label></div>';
  html +=           '<div class="value_field">';
  html +=             '<input id="icmp_echo_protection_' + iFaceId + '" type="checkbox" name="icmp_echo_protection">';
  html +=           '</div>';
  html +=         '</div>';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="icmp_echo_burst_limit_' + iFaceId + '">ICMP echo limit per <span class="icmp_echo_burst_time_unity">???</span>:</label></div>';
  html +=           '<div class="value_field">';
  html +=             '<input id="icmp_echo_burst_limit_' + iFaceId + '" class="input_field" type="text" size="5" name="icmp_echo_burst_limit">';
  html +=           '</div>';
  html +=         '</div>';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="icmp_burst_limit_' + iFaceId + '">ICMP burst limit: (0 = disabled)</label></div>';
  html +=           '<div class="value_field">';
  html +=             '<input id="icmp_burst_limit_' + iFaceId + '" class="input_field" type="text" size="5" name="icmp_burst_limit">';
  html +=           '</div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input class="button" type="submit" value="Submit" name="submit">';
  html +=       '</p>';
  html +=     '</form>';
  html +=   '</div>';

  // service configuration
  html +=   '<div class="config_form_box" >';
  html +=     '<form id="form_firewall_interface_service_config_' + iFaceId + '" action="javascript_requested.php" method="POST">';
  html +=       '<input type="hidden" name="interface_id" value="'+ iFaceId + '">';
  html +=       '<div class="config_data_list">';

  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field">Service enabled:</div>';
  html +=           '<div class="value_field">';
  html +=             '<ul class="check_list_important">'

  $.each(firewallGeneralConfigContent.serviceList, function(index, service)
  {
    html +=             '<li>';
    html +=               '<input id="firewall_service_config_' + service.id + '_' + iFaceId + '" type="checkbox" name="' + service.id + '_state">';
    html +=               '<label for="firewall_service_config_' + service.id + '_' + iFaceId + '">' + service.outputText + '</label>';
    html +=             '</li>';
    /*
    html +=         '<div class="label_value_pair">';
    html +=           '<div class="label_field"><label for="firewall_service_config_' + service.id + '_' + iFaceId + '">' + service.outputText + ' enabled:</label></div>';
    html +=           '<div class="value_field">';
    html +=             '<input id="firewall_service_config_' + service.id + '_' + iFaceId + '" type="checkbox" name="' + service.id + '_state">';
    html +=           '</div>';
    html +=         '</div>';
    */
  });

  html +=             '</ul>';
  html +=           '</div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input class="button" type="submit" value="Submit" name="submit">';
  html +=       '</p>';
  html +=     '</form>';
  html +=   '</div>';

  return html;
};


FirewallGeneralConfigContent.prototype.ShowIcmpBroadcastProtectionConfig = function(iptables)
{
  var firewallGeneralConfigContent = this;

  $('#' + firewallGeneralConfigContent.id + '_content #state_firewall_icmp_echo_broadcast_protection').removeAttr('checked');

  if(iptables.ipv4.echo.broadcast_protection && ('on' === iptables.ipv4.echo.broadcast_protection))
  {
    $('#' + firewallGeneralConfigContent.id + '_content #state_firewall_icmp_echo_broadcast_protection').prop('checked', 'checked');
  }
};


FirewallGeneralConfigContent.prototype.ShowFirewallStateForInterface = function(iFaceId, iptables)
{
  var firewallGeneralConfigContent  = this;
  var firewallState                 = 'filtered';

  // show default as "firewall active" (box checked), no entry means, firewall is active
  $('#' + firewallGeneralConfigContent.id + '_content #firewall_enabled_interface_' + iFaceId).prop('checked', 'checked');
  //$('#' + firewallGeneralConfigContent.id + '_content #firewall_enabled_interface_' + iFaceId).removeAttr('checked');

  if(iptables.ipv4.input.open['interface'])
  {
    // if there is only one host (in this case, xml conversion creates no array - host has no length)
    if(!iptables.ipv4.input.open['interface'].length)
    {
      // create an array on our own (we need it for following $.each)
      iptables.ipv4.input.open['interface'] = [ iptables.ipv4.input.open['interface'] ];
    }

    // seach after the right interface entry (with the given interface id)
    $.each(iptables.ipv4.input.open['interface'], function(index, iFace)
    {
      // if we found our interface and its state is "off"
      if((iFaceId === iFace['if']) && ('on' === iFace.state))
      {
        $('#' + firewallGeneralConfigContent.id + '_content #firewall_enabled_interface_' + iFaceId).removeAttr('checked');
      }
    });
  }
};


FirewallGeneralConfigContent.prototype.ShowIcmpConfigForInterface = function(iFaceId, iptables)
{
  var firewallGeneralConfigContent  = this;
  var limitTimeUnity                = 'second';
  var limitValue                    = '';
  var policy                        = '';
  var burstlimit                    = '';

  $.each(iptables.ipv4.echo.request, function(index, request)
  {
    if(request['if'] && (request['if'] === iFaceId))
    {
      policy          = request.policy;
      limitValue      = request.limit ? request.limit.replace(/\/.*/, '') : '';
      limitTimeUnity  = request.limit ? request.limit.replace(/.*\//, '') : 'second';
      burstlimit      = request.burst ? request.burst : '0';

      return false;
    }
  });

  if('drop' === policy)
  {
    $('#' + firewallGeneralConfigContent.id + '_content #icmp_echo_protection_' + iFaceId).prop('checked', 'checked');
  }

  $('#' + firewallGeneralConfigContent.id + '_content .icmp_echo_burst_time_unity').text(limitTimeUnity);
  $('#' + firewallGeneralConfigContent.id + '_content #icmp_echo_burst_limit_' + iFaceId).val(limitValue);
  $('#' + firewallGeneralConfigContent.id + '_content #icmp_burst_limit_' + iFaceId).val(burstlimit);
};


FirewallGeneralConfigContent.prototype.ShowLimitsConfig = function(iptables)
{
  var firewallGeneralConfigContent = this;

  udpTimeValue  = iptables.ipv4.climits.udp ? iptables.ipv4.climits.udp.replace(/\/.*/, '') : '';
  udpTimeUnity  = iptables.ipv4.climits.udp ? iptables.ipv4.climits.udp.replace(/.*\//, '') : 'second';
  tcpTimeValue  = iptables.ipv4.climits.tcp ? iptables.ipv4.climits.tcp.replace(/\/.*/, '') : '';
  tcpTimeUnity  = iptables.ipv4.climits.tcp ? iptables.ipv4.climits.tcp.replace(/.*\//, '') : 'second';

  $('#' + firewallGeneralConfigContent.id + '_content .firewall_max_udp_connections_time_unity').text(udpTimeUnity);
  $('#' + firewallGeneralConfigContent.id + '_content .firewall_max_tcp_connections_time_unity').text(tcpTimeUnity);
  $('#' + firewallGeneralConfigContent.id + '_content #firewall_max_udp_connections').val(udpTimeValue);
  $('#' + firewallGeneralConfigContent.id + '_content #firewall_max_tcp_connections').val(tcpTimeValue);
};


FirewallGeneralConfigContent.prototype.ShowServiceConfigForInterface = function(iFaceId)
{
  var firewallGeneralConfigContent  =  this;
  var serviceStateConfig            = deviceParams.list['firewall_service_config'].value;
  var status                        = SUCCESS;
  var errorText                     = '';

  // check if basic entries in config are existing
  if(   (SUCCESS != deviceParams.list['firewall_service_config'].status)
     || !serviceStateConfig
     || (undefined === serviceStateConfig.interfaces)
     || (undefined === serviceStateConfig.interfaces[iFaceId]) )
  {
    status    = ERROR;
    errorText = 'Configuration data invalid.';
  }

  else
  {
    // for all service on webpage
    $.each(firewallGeneralConfigContent.serviceList, function(index, frontendServiceConfig)
    {
      var serviceId = frontendServiceConfig.id;

      // remove check mark
      $('#form_firewall_interface_service_config_' + iFaceId + 'input[name=' + serviceId + '_state]').removeAttr('checked');

      // check if service entry is existing
      if(   (undefined === serviceStateConfig.interfaces[iFaceId].services)
         || (undefined === serviceStateConfig.interfaces[iFaceId].services[serviceId])
         || (undefined === serviceStateConfig.interfaces[iFaceId].services[serviceId].state) )
      {
        status    = ERROR;
        errorText = 'Configuration data invalid.';
      }

      // if state is 'on', set checkbox
      else if('on' === serviceStateConfig.interfaces[iFaceId].services[serviceId].state)
      {
        $('#form_firewall_interface_service_config_' + iFaceId + ' input[name=' + serviceId + '_state]').prop('checked', 'checked');
      }

    /*
    if(SUCCESS === deviceParams.list[service.id + '_firewall_config'].status)
    {
      var serviceFirewallConfig = deviceParams.list[service.id + '_firewall_config'].value;

      if(serviceFirewallConfig.iFace[iFaceId].state && ('on' === serviceFirewallConfig.iFace[iFaceId].state))
      {
        $('#form_firewall_interface_service_config_' + iFaceId + ' input[name=' + service.id + '_state]').prop('checked', 'checked');
      }
    }
    */
    });
  }

  if(SUCCESS != status)
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading firewall service configuration.', errorText ] );
  }
};


FirewallGeneralConfigContent.prototype.ProcessIcmpBroadcastProtectionSubmit = function(event)
{
  var firewallGeneralConfigContent = this;

  event.preventDefault();
  pageElements.ShowBusyScreen('Change firewall configuration...');

  var newState      = ($(event.target).find('[name=state_firewall_icmp_echo_broadcast_protection]:checked').length) ? 'on' : 'off';
  var newValueList  = { state: newState };

  deviceParams.ChangeValue('icmp_echo_broadcast_protection', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing firewall configuration', errorText ]);
      firewallGeneralConfigContent.Refresh();
    }
  });
};


FirewallGeneralConfigContent.prototype.ProcessChangeState = function(event)
{
  var firewallGeneralConfigContent = this;

  event.preventDefault();
  pageElements.ShowBusyScreen('Change firewall state...');

  var newState      = ($(event.target).find('[name=firewall_state]:checked').length) ? 'enable' : 'disable';
  var newValueList  = { state: newState };

  deviceParams.ChangeValue('firewall_state', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing firewall state', errorText ]);
      firewallGeneralConfigContent.Refresh();
    }
    else
    {
      // conform note text at firewall interface states
      var globalStateNote = ('enable' === newState) ? '(currently effective)' : '(currently non-effective)';
      $('#' + firewallGeneralConfigContent.id + '_content #firewall_enabled_interface_global_state_note').text(globalStateNote);
    }
  });
};


FirewallGeneralConfigContent.prototype.ProcessChangeLimitsSubmit = function(event)
{
  var firewallGeneralConfigContent = this;

  event.preventDefault();
  pageElements.ShowBusyScreen('Change firewall configuration...');

  var tcpLimitCount     = $(event.target).find('[name=firewall_max_tcp_connections]').val();
  var tcpLimitTimeUnity = $(event.target).find('.firewall_max_tcp_connections_time_unity').text();
  var tcpLimit          = tcpLimitCount ? tcpLimitCount + '/' + tcpLimitTimeUnity : '-';

  var udpLimitCount     = $(event.target).find('[name=firewall_max_udp_connections]').val();
  var udpLimitTimeUnity = $(event.target).find('.firewall_max_udp_connections_time_unity').text();
  var udpLimit          = udpLimitCount ? udpLimitCount + '/' + udpLimitTimeUnity : '-';

  var newValueList  = { tcpLimit: tcpLimit, udpLimit: udpLimit };

  deviceParams.ChangeValue('firewall_limits', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing firewall configuration', errorText ]);
      firewallGeneralConfigContent.Refresh();
    }
  });
};


FirewallGeneralConfigContent.prototype.ProcessChangeInterfaceStateSubmit = function(event)
{
  var firewallGeneralConfigContent = this;

  event.preventDefault();

  pageElements.ShowBusyScreen('Change firewall configuration...');

  var iFaceId       = $(event.target).attr('id').replace('form_firewall_interface_state_', '');
  var newValueList  = { state: 'on', iFaceId: iFaceId };

  // firewall should be enabled (rule for interface must be removed)
  if($(event.target).find('[name=firewall_enabled]:checked').length)
  {
    var paramId = 'interface_ip_firewall_rule_remove';
  }

  // firewall should not be inactive (-> we need an exeption rule for this interface)
  else
  {
    var paramId         = 'interface_ip_firewall_rule_state';
    newValueList.state  = 'on';
  }

  deviceParams.ChangeValue(paramId, newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing firewall configuration', errorText ]);
      firewallGeneralConfigContent.Refresh();
    }
  });

};


FirewallGeneralConfigContent.prototype.ProcessChangeInterfaceIcmpConfigSubmit = function(event)
{
  var firewallGeneralConfigContent = this;

  event.preventDefault();
  pageElements.ShowBusyScreen('Change firewall configuration...');

  var iFaceId         = $(event.target).attr('id').replace('form_firewall_interface_icmp_config_', '');
  var policy          = ($(event.target).find('[name=icmp_echo_protection]:checked').length) ? 'drop' : 'accept';
  var limitCount      = $(event.target).find('[name=icmp_echo_burst_limit]').val();
  var limitTimeUnity  = $(event.target).find('.icmp_echo_burst_time_unity').text();
  var burstlimit      = $(event.target).find('[name=icmp_burst_limit]').val();
  if ('0' === burstlimit) burstlimit = '-';

  var limit           = limitCount + '/' + limitTimeUnity;

  if(!limitCount.length) limitCount = '-';
  if(('0' === limitCount) || ('-' === limitCount)) limit = limitCount;

  var newValueList  = { policy: policy, iFaceId: iFaceId, limit: limit, burstlimit: burstlimit};

  deviceParams.ChangeValue('icmp_echo_interface_config', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing firewall configuration', errorText ]);
      firewallGeneralConfigContent.Refresh();
    }
  });
};


FirewallGeneralConfigContent.prototype.ProcessChangeInterfaceServiceStateSubmit = function(event)
{
  var firewallGeneralConfigContent = this;

  event.preventDefault();

  var iFaceId               = $(event.target).attr('id').replace('form_firewall_interface_service_config_', '');

  //var service                       = firewallServiceConfiguration.service;
  var changeValueGroupData          = [];
  var changeValueIndex              = 0;

  $(event.target).find('input[name$=state]').each(function(serviceIndex)
  {
    var checkboxObj = this;
    var service     = $(checkboxObj).attr('name').replace('_state', '');

    changeValueGroupData[changeValueIndex]            = {};
    changeValueGroupData[changeValueIndex].paramId    = 'set_service_firewall_interface_state';
    changeValueGroupData[changeValueIndex].valueList  = { service: service, state: 'off', iFaceId: iFaceId };

    if($(checkboxObj).prop('checked'))
    {
      changeValueGroupData[changeValueIndex].valueList.state = 'on';
    }

    ++changeValueIndex;
  });

  pageElements.ShowBusyScreen('Change firewall configuration...');

  deviceParams.ChangeValueGroup(changeValueGroupData, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing firewall configuration.', errorText ]);
      firewallGeneralConfigContent.Refresh();
    }
  });

};
