/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var FirmwareBackupContent = function(id)
{
  var firmwareBackupContent = this;
  
  firmwareBackupContent.id = id || 'firmware_backup';
  

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var DisplayActivePartitionMediumCallback = function(paramStatus, activePartitionMedium, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      var outputText = DeviceMediumOutputText(activePartitionMedium);
      $('#' + outputElementId).html(outputText);
    }
  };
  
  var CodesysLicenseInfoCallback = function(paramStatus, codesysLiceseInfo, outputElementId)
  {
    if(codesysLiceseInfo.length)
    {
      $('#' + outputElementId).addClass('existing');
    }
  };
  
  
  
  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var FirmwareBackupContentParamView = (function()
  {
    firmwareBackupContent.paramView = new DeviceParamView();
                
    firmwareBackupContent.paramView.Add(
    {
      paramId         : 'active_partition_medium',
      outputElementId : firmwareBackupContent.id + '_content .active_partition_medium',
      outputFkt       : DisplayActivePartitionMediumCallback
    });
            
    firmwareBackupContent.paramView.Add(
    {
      paramId         : 'rts_license_information',
      outputElementId : firmwareBackupContent.id + '_content .only_codesys',
      outputFkt       : CodesysLicenseInfoCallback
    });
        
    firmwareBackupContent.paramView.Add(
    {
      paramId         : 'device_medium_list'
    });

  })();
      

  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(firmwareBackupContent.id + '_create', function(event)
  {
    // show iframe FOR TEST to see the download results on screen
    //$('#' + firmwareBackupContent.id + '_content #firmware_backup_download_iframe').show();

    // event: change in destination selection menu
    //------------------------------------------------------------------------------
    $('#' + firmwareBackupContent.id + '_content select#firmware_backup_destination_medium_select').change(function(event)
    {
      firmwareBackupContent.ShowDestinationSelectionDependencies($(this).val());
    });

    // event: click to checkbox for "all" packages
    //------------------------------------------------------------------------------
    $('#' + firmwareBackupContent.id + '_content input[name=package-all]').click(function(event) 
    {
      // if "all" is selected -> switch all single package checkboxes to "checked"
      if($(this).prop('checked'))
      {
        $('#' + firmwareBackupContent.id + '_content ul#firmware_backup_package_list input[type=checkbox]').prop('checked', 'checked');
      }
    });
    
    // event: click to checkbox for one single package
    //------------------------------------------------------------------------------
    $('#' + firmwareBackupContent.id + '_content input[id^=firmware_backup_package_checkbox]').change(function(event) 
    {
      // checkbox was deselected - checkbox for "all packages" must be deselected, too
      if(!$(this).prop('checked'))
      {
        $('#' + firmwareBackupContent.id + '_content input[name=package-all]').removeAttr('checked');
      }
      
      // if destination actually is network, reset all other packages (only one then can be selected at one time)
      else if('network' === $('#' + firmwareBackupContent.id + '_content select#firmware_backup_destination_medium_select').val()) 
      {
        var encryption_selected = $('#' + firmwareBackupContent.id + '_content input[id=encryption_active_state]').prop('checked');
        $('#' + firmwareBackupContent.id + '_content input[type=checkbox]').removeAttr('checked');
        if( encryption_selected )
        {
            $('#' + firmwareBackupContent.id + '_content input[id=encryption_active_state]').prop('checked', 'checked');
        }
        else
        {
            $('#' + firmwareBackupContent.id + '_content input[name=encryption-passphrase]').val('');
            $('#' + firmwareBackupContent.id + '_content input[name=confirm_encryption-passphrase]').val('');
        }

        $(this).prop('checked','checked');
      }
    });

    // event: click to checkbox encryption_active_state
    //------------------------------------------------------------------------------
    $('#' + firmwareBackupContent.id + '_content input[id=encryption_active_state]').change(function(event)
    {
      var destinationMedium = $('#' + firmwareBackupContent.id + '_content select#firmware_backup_destination_medium_select').val();

      if($(this).prop('checked') && 'network' != destinationMedium)
      {
          // hide and reset auto update feature area because in encryption mode it is not allowed
          $('#' + firmwareBackupContent.id + '_content #firmware_backup_auto_update_area').hide();
          $('#' + firmwareBackupContent.id + '_content #firmware_backup_auto_update_checkbox').removeAttr('checked');
      }
      else if(!$(this).prop('checked') && 'network' != destinationMedium)
      {
          // show auto update feature area
          $('#' + firmwareBackupContent.id + '_content #firmware_backup_auto_update_area').show();
      }
      $('#' + firmwareBackupContent.id + '_content #encryption_passwords').toggle(!!$(this).prop('checked'));
    });

    // event - click to submit button
    //------------------------------------------------------------------------------
    $('#' + firmwareBackupContent.id + '_content #firmware_backup_form').bind('submit', function(event) 
    {
      event.preventDefault();
      firmwareBackupContent.CreateBackup(this);
    });

  });


  //------------------------------------------------------------------------------
  // even: refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(firmwareBackupContent.id + '_refresh', function(event)
  {
    firmwareBackupContent.Refresh();
  });
  
};


