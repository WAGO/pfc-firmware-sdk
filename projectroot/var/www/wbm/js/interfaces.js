/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

/*------------------------------------------------------------------------------
 * Object IpAddressConfiguration
 * 
 * needs object interface configuration 
 *------------------------------------------------------------------------------
 */
function IpAddressConfiguration()
{
  var ipAddressConfiguration = this;

};
  

/*
 * Disable or enable several input fiels which are only interesting, if interface is on
 */
IpAddressConfiguration.prototype.ShowInterfaceStateDependencies = function(iFaceId)
{
  var ipAddressConfiguration  = this;
  var iFaceArea               = $('#ip_address_config_area #ip_address_config_' + iFaceId);
  var interfaceState          = $(iFaceArea).find('name^=state]:checked').val();
  
  if('disabled' === interfaceState)
  {
    $(iFaceArea).find('[name^=config_type]').attr('disabled', true);
    $(iFaceArea).find('[name^=static_ip_address]').attr('disabled', true);
    $(iFaceArea).find('[name^=subnet_mask]').attr('disabled', true);
    //$("input#ip_address_" + interfaceId + "").addClass("input_field_greyed");
    //$("input#subnet_mask_" + interfaceId + "").addClass("input_field_greyed");
  }
  else if('enabled' === interfaceState)
  {
    $(iFaceArea).find('[name^=config_type]').removeAttr('disabled');
    $(iFaceArea).find('[name^=static_ip_address]').removeAttr('disabled');
    $(iFaceArea).find('[name^=subnet_mask]').removeAttr('disabled');
    //$("input#ip_address_" + interfaceId + "").removeClass("input_field_greyed");
    //$("input#subnet_mask_" + interfaceId + "").removeClass("input_field_greyed");
  }
};


/*
 * Disable or enable several input fiels which are only interesting, if config type is static
 */
IpAddressConfiguration.prototype.ShowConfigTypeDependencies = function(iFaceId)
{
  var ipAddressConfiguration  = this;
  var iFaceArea               = $('#ip_address_config_area #ip_address_config_' + iFaceId);
  var configType              = $(iFaceArea).find('[name^=config_type]:checked').val();
  //var interfaceState          = $(iFaceArea).find('[name^=state]:checked').val();
  var interfaceState          = 'enabled';
  
  if(('static' === configType) && ('enabled' === interfaceState))
  {
    $(iFaceArea).find('[name^=static_ip_address]').removeAttr('disabled');
    $(iFaceArea).find('[name^=subnet_mask]').removeAttr('disabled');
    //$("input#static_ip_address_" + interfaceId + "").removeClass("input_field_greyed");
    //$("input#subnet_mask_" + interfaceId + "").removeClass("input_field_greyed");
  }

  else if(('' === configType) || ('dhcp' === configType) || ('bootp' === configType))
  {
    $(iFaceArea).find('[name^=static_ip_address]').attr('disabled', true);
    $(iFaceArea).find('[name^=subnet_mask]').attr('disabled', true);
    //$("input#static_ip_address_" + interfaceId + "").addClass("input_field_greyed");
    //$("input#subnet_mask_" + interfaceId + "").addClass("input_field_greyed");
  }
};



