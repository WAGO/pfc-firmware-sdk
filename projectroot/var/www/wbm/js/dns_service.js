/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var DnsServiceContent = function(id)
{
  var dnsServiceContent                = this;
  
  dnsServiceContent.id                 = id || 'dhcpd';
  
  dnsServiceContent.maxStaticHostCount = 15;
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  
  var CreateDnsServiceConfigurationParamView = (function()
  {
    dnsServiceContent.paramView = new DeviceParamView();
            
    dnsServiceContent.paramView.Add(
    {
      paramId         : 'dns_service_config'
    });

  })();
  

  $('#content_area').bind(dnsServiceContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + dnsServiceContent.id + '_content #dns_service_configuration_area').bind('submit', function(event)
    {
      if($(event.target).attr('id').match('config_dns_server_form'))
      {
        dnsServiceContent.ProcessBasicConfigChangeSubmit(event);
      }
      
      else if($(event.target).attr('id').match('dns_static_host'))
      {
        dnsServiceContent.ProcessStaticHostsConfigSubmit(event);
      }
    });
    
  });
    

  $('#content_area').bind(dnsServiceContent.id + '_refresh', function(event)
  {
    dnsServiceContent.Refresh();
  });
  
};


DnsServiceContent.prototype.Refresh = function()
{
  var dnsServiceContent = this;

  deviceParams.ReadValueGroup(dnsServiceContent.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(dnsServiceContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(dnsServiceContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading DNS configuration.', errorString ] );
    }
    else
    {
      var dnsConfig = deviceParams.list['dns_service_config'].value;
      
      $('#' + dnsServiceContent.id + '_content #dns_service_configuration_area').html('');
      
      // create html frame for output dhcpd values of interface
      var htmlFrame = dnsServiceContent.HtmlFrame();
      $('#' + dnsServiceContent.id + '_content #dns_service_configuration_area').append(htmlFrame);
        
      // show DNS values
      var dnsServiceArea = $('#' + dnsServiceContent.id + '_content #dns_service_configuration_area');
        
      if('enabled' === dnsConfig.dnsState) $(dnsServiceArea).find('input[name=state]').prop('checked', 'checked');
      
      $(dnsServiceArea).find('input[name=mode]').removeAttr('checked');
      $(dnsServiceArea).find('input[name=mode][value=' + dnsConfig.dnsMode + ']').prop('checked', 'checked');
      
      dnsServiceContent.ShowStaticHosts(dnsConfig.dnsFixHost);
    }
    
  });

};


DnsServiceContent.prototype.HtmlFrame = function()
{
  var dnsServiceContent = this;
  var html              = '';
  
  html += '<div class="config_form_box group" >';
  html +=   '<form id="config_dns_server_form" action="javascript_requested.php" method="POST">';
  html +=     '<div class="config_data_list">';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"><label for="dns_service_state">Service active:</label></div>';
  html +=         '<div class="value_field">';
  html +=           '<input id="dns_service_state" type="checkbox" name="state" value="enabled" >';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"><label for="dns_service_mode">Mode:</label></div>';
  html +=         '<div class="value_field">';
  html +=           '<ul class="check_list_important">';
  html +=             '<li><input type="radio" id="dns_service_mode_proxy" name="mode" value="proxy" >';
  html +=               '<label for="dns_service_mode_proxy">Proxy</label>';
  html +=             '</li>';
  html +=             '<li><input type="radio" id="dns_service_mode_relay" name="mode" value="relay" >';
  html +=               '<label for="dns_service_mode_relay">Relay</label>';
  html +=             '</li>';
  html +=           '</ul>';
  html +=         '</div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=     '<p class="button_area">';
  html +=       '<input class="button" type="submit" value="Submit" name="submit">';
  html +=     '</p>';
  html +=   '</form>';
  html += '</div>';
  
  html += '<div id="dns_static_host_list_area">';
  html +=   '<div class="config_form_box group" >';
  html +=     '<div class="config_data_list">';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Static hosts:</div>';
  html +=         '<div class="value_field">No static hosts configured</div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=   '</div>';
  html += '</div>';
  
  html += '<div class="config_form_box" >';
  html +=   '<form id="add_dns_static_host_form" action="javascript_requested.php" method="POST">';
  html +=     '<div class="config_data_list">';
  
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"><label for="dns_service_new_static_host_ip">New static Host:</label></div>';
  html +=         '<div class="value_field">';
  html +=           '<input id="dns_service_new_static_host_ip" class="input_field half_size" type="text" name="static_host_ip" size="15" value="" >:';
  html +=           '<input id="dns_service_new_static_host_hostname" class="input_field" type="text" name="static_hostname" size="15" value="" >';
  html +=         '</div>';
  html +=       '</div>';
  
  html +=     '</div>';
  html +=     '<p class="button_area">';
  html +=       '<input class="button" type="submit" value="Add" name="submit">';
  html +=     '</p>';
  html +=   '</form>';
  html += '</div>';
  
  return html;
};


