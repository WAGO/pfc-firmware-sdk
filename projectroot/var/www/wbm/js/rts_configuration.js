/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var RtsConfigurationContent = function(id)
{
  var rtsConfigurationContent = this;
  
  rtsConfigurationContent.id  = id || 'rts_configuration';
  
  rtsConfigurationContent.webserverRestartTimeout = 1000 * 5; // 5 sec
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var SetLocationEnableStatesCallback = function(paramStatus, activePartitionMedium, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      // boot via internal flash - only in this case internal flash is available, so enable according checkbox
      if((INTERNAL_FLASH === activePartitionMedium) || IS_INTERNAL_FLASH(activePartitionMedium))
      {
        $('#' + rtsConfigurationContent.id + '_content input#internal_flash').removeAttr('disabled');
        $('#' + rtsConfigurationContent.id + '_content label[for=internal_flash]').removeClass('greyed');
      }
      
      // boot not via internal flash - disable checkbox
      else
      {
        $('#' + rtsConfigurationContent.id + '_content input#internal_flash').prop('disabled', true);
        $('#' + rtsConfigurationContent.id + '_content label[for=internal_flash]').addClass('greyed');
      }
    }
  };

  var CodesysVersionCallback = function(paramStatus, codesysVersion, outputElementId)
  {
    // remove old value
    $('#' + outputElementId).find('input[name=codesys_version]').removeAttr('checked');
    
    if(SUCCESS === paramStatus)
    {
      // search after fitting checkbox for codesys version
      var listElement = $('#' + outputElementId).find('input[name=codesys_version][value=' + codesysVersion + ']'); 
      
      // if checkbox is not existing, add new line in version list for actual version
      if(0 === listElement.length)
      {
        var html = GetCodesysVersionListElementHtml(codesysVersion);
        $('#' + outputElementId).find('ul.rts_version_list').append(html);
        listElement = $('#' + outputElementId).find('input[name=codesys_version][value=' + codesysVersion + ']'); 
      }

      // set fitting checkbox checked
      $(listElement).prop('checked', 'checked');
    }
  };

  var CodesysVersionListCallback = function(paramStatus, availableRuntimes, outputElementId)
  {
    if((SUCCESS === paramStatus) && availableRuntimes && availableRuntimes.availableVersions)
    {
      $('#' + outputElementId).html('');
      var html = '';
      
      $.each(availableRuntimes.availableVersions, function(versionIndex, version)
      {
        html += GetCodesysVersionListElementHtml(version);
      });
    }
    
    $('#' + outputElementId).html(html);
  };
  
  var GetCodesysVersionListElementHtml = function(version)
  {
    var aVersionText  = { '0': 'None', '2': 'CODESYS 2', '3': 'e!RUNTIME' };
    var outputText    = aVersionText[version] ? aVersionText[version] : version;
    var html          = '';
  
    html += '<li>';
    html +=   '<input id="codesys_version_' + version + '" type="radio" name="codesys_version" value="' + version + '" >';
    html +=   '<label for="codesys_version_' + version +'">' + outputText + '</label>';
    html += '</li>';
    
    return html;
  };
  
  var CodesysHomedirCallback = function(paramStatus, codesysHomedirState, outputElementId)
  {
    $('#' + outputElementId).removeAttr('checked');
    if((SUCCESS === paramStatus) && ('enabled' === codesysHomedirState))
    {
      $('#' + outputElementId).prop('checked', 'checked');
    }
  };
  

  var CreateRtsConfigurationParamView = (function()
  {
    rtsConfigurationContent.paramView = new DeviceParamView();

    rtsConfigurationContent.paramView.Add(
    {
      paramId         : 'rts_available_runtimes',
      outputElementId : rtsConfigurationContent.id + '_content ul.rts_version_list',
      outputFkt       : CodesysVersionListCallback
    });
    
    rtsConfigurationContent.paramView.Add(
    {
      paramId         : 'codesys_version',
      outputElementId : rtsConfigurationContent.id + '_content',
      outputFkt       : CodesysVersionCallback
    });

    rtsConfigurationContent.paramView.Add(
    {
      paramId         : 'rts_home_directory_on_sd_card',
      outputElementId : rtsConfigurationContent.id + '_content #rts_configuration_home_directory_on_sd_card_enabled',
      outputFkt       : CodesysHomedirCallback
    });

  })();
  

  $('#content_area').bind(rtsConfigurationContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + rtsConfigurationContent.id + '_content form#bootproject_location_form').bind('submit', function(event)
    {
      event.preventDefault();
      rtsConfigurationContent.ChangeBootprojectLocation();
    });
    
    $('#' + rtsConfigurationContent.id + '_content form#codesys_version_form').bind('submit', function(event)
    {
      event.preventDefault();
      rtsConfigurationContent.ChangeCodesysVersion();
    });
    
    $('#' + rtsConfigurationContent.id + '_content form#rts_configuration_home_directory_form').bind('submit', function(event)
    {
      event.preventDefault();
      var newState  = ($(event.target).find('[name=home_directory_on_sd_card_enabled]:checked').length) ? 'enabled' : 'disabled';
      rtsConfigurationContent.ChangeHomedirOnSdCardState(newState);
    });
    
  });
    

  $('#content_area').bind(rtsConfigurationContent.id + '_refresh', function(event)
  {
    rtsConfigurationContent.Refresh();
  });
  
};


