/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var ModbusServicesContent = function(id)
{
  var modbusServicesContent = this;
  
  modbusServicesContent.id = id || 'modbus_services';
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var CodesysVersionCallback = function(paramStatus, codesysVersion, outputElementId)
  {
    if((SUCCESS === paramStatus) && ('3' === codesysVersion))
    {
      $('#' + outputElementId + ' .modbus_no_codesys_3').hide();
      $('#' + outputElementId + ' .modbus_codesys_3').show();
    }
    else
    {
      $('#' + outputElementId + ' .modbus_codesys_3').hide();
      $('#' + outputElementId + ' .modbus_no_codesys_3').show();
    }
  };

  
  
  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var ModbusServicesParamView = (function()
  {
    modbusServicesContent.paramView = new DeviceParamView();
                
    modbusServicesContent.paramView.Add(
    {
      paramId         : 'codesys_version',
      outputElementId : modbusServicesContent.id + '_content',
      outputFkt       : CodesysVersionCallback
    });

    modbusServicesContent.paramView.Add(
    {
      paramId         : 'modbus_tcp_service_state',
      outputElementId : modbusServicesContent.id + '_content #modbus_tcp_service_active_state'
    });
            
    modbusServicesContent.paramView.Add(
    {
      paramId         : 'modbus_udp_service_state',
      outputElementId : modbusServicesContent.id + '_content #modbus_udp_service_active_state'
    });

  })();
      

  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(modbusServicesContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + modbusServicesContent.id + '_content #modbus_tcp_service_config_form').bind('submit', function(event) 
    {
      event.preventDefault();
      var bEnabled  = $(event.target).find('[name=service_active_state]:checked').length ? 'true' : 'false';
      modbusServicesContent.ChangeState('tcp', bEnabled);
    });

    $('#' + modbusServicesContent.id + '_content #modbus_udp_service_config_form').bind('submit', function(event) 
    {
      event.preventDefault();
      var bEnabled  = $(event.target).find('[name=service_active_state]:checked').length ? 'true' : 'false';
      modbusServicesContent.ChangeState('udp', bEnabled);
    });

  });


  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(modbusServicesContent.id + '_refresh', function(event)
  {
    modbusServicesContent.Refresh();
  });
  
};


ModbusServicesContent.prototype.Refresh = function()
{
  var modbusServicesContent = this;
  
  deviceParams.ReadValueGroup(modbusServicesContent.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(modbusServicesContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(modbusServicesContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading Modbus® services configuration.', errorString ] );
    }
    else
    {
      modbusServicesContent.paramView.ShowValues();
    }
  });

};


ModbusServicesContent.prototype.ChangeState = function(modbusType, bEnabled)
{
  var modbusServicesContent = this;

  // check input parameter
  if(('true' != bEnabled) && ('false' != bEnabled))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for service state.', bEnabled ] );
  }
  
  else if(('udp' != modbusType) && ('tcp' != modbusType))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for Modbus® type.', modbusType ] );
  }
  
  
  else
  {
    pageElements.ShowBusyScreen('Changing Modbus® service configuration...');

    var newValueList  = { modbusType: modbusType, bEnabled: bEnabled };

    deviceParams.ChangeValue('modbus_service_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
  
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing Modbus® service configuration.', errorText ]);
      }
  
      // show actual configuration
      modbusServicesContent.Refresh();
    });
  }
};