IpAddressConfiguration.prototype.InterfaceHtml = function(interfaceId, iFaceLabel)
{
  var iFaceLabel = iFaceLabel || interfaceId;
  var html = '';
  
  html += '<div id="ip_address_config_' + interfaceId + '">';
  html +=   '<h3>IP Configuration ' + iFaceLabel + '</span></h3>';
  html +=   '<form id="config_interface_form_' + interfaceId + '" action="javascript_requested.php" method="POST">';
  html +=     '<input type="hidden" name="interface_id" value="'+ interfaceId + '">';
  html +=     '<div class="config_form_box" >';
  html +=       '<div class="config_data_list">';
  
//  html +=         '<div class="label_value_pair several_interfaces">';
//  html +=           '<div class="label_field">State:</div>';
//  html +=           '<div class="value_field">';
//  html +=             '<ul class="check_list_important">';
//  html +=               '<li><input id="enabled_' + interfaceId + '" type="radio" name="state_' + interfaceId + '" value="enabled">';
//  html +=                 '<label for="enabled_' + interfaceId + '">enabled</label></li>';
//  html +=               '<li><input id="disabled_' + interfaceId + '" type="radio" name="state_' + interfaceId + '" value="disabled" >';
//  html +=                 '<label for="disabled_' + interfaceId + '">disabled</label></li>';
//  html +=             '</ul>';
//  html +=           '</div>';
//  html +=         '</div>';

//  html += '      <div class="label_value_pair">';
//  html += '        <div class="label_field">Type of Configuration:</div>';
//  html += '        <div class="value_field">';
//  html += '          <select class="input_field" id="config_type_' + interfaceId + '" name="config_type_' + interfaceId + '">';
//  html += '            <option id="static_' + interfaceId + '"  value="static" >Static IP</option>';
//  html += '            <option id="dhcp_' + interfaceId + '"    value="dhcp" >DHCP</option>';
//  html += '            <option id="bootp_' + interfaceId + '"   value="bootp" >BootP</option>';
//  html += '          </select> 
//  html += '        </div>
//  html += '      </div>
    
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field">Configuration Type:</div>';
  html +=           '<div class="value_field">';
  html +=             '<ul class="check_list_important">';
  html +=               '<li><input id="static_' + interfaceId + '" type="radio" name="config_type_' + interfaceId + '" value="static" >';
  html +=                 '<label for="static_' + interfaceId + '">Static IP</label></li>';
  html +=               '<li><input id="dhcp_' + interfaceId + '" type="radio" name="config_type_' + interfaceId + '" value="dhcp" >';
  html +=                 '<label for="dhcp_' + interfaceId + '">DHCP</label></li>';
  html +=               '<li><input id="bootp_' + interfaceId + '" type="radio" name="config_type_' + interfaceId + '" value="bootp" >';
  html +=                 '<label for="bootp_' + interfaceId + '">BootP</label></li>';
  html +=             '</ul>';
  html +=           '</div>';
  html +=         '</div>';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="static_ip_address_' + interfaceId + '">IP Address:</label></div>';
  html +=           '<div class="value_field"><input id="static_ip_address_' + interfaceId + '" class="input_field" type="text" name="static_ip_address_' + interfaceId + '" maxlength="16" value="" ></div>';
  html +=         '</div>';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="subnet_mask_' + interfaceId + '">Subnet Mask:</label></div>';
  html +=           '<div class="value_field"><input id="subnet_mask_' + interfaceId + '" class="input_field" type="text" name="subnet_mask_' + interfaceId + '" maxlength="16"  value="" ></div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input id="config_interface_submit_button_' + interfaceId + '" class="button" type="submit" value="Submit" name="submit">';
  html +=       '</p>';
  html +=     '</div>';
      
  html +=   '</form>';
  html += '</div>';
  
  return html;
};