RtsConfigurationContent.prototype.Refresh = function()
{
  var rtsConfigurationContent = this;

  deviceParams.ReadValueGroup(rtsConfigurationContent.paramView.list, function()
  {
    rtsConfigurationContent.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(rtsConfigurationContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(rtsConfigurationContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading PLC runtime configuration.', errorString ] );
    }
    
    // process read results here, because we need all values together to do our outputs
    else
    {
      rtsConfigurationContent.DisplayBootprojectLocation(deviceParams.list['rts_bootproject_location'].value, 
                                                         deviceParams.list['active_partition_medium'].value);
    }
  });

};


RtsConfigurationContent.prototype.DisplayBootprojectLocation = function(bootprojectLocation, bootLocation)
{
  var rtsConfigurationContent = this;

  // bootproject location is on SD card
  // special case - if controller booted on CF card, bootproject location "home" also means CF card
  if(   (('CARD://' === bootprojectLocation) )
     || (('HOME://' === bootprojectLocation) && (bootLocation === SD_CARD)) )
  {
    //$('select[name=bootproject_location]').val('sd_card');
    $('#' + rtsConfigurationContent.id + '_content input#sd_card').prop('checked', 'checked');
  }
  
  // bootproject location is on internal flash 
  else if(('HOME://' === bootprojectLocation) && IS_INTERNAL_FLASH(bootLocation))
  {
    //$('select[name=bootproject_location]').val('internal_flash');
    $('#' + rtsConfigurationContent.id + '_content input#internal_flash').prop('checked', 'checked');
  }
  else
  {
    // extra output only necessary if selection menu is used
    //$('select[name=bootproject_location]').prepend('<option value="unknown" selected >Unknown</option>');
  }
};


//------------------------------------------------------------------------------
// change bootproject location
// event: click to submit button off hostname form
//------------------------------------------------------------------------------
RtsConfigurationContent.prototype.ChangeBootprojectLocation = function()
{
  var rtsConfigurationContent = this;
  var bootLocation            = deviceParams.list['active_partition_medium'].value;
  
  var newLocation   = '';
  var selectedValue = $('#' + rtsConfigurationContent.id + '_content input[name=bootproject_location]:checked').val();
  //var selectedValue = $('select[name=bootproject_location] option:checked').val())

  // assign selected value to string for config file
  // special case: after boot from SD card, project location SD card requires also value "home" 
  if(   (('internal_flash' === selectedValue) && IS_INTERNAL_FLASH(bootLocation))
     || (('sd_card'        === selectedValue) && (bootLocation === SD_CARD)) )
  {
    newLocation = 'HOME://';
  }
  
  // config file value "SD card" only makes sense if boot location is internal flash 
  else if(('sd_card' === selectedValue) && IS_INTERNAL_FLASH(bootLocation))
  {
    newLocation = 'CARD://';
  }
  else
  {
    $('body').trigger('event_errorOccured', [ 'Invalid bootproject location.' ] );
  }
  
  if(newLocation.length)
  {
    pageElements.ShowBusyScreen("Changing bootproject location...");
  
    var newValueList = { bootprojectLocation: newLocation };
    
    deviceParams.ChangeValue('rts_bootproject_location', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing bootproject location.', errorText ]);
      }
      
      rtsConfigurationContent.Refresh();
    });
  }
  
};


RtsConfigurationContent.prototype.ChangeCodesysVersion = function()
{
  var rtsConfigurationContent = this;
  
  var versionNo     = $('#' + rtsConfigurationContent.id + '_content form#codesys_version_form input[name=codesys_version]:checked').val();

  pageElements.ShowBusyScreen("Changing PLC runtime version...");
  
  var newValueList = { codesysVersion: versionNo };
    
  deviceParams.ChangeValue('codesys_version', newValueList, function(status, errorText)
  {
    if(SUCCESS != status)
    {
      pageElements.RemoveBusyScreen();
      $('body').trigger('event_errorOccured', [ 'Error while changing PLC runtime version.', errorText ]);
      rtsConfigurationContent.Refresh();
    }
    
    // version switch done, now we must explicit restart webserver (it's configuration has changed)
    else
    {
      // initiate reload of page after webserver had time to process restart
      var timeoutObj = window.setTimeout('window.location.reload()', rtsConfigurationContent.webserverRestartTimeout);
  
      // then initiate restart of webserver
      deviceParams.ChangeValue('restart_webserver', { }, function(status, errorText)
      {
        // webserver restart will always return with an error - do nothing!
      });
    }
  });
  
};


RtsConfigurationContent.prototype.ChangeHomedirOnSdCardState = function(newState)
{
  var rtsConfigurationContent = this;
  var newValueList            = { homedirOnSdCardState: newState };

  pageElements.ShowBusyScreen('Change state...');
  
  deviceParams.ChangeValue('rts_home_directory_on_sd_card', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing home directory on memory card state', errorText ]);
      rtsConfigurationContent.Refresh();
    }
  });
};

