/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function FirewallMacAddressFilterContent(id, clientMacAddress)
{
  var macAddressFilterContent = this;
  
  macAddressFilterContent.id = id || 'mac_address_filter_content';
  macAddressFilterContent.clientMacAddress  = clientMacAddress || '';
  macAddressFilterContent.clientMacMask     = 'ff:ff:ff:00:00:00';
  
  // data of first (default) entry in whitelist 
  macAddressFilterContent.defaultEntryMac   = '00:30:DE:00:00:00';
  macAddressFilterContent.defaultEntryMask  = 'ff:ff:ff:00:00:00';
  
  
  var CreateMacAddressFilterContentParamView = (function()
  {
    macAddressFilterContent.paramView = new DeviceParamView();
                
    macAddressFilterContent.paramView.Add(
    {
      paramId         : 'ebtables_config'
    });
        
    macAddressFilterContent.paramView.Add(
    {
      paramId         : 'interface_config_xml'
    });

  })();

  

  //------------------------------------------------------------------------------
  //first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(macAddressFilterContent.id + '_create', function(event)
  {
    // change mac address whitelist mode (=general state)
    //------------------------------------------------------------------------------
    $('#' + macAddressFilterContent.id + '_content #mac_address_filter_mode_form').bind('submit', function(event)
    {
      macAddressFilterContent.ProcessChangeModeSubmit(event);
    });
      
    
    // change some of the configurations for (unknown count) of single interface 
    $('#' + macAddressFilterContent.id + '_content #firewall_ethernet_interface_config_area').bind('submit', function(event)
    {
      macAddressFilterContent.ProcessChangeInterfaceStateSubmit(event);
    });


    // edit/delete single mac address
    //------------------------------------------------------------------------------
    $('#' + macAddressFilterContent.id + '_content #mac_address_filter_whitelist_area').bind('click', function(event)
    //$('#' + macAddressFilterContent.id + '_content #mac_address_filter_whitelist_area').bind('submit', function(event)
    {
      if('submit' === event.target.type)
      {
        if($(event.target).attr('id').match('delete')) macAddressFilterContent.ProcessDeleteFilterClick(event);
        else if ($(event.target).attr('id').match('change_state')) macAddressFilterContent.ProcessStateChangeFilterClick(event);
      }
    });
    
    
    // add new mac address filter
    //------------------------------------------------------------------------------
    $('#' + macAddressFilterContent.id + '_content #add_mac_address_filter_form').bind('submit', function(event)
    {
      macAddressFilterContent.ProcessAddFilterSubmit(event);
    });
      
  });

   
  //------------------------------------------------------------------------------
  //refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(macAddressFilterContent.id + '_refresh', function(event)
  {
    macAddressFilterContent.Refresh();
  });

};