IpAddressConfiguration.prototype.Create = function()
{
  var ipAddressConfig = this;

  // add globlal event handling (submit of form in ip address area)
  $('#ip_address_config_area').bind('submit', function(event)
  {
    event.preventDefault();
    ipAddressConfig.ProcessConfigFormSubmit(event.target);
  });

  // event - click on link to another content area
  $('#default_gateway_config_area a.content_area_link').bind('click', function(event)
  {
    event.preventDefault();
    event.stopPropagation();

    // filter last element from complete link ("index.php#routing") - right format for FollowNavLink function
    var navLink = $(event.target).prop('href').replace(/.*\//, '');

    // change content
    pageBuildup.FollowNavLink(navLink);
  });

};

  

IpAddressConfiguration.prototype.Refresh = function()
{
  var ipAddressConfig = this;
  
  deviceParams.ReadValue({ id: 'interface_config_xml' }, function()
  {
    //Log('ipAddressConfig - refresh by xml struct');
    ipAddressConfig.RefreshDisplay();
  });
};


IpAddressConfiguration.prototype.RefreshDisplay = function()
{
  var ipAddressConfig = this;
  var interfaceConfig = deviceParams.list['interface_config_xml'].value;
  var iFaceCount      = 0;
  
  $('#ip_address_config_area').html('');
  
  if((SUCCESS != deviceParams.list['interface_config_xml'].status) || !interfaceConfig.ifaces)
  {
    var errorString = deviceParams.list['interface_config_xml'].errorText;
    $('body').trigger('event_errorOccured', [ 'Error while reading interface configuration.', errorString ] );
  }
    
  else
  {
    // create html areas for all configured interfaces
    $.each(interfaceConfig.ifaces, function(iFaceId, iFace)
    {
      if(iFace.ipSettings) // && iFace.ipSettings.showInWbm)
      {
        // if interfaces are switched - conform output port term
        var iFaceLabel = (0 == interfaceConfig.dsaMode) ? 'X1/X2' : iFace.ipSettings.portName;

        // create html frame and add single values explicit
        var html = ipAddressConfig.InterfaceHtml(iFace.ipSettings.portName, iFaceLabel); 
        $('#ip_address_config_area').append(html);
    
        var iFaceArea = $('#ip_address_config_area #ip_address_config_' + iFace.ipSettings.portName);
        $(iFaceArea).find('[id^=device_id]').html(iFace.ipSettings.portName);
        //$(iFaceArea).find('[name^=state][value=' + iFace.state + ']').attr('checked', 'checked');
        $(iFaceArea).find('[name^=config_type][value=' + iFace.ipSettings.type + ']').prop('checked', 'checked');
        $(iFaceArea).find('[name^=static_ip_address]').val(iFace.ipSettings.staticIpAddr);
      
        var ipstatus = new $.CIDR(iFace.ipSettings.staticIpAddr + "/" + iFace.ipSettings.staticNetmask);
        $(iFaceArea).find('[name^=subnet_mask]').val(ipstatus.getNetmask().getDot());
      
        ++iFaceCount;
      }
    });
  
    // show DOM elements which are only necessary in case of more than one interface
    if(iFaceCount > 1)
    {
      $('.several_interfaces').addClass('existing');
    }
  
    // actually no disabling of input fields depending on other parameters, because user should change these params in this cases, too 
    //ipAddressConfig.AddSpecificEventHandling();
  }
};


IpAddressConfiguration.prototype.AddSpecificEventHandling = function()
{
  var ipAddressConfig = this;
 
  // add event processing for all interface areas
  $('#ip_address_config_area input[name^=state]').bind('change', function(event)
  {
    var interfaceId = $(this).attr('name').replace('state_', '');
    ipAddressConfig.ShowInterfaceStateDependencies(interfaceId);
    ipAddressConfig.ShowConfigTypeDependencies(interfaceId);
  });

  $('#ip_address_config_area input[name^=config_type]').bind('change', function(event)
  {
    var interfaceId = $(this).attr('name').replace('config_type_', '');
    ipAddressConfig.ShowConfigTypeDependencies(interfaceId);
  });

  // trigger events to pass event handling once 
  $('#ip_address_config_area input[name^=state]').trigger('change');
  $('#ip_address_config_area input[name^=config_type]').trigger('change');
};


IpAddressConfiguration.prototype.ProcessConfigFormSubmit = function(formObj)
{  
  var ipAddressConfiguration = this;
  
  var status      = SUCCESS;
  var iFaceId     = $(formObj).find('input[name^=interface_id]').val();
  //var state       = $(formObj).find('input[name^=state]:checked').val();
  var state       = 'enabled';
  var configType  = $(formObj).find('input[name^=config_type]:checked').val();
  var ipAddress   = $(formObj).find('input[name^=static_ip_address]').val();
  var subnetMask  = $(formObj).find('input[name^=subnet_mask]').val();
  
  // input parameter check
  if(!interfaceConfig.IpAddressOk(ipAddress))
  {
    status  = ERROR;
    $('body').trigger('event_errorOccured', [ 'Invalid IP address.' ]);
  }
  /*
  else if(!interfaceConfig.IpAddressOk(subnetMask))
  {
    status  = ERROR;
    $('body').trigger('event_errorOccured', [ 'Invalid subnet Mask.' ]);
  }
  */
  
  if(SUCCESS === status)
  {
    pageElements.ShowBusyScreen('Change interface configuration...\nNote: connection may be lost afterwards!');

    var newValueList = { interfaceId: iFaceId, state: state, configType: configType, ipAddress: ipAddress, subnetMask: subnetMask };

    deviceParams.ChangeValue('config_ip_address', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
      
      // execution errors while configuration of interface - show error, reload actual values
      if((SUCCESS != status) && (COM_TIMEOUT != status)) //!errorText.match(/^timeout/i))
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing interface configuration.', errorText ]);
        ipAddressConfiguration.Refresh();
      }
        
      // timeout while waiting for answer -> we lost connection, interface address changed obviously successful
      else if((SUCCESS != status) && (COM_TIMEOUT === status)) //errorText.match(/^timeout/i))
      {
        // if we now new ip address (ip is still static) - try to reload WBM from it
        if('static' === configType)
        {
          var newLocation = window.location.href.replace(window.location.host, ipAddress);
          pageElements.ShowBusyScreen('Connection lost - try forwarding to ' + ipAddress);
          window.location.href = newLocation;
        }
        
        // bootp or dhcp - we don't know new WBM address and just show an hint for user
        else
        {
          $('body').trigger('event_errorOccured', [ 'Connection lost - please reload page with proper address.' ]);
        }
      }
      else
      {
        //ipAddressConfiguration.Refresh();
        $('#content_area').trigger('interfaces_refresh');
      }
    });
  }
  
  return false;
};



