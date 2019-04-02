/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var BacnetGeneralContent = function(id)
{
  var bacnetGeneralContent = this;

  bacnetGeneralContent.id    = id || 'bacnet_general_configuration';
  
  /*
   * Display BACnet device id
   */
  var DisplayDeviceId = function(paramStatus, paramValue, outputElementId)
  {    
    if((SUCCESS === paramStatus) && ('-1' === paramValue))
    {
      $('#' + outputElementId).text('currently unconfigured');
    }
    else
    {
      $('#' + outputElementId).text(paramValue);
    }
  };
  
  /*
   * Display BACnet fieldbus state
   */
  var DisplayFieldbusState = function(paramStatus, paramValue, outputElementId)
  {
    $(outputElementId).removeAttr('checked');

    if((SUCCESS === paramStatus) && ('true' === paramValue))
    {
      $('#' + outputElementId).prop('checked', true);
    }
  };
  
  //------------------------------------------------------------------------------
  // reference bacnet content device parameter
  //------------------------------------------------------------------------------
  var CreateBacnetParamView = (function()
  {
    bacnetGeneralContent.paramView = new DeviceParamView();

    bacnetGeneralContent.paramView.Add(
    {
      paramId         : 'bacnet_device_id',
      outputElementId : bacnetGeneralContent.id + '_content #spanDeviceId',
      outputFkt       : DisplayDeviceId
    });
    
    bacnetGeneralContent.paramView.Add(
    {
      paramId         : 'bacnet_fieldbus_state',
      outputElementId : bacnetGeneralContent.id + '_content #bacnet_fieldbus_active_state',
      outputFkt       : DisplayFieldbusState
    });
    
    bacnetGeneralContent.paramView.Add(
    {
      paramId         : 'bacnet_udp_port',
      outputElementId : bacnetGeneralContent.id + '_content #bacnet_port_number'
    });

    bacnetGeneralContent.paramView.Add(
    {
      paramId         : 'bacnet_who_is_online_interval',
      outputElementId : bacnetGeneralContent.id + '_content #bacnet_who_is_online_interval_number'
    });
    
  })();
  
  //------------------------------------------------------------------------------
  // creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(bacnetGeneralContent.id + '_create', function(event)
  {
	// add events for form processing
    $("#bacnet_state_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetGeneralContent.ChangeState();
    });
    
    $("#bacnet_port_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetGeneralContent.ChangePort();
    });
    
    $("#bacnet_who_is_online_interval_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetGeneralContent.ChangeWhoIsOnlineInterval();
    });
  });
  
  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(bacnetGeneralContent.id + '_refresh', function(event)
  {
    bacnetGeneralContent.Refresh();
  });
  
};

/*
 * Refresh values
 */
BacnetGeneralContent.prototype.Refresh = function()
{
  var bacnetGeneralContent = this;
  
  deviceParams.ReadValueGroup(bacnetGeneralContent.paramView.list, function()
  {
        
    if(deviceParams.ReadErrorOccured(bacnetGeneralContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(bacnetGeneralContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading BACnet services configuration.', errorString ] );
    }
    else
    {
      bacnetGeneralContent.paramView.ShowValues();
    }
  });
};

/*
 * Change state (enable/disable BACnet fieldbus)
 */
BacnetGeneralContent.prototype.ChangeState = function()
{
  var bacnetGeneralContent = this;
  
  pageElements.ShowBusyScreen("Changing BACnet state ...");
  
  var newValueList = { fieldbusState: 'false' };
  
  if(('true' === $("input[name=bacnet_fieldbus_active_state]").val()) && $("input[name=bacnet_fieldbus_active_state]").prop("checked") )
  {
    newValueList = { fieldbusState: 'true' };
  }
  
  deviceParams.ChangeValue('bacnet_fieldbus_state', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing BACnet state', errorText ]);
    }
    
    bacnetGeneralContent.Refresh();
  });
};

/*
 * Change port number 
 */
BacnetGeneralContent.prototype.ChangePort = function()
{
  var bacnetGeneralContent = this;
  
  pageElements.ShowBusyScreen("Changing BACnet port number...");
  
  var newValueList = { udpPort: $('[name=port_number]').val() };
  
  deviceParams.ChangeValue('bacnet_udp_port', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing port number.', errorText ]);
    }
    
    bacnetGeneralContent.Refresh();
  });
};

/*
 * Change Who-Is online interval time
 */
BacnetGeneralContent.prototype.ChangeWhoIsOnlineInterval = function()
{
  var bacnetGeneralContent = this;
  
  pageElements.ShowBusyScreen("Changing BACnet Who-Is online interval time...");
  
  var newValueList = { whoIsOnlineInterval: $('[name=who_is_online_interval_time]').val() };
  
  deviceParams.ChangeValue('bacnet_who_is_online_interval', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Who-Is online interval time.', errorText ]);
    }
    
    bacnetGeneralContent.Refresh();
  });
};