FirewallMacAddressFilterContent.prototype.Refresh = function()
{
  var firewallMacAddressFilterContent = this;

  deviceParams.ReadValueGroup(firewallMacAddressFilterContent.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(firewallMacAddressFilterContent.paramView.list)) 
    {
      $('body').trigger('event_errorOccured', [ 'Error while reading firewall configuration.', 
                                                deviceParams.list['ebtables_config'].errorText ] );
    }
    else
    {
      var interfaceConfig = deviceParams.list['interface_config_xml'].value;
      var ebtables        = deviceParams.list['ebtables_config'].value;
      
      firewallMacAddressFilterContent.CreateInterfaceStateContent(interfaceConfig, ebtables);

      // show general address filter state
      $('#mac_address_filter_mode_enabled').removeAttr('checked');
      if('whitelist' === ebtables.ethernet.mode)
      {
        $('#mac_address_filter_mode_enabled').prop('checked', 'checked');
      }
      
      // show list - first remove old content
      $("#mac_address_filter_whitelist_area").html('');
      
      var filterCount = 0;
      
      // if at least than one host is existing
      if(ebtables.ethernet.whitelist.host)
      {
        // if there is only one host (in this case, xml conversion creates no array - host has no length)
        if(!ebtables.ethernet.whitelist.host.length)
        {
          // create an array on our own (we need it for following $.each)
          ebtables.ethernet.whitelist.host = [ ebtables.ethernet.whitelist.host ];
        }
        
        $.each(ebtables.ethernet.whitelist.host, function(filterIndex, host)
        {
          // get and show html frame for filter
          $("#mac_address_filter_whitelist_area").append(firewallMacAddressFilterContent.FilterHtmlFrame(filterIndex));
        
          var hostArea  = $("#mac_address_filter_form_" + filterIndex);
          if(host.mac) $(hostArea).find('.mac_address_filter_address').html(host.mac);
          if(host.mask) $(hostArea).find('.mac_address_filter_mask').html(host.mask);

          // insert filter data
          $('#state_output_mac_address_filter_' + filterIndex).removeAttr('checked');
          if('on' === host.state)
          {
            $('#state_output_mac_address_filter_' + filterIndex).prop('checked', 'checked');
          }
        
          ++filterCount;
        });
      }
      
      // no filter configured? Show according text
      if(0 === filterCount)
      {
        var html = '<div class="config_form_box group"><div class="config_data_list"><div class="label_value_pair"><div class="value_field">No filter configured.</div></div></div></div>';
        $("#mac_address_filter_whitelist_area").append(html);
      }
    }
    
  });

};


FirewallMacAddressFilterContent.prototype.CreateInterfaceStateContent = function(interfaceConfig, ebtables)
{
  var firewallMacAddressFilterContent = this;

  // remove old content
  $('#firewall_ethernet_interface_config_area').html('');
  
  if(interfaceConfig && ebtables)
  {
    // create html areas for all configured interfaces
    $.each(interfaceConfig.ifaces, function(deviceId, iFace)
    {
      if(iFace.ipSettings) // && iFace.ipSettings.showInWbm)
      {
        var iFaceId = iFace.ipSettings.portName;
    
        var iFaceHtmlFrame = firewallMacAddressFilterContent.InterfaceHtmlFrame(iFace.ipSettings.portName);
        $('#firewall_ethernet_interface_config_area').append(iFaceHtmlFrame);
    
        firewallMacAddressFilterContent.ShowFirewallStateForInterface(iFaceId, ebtables);
      }
    });
    
    if(1 == $('#' + firewallMacAddressFilterContent.id + '_content form[id^=form_firewall_ethernet_interface_state]').length)
    {
      $('#' + firewallMacAddressFilterContent.id + '_content .firewall_ethernet_interface_state_port_id').text('X1/X2');
    }
  }
};



FirewallMacAddressFilterContent.prototype.InterfaceHtmlFrame = function(iFaceId)
{
  var html = '';

  html +=   '<h3>MAC address filter state <span class="firewall_ethernet_interface_state_port_id">' + iFaceId + '</span></h3>';

  html +=   '<div class="config_form_box" >';
  html +=     '<form id="form_firewall_ethernet_interface_state_' + iFaceId + '" action="javascript_requested.php" method="POST">';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="firewall_ethernet_enabled_interface_' + iFaceId + '">Filter enabled:</label></div>';
  html +=           '<div class="value_field" >';
  html +=             '<input id="firewall_ethernet_enabled_interface_' + iFaceId + '" type="checkbox" name="firewall_enabled">';
  html +=           '</div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area" >';
  html +=         '<input id="change_state_button_firewall_ethernet_interface_state_' + iFaceId + '" class="button" type="submit" value="Submit" name="change_state">';
  html +=       '</p>';
  html +=     '</form>';
  html +=   '</div>';

  return html;
};

