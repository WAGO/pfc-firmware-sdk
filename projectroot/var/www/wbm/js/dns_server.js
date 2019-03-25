/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var DnsServerConfiguration = function()
{
  var dnsServerConfig = this;
  
  dnsServerConfig.paramView = new DeviceParamView();
  
  var CreateDnsServerConfigurationParamView = (function()
  {
    dnsServerConfig.paramView = new DeviceParamView();
                
    dnsServerConfig.paramView.Add(
    {
      paramId         : 'dnsServerConfig'
    });
        
    dnsServerConfig.paramView.Add(
    {
      paramId         : 'dns_server_currently_used'
    });

  })();
  
};


DnsServerConfiguration.prototype.ShowConfig = function(status, aDnsServerValues, outputElementId)
{
  var dnsServerConfig = this;

  if(SUCCESS === status)
  {
    if(!aDnsServerValues || !aDnsServerValues.length)
    {
      $('#no_server_area .value_field').html('none');
    }
    
    else 
    {
      $('#no_server_area').hide();
      
      $(aDnsServerValues).each(function(dnsServerIndex)
      {
        var dnsServerValue = this;
        var html = dnsServerConfig.GetDnsServerHtml(dnsServerIndex + 1, dnsServerValue);
      
        $('#' + outputElementId).append(html);
      });
    }
  }

};


DnsServerConfiguration.prototype.ShowCurrentlyUsedList = function(aDnsServerValues, outputElementId)
{
  var dnsServerConfig = this;
  var outputText      = '';

  if(!aDnsServerValues || !aDnsServerValues.length)
  {
    outputText = 'none';
  }
  else 
  {
    $.each(aDnsServerValues, function(dnsServerIndex, dnsServerValue)
    {
      outputText = outputText + dnsServerValue + '<br>';
    });
  }
    
  $('#' + outputElementId).html(outputText);
};


/*
 * create htmal area fram to show, add and delete DNS servers, without the actually configured servers
 */
DnsServerConfiguration.prototype.GetDnsServerAreaHtml = function()
{
  var html        = '';

  html += '<h3>DNS Server</h3>';
  html +=   '<div class="config_form_box group">';
  
  html +=     '<div id="no_server_area" >'; 
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field">Configured:</div>';
  html +=           '<div class="value_field">Unknown</div>'; 
  html +=         '</div>';
  html +=       '</div>';
  html +=     '</div>';
  
  html +=     '<div id="existing-dns-server"></div>';
  
  html +=     '<form id="add_dns_server_form"  action="javascript_requested.php" method="POST">';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="dns-server-name">New Server IP:</label></div>';
  html +=           '<div class="value_field">';
  html +=             '<input id="dns-server-name" class="input_field" type="text" name="dns-server-name" maxlength="200" >';
  html +=           '</div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input class="button" type="submit" value="Add" name="submit">';
  html +=       '</p>';
  html +=     '</form>';
  html +=   '</div>';

  html +=   '<div class="config_form_box group">';
  html +=     '<div class="config_data_list">';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Additionally used:<br>(assigned by DHCP)</div>';
  html +=         '<div class="value_field">';
  html +=           '<span id="dns_server_currently_used_list"></span>';
  html +=         '</div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=   '</div>';
  
  return html;
}



DnsServerConfiguration.prototype.GetDnsServerHtml = function(dnsServerNo, dnsServerValue)
{
  var dnsServerNo = dnsServerNo | 0;
  var html        = '';
  
  html += '<div class="config_data_list">';
  
  html +=   '<div class="label_value_pair">';
  html +=     '<div class="label_field">DNS Server ' + dnsServerNo + ':</div>';
  html +=     '<div class="value_field"><span id="existing_dns_server_name_' + dnsServerNo + '">' + dnsServerValue + '</span></div>';
  html +=   '</div>';
  html += '</div>';
  html += '<p class="button_area">';
  html +=   '<input class="button delete_dns_server_button" type="submit" value="Delete" name="delete_dns_server_' + dnsServerNo + '">';
  html += '</p>';

  return html;
};


DnsServerConfiguration.prototype.AddDnsServer = function()
{
  var dnsServerConfig  = this;

  pageElements.ShowBusyScreen("Add new DNS server...");
  
  var newValueList = { dnsServerName: $('form#add_dns_server_form input#dns-server-name').val() };

  deviceParams.ChangeValue('add_dns_server', newValueList, function(status, errorText)
  {
    $('form#add_dns_server_form input#dns-server-name').val('');
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while adding new DNS server.', errorText ]);
    }
    
    dnsServerConfig.Refresh();
  });
};


DnsServerConfiguration.prototype.DeleteDnsServer = function(dnsServerNo, dnsServerName)
{
  var dnsServerConfig  = this;

  pageElements.ShowBusyScreen("Delete DNS server " + dnsServerNo + "...");
  
  var newValueList = { dnsServerNo: dnsServerNo, dnsServerName: dnsServerName };

  deviceParams.ChangeValue('delete_dns_server', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while deleting new DNS server.', errorText ]);
    }
    
    dnsServerConfig.Refresh();
  });
};


/*
 * reset area in webpate to state of unknown DNS server configuration
 */
DnsServerConfiguration.prototype.ResetArea = function()
{
  $('#existing-dns-server').html('');
  $('#no_server_area .value_field').html('Unknown');
  $('#no_server_area').show();
};


/*
 * first creation of DNS server area in webpage
 */
DnsServerConfiguration.prototype.CreateContent = function()
{
  var dnsServerConfig = this;

  var areaHtml = dnsServerConfig.GetDnsServerAreaHtml();
  $('#dns_server_config_area').html(areaHtml);
  
  $('form#add_dns_server_form').bind('submit', function(event)
  {
    event.preventDefault();
    dnsServerConfig.AddDnsServer();
  });
};



/*
 * read complete DNS server configuration (again)
 * necessary when page is shown repeated and also if configuration (even only one DNS server) has changed 
 */
DnsServerConfiguration.prototype.Refresh = function()
{
  var dnsServerConfig  = this;

  dnsServerConfig.ResetArea();
  
  deviceParams.ReadValueGroup(dnsServerConfig.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(dnsServerConfig.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(dnsServerConfig.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading DNS server data.', errorText ]);
    }
    else
    {
      dnsServerConfig.ShowConfig(deviceParams.list['dnsServerConfig'].status, 
                                 deviceParams.list['dnsServerConfig'].value, 
                                 'existing-dns-server');
      
      $('input[name^=delete_dns_server_]').bind('click', function(event)
      {
        event.preventDefault();
        
        var dnsServerNo   = $(this).attr('name').replace('delete_dns_server_', '');
        var dnsServerName = $('#existing_dns_server_name_' + dnsServerNo).html();
        
        dnsServerConfig.DeleteDnsServer(dnsServerNo, dnsServerName);
      });
    }
    
    // for TEST
    //deviceParams.list['dns_server_currently_used'].value.push("123.321.123.321");
    //deviceParams.list['dns_server_currently_used'].value = [];
    //deviceParams.list['dns_server_currently_used'].value.push('192.168.1.18');

    var additionalUsedList = $.getUniqueEntriesOfExtendedList(deviceParams.list['dnsServerConfig'].value,   
                                                              deviceParams.list['dns_server_currently_used'].value,
                                                              'ip');
    dnsServerConfig.ShowCurrentlyUsedList(additionalUsedList, 'dns_server_currently_used_list');
  });
};
