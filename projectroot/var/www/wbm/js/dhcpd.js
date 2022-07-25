/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var DhcpdContent = function(id)
{
  var dhcpdContent                = this;
  
  dhcpdContent.id                 = id || 'dhcpd';
  
  dhcpdContent.maxStaticHostCount = 15;

  dhcpdContent.aStaticHosts       = [];
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  
  var CreateDhcpdConfigurationParamView = (function()
  {
    dhcpdContent.paramView = new DeviceParamView();
            
    dhcpdContent.paramView.Add(
    {
      paramId         : 'dhcpd_config'
    });

  })();
  

  $('#content_area').bind(dhcpdContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + dhcpdContent.id + '_content #dhcpd_configuration_area').bind('submit', function(event)
    {
      if($(event.target).attr('id').match('config_dhcpd_form'))
      {
        dhcpdContent.ProcessBasicConfigChangeSubmit(event);
      }
      
      else if($(event.target).attr('id').match('dhcpd_static_host'))
      {
        dhcpdContent.ProcessStaticHostsConfigSubmit(event);
      }
    });
    
  });
    

  $('#content_area').bind(dhcpdContent.id + '_refresh', function(event)
  {
    dhcpdContent.Refresh();
  });
  
};


DhcpdContent.prototype.Refresh = function()
{
  var dhcpdContent = this;

  deviceParams.ReadValueGroup(dhcpdContent.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(dhcpdContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(dhcpdContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading DHCPD configuration.', errorString ] );
    }
    else
    {
      var dhcpdConfig = deviceParams.list['dhcpd_config'].value;
      
      $('#' + dhcpdContent.id + '_content #dhcpd_configuration_area').html('');
      
      // loop over all interfaces
      var iFaceCount = 0;
      $.each(dhcpdConfig, function(iFaceId, dhcpdConfig)
      {
        // create html frame for output dhcpd values of interface
        var interfaceHtmlFrame = dhcpdContent.InterfaceHtmlFrame(iFaceId);
        $('#' + dhcpdContent.id + '_content #dhcpd_configuration_area').append(interfaceHtmlFrame);
        
        // show dhcpd values
        var iFaceArea = $('#' + dhcpdContent.id + '_content #dhcpd_configuration_area #dhcpd_config_' + iFaceId);
        
        if('enabled' === dhcpdConfig.dhcpdState) $(iFaceArea).find('input[name=state]').attr('checked', 'checked');
        
        //$(iFaceArea).find('input[name=ranges]').val(dhcpdConfig.dhcpdRange);
        dhcpdContent.ShowIpRange(iFaceId, dhcpdConfig.dhcpdRange);
        
        $(iFaceArea).find('input[name=lease_time]').val(dhcpdConfig.dhcpdLeaseTime);
        
        dhcpdContent.ShowStaticHosts(iFaceId, dhcpdConfig.dhcpdFixHost);
        
        ++iFaceCount;
      });
      
      if(1 === iFaceCount)
      {
        $('#' + dhcpdContent.id + '_content #dhcpd_configuration_area [id^=device_id]').html('X1/X2');
      }
      
      if(0 === iFaceCount)
      {
        $('#' + dhcpdContent.id + '_content #dhcpd_configuration_area').html('<p>No configuration found.</p>');
      }
    }
    
  });

};