/*------------------------------------------------------------------------------
 * Object InterfacesContent
 * 
 * handle WBM content page "TCP/IP" 
 *------------------------------------------------------------------------------
 */
function InterfacesContent(id)
{
  var interfacesContent = this;
  
  interfacesContent.id = id || 'interfaces';
  interfacesContent.ipAddressConfiguration      = new IpAddressConfiguration;
  interfacesContent.defaultGatewayConfiguration = new DefaultGatewayConfiguration;
  interfacesContent.dnsServerConfiguration      = new DnsServerConfiguration;
  
  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(interfacesContent.id + '_create', function(event)
  {
    //Log("Event - interfaces create")
    
    interfacesContent.ipAddressConfiguration.Create();
    interfacesContent.defaultGatewayConfiguration.Create();
    interfacesContent.dnsServerConfiguration.CreateContent();
  });

      
  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(interfacesContent.id + '_refresh', function(event)
  {
    //Log("Event - interfaces refresh");
    
    deviceParams.ReadValue({ id: 'interface_config_xml' }, function()
    {
      if(SUCCESS != deviceParams.list['interface_config_xml'].status)
      {
        var errorText = deviceParams.list['interface_config_xml'].errorText;
        $('body').trigger('event_errorOccured', [ 'Error while reading interface configuration.', errorText ]);
      }
      else
      {
        //interfacesContent.dsaConfig.RefreshDisplay();
        interfacesContent.ipAddressConfiguration.RefreshDisplay();
      }
    });

    //interfacesContent.switchConfiguration.Refresh();
    interfacesContent.defaultGatewayConfiguration.Refresh();
    interfacesContent.dnsServerConfiguration.Refresh();
  });
  
};



