/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function FirewallUserFilterContent(id)
{
  var firewallUserFilterContent = this;
  
  firewallUserFilterContent.id = id || 'firewall_user_filter';
  

  var CreateFirewallUserFilterContentParamView = (function()
  {
    firewallUserFilterContent.paramView = new DeviceParamView();
                    
    firewallUserFilterContent.paramView.Add(
    {
      paramId         : 'iptables_config'
    });

    firewallUserFilterContent.paramView.Add(
    {
      paramId         : 'interface_config_xml'
    });
    
    firewallUserFilterContent.paramView.Add(
    {
      paramId         : 'wwan_status'
    });

  })();


  //------------------------------------------------------------------------------
  //first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(firewallUserFilterContent.id + '_create', function(event)
  {
    // delete user filter
    $('#' + firewallUserFilterContent.id + '_content #firewall_user_filter_area').bind('submit', function(event)
    {
      firewallUserFilterContent.ProcessDeleteRuleSubmit(event);
    });
    
    // add new mac address filter
    $('#' + firewallUserFilterContent.id + '_content #add_firewall_user_filter_form').bind('submit', function(event)
    {
      firewallUserFilterContent.ProcessAddRuleSubmit(event);
    });
      
  });

   
  //------------------------------------------------------------------------------
  //refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(firewallUserFilterContent.id + '_refresh', function(event)
  {
    firewallUserFilterContent.Refresh();
  });

};


FirewallUserFilterContent.prototype.Refresh = function()
{
  var firewallUserFilterContent = this;

  deviceParams.ReadValueGroup(firewallUserFilterContent.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(firewallUserFilterContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(firewallUserFilterContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading firewall configuration.', errorString ] );
    }
    else
    {
      //firewallUserFilterContent.CreateProtocolSelection();  // now static
      firewallUserFilterContent.CreateInterfaceSelection();
      
      // show list - first remove old content
      $('#' + firewallUserFilterContent.id + '_content #firewall_user_filter_area').html('');
      
      var iptables    = deviceParams.list['iptables_config'].value;
      var filterIndex = 0;
      
      // at least one filter existing
      if(iptables.ipv4.input.filter.rule)
      {
        // if there is only one filter (in this case, xml conversion creates no array - rule has no length)
        if(!iptables.ipv4.input.filter.rule.length)
        {
          // create an array on our own here (we need it for following $.each)
          iptables.ipv4.input.filter.rule = [ iptables.ipv4.input.filter.rule ];
        }
      
        $.each(iptables.ipv4.input.filter.rule, function(ruleId, rule)
        {
          $("#firewall_user_filter_area").append(firewallUserFilterContent.FilterHtmlFrame(filterIndex, rule));
         ++filterIndex;
        });
      }
      
      $('#' + firewallUserFilterContent.id + '_content #firewall_user_filter_count').html(filterIndex);
    }
  });

};


FirewallUserFilterContent.prototype.FilterHtmlFrame = function(filterIndex, rule, ifCount)
{
  var disabledString  =  ('off' === rule.state) ? 'disabled' : '';
  var ifCount         = ifCount || 3;
  var html            = '';

  html += '<h3>User filter ' + (parseInt(filterIndex, 10) + 1) + disabledString + '</h3>';
  html += '<form id="firewall_user_filter_delete_form_' + filterIndex + '" action="javascript_requested.php" method="POST">';
  html +=   '<div class="config_form_box">';
  html +=     '<div class="config_data_list">';
  
  if(rule.src_ip)
  {
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">Source IP address:</div>';
    html +=       '<div class="value_field">' + rule.src_ip + '</div>';
    html +=     '</div>';
  }

  if(rule.src_mask)
  {
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">Source netmask:</div>';
    html +=       '<div class="value_field">' + rule.src_mask + '</div>';
    html +=     '</div>';
  }
  
  if(rule.src_port)
  {
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">Source Port:</div>';
    html +=       '<div class="value_field">' + rule.src_port + '</div>';
    html +=     '</div>';
  }

  if(rule.dst_ip)
  {
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">Destination IP address:</div>';
    html +=       '<div class="value_field">' + rule.dst_ip + '</div>';
    html +=     '</div>';
  }
  
  if(rule.dst_mask)
  {
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">Destination subnet mask:</div>';
    html +=       '<div class="value_field">' + rule.dst_mask + '</div>';
    html +=     '</div>';
  }
  
  if(rule.dst_port)
  {
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">Destination port:</div>';
    html +=       '<div class="value_field">' + rule.dst_port + '</div>';
    html +=     '</div>';
  }

  if(rule.proto)
  {
    var protocol = ('tcpudp' === rule.proto) ? 'TCP/UDP' : rule.proto.toUpperCase();
    
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">Protocol:</div>';
    html +=       '<div class="value_field">' + protocol + '</div>';
    html +=     '</div>';
  }

  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Input interface:</div>';
  
  if(rule['if'])
  {
    //var ifString = (('X1' === rule['if']) && (1 == ifCount)) ? 'X1/X2' : rule['if'];
    var ifString = rule['if'];
    html +=       '<div class="value_field">' + ifString + '</div>';
  }
  else 
  {
    html +=       '<div class="value_field">Any</div>';
  }

  html +=     '</div>';

  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"> Policy:</div>';
  html +=         '<div class="value_field">' + rule.policy.toUpperCase() + '</div>';
  html +=       '</div>';

  html +=     '</div>';
  html +=     '<p class="button_area">';
  html +=       '<input class="button" type="submit" value="Delete" name="delete">';
  html +=     '</p>';
  html +=   '</div>';
  html += '</form>';

  return html;
};


