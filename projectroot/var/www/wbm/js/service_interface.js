/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var ServiceInterfaceContent = function(id)
{
  var serviceInterfaceContent = this;
  
  serviceInterfaceContent.id = id || 'service_interface';
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  
  // get fitting text for "no owner for service interface" depending on CODESYS license
  var ProcessRtsLicense = function(paramStatus, rtsLicenseInfo, outputElementId)
  {
    var outputText = 'Unassigned (usage by Applications, Libraries)';
    
    if((SUCCESS === paramStatus) && rtsLicenseInfo.length) 
    {
      outputText = 'Unassigned (usage by Applications, Libraries, PLC Runtime)';
    }

    $('#' + outputElementId + ' label[for="service_interface_owner_free"]').html(outputText);
  };
  
  //------------------------------------------------------------------------------
  // initializations
  //------------------------------------------------------------------------------

  var CreateContentParamView = (function()
  {
    serviceInterfaceContent.paramView = new DeviceParamView();
                
    serviceInterfaceContent.paramView.Add(
    {
      paramId         : 'rts_license_information',
      outputFkt       : ProcessRtsLicense,
      outputElementId : serviceInterfaceContent.id + '_content #config_service_interface_form'
    });

    serviceInterfaceContent.paramView.Add(
    {
      paramId         : 'service_interface_config'
    });

  })();
      
  
  $('#content_area').bind(serviceInterfaceContent.id + '_create', function(event)
  {
    // add event processing
    $('#' + serviceInterfaceContent.id + '_content #config_service_interface_form').bind('submit', function(event) 
    {
      event.preventDefault();
      
      var newMode = $(event.target).find('[name=service_interface_mode]:checked').val();
      serviceInterfaceContent.ChangeMode(newMode);
    });
    
  });


  $('#content_area').bind(serviceInterfaceContent.id + '_refresh', function(event)
  {
    serviceInterfaceContent.Refresh();
  });

};


ServiceInterfaceContent.prototype.Refresh = function()
{
  var serviceInterfaceContent = this;

  deviceParams.ReadValueGroup(serviceInterfaceContent.paramView.list, function()
  {
    serviceInterfaceContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(serviceInterfaceContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(serviceInterfaceContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading service interface data.', errorString ] );
    }
    else
    {
      var serviceInterfaceConfig = deviceParams.list['service_interface_config'].value;
      
      // show text for current owner (depending on codesys license, already considered for interface selection)
      var activeMode = serviceInterfaceConfig.ActiveMode;
      var outputText = activeMode; //'Unknown';
        
      if('service' === activeMode)
      {
        outputText = 'Wago Service Communication';
      }
      else if('linux' === activeMode)
      {
        outputText = 'Linux Console';
      }
      // no owner for service interface - get output text from checkbox selection
      else if('free' === activeMode)
      {
        outputText = $('#config_service_interface_form label[for="service_interface_owner_free"]').text();
      }
        
      $('#' + serviceInterfaceContent.id + '_content #service_interface_current_owner').html(outputText);

      // show configured owner by checkbox
      var configuredMode = serviceInterfaceConfig.ConfiguredMode;
      $('#' + serviceInterfaceContent.id + '_content input[value=' + configuredMode + ']').prop('checked', 'checked');
    }
  });
  
};


ServiceInterfaceContent.prototype.ChangeMode= function(newMode)
{
  var serviceInterfaceContent = this;
  
  // as a precaution recheck
  //if(confirm('Really want to reboot controller?'))

  pageElements.ShowBusyScreen("Changing prospective owner of service interface...");
  
  var newValueList = { mode: newMode };

  deviceParams.ChangeValue('service_interface_configured_mode', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing user of service interface.', errorText ]);
    }
    
    serviceInterfaceContent.Refresh();
  });
 
};