FirewallMacAddressFilterContent.prototype.ShowFirewallStateForInterface = function(iFaceId, ebtables)
{
  var firewallMacAddressFilterContent = this;

  // show default as "firewall active" (box checked), no entry means, firewall is active
  $('#' + firewallMacAddressFilterContent.id + '_content #firewall_ethernet_enabled_interface_' + iFaceId).prop('checked', 'checked');

  if(ebtables.ethernet.interfaces[iFaceId].state && ('open' === ebtables.ethernet.interfaces[iFaceId].state))
  {
    $('#' + firewallMacAddressFilterContent.id + '_content #firewall_ethernet_enabled_interface_' + iFaceId).removeAttr('checked');
  }
};



FirewallMacAddressFilterContent.prototype.FilterHtmlFrame = function(filterIndex)
{
  var html = '';

  html +=  '<div class="config_form_box group">';

  html +=    '<form id="mac_address_filter_form_' + filterIndex + '" action="javascript_requested.php" method="POST">';
  html +=      '<div class="config_data_list">';
  html +=        '<div class="label_value_pair">';
  html +=          '<div class="label_field">MAC address:</div>';
  html +=          '<div class="value_field"><span class="mac_address_filter_address"></span></div>';
  html +=        '</div>';
  html +=        '<div class="label_value_pair">';
  html +=          '<div class="label_field">MAC mask:</div>';
  html +=          '<div class="value_field"><span class="mac_address_filter_mask"></span></div>';
  html +=        '</div>';
  html +=      '</div>';
  html +=      '<p class="button_area">';
  html +=        '<input id="delete_button_mac_address_filter_' + filterIndex + '" class="button" type="submit" value="Delete" name="delete">';
  html +=      '</p>';
 
  html +=      '<div class="config_data_list">';
  html +=        '<div class="label_value_pair">'
  html +=          '<div class="label_field">';
  html +=            '<label for="mac_address_filter_state_' + filterIndex + '">Filter enabled:</label>';
  html +=          '</div>';
  html +=          '<div class="value_field">';
  html +=            '<input id="state_output_mac_address_filter_' + filterIndex + '" type="checkbox" name="state_output_mac_address_filter" value="enabled" >';
  html +=          '</div>';
  html +=        '</div>';
  html +=      '</div>';
  html +=      '<p class="button_area">';
  html +=       '<input id="change_state_button_mac_address_filter_' + filterIndex + '" class="button" type="submit" value="Submit" name="submit">';
  html +=      '</p>';
  html +=    '</form>';
  html +=  '</div>';

  return html;
};


/*
FirewallMacAddressFilterContent.prototype.ClientMacIsWhitelistMember = function()
{
  var firewallMacAddressFilterContent = this;
  var clientMacIsWhitelistMember      = false;
  var ebtables                        = deviceParams.list['ebtables_config'].value;

  if(firewallMacAddressFilterContent.clientMacAddress)
  {
    // if at least than one host is existing
    if(ebtables.ethernet.whitelist.host)
    {
      // if there is only one host (in this case, xml conversion creates no array - host has no length)
      if(!ebtables.ethernet.whitelist.host.length)
      {
        // create an array on our own (we need it for following $.each)
        ebtables.ethernet.whitelist.host = [ ebtables.ethernet.whitelist.host ];
      }
    
      $.each(ebtables.ethernet.whitelist.host, function(filterIndex, host)
      {
        if(host.mac && (host.mac  === firewallMacAddressFilterContent.clientMacAddress))
        {
          clientMacIsWhitelistMember = true;
          return false;
        }
      });
    }
  }
  
  return clientMacIsWhitelistMember;
};
*/


/*
 * returns true, if whitelist includes at least one enabled entry which is not the default entry
 */