FirewallUserFilterContent.prototype.CreateProtocolSelection = function()
{
  var firewallUserFilterContent = this;
  var aProtocols                = deviceParams.list['protocols'].value;

  var protocolHtml = '';
  protocolHtml += '<li>';
  protocolHtml +=   '<input id="firewall_user_filter_protocol_none" type="radio" name="protocol" value="none" checked>';
  protocolHtml +=   '<label for="firewall_user_filter_protocol_none" >None</label>';
  protocolHtml += '</li>';

  // show list - first remove old content
  $('#' + firewallUserFilterContent.id + '_content .protocol_selection').html(protocolHtml);

  // create html areas for all configured interfaces
  $.each(aProtocols, function(protocolIndex, protocol)
  {
    protocolHtml = '';
    protocolHtml += '<li>';
    protocolHtml +=   '<input id="firewall_user_filter_protocol_' + protocol + '" type="radio" name="protocol" value="' + protocol + '">';
    protocolHtml +=   '<label for="firewall_user_filter_protocol_' + protocol + '" >' + protocol + '</label>';
    protocolHtml += '</li>';
      
    $('#' + firewallUserFilterContent.id + '_content .protocol_selection').append(protocolHtml);
  });

};


FirewallUserFilterContent.prototype.CreateInterfaceSelection = function()
{
  var firewallUserFilterContent = this;
  var interfaceConfig           = deviceParams.list['interface_config_xml'].value;

  var portHtml = '';

  // show list - first remove old content
  $('#' + firewallUserFilterContent.id + '_content .interfaces_selection').html(portHtml);

  //WAT19366 add radiobutton "any" value has to be empty
  portHtml += '<li>';
  portHtml +=   '<input id="firewall_user_filter_interface_any" type="radio" name="port_id" value="" checked="true">';
  portHtml +=   '<label for="firewall_user_filter_interface_any" >Any</label>';
  portHtml += '</li>';
      
  $('#' + firewallUserFilterContent.id + '_content .interfaces_selection').append(portHtml);

  // create html areas for all configured interfaces
  $.each(interfaceConfig.ifaces, function(iFaceId, iFace)
  {
    if(iFace.ipSettings) // && iFace.ipSettings.showInWbm)
    {
      var portName = iFace.ipSettings.portName;
      
      portHtml = '';
      portHtml += '<li>';
      portHtml +=   '<input id="firewall_user_filter_interface_' + portName + '" type="radio" name="port_id" value="' + portName + '">';
      portHtml +=   '<label for="firewall_user_filter_interface_' + portName + '" >' + portName + '</label>';
      
      portHtml += '</li>';
      
      $('#' + firewallUserFilterContent.id + '_content .interfaces_selection').append(portHtml);
    }
  });
  
  // (always) add option for VPN 
  portHtml = '';
  portHtml += '<li>';
  portHtml +=   '<input id="firewall_user_filter_interface_vpn" type="radio" name="port_id" value="VPN">';
  portHtml +=   '<label for="firewall_user_filter_interface_vpn" >VPN</label>';
  portHtml += '</li>';
  
  // if wwan interface is existing, add option
  if(deviceParams.list['wwan_status'].value !== 'NOT_CONFIGURED')
  {
    portHtml += '<li>';
    portHtml +=   '<input id="firewall_user_filter_interface_wan" type="radio" name="port_id" value="WAN">';
    portHtml +=   '<label for="firewall_user_filter_interface_wan" >WAN</label>';
    portHtml += '</li>';
  }

  
  $('#' + firewallUserFilterContent.id + '_content .interfaces_selection').append(portHtml);
};