DnsServiceContent.prototype.StaticHostHtml = function(hostIndex, staticHost)
{
  var dnsServiceContent = this;
  var html              = '';

  html +=   '<form id="delete_dns_static_host_form_' + hostIndex + '" action="javascript_requested.php" method="POST">';
  html +=     '<input type="hidden" name="host_index" value="'+ hostIndex + '">';
  html +=     '<div class="config_form_box group" >';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field">Static Host ' + (parseInt(hostIndex, 10) + 1) + ':</div>';
  html +=           '<div class="value_field">';
  html +=             '<span class="host_ip">' + staticHost.ipAddr + '</span>';
  html +=             ' : <span class="hostname">';
  
  var separator = '';
  $.each(staticHost.names, function(hostnameIndex, hostname)
  {
    html += separator + hostname;
    //separator = '_';
    separator = ', ';
  });
  
  html +=             '</span>';
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


DnsServiceContent.prototype.ShowStaticHosts = function(dnsStaticHost)
{
  var dnsServiceContent  = this;

  if(dnsStaticHost.length)
  {
    $('#dns_service_content #dns_static_host_list_area').html('');
  
    $.each(dnsStaticHost, function(hostIndex, staticHost)
    {
      if(staticHost.ipAddr)
      {
        var html = dnsServiceContent.StaticHostHtml(hostIndex, staticHost);
        $('#dns_service_content #dns_static_host_list_area').append(html);
      }
    });
  }
};




DnsServiceContent.prototype.ProcessBasicConfigChangeSubmit = function(event)
{
  var dnsServiceContent = this;

  event.preventDefault();

  // get new values from form fields
  var state     = $(event.target).find('input[name=state]:checked').length ? 'enabled' : 'disabled';
  var mode      = $(event.target).find('input[name=mode]:checked').val();
  
  pageElements.ShowBusyScreen('Changing DNS configuration...');

  var newValueList  = { state: state, mode: mode };

  deviceParams.ChangeValue('dns_service_basic_config', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
  
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing DNS configuration.', errorText ]);
    }
  
    dnsServiceContent.Refresh();
  });
  
};




/*
 * create parameter string for configtool, which is a ','-separated list including all static hosts which shoul be configured
 */
DnsServiceContent.prototype.CreateStaticHostParameterString = function(deleteHostIndex, newHostIpAddr, newHostname)
{
  var dnsServiceContent  = this;
  
  var staticHostParameterString = '';
  var delimiter                 = '';
  
  // loop over all forms which show the static hosts (to delete them)
  $.each($('form[id^=delete_dns_static_host_form]'), function(formIndex, staticHostForm)
  {
    var staticHostIndex = $(staticHostForm).find('input[name=host_index]').val();
    
    // if actual host should not be deleted - add it to parameter string (else jump over, so it will no longer be configured)
    if(staticHostIndex != deleteHostIndex)
    {
      var hostIp    = $(staticHostForm).find('.host_ip').text();
      var hostname  = $(staticHostForm).find('.hostname').text();
      
      // hostname can consist of several hostnames separated by ',' - substitute them with '_'
      hostname      = hostname.replace(', ', '_');
      staticHostParameterString = staticHostParameterString + delimiter + hostIp + ':' + hostname;
      delimiter = ',';
    }
  });
  
  // if a new host should be added - do it
  if(newHostIpAddr && newHostname)
  {
    newHostname = newHostname.replace(', ', '_');
    newHostname = newHostname.replace(',', '_');
    staticHostParameterString = staticHostParameterString + delimiter + newHostIpAddr + ':' + newHostname;
  }

  return staticHostParameterString;
};



DnsServiceContent.prototype.ProcessStaticHostsConfigSubmit = function(event)
{
  var dnsServiceContent = this;
  var status            = SUCCESS;
  var errorText         = '';

  event.preventDefault();
  
  var deleteHostIndex = -1;

  if($(event.target).attr('id').match('delete_dns_static_host'))
  {
    var deleteHostIndex = $(event.target).find('[name=host_index]').val();
  }
  
  else if($(event.target).attr('id').match('add_dns_static_host'))
  {
    var newHostIp   = $(event.target).find('[name=static_host_ip]').val();
    var newHostname = $(event.target).find('[name=static_hostname]').val();
    
    if(!newHostIp || !newHostname)
    {
      $('body').trigger('event_errorOccured', [ 'Missing parameter for new static host.' ]);
      status = ERROR;
    }
  }

  if(SUCCESS === status)
  {
    var staticHostParameterString = dnsServiceContent.CreateStaticHostParameterString(deleteHostIndex, newHostIp, newHostname);
    //Log(staticHostParameterString);
    
    if(false !== staticHostParameterString)
    {
      pageElements.ShowBusyScreen('Changing DNS service configuration...');

      var newValueList  = { fixHost: staticHostParameterString };

      deviceParams.ChangeValue('dns_fix_hosts', newValueList, function(status, errorText)
      {
        pageElements.RemoveBusyScreen();
    
        if(SUCCESS != status)
        {
          $('body').trigger('event_errorOccured', [ 'Error while changing DNS service configuration.', errorText ]);
        }
    
        dnsServiceContent.Refresh();
      });
    }
  }

};