FirmwareBackupContent.prototype.Refresh = function()
{
  var firmwareBackupContent = this;
  
  deviceParams.ReadValueGroup(firmwareBackupContent.paramView.list, function()
  {
    pageElements.RemoveBusyScreen();
    
    if(deviceParams.ReadErrorOccured(firmwareBackupContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(firmwareBackupContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading firmware backup values.', errorString ] );
    }
    else
    {
      firmwareBackupContent.paramView.ShowValues();
      firmwareBackupContent.CreateDestinationSelection();
    }
  });

};


FirmwareBackupContent.prototype.CreateDestinationSelection = function()
{
  var firmwareBackupContent = this;
  var activePartitionMedium   = deviceParams.list['active_partition_medium'].value;
  var selectionArea           = $('#' + firmwareBackupContent.id + '_content select[name=destination_medium]');
  var optionCount             = 1;
  
  $(selectionArea).html('');

  // create selection menu by medium list
  $.each(deviceParams.list['device_medium_list'].value, function(mediumId, actualMedium)
  {
    // don't offer actual active device amd internal flash
    if((actualMedium != activePartitionMedium) && !IS_INTERNAL_FLASH(actualMedium))
    {
      var optionHtml = '<option value="' + actualMedium + '" >' + DeviceMediumOutputText(actualMedium) + '</option>';
      $(selectionArea).append(optionHtml);
      ++optionCount;
    }
  });
  
  // offer network at last option and set first option default selected 
  $(selectionArea).append('<option value="network">Network</option>');
  
  // set first entry selected by using first element of options array - just unsing val(1) does not longer work after jQuery upgrade
  var selectionOptions = $(selectionArea).find('option');
  $(selectionOptions[0]).prop('selected', true);
  
  $(selectionArea).trigger('change');

  // exactly one device was found - offer selection menu to user is needless
  // - set data accordingly, but show value only on "read only" area
  if(1 === optionCount)
  {
    var selectedOptionText = $(selectionArea).find('option:selected').text();
    $('#' + firmwareBackupContent.id + '_content #firmware_backup_read_only_destination_medium').html(selectedOptionText);
    $('#' + firmwareBackupContent.id + '_content #firmware_backup_read_only_destination_medium').show();
    $(selectionArea).hide();
  }
  // more than one device was found - show selection menu and hide "read only" area
  else
  {
    $('#' + firmwareBackupContent.id + '_content #firmware_backup_read_only_destination_medium').hide();
    $(selectionArea).show();
  }


  
};
  

FirmwareBackupContent.prototype.ShowDestinationSelectionDependencies = function(destinationMedium)
{
  var firmwareBackupContent = this;
  var packageListArea       = $('#' + firmwareBackupContent.id + '_content ul#firmware_backup_package_list');
  var encryptionArea  		= $('#' + firmwareBackupContent.id + '_content #encryption_area');
  
  // network was selected - package "all" must be unavailable now
  if('network' === destinationMedium)
  {
    // hide and reset auto update feature area
    $('#' + firmwareBackupContent.id + '_content #firmware_backup_auto_update_area').hide();
    $('#' + firmwareBackupContent.id + '_content #firmware_backup_auto_update_checkbox').removeAttr('checked');
    
    // make all single packages selectable, but disable option "all"
    $(packageListArea).find('li input[name=package-all]').attr('disabled', true);
    $(packageListArea).find('li label[for=package-all]').addClass('grayed');

    // if more than one checkbox is selected, reset all
    if(1 < $(packageListArea).find('input:checked').length)
    {
      $(packageListArea).find('li input').removeAttr('checked');
    }
  }

  // not network - show available packages on actual selected source device
  else
  {
    // show auto update feature area
    if($(encryptionArea).find('input[name=encryption_active_state]:checked').length)
        $('#' + firmwareBackupContent.id + '_content #firmware_backup_auto_update_area').hide();
    else
        $('#' + firmwareBackupContent.id + '_content #firmware_backup_auto_update_area').show();

    // enable checkbox "all"
    $(packageListArea).find('input[name=package-all]').removeAttr('disabled');
    $(packageListArea).find('label[for=package-all]').removeClass('grayed');
  }
  
};

FirmwareBackupContent.prototype.PrepareBackup = function(formObj, callback) {
  var deviceMedium    = $(formObj).find('[name=destination_medium]').val();  
  if('network' === deviceMedium)
  {
    Xhr.sessionToken = deviceCommunication.csrfToken;
    transfer = new FileTransfer();
    transfer.prepare(function(error) {
      if (error) {
        $('body').trigger('event_errorOccured', [ 'Error while preparing download.' ] );        
      } else {
        callback(transfer);
      }
    });
  } else {
    callback(null);
  }
}


FirmwareBackupContent.prototype.CreateBackup = function(formObj)
{
  var firmwareBackupContent = this;
  var deviceMedium    = $(formObj).find('[name=destination_medium]').val();  
  var packageCodesys  = $(formObj).find('input[name=package-codesys]:checked').length;
  var packageSettings = $(formObj).find('input[name=package-settings]:checked').length;
  var packageSystem   = $(formObj).find('input[name=package-system]:checked').length;
  var autoUpdateFlag  = $(formObj).find('input[name=auto_update]:checked').length;
  var encState        = $(formObj).find('input[name=encryption_active_state]:checked').length;
  var encPassphrase   = $(formObj).find('input#encryption-passphrase').val();
  var confirmEncPass  = $(formObj).find('input#confirm_encryption-passphrase').val();

  firmwareBackupContent.PrepareBackup(formObj, function(transfer) {

    var downloadDir     = transfer && transfer.transferPath || '/tmp/firmware_backup';
    if(0 === encState && '' != encPassphrase)
    {
       encPassphrase = '';
       $('body').trigger('event_errorOccured', [ 'Encryption not selected but setting passphrase.' ] );
    }
    else if(1 === encState && '' == encPassphrase)
    {
       $('body').trigger('event_errorOccured', [ 'Encryption selected without setting passphrase.' ] );
    }
    else if(!(confirmEncPass === encPassphrase))
    {
       $('body').trigger('event_errorOccured', [ 'The supplied Passphrase and Confirm Passphrase do not match.' ] );
    }
    else if((0 === packageCodesys) && (0 === packageSettings) && ( 0 === packageSystem)) // check input error - no package selected (or not existing either)
    {
       $('body').trigger('event_errorOccured', [ 'No package selected.' ] );
    }
    else
    {
      pageElements.ShowBusyScreen('Create firmware backup package(s)...');

      var newValueList = { destinationDeviceMedium: deviceMedium, downloadDir: downloadDir,
                             autoUpdateFlag: autoUpdateFlag, packageCodesysFlag: packageCodesys,
                             encPassphrase: encPassphrase, packageSettingsFlag: packageSettings,
                             packageSystemFlag: packageSystem };


      deviceParams.ChangeValue('process_firmware_backup', newValueList, function(status, errorText)
      {
        window.clearInterval(showBackupStateInterval);
        pageElements.RemoveBusyScreen();
      
        if(SUCCESS != status)
        {
          $('body').trigger('event_errorOccured', [ 'Error while processing firmware backup.', errorText ]);
        }
        else
        {
          // if medium is network, now we must get the just created backup file as download
          if('network' === deviceMedium)
          {
            // provide download file via iframe, transfair csrf id also to ensure access rights
            var downloadFrame = $('#firmware_backup_download_iframe');
            $(downloadFrame).attr('src', 'offer_download.php?transfer=' + transfer.transferToken + '&csrf_id=' + deviceCommunication.csrfToken);

            // evaluate result when iframe content is ready
            $(downloadFrame).bind('load', function()
            {
              // status and error only are available in case of errors (otherwise we only got the download file)
              var status  = downloadFrame.contents().find('.status').html();
              if(status && (SUCCESS != status))
              {
                var errorText   = downloadFrame.contents().find('.error_text').html();
                $('body').trigger('event_errorOccured', [ 'Error while processing firmware backup.', errorText ]);
              }
              $( this ).unbind();
            });
          }
          
          // else - medium is not network -> we must do nothing, configtool has handeld the transfer of backup file

          // reset inputs
          $(formObj).find('input[id^=firmware_backup_package_checkbox]').removeAttr('checked');
          $(formObj).find('input[name=auto_update]').removeAttr('checked');
          $(formObj).find('input[name=encryption-passphrase]').val('');
          $(formObj).find('input[name=confirm_encryption-passphrase]').val('');
          // encryption is default
          //$(formObj).find('input[id=encryption_active_state]').removeAttr('checked');
        }
      });
      
      // start progress indicator cyclic in background
      var showBackupStateInterval = window.setInterval(firmwareBackupContent.ShowBackupState, 1000);
      //Log("getStatusInterval:" + getStatusInterval);
    }
  });

};



FirmwareBackupContent.prototype.ShowBackupState = function()
{
  //console.log("GetFirmwareBackupState");
  // start progress indicator
  $.ajax({
    url     : 'firmware_backup_status',
    success : function(response)
              {
                // backup status file is existing - put actual text from status file to "busy box"
                //Log("status:" + response);
                if(response.length) pageElements.ChangeBusyScreenText(response);
              },
    error:    function(jqXHR, textStatus)
              {
                // backup status file is not existing - just do nothing
                //Log("no status file");
              }
  });
};