DhcpdContent.prototype.InterfaceHtmlFrame = function(iFaceId)
{
  var dhcpdContent  = this;
  var html          = '';
  
  html += '<div id="dhcpd_config_' + iFaceId + '">';
  html +=   '<h3>DHCP Configuration <span id="device_id_' + iFaceId + '" >' + iFaceId + '</span></h3>';
  html +=   '<form id="config_dhcpd_form_' + iFaceId + '" action="javascript_requested.php" method="POST">';
  html +=     '<input type="hidden" name="interface_id" value="'+ iFaceId + '">';
  html +=     '<div class="config_form_box group" >';
  html +=       '<div class="config_data_list">';
  
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="dhcpd_state_' + iFaceId + '">Service active:</label></div>';
  html +=           '<div class="value_field">';
  html +=             '<input id="dhcpd_state_' + iFaceId + '" type="checkbox" name="state" value="enabled" >';
  html +=           '</div>';
  html +=         '</div>';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="dhcpd_ranges_' + iFaceId + '">IP Range:</label></div>';
  //html +=           '<div class="value_field"><input id="dhcpd_ranges_' + iFaceId + '" class="input_field" type="text" name="ranges" size="31" value="" ></div>';
  html +=           '<div class="value_field">';
  html +=             '<input id="dhcpd_range_start_' + iFaceId + '" class="input_field half_size" type="text" name="range_start" value="" >-';
  html +=             '<input id="dhcpd_range_end_' + iFaceId + '" class="input_field half_size" type="text" name="range_end" value="" >';
  html +=           '</div>';
  html +=         '</div>';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="dhcpd_lease_time_' + iFaceId + '">Lease time (min):</label></div>';
  html +=           '<div class="value_field"><input id="dhcpd_lease_time_' + iFaceId + '" class="input_field half_size" type="text" name="lease_time" value="" ></div>';
  html +=         '</div>';
  /*
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="dhcpd_static_hosts_' + iFaceId + '_0">Static hosts:</label></div>';
  html +=           '<div class="value_field">';
  //html +=             '<textarea class="input_field" id="dhcpd_fix_hosts_' + iFaceId + '" name="fix_hosts" cols="29" rows="5"></textarea></div>';
  
  for(var staticHostNo = 0; staticHostNo <= dhcpdContent.maxStaticHostCount; ++staticHostNo)
  {
    html +=             '<input id="dhcpd_static_host_' + iFaceId + '_' + staticHostNo + '" class="input_field" type="text" name="static_host_' + staticHostNo + '" size="31" value="" ><br>';
  }
  
  
  html +=           '</div>';
  html +=         '</div>';
  */
  
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input class="button" type="submit" value="Submit" name="submit">';
  html +=       '</p>';
  html +=     '</div>';
      
  html +=   '</form>';

  html +=   '<div id="dhcpd_static_host_list_area_' + iFaceId + '">';
  html +=     '<div class="config_form_box group" >';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field">Static hosts:</div>';
  html +=           '<div class="value_field">No static hosts configured</div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=   '</div>';
  
  html +=   '<form id="add_dhcpd_static_host_form_' + iFaceId + '" action="javascript_requested.php" method="POST">';
  html +=     '<input type="hidden" name="interface_id" value="'+ iFaceId + '">';
  html +=     '<div class="config_form_box" >';
  html +=       '<div class="config_data_list">';
  
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="dhcpd_static_hosts_' + iFaceId + '_0">New static Host:</label></div>';
  html +=           '<div class="value_field">';
  html +=             '<input id="dhcpd_static_host_add_mac_' + iFaceId + '" class="input_field half_size" type="text" name="static_host_mac" value="" >=';
  html +=             '<input id="dhcpd_static_host_add_ip_' + iFaceId + '" class="input_field half_size" type="text" name="static_host_ip" value="" >';
  //html +=             '<textarea class="input_field" id="dhcpd_fix_hosts_' + iFaceId + '" name="fix_hosts" cols="29" rows="5"></textarea></div>';
  /*
  for(var staticHostNo = 0; staticHostNo <= dhcpdContent.maxStaticHostCount; ++staticHostNo)
  {
    html +=             '<input id="dhcpd_static_host_mac_' + iFaceId + '_' + staticHostNo + '" class="input_field" type="text" name="static_host_mac_' + staticHostNo + '" size="17" value="" >=';
    html +=             '<input id="dhcpd_static_host_ip_' + iFaceId + '_' + staticHostNo + '" class="input_field" type="text" name="static_host_ip_' + staticHostNo + '" size="15" value="" ><br>';
  }
  */
  html +=           '</div>';
  html +=         '</div>';
  
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input class="button" type="submit" value="Add" name="submit">';
  html +=       '</p>';
  html +=     '</div>';
      
  html +=   '</form>';
  
  html += '</div>';
  
  return html;
};


DhcpdContent.prototype.StaticHostHtml = function(iFaceId, hostIndex, staticHost)
{
  var dhcpdContent  = this;
  var html          = '';

  html +=   '<form id="delete_dhcpd_static_host_form_' + iFaceId + '_' + hostIndex + '" action="javascript_requested.php" method="POST">';
  html +=     '<input type="hidden" name="interface_id" value="'+ iFaceId + '">';
  html +=     '<input type="hidden" name="host_index" value="'+ hostIndex + '">';
  html +=     '<div class="config_form_box group" >';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field">Static Host ' + (parseInt(hostIndex, 10) + 1) + ':</div>';
  html +=           '<div class="value_field">';
  html +=             '<span class="host_mac">' + staticHost.hostMac + '</span>';
  html +=             ' = <span class="ip_addr">' + staticHost.ipAddr + '</span>';
  html +=           '</div>'; 
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input class="button" type="submit" value="Delete" name="delete_static_host_' + hostIndex + '">';
  html +=       '</p>';
  html +=     '</div>';
  html +=   '</form>';
  
  return html;
};



DhcpdContent.prototype.ShowIpRange = function(iFaceId, ipRange)
{
  var dhcpdContent  = this;
  var rangeStart    = '';
  var rangeEnd      = '';
  
  if(ipRange)
  {
    rangeStart = ipRange.replace(/_.*$/, '');
    rangeEnd   = ipRange.replace(/^.*_/, '');
  }
  
  $('#dhcpd_config_' + iFaceId + ' [name=range_start]').val(rangeStart);
  $('#dhcpd_config_' + iFaceId + ' [name=range_end]').val(rangeEnd);
};



