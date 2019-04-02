/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

/*---------------------------------------------------------------
 * OBJECT DeviceDetails ("ControllerDetails")
 *---------------------------------------------------------------
 */
function DeviceDetails()
{
  var deviceDetails = this;
  
  
  var CreateDeviceDetailsParamView = (function()
  {
    deviceDetails.paramView = new DeviceParamView();
    
    deviceDetails.paramView.Add(
    {
      paramId         : 'product_description',
      outputElementId : 'product_description'
    });
    
    deviceDetails.paramView.Add(
    {
      paramId         : 'order_number',
      outputElementId : 'order_number'
    });

    deviceDetails.paramView.Add(
    {
      paramId         : 'rts_license_information',
      outputElementId : 'license_information'
    });

    deviceDetails.paramView.Add(
    {
      paramId         : 'firmware_revision',
      outputElementId : 'firmware_revision'
    });
    
  })();

};


DeviceDetails.prototype.Refresh = function()
{
  var deviceDetails = this;

  deviceParams.ReadValueGroup(deviceDetails.paramView.list, function()
  {
    deviceDetails.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(deviceDetails.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(deviceDetails.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading controller details info.', errorString ] );
    };
  });
};
  

DeviceDetails.prototype.Create = function()
{
  // no creation necessary, all content included in html
};




/*---------------------------------------------------------------
 * OBJECT ActualNetworkDetails
 *---------------------------------------------------------------
 */
function ActualNetworkDetails(interfaceId)
{
  var actualNetworkDetails = this;
  
  actualNetworkDetails.paramView          = {};
};


ActualNetworkDetails.prototype.CreatePortParamView = function(portName)
{
  if(!portName) return ERROR;
  
  var actualNetworkDetails  = this;
  var paramView             = new DeviceParamView();
  
  actualNetworkDetails.paramView[portName] = paramView;
        
  paramView.Add(
  {
    paramId           : 'actual_ip_address_' + portName,
    outputElementId   : 'actual_ip_address_' + portName
  });

  paramView.Add(
  {
    paramId           : 'actual_subnet_mask_address_' + portName,
    outputElementId   : 'actual_network_details_subnet_mask_' + portName
  });

  paramView.Add(
  {
    paramId           : 'actual_mac_address_' + portName,
    outputElementId   : 'actual_network_details_mac_address_' + portName
  });

  paramView.Add(
  {
    paramId           : 'actual_config_type_' + portName,
    outputElementId   : 'actual_network_details_ip_config_type_' + portName
  });

};


ActualNetworkDetails.prototype.DisplayInterfaceState = function(paramStatus, paramValue, outputElementId)
{
  if((SUCCESS === paramStatus) && ('enabled' === paramValue))
  {
    $('#' + outputElementId).html('<img class="state" src="images/haken_gruen_10.gif" alt="on" title="enabled">enabled');
  }

  else if((SUCCESS === paramStatus) && ("disabled" === paramValue))
  {
    $('#' + outputElementId).html('<img class="state" src="images/kreuz_rot_10.gif" alt="off" title="enabled">enabled');
  }
  
  else
  {
    $('#' + outputElementId).html('unknown');
  }
};


/*
 * show specified text, if ip address should be configured via BootP or DHCP, but by now, no address was given
 */
ActualNetworkDetails.prototype.ShowWaitingText = function(interfaceId, ipAddress, ipConfigType)
{
  if(0 === ipAddress.length)
  {
    if('bootp' === ipConfigType)
    {
      $("#actual_network_details_waiting_text_" + interfaceId).html(', waiting for IP address by BootP');
    }
    else if('dhcp' === ipConfigType)
    {
      $("#actual_network_details_waiting_text_" + interfaceId).html(', waiting for IP address by DHCP');
    }
  }
};


/*
 * Create html code of whole block for one interface 
 */
ActualNetworkDetails.prototype.InterfaceHtml = function(interfaceId)
{
  var html = '';
  
  html += '<h3>Network Details <span id="actual_network_details_device_id_' + interfaceId + '"></span></h3>';
  
  html += '<div class="config_form_box" >';

  html +=   '<div class="config_data_list">';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">State:</div>';
  html +=       '<div class="value_field"><span id="actual_network_details_interface_state_' + interfaceId + '"></span>';
  html +=       '<span id="actual_network_details_waiting_text_' + interfaceId + '"></span></div>';
  html +=     '</div>';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">MAC Address:</div>';
  html +=       '<div class="value_field"><span id="actual_network_details_mac_address_' + interfaceId + '"></span></div>';
  html +=     '</div>';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">IP Address:</div>';
  html +=       '<div class="value_field">';
  html +=         '<span id="actual_ip_address_' + interfaceId + '"></span>';
  html +=         ' (<span id="actual_network_details_ip_config_type_' + interfaceId + '"></span>)';
  html +=       '</div>';
  html +=     '</div>';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Subnet Mask:</div>';
  html +=       '<div class="value_field"><span id="actual_network_details_subnet_mask_' + interfaceId + '"></span></div>';
  html +=     '</div>';
  html +=   '</div>';
  html += '</div>';
  
  return html;
}



//------------------------------------------------------------------------------
// first creation of area
//------------------------------------------------------------------------------
ActualNetworkDetails.prototype.Create = function()
{
  var actualNetworkDetails = this;
  
  actualNetworkDetails.CreatePortParamView('X1');
  actualNetworkDetails.CreatePortParamView('X2');
};
  
  

//------------------------------------------------------------------------------
// refresh of area
//------------------------------------------------------------------------------
ActualNetworkDetails.prototype.Refresh = function()
{
  var actualNetworkDetails  = this;

  deviceParams.ReadValue({ id: 'interface_config_xml' }, function()
  {
    actualNetworkDetails.RefreshDisplay();
  });
  
};


/**
 * refresh interface area
 */
ActualNetworkDetails.prototype.RefreshDisplay = function()
{
  var actualNetworkDetails  = this;
  var iFaceCount            = 0;
  var interfaceConfig       = deviceParams.list['interface_config_xml'].value;

  // remove old content
  $("#actual_tcpip_interfaces").html('');
  
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
        var portName = iFace.ipSettings.portName;
   
        // create and show html frame and add single values explicit
        var html  = actualNetworkDetails.InterfaceHtml(portName); 
        $('#actual_tcpip_interfaces').append(html);

        // show port name
        actualNetworkDetails.DisplayInterfaceState(SUCCESS, iFace.state, 'actual_network_details_interface_state_' + portName);
        $('#actual_tcpip_interfaces #actual_network_details_device_id_' + portName).html(portName);
   
        // get acutal values separate (via configtool), use no single function, because we must call "ShowWaitingText" in this context
        deviceParams.ReadValueGroup(actualNetworkDetails.paramView[portName].list, function()
        {
          actualNetworkDetails.paramView[portName].ShowValues();

          if(deviceParams.ReadErrorOccured(actualNetworkDetails.paramView[portName].list)) 
          {
            var errorString = deviceParams.CollectedErrorStrings(actualNetworkDetails.paramView[portName].list);
            $('body').trigger('event_errorOccured', [ 'Error while reading network details for interface "' + portName + '".', errorString ] );
          };
        
          actualNetworkDetails.ShowWaitingText(portName, deviceParams.list['actual_ip_address_' + portName].value, 
                                                         deviceParams.list['actual_config_type_' + portName].value);
        });
 
        ++iFaceCount;
      }
    });
  
    // if only one interface is existing (ports are switched) change port name to 'X1/X2'
    if(1 == iFaceCount)
    {
      $('#actual_tcpip_interfaces span[id^=actual_network_details_device_id]').html('X1/X2');
    }
  }
};


  

