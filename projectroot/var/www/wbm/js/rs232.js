/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var RS232Content = function(id)
{
  var rs232Content = this;
  
  rs232Content.id = id || 'rs232';
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  
  // get fitting text for "no owner for RS232" depending on CODESYS license
  var ProcessRtsLicense = function(paramStatus, rtsLicenseInfo, outputElementId)
  {
    var outputText = 'Unassigned (usage by Applications, Libraries)';
    
    if((SUCCESS === paramStatus) && rtsLicenseInfo.length) 
    {
      outputText = 'Unassigned (usage by Applications, Libraries, PLC Runtime)';
    }

    $('#' + outputElementId + ' label[for="none"]').html(outputText);
  };
  
  
  // show text for current owner (depending on codesys license, already considered for interface selection)
  var ShowCurrentOwner = function(paramStatus, currentOwner, outputElementId)
  {
    var outputText = 'Unknown';
    
    if('Linux' === currentOwner)
    {
      outputText = 'Linux Console';
    }
    
    // no owner for RS232 - get output text from checkbox selection
    else if('None' === currentOwner)
    {
      outputText = $('#config_rs232_form label[for="none"]').text();
    }
    
    $('#' + outputElementId).html(outputText);
  };
  
  // show configured owner by checkbox
  var ShowConfiguredOwner = function(paramStatus, configuredOwner, outputElementId)
  {
    $('#' + outputElementId + ' input[value=' + configuredOwner + ']').prop('checked', 'checked');
  };
  
  
  //------------------------------------------------------------------------------
  // initializations
  //------------------------------------------------------------------------------

  var CreateRS232ContentParamView = (function()
  {
    rs232Content.paramView = new DeviceParamView();
                
    rs232Content.paramView.Add(
    {
      paramId         : 'rts_license_information',
      outputFkt       : ProcessRtsLicense,
      outputElementId : rs232Content.id + '_content #config_rs232_form'
    });

    rs232Content.paramView.Add(
    {
      paramId         : 'current_rs232_owner',
      defaultText     : 'Unknown',
      outputElementId : rs232Content.id + '_content #rs232_current_owner',
      outputFkt       : ShowCurrentOwner
    });
        
    rs232Content.paramView.Add(
    {
      paramId         : 'configured_rs232_owner',
      outputElementId : rs232Content.id + '_content form#config_rs232_form',
      outputFkt       : ShowConfiguredOwner
    });

  })();
      
  
  $('#content_area').bind(rs232Content.id + '_create', function(event)
  {
    // add event processing
    $('#' + rs232Content.id + '_content #config_rs232_form').bind('submit', function(event) 
    {
      event.preventDefault();
      
      var newOwner = $('#' + rs232Content.id + '_content #config_rs232_form [name=rs232_owner]:checked').val();
      rs232Content.ChangeRS232Owner(newOwner);
    });
    
  });


  $('#content_area').bind(rs232Content.id + '_refresh', function(event)
  {
    rs232Content.Refresh();
  });

};


RS232Content.prototype.Refresh = function()
{
  var rs232Content = this;

  deviceParams.ReadValueGroup(rs232Content.paramView.list, function()
  {
    rs232Content.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(rs232Content.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(rs232Content.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading RS232 data.', errorString ] );
    }
  });
  
};


RS232Content.prototype.ChangeRS232Owner= function(newOwner)
{
  var rs232Content = this;
  
  pageElements.ShowBusyScreen("Changing prospective owner of serial interface...");
  
  var newValueList = { owner: newOwner };

  deviceParams.ChangeValue('configured_rs232_owner', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing user of serial interface.', errorText ]);
    }
    
    rs232Content.Refresh();
  });
 
};