DhcpdContent.prototype.ShowStaticHosts = function(iFaceId, dhcpdFixHost)
{
  var dhcpdContent  = this;

  if(dhcpdFixHost.length)
  {
    $('#dhcpd_config_' + iFaceId + ' #dhcpd_static_host_list_area_' + iFaceId).html('');
  
    $.each(dhcpdFixHost, function(hostIndex, staticHost)
    {
      if(staticHost.hostMac && staticHost.ipAddr)
      {
        var html = dhcpdContent.StaticHostHtml(iFaceId, hostIndex, staticHost);
        $('#dhcpd_config_' + iFaceId + ' #dhcpd_static_host_list_area_' + iFaceId).append(html);
      }
    });
  }
};



/*
 * change config values without static hosts (state, ip range, lease time)
 */
DhcpdContent.prototype.ProcessBasicConfigChangeSubmit = function(event)
{
  event.preventDefault();

  var dhcpdContent  = this;
  var status        = SUCCESS;
  var errorText     = '';

  // get new values from form fields
  var portId    = $(event.target).find('input[name=interface_id]').val();
  var state     = $(event.target).find('input[name=state]:checked').length ? 'enabled' : 'disabled';
  var leaseTime = $(event.target).find('input[name=lease_time]').val();
  
  // merge ip range string
  var ipRangeString = '';
  if($(event.target).find('input[name=range_start]').val() && $(event.target).find('input[name=range_end]').val())
  {
    ipRangeString = $(event.target).find('input[name=range_start]').val() + '_' + $(event.target).find('input[name=range_end]').val();
  }
  
  if(SUCCESS != status)
  {
    $('body').trigger('event_errorOccured', [ errorText ]);
  }
  else
  {
    pageElements.ShowBusyScreen('Changing DHCP configuration...');

    var newValueList  = { port: portId, state: state, range: ipRangeString, leaseTime: leaseTime };

    deviceParams.ChangeValue('dhcpd_basic_config', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
  
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing DHCP configuration.', errorText ]);
      }
  
      dhcpdContent.Refresh();
    });
  }
};



/*
 * create parameter string for configtool, which is a ','-separated list including all static hosts which shoul be configured
 */
DhcpdContent.prototype.CreateStaticHostParameterString = function(iFaceId, deleteHostIndex, newHostMac, newHostIpAddr)
{
  var dhcpdContent  = this;
  
  var staticHostParameterString = '';
  var delimiter                 = '';
  
  // loop over all forms which show the static hosts (to delete them)
  $.each($('form[id^=delete_dhcpd_static_host_form_' + iFaceId + ']'), function(formIndex, staticHostForm)
  {
    var staticHostIndex = $(staticHostForm).find('input[name=host_index]').val();
    
    // if actual host should not be deleted - add it to parameter string (else jump over, so it will no longer be configured)
    if(staticHostIndex != deleteHostIndex)
    {
      var hostMac = $(staticHostForm).find('.host_mac').text();
      var ipAddr  = $(staticHostForm).find('.ip_addr').text();
    
      staticHostParameterString = staticHostParameterString + delimiter + hostMac + '_' + ipAddr;
      delimiter = ',';
    }
  });
  
  // if a new host should be added - do it
  if(newHostMac && newHostIpAddr)
  {
    staticHostParameterString = staticHostParameterString + delimiter + newHostMac + '_' + newHostIpAddr;
  }

  return staticHostParameterString;
};



DhcpdContent.prototype.ProcessStaticHostsConfigSubmit = function(event)
{
  var dhcpdContent  = this;
  var status        = SUCCESS;
  var errorText     = '';

  event.preventDefault();
  
  var iFaceId         = $(event.target).find('[name=interface_id]').val();
  var deleteHostIndex = -1;

  if($(event.target).attr('id').match('delete_dhcpd_static_host'))
  {
    var deleteHostIndex = $(event.target).find('[name=host_index]').val();
  }
  
  else if($(event.target).attr('id').match('add_dhcpd_static_host'))
  {
    var newHostMac  = $(event.target).find('[name=static_host_mac]').val();
    var newIpAddr   = $(event.target).find('[name=static_host_ip]').val();
    
    if(!newHostMac || !newIpAddr)
    {
      $('body').trigger('event_errorOccured', [ 'Missing parameter for new static host.' ]);
      status = ERROR;
    }
  }

  if(SUCCESS === status)
  {
    var staticHostParameterString = dhcpdContent.CreateStaticHostParameterString(iFaceId, deleteHostIndex, newHostMac, newIpAddr);
    //Log(staticHostParameterString);
    
    if(false !== staticHostParameterString)
    {
      pageElements.ShowBusyScreen('Changing DHCPD configuration...');

      var newValueList  = { port: iFaceId, fixHost: staticHostParameterString };

      deviceParams.ChangeValue('dhcpd_fix_hosts', newValueList, function(status, errorText)
      {
        pageElements.RemoveBusyScreen();
    
        if(SUCCESS != status)
        {
          $('body').trigger('event_errorOccured', [ 'Error while changing DHCPD configuration.', errorText ]);
        }
    
        dhcpdContent.Refresh();
      });
    }
  }

};