FirewallMacAddressFilterContent.prototype.EnabledUserEntryExcludingDefaultExisting = function()
{
  var firewallMacAddressFilterContent = this;
  var userEntryExisting               = false;
  var ebtables                        = deviceParams.list['ebtables_config'].value;
  
  // if at least than one host is existing
  if(ebtables.ethernet.whitelist.host)
  {
    // if there is only one host (in this case, xml conversion creates no array - host has no length)
    if(!ebtables.ethernet.whitelist.host.length)
    {
      // create an array on our own (we need it for following $.each)
      ebtables.ethernet.whitelist.host = [ ebtables.ethernet.whitelist.host ];
    }
  
    $.each(ebtables.ethernet.whitelist.host, function(filterIndex, host)
    {
      // found default entry - search along
      if(   (host.mac   && (host.mac    === firewallMacAddressFilterContent.defaultEntryMac))
         && (host.mask  && (host.mask   === firewallMacAddressFilterContent.defaultEntryMask)) )
      {
        return true;
      }
      
      // entry is not default - if if is enabled, too, we found a specified user entry
      if(host.state && (host.state === 'on'))
      {
        userEntryExisting = true;
        return false;
      }
    });
  }
  
  return userEntryExisting;
};


/*
 * returns the count of enabled whitelist entries excluding default address
 */
FirewallMacAddressFilterContent.prototype.EnabledEntryCountExcludingDefault = function()
{
  var firewallMacAddressFilterContent = this;
  var userEntryCount                  = 0;
  var ebtables                        = deviceParams.list['ebtables_config'].value;
  
  // if at least than one host is existing
  if(ebtables.ethernet.whitelist.host)
  {
    // if there is only one host (in this case, xml conversion creates no array - host has no length)
    if(!ebtables.ethernet.whitelist.host.length)
    {
      // create an array on our own (we need it for following $.each)
      ebtables.ethernet.whitelist.host = [ ebtables.ethernet.whitelist.host ];
    }
  
    $.each(ebtables.ethernet.whitelist.host, function(filterIndex, host)
    {
      // found default entry - search along
      if(   (host.mac   && (host.mac    === firewallMacAddressFilterContent.defaultEntryMac))
         && (host.mask  && (host.mask   === firewallMacAddressFilterContent.defaultEntryMask)) )
      {
        return true;
      }
      
      // entry is not default - if if is enabled, too, we found a specified user entry
      if(host.state && (host.state === 'on'))
      {
        ++userEntryCount;
      }
    });
  }
  return userEntryCount;
};


/*
 * returns the count of enabled whitelist entries
 */
FirewallMacAddressFilterContent.prototype.EnabledEntryCount = function()
{
  var firewallMacAddressFilterContent = this;
  var ebtables                        = deviceParams.list['ebtables_config'].value;
  var enabledEntryCount               = 0;
  
  // if at least than one host is existing
  if(ebtables.ethernet.whitelist.host)
  {
    // if there is only one host (in this case, xml conversion creates no array - host has no length)
    if(!ebtables.ethernet.whitelist.host.length)
    {
      // create an array on our own (we need it for following $.each)
      ebtables.ethernet.whitelist.host = [ ebtables.ethernet.whitelist.host ];
    }
  
    $.each(ebtables.ethernet.whitelist.host, function(filterIndex, host)
    {
      if(host.state && (host.state === 'on'))
      {
        ++enabledEntryCount;
      }
    });
  }
  
  return enabledEntryCount;
};


/*
FirewallMacAddressFilterContent.prototype.EntryIsEnabledUserEntry = function(filterIndex)
{
  var firewallMacAddressFilterContent = this;
  var ebtables                        = deviceParams.list['ebtables_config'].value;
  var entryIsEnabledUserEntry         = false;
  
  // if at least than one host is existing
  if(ebtables.ethernet.whitelist.host)
  {
    // if there is only one host (in this case, xml conversion creates no array - host has no length)
    if(!ebtables.ethernet.whitelist.host.length)
    {
      // create an array on our own (we need it for following $.each)
      ebtables.ethernet.whitelist.host = [ ebtables.ethernet.whitelist.host ];
    }
  
    var host = ebtables.ethernet.whitelist.host[filterIndex];
    if(   host 
       && host.mac   && (host.mac  != firewallMacAddressFilterContent.defaultEntryMac)
       && host.mask  && (host.mask != firewallMacAddressFilterContent.defaultEntryMask)
       && host.state && (host.state === 'on') )
    {
      entryIsEnabledUserEntry = true;
    }
  }

  return entryIsEnabledUserEntry;
};
*/