/*
 * get all actual values from coupler - old (compatible) version for only one interface (eth0)
 */
ActualNetworkDetails.prototype.RefreshAllValuesViaConfigtool = function()
{
  var actualNetworkDetails  = this;
  var iFaceId               = 'eth0';
        
  deviceParams.ReadValueGroup(actualNetworkDetails.paramView[iFaceId].list, function()
  {
    actualNetworkDetails.paramView[iFaceId].ShowValues();

   if(deviceParams.ReadErrorOccured(actualNetworkDetails.paramView[iFaceId].list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(actualNetworkDetails.paramView[iFaceId].list);
      $('body').trigger('event_errorOccured', [ 'Error while reading network details for interface "' + iFaceId + '".', errorString ] );
    };
    
    var ipAddress     = deviceParams.list['actual_ip_address_' + iFaceId].value;
    var ipConfigType  = deviceParams.list['ip_config_type_' + iFaceId].value;
    actualNetworkDetails.ShowWaitingText(iFaceId, ipAddress, ipConfigType);
  });
  
};
  


/*---------------------------------------------------------------
 * OBJECT information content area 
 *---------------------------------------------------------------
 */
function InformationContent(id)
{
  var informationContent    = this;
  
  informationContent.id = id || 'information';
  
  informationContent.actualNetworkDetails = new ActualNetworkDetails;
  informationContent.deviceDetails        = new DeviceDetails;
  
  $('#content_area').bind(informationContent.id + '_create', function(event)
  {
    //Log("Event - information create")
    informationContent.deviceDetails.Create();
    informationContent.actualNetworkDetails.Create();
  });
    

  $('#content_area').bind(informationContent.id + '_refresh', function(event)
  {
    //Log("Event - information refresh")
    informationContent.deviceDetails.Refresh();
    informationContent.actualNetworkDetails.Refresh();
  });

}