FirewallUserFilterContent.prototype.ProcessDeleteRuleSubmit = function(event)
{
  var firewallUserFilterContent = this;

  event.preventDefault();

  pageElements.ShowBusyScreen('Delete firewall rule...');
  
  var ruleIndex = $(event.target).prop('id').replace('firewall_user_filter_delete_form_', '');
  
  var newValueList  = { ruleNo: parseInt(ruleIndex, 10) + 1 };

  deviceParams.ChangeValue('firewall_user_filter_delete', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while deleting firewall rule.', errorText ]);
    }
    
    firewallUserFilterContent.Refresh();
  });
  
};


FirewallUserFilterContent.prototype.ProcessAddRuleSubmit = function(event)
{
  var firewallUserFilterContent = this;
  var status                    = SUCCESS;

  event.preventDefault();

  var sourceIpAddress = $(event.target).find('[name=source_ip_address]').val();
  var sourceNetmask   = $(event.target).find('[name=source_netmask]').val();
  var destIpAddress   = $(event.target).find('[name=destination_ip_address]').val();
  var destSubnetMask  = $(event.target).find('[name=destination_subnet_mask]').val();
  var sourcePort      = $(event.target).find('[name=source_port]').val();
  var destPort        = $(event.target).find('[name=destination_port]').val();
  var inputPortId     = $(event.target).find('[name^=port_id]:checked').val();

  var protocol        = '';
  if($(event.target).find('[name=protocol_tcp]:checked').length) protocol = 'tcp';
  if($(event.target).find('[name=protocol_udp]:checked').length) protocol += 'udp';

  var policy         = $(event.target).find('[name=firewall_policy]:checked').val();

  // input parameter check
  if(sourceIpAddress && !interfaceConfig.IpAddressOk(sourceIpAddress))
  {
    status  = ERROR;
    $('body').trigger('event_errorOccured', [ 'Invalid source IP address.' ]);
  }
  
  else if(destIpAddress && !interfaceConfig.IpAddressOk(destIpAddress))
  {
    status  = ERROR;
    $('body').trigger('event_errorOccured', [ 'Invalid destination IP address.' ]);
  }
  else if(!sourceIpAddress && !sourceNetmask && !destIpAddress && !destSubnetMask && !sourcePort && !destPort && !protocol && !inputPortId)
  {
    status  = ERROR;
    $('body').trigger('event_errorOccured', [ 'At least one protocol and a specific interface has to be selected.' ]);
  }
  else if(!protocol)
  {
    status = ERROR;
    $('body').trigger('event_errorOccured', [ 'At least one protocol type has to be selected.' ]);
  }
  
  if(SUCCESS == status)
  {
    pageElements.ShowBusyScreen('Creating new firewall rule...');
    if(!sourceIpAddress)        sourceIpAddress  = '-';
    if(!sourceNetmask)          sourceNetmask    = '-';
    if(!destIpAddress)          destIpAddress    = '-';
    if(!destSubnetMask)         destSubnetMask   = '-';
    if(!sourcePort)             sourcePort       = '-';
    if(!destPort)               destPort         = '-';
    if('' === protocol)         protocol         = '-';
    if(!inputPortId)            inputPortId      = '-';
  
    var newValueList  = { interfaceId: inputPortId, protocol: protocol, sourceIpAddr: sourceIpAddress, sourceNetmask: sourceNetmask, sourcePort: sourcePort, destIpAddr: destIpAddress, destSubnetMask: destSubnetMask, destPort: destPort , policy: policy};

    deviceParams.ChangeValue('firewall_user_filter_add', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
    
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while adding firewall rule.', errorText ]);
      }
      else
      {
        // reset form values and reload data
        $(event.target).find('input[type=text]').val('');
        $(event.target).find('input[type=checkbox]').removeAttr('checked');
        
        $(event.target).find('input[type=radio]').removeAttr('checked');
        $(event.target).find('[name=firewall_policy]#firewall_policy_accept').prop('checked', true);
        $(event.target).find('[name=port_id]#firewall_user_filter_interface_any').prop('checked', true);
      }
    
      firewallUserFilterContent.Refresh();
    });
  }

};