FirewallMacAddressFilterContent.prototype.UserClientCheck = function()
{
  var firewallMacAddressFilterContent = this;
  var checkOk                         = true;
  
  if(0 === firewallMacAddressFilterContent.EnabledEntryCount())
  {
    $('body').trigger('event_errorOccured', [ 'Attention:\nWhite List is currently empty;\nthe MAC address filter will not be activated!' ]);
    checkOk = false;
  }
  
  //else if(!confirm('Connection can be lost, if your own MAC address is not contained in whitelist. Do you want to continue?'))
  else if(!confirm('Attention: Have you checked whether your own MAC address is given and enabled in the whitelist?\nIf not, you will have no access from your system once it has been activated.\nDo you want to continue?'))
  {
    checkOk = false;
  }
  
  return checkOk;
};

  



FirewallMacAddressFilterContent.prototype.ProcessChangeModeSubmit = function(event)
{
  var firewallMacAddressFilterContent = this;

  event.preventDefault();
  var newMode = ($(event.target).find('input[name=mac_address_filter_mode_enabled]:checked').length) ? 'whitelist' : 'all-allow';
  
  // if filter should be enabled - check if it is ok
  if('whitelist' === newMode)
  {
    if(!firewallMacAddressFilterContent.UserClientCheck())
    {
      newMode = '';
      $(event.target).find('input[name=mac_address_filter_mode_enabled]').removeAttr('checked');
    }
  }
  
  if(newMode.length)
  {
    pageElements.ShowBusyScreen('Changing MAC address filter mode...');
  
    var newValueList  = { mode: newMode };

    deviceParams.ChangeValue('mac_address_filter_mode', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
    
      // error while processing - evaluate if problems with connection are probable
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing MAC address filter mode.', errorText ]);
        firewallMacAddressFilterContent.Refresh();
      }
    
      // no error while processing
      else
      {
        // nothing to do, new state is already visible on screen (only one checkbox) 
      }
    });
  }

};



FirewallMacAddressFilterContent.prototype.ProcessStateChangeFilterClick = function(event)
{
  var firewallMacAddressFilterContent = this;
  event.preventDefault();

  var formObj     = $(event.target).parents('form');
  
  var filterIndex = $(event.target).attr('id').replace('change_state_button_mac_address_filter_', '');
  var newState    = ($(formObj).find('[name=state_output_mac_address_filter]:checked').length) ? 'on' : 'off';
  var macAddress  = $(formObj).find('.mac_address_filter_address').text();
  var macMask     = $(formObj).find('.mac_address_filter_mask').text();
  
  var globalFilterEnabled     = $('#' + firewallMacAddressFilterContent.id + '_content input#mac_address_filter_mode_enabled:checked').length ? true : false;
  var interfaceFilterEnabled  = $('#' + firewallMacAddressFilterContent.id + '_content input[id^=firewall_ethernet_enabled_interface]:checked').length ? true : false;
  
  // MAC address filter is enabled and user tries to disable the last enabled whitelist entry
  if((globalFilterEnabled || interfaceFilterEnabled) && ('off' === newState) && (1 === firewallMacAddressFilterContent.EnabledEntryCount()))
  {
    var errorText = 'Connection to the device may be lost by disabling this whitelist entry.\nPlease, deactivate the filter first.';
    $('body').trigger('event_errorOccured', [ errorText ]);
    $(formObj).find('[name=state_output_mac_address_filter]').prop('checked', 'checked');
  }
  
  else
  {
    pageElements.ShowBusyScreen('Changing MAC address filter state...');
  
    var newValueList  = { hostNumber: parseInt(filterIndex, 10) + 1, state: newState, macAddress: macAddress, macMask: macMask };

    deviceParams.ChangeValue('mac_address_filter_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
    
      // error while processing - evaluate if problems with connection are probable
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing MAC address filter state.', errorText ]);
      }
    
      // refresh page all the time because we need the actual data for checks!
      firewallMacAddressFilterContent.Refresh();
    });
  }

};

FirewallMacAddressFilterContent.prototype.ProcessChangeInterfaceStateSubmit = function(event)
{
  var firewallMacAddressFilterContent = this;

  event.preventDefault();
  
  var iFaceId       = $(event.target).attr('id').replace('form_firewall_ethernet_interface_state_', '');
  var newState      = ($(event.target).find('[name=firewall_enabled]:checked').length) ? 'filtered' : 'open'; 
  var newValueList  = { state: newState, iFaceId: iFaceId };

  // if filter should be enabled - check if it is ok
  if('filtered' === newState)
  {
    if(!firewallMacAddressFilterContent.UserClientCheck()) 
    {
      newState = ''; 
      $(event.target).find('[name=firewall_enabled]').removeAttr('checked');
    }
  }
  
  if(newState)
  {
    pageElements.ShowBusyScreen('Change firewall configuration...');
    
    deviceParams.ChangeValue('interface_firewall_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();

      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing firewall configuration', errorText ]);
      }
    
      firewallMacAddressFilterContent.Refresh();
    });
  }
};


FirewallMacAddressFilterContent.prototype.ProcessDeleteFilterClick = function(event)
{
  var firewallMacAddressFilterContent = this;

  event.preventDefault();
  
  var whitelistEntryIndex     = $(event.target).attr('id').replace('delete_button_mac_address_filter_', '');
  var whitelistEntryEnabled   = $(event.target).parents('form').find('[name=state_output_mac_address_filter]:checked').length ? true : false;
  var globalFilterEnabled     = $('#' + firewallMacAddressFilterContent.id + '_content input#mac_address_filter_mode_enabled:checked').length ? true : false;
  var interfaceFilterEnabled  = $('#' + firewallMacAddressFilterContent.id + '_content input[id^=firewall_ethernet_enabled_interface]:checked').length ? true : false;
  
  // MAC address filter is enabled and user tries to delete the last enabled whitelist entry
  if((globalFilterEnabled || interfaceFilterEnabled) && whitelistEntryEnabled  && (1 === firewallMacAddressFilterContent.EnabledEntryCount()))
  {
    var errorText = 'Connection to the device may be lost by deleting this whitelist entry.\nPlease, deactivate the filter first.';
    $('body').trigger('event_errorOccured', [ errorText ]);
  }
  
  else
  {
    pageElements.ShowBusyScreen('Deleting MAC address filter...');
  
    var newValueList  = { hostNumber: parseInt(whitelistEntryIndex, 10) + 1 };

    deviceParams.ChangeValue('mac_address_filter_delete', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
    
      // error while processing - evaluate if problems with connection are probable
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while deleting MAC address filter.', errorText ]);
      }
    
      firewallMacAddressFilterContent.Refresh();
    });
  }
};


FirewallMacAddressFilterContent.prototype.ProcessAddFilterSubmit = function(event)
{
  var firewallMacAddressFilterContent = this;

  event.preventDefault();

  pageElements.ShowBusyScreen('Creating new MAC address filter...');
  
  var macAddress    = $(event.target).find('[name=new_mac_address]').val();
  var macMask       = $(event.target).find('[name=new_mac_mask]').val();
  var state         = $(event.target).find('[name=new_mac_address_filter_state]:checked').length ? 'on' : 'off';
  var newValueList  = { state: state, macAddress: macAddress, macMask: macMask };

  deviceParams.ChangeValue('mac_address_filter_add', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    // error while processing - evaluate if problems with connection are probable
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while creating MAC address filter.', errorText ]);
    }
    
    // reset form values and reload data
    $(event.target).find('[name=new_mac_address]').val('');
    $(event.target).find('[name=new_mac_mask]').val('');
    $(event.target).find('[name=new_mac_address_filter_state]').removeAttr('checked');
    
    firewallMacAddressFilterContent.Refresh();
  });
  

};

