/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var FirmwareRestoreContent = function(id)
{
  var firmwareRestoreContent = this;
  
  firmwareRestoreContent.id = id || 'firmware_restore';
  

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var DisplayActivePartitionMediumCallback = function(paramStatus, activePartitionMedium, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      var outputText = DeviceMediumOutputText(activePartitionMedium);
      $('#' + outputElementId).html(outputText);
      
      
      $('#firmware_restore_submit_button').removeClass('greyed');
      if('sd-card' === activePartitionMedium)
      {
        $('#firmware_restore_submit_button').addClass('greyed');
      }
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
  var FirmwareRestoreContentParamView = (function()
  {
    firmwareRestoreContent.paramView = new DeviceParamView();
                
    firmwareRestoreContent.paramView.Add(
    {
      paramId         : 'active_partition_medium',
      outputElementId : firmwareRestoreContent.id + '_content .active_partition_medium',
      outputFkt       : DisplayActivePartitionMediumCallback
    });
            
    firmwareRestoreContent.paramView.Add(
    {
      paramId         : 'firmware_restore_packages'
    });
        
    firmwareRestoreContent.paramView.Add(
    {
      paramId         : 'rts_license_information',
      outputElementId : firmwareRestoreContent.id + '_content .only_codesys',
      outputFkt       : CodesysLicenseInfoCallback
    });
        
  })();
      

  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(firmwareRestoreContent.id + '_create', function(event)
  {
    var firmwareRestoreForm     = $('#' + firmwareRestoreContent.id + '_content #firmware_restore_form');

    // write upload directory on device in form member
    $(firmwareRestoreForm).append('<input type="hidden" name="upload_directory" value="' + UPLOAD_DIR + '">');

    // show iframe FOR TEST to see the upload results on screen
    //$('#' + firmwareRestoreContent.id + '_content #firmware_restore_upload_iframe').show();

    // add events to handle file selection in our own CD
    // Does not work with IE! After sending click event to browse button, file selection apparently is ok - but form can't be submitted
    // Take solution with transparent "browse" button over real button to click it directly... 
    //pageElements.InitFakeFileInput('#' + firmwareRestoreContent.id + '_content', 'codesys_backup_upload');
    //pageElements.InitFakeFileInput('#' + firmwareRestoreContent.id + '_content', 'settings_backup_upload');
    //pageElements.InitFakeFileInput('#' + firmwareRestoreContent.id + '_content', 'system_backup_upload');

    
    // event: file was selected
    //------------------------------------------------------------------------------
    $('#' + firmwareRestoreContent.id + '_content [id$=_backup_upload_file_real_input]').bind('change', function(event)
    {
      var backupType = $(event.target).attr('id').replace('_backup_upload_file_real_input', '');

      // get filename of files array, because reading fileupload value is not the correct way and adds funny strings to filename ("fakepath etc.")
      // respect, that filearray is not always existing
      var filename = "";
      if($(event.target)[0].files[0] && $(event.target)[0].files[0].name) {
        filename = $(event.target)[0].files[0].name;
      }
      
      $('#' + firmwareRestoreContent.id + '_content #firmware_restore_form').find('#' + backupType + '_backup_upload_file_fake_input').val(filename);
    });

    // event: change in source selection menu
    //------------------------------------------------------------------------------
    $('#' + firmwareRestoreContent.id + '_content select#firmware_restore_source_medium_select').change(function(event)
    {
      firmwareRestoreContent.ShowSourceSelectionDependencies($(this).val());
    });
    
    
    // event - backup file was selected from harddisc - check naming conventions
    //------------------------------------------------------------------------------
    $('#' + firmwareRestoreContent.id + '_content input[name$=source_file]').bind('change', function(event)
    {
      firmwareRestoreContent.CheckUploadFileSelection(this);
    });
    

    // event: click to checkbox for "all" packages
    //------------------------------------------------------------------------------
    $('#' + firmwareRestoreContent.id + '_content input[name=package-all]').click(function(event) 
    {
      // if "all" is selected -> switch all single package checkboxes to "checked"
      if($(this).prop('checked'))
      {
        $('#' + firmwareRestoreContent.id + '_content ul#firmware_restore_package_list input[type=checkbox]').prop('checked', 'checked');
      }
    });
    
    
    //event - change of checkbox for a single package
    //------------------------------------------------------------------------------
    $('#' + firmwareRestoreContent.id + '_content input[id^=firmware_restore_package_checkbox]').change(function(event) 
    {
      // checkbox was deselected
      if(!$(this).prop('checked'))
      {
        // checkbox for "all packages" must be deselected, too
        $('#' + firmwareRestoreContent.id + '_content input[name=package-all]').removeAttr('checked');

        // reset possible file selection for this package - does not work with IE!!!
        //var uploadPackage = $(this).attr('id').replace('firmware_restore_package_checkbox_', '');
        //$('#' + firmwareRestoreContent.id + '_content #' + uploadPackage + '_backup_upload_file_real_input').val('');
        //$('#' + firmwareRestoreContent.id + '_content #' + uploadPackage + '_backup_upload_file_fake_input').val('');
      }
    });
    
    
    // event - click to submit button
    //------------------------------------------------------------------------------
    $('#' + firmwareRestoreContent.id + '_content #firmware_restore_form').bind('submit', function(event) 
    {
      var decryptionArea = $('#' + firmwareRestoreContent.id + '_content #decryption_area');
      var errorTxt  = firmwareRestoreContent.CheckFrontendDecryptionParas(decryptionArea);

      if('sd-card' === deviceParams.list['active_partition_medium'].value)
      {
         event.preventDefault();
    	 $('body').trigger('event_errorOccured', [ 'Firmware restore is not allowed, if "Memory Card" is active partition.' ] );
      }
      else if ('' == errorTxt) // for usability reasons check right decryption arguments of frontend
      {
        // source is our own harddisc ("network") -> upload backup files via iframe, after completion, activate via ajax
        if('network' === $(this).find('[name=source_medium]').val())
        {
          // don't prevent form default!!! File upload is handled by sending form to php file in iframe (not via ajax)
          if(SUCCESS != firmwareRestoreContent.UploadBackupFiles(this))
          {
            // only in case of input error
            event.preventDefault();
          }
        }
      
        // source is another device -> start copying and activation of backup directly via ajax
        else
        {
          event.preventDefault();
          firmwareRestoreContent.ActivateBackup(this);
        }
      }
      else
      {
    	event.preventDefault();
    	$('body').trigger('event_errorOccured', [ errorTxt ] );
      }
    });
    
    // event - click to submit button
    //------------------------------------------------------------------------------
    $('#' + firmwareRestoreContent.id + '_content #firmware_restore_submit_button').bind('click', function(event) 
    {
      if('sd-card' === deviceParams.list['active_partition_medium'].value)
      {
        $('body').trigger('event_errorOccured', [ 'Firmware restore is not allowed, if active partition is "Memory Card".' ] );
      }
    });



    // catch uploads by form to prevent POST 
    // formerly uploads were done via multipart/form-data. this was changed 
    // to a chunked upload using the File object solely, which makes the form data 
    // obsolete to be sent to the backend.
    $('#firmware_restore_form').bind('submit', function(event)
    {
      event.preventDefault();
    });
    

  });


  //------------------------------------------------------------------------------
  // even: refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(firmwareRestoreContent.id + '_refresh', function(event)
  {
    firmwareRestoreContent.Refresh();
  });
  
};


FirmwareRestoreContent.prototype.Refresh = function()
{
  var firmwareRestoreContent = this;
  
  pageElements.ShowBusyScreen('Get available updates.');
  
  deviceParams.ReadValueGroup(firmwareRestoreContent.paramView.list, function()
  {
    pageElements.RemoveBusyScreen();
    
    if(deviceParams.ReadErrorOccured(firmwareRestoreContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(firmwareRestoreContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading firmware restore values.', errorString ] );
    }
    else
    {
      firmwareRestoreContent.paramView.ShowValues();
      firmwareRestoreContent.CreateSourceMediumSelection();
    }
  });

};


FirmwareRestoreContent.prototype.CreateSourceMediumSelection = function()
{
  var firmwareRestoreContent  = this;
  var activePartitionMedium   = deviceParams.list['active_partition_medium'].value;
  var selectionArea           = $('#' + firmwareRestoreContent.id + '_content select[name=source_medium]');
  
  $(selectionArea).html('');

  /*
  // create selection menu by extra medium list
  $.each(deviceParams.list['device_medium_list'].value, function(mediumId, actualMedium)
  {
    // don't offer the actual active device
    // and don't offer internal flash, if sd card is destination
    if(    (actualMedium != activePartitionMedium) 
       && !((SD_CARD === activePartitionMedium) && IS_INTERNAL_FLASH(actualMedium))
    {
      var optionHtml = '<option value="' + actualMedium + '" >' + DeviceMediumOutputText(actualMedium) + '</option>';
      $(selectionArea).append(optionHtml);
    }
  });
  */
  
  var optionCount = 1;
  
  // loop over all devices and create selection menu out of it
  //if(SUCCESS === deviceParams.list['firmware_restore_packages'].status)
  $.each(deviceParams.list['firmware_restore_packages'].value, function(medium, mediumData)
  {
    var optionHtml = '<option value="' + medium + '" >' + DeviceMediumOutputText(medium) + '</option>';
    $(selectionArea).append(optionHtml);
    ++optionCount;
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
    $('#' + firmwareRestoreContent.id + '_content #firmware_restore_read_only_source_medium').html(selectedOptionText);
    $(selectionArea).hide();
  }
  // more than one device was found - show selection menu and hide "read only" area
  else
  {
    $('#' + firmwareRestoreContent.id + '_content #firmware_restore_read_only_source_medium').hide();
    $(selectionArea).show();
  }

};
  

FirmwareRestoreContent.prototype.ShowSourceSelectionDependencies = function(sourceMedium)
{
  var firmwareRestoreContent  = this;
  var packageListArea         = $('#' + firmwareRestoreContent.id + '_content ul#firmware_restore_package_list');
  var sourceFileArea          = $('#' + firmwareRestoreContent.id + '_content #firmware_restore_network_source_file');
  var decryptionArea          = $('#' + firmwareRestoreContent.id + '_content #decryption_area');

  // remove all actual "checked" hooks
  $(packageListArea).find('li input').removeAttr('checked');
  firmwareRestoreContent.InitDecryptionArea(decryptionArea);

  // network was selected - package "all" must be unavailable now
  if('network' === sourceMedium)
  { 
    // make all single packages selectable, but disable option "all"
    $(packageListArea).find('li input').removeAttr('disabled');
    $(packageListArea).find('li label').removeClass('grayed');
    $(packageListArea).find('li input[name=package-all]').attr('disabled', true);
    $(packageListArea).find('li label[for=package-all]').addClass('grayed');

    // make upload source file fields visible
    $(sourceFileArea).show();
    $(decryptionArea).show();
    
  }

  // not network - show available packages on actual selected source device
  else
  {
    var availablePackages = deviceParams.list['firmware_restore_packages'].value[sourceMedium]; 

    // clear and hide input field for file 
    $(sourceFileArea).hide();
    $('#' + firmwareRestoreContent.id + '_content [id$=backup_upload_file_real_input]').val('');
    $('#' + firmwareRestoreContent.id + '_content [id$=backup_upload_file_fake_input]').val('');
    
    $(packageListArea).find('input').attr('disabled', true);
    $(packageListArea).find('label').addClass('grayed');
    $(decryptionArea).hide();

    // loop over all packages and enable according checkbox if package is available 
    $.each(availablePackages, function(packageName, packageAvailable)
    {
      if(packageAvailable)
      {
	// This is sent by "get_firmware_restore_packages"
        if( 'encryption_active_state' == packageName)
        {
            $(decryptionArea).show();
            $(decryptionArea).find('input[id^=decryption_active_state]').prop('checked', 'checked');
            $(decryptionArea).find('input[id^=decryption_active_state]').attr('disabled', true);
        }
        else
        {

            var checkboxId  = 'firmware_restore_package_checkbox_' + packageName.replace('package', '').toLowerCase();
            $(packageListArea).find('input[id=' + checkboxId + ']').removeAttr('disabled');
            $(packageListArea).find('label[for=' + checkboxId + ']').removeClass('grayed');
    	}
      }
    });
    
    // all single packages are available (only "package all" is still disabled) - enable "package all" also
    if(1 === $(packageListArea).find('input:disabled').length)
    { 
      $(packageListArea).find('input[name=package-all]').removeAttr('disabled');
      $(packageListArea).find('label[for=package-all]').removeClass('grayed');
    }
  }

};




/*
 * check if selected file fitts to naming conventions and show warning if not
 */
FirmwareRestoreContent.prototype.CheckUploadFileSelection = function(inputObj)
{
  var firmwareRestoreContent  = this;
  var decryptionArea = $('#' + firmwareRestoreContent.id + '_content #decryption_area');

  // filter package name from input field name
  var uploadPackage = $(inputObj).attr('name').replace('_source_file','');
  var filename      = $(inputObj).val();
  var msg = [];
  var fname_match = DOWNLOAD_FILENAME_FRAGMENT['prefix'] + '.*' + DOWNLOAD_FILENAME_FRAGMENT[uploadPackage];

  $('#firmware_restore_package_list input[type=checkbox][name$=' + uploadPackage + ']').prop('checked', 'checked');

  if (!RegExp(fname_match).test(filename))
  {
    msg.push('The "' + uploadPackage + '" restore file name should match the pattern: "' + fname_match + '".');
  }
  var enc_checked = $('input[name=decryption_active_state]').prop('checked');
  var enc_file = /\.enc$/.test(filename);

  console.log("enc_checked", enc_checked, "enc_file", enc_file, "fname_match", fname_match, RegExp(fname_match).test(filename));
  if (enc_checked != enc_file) {
    if (enc_file)
      msg.push("The file name looks like an encrypted file, but decryption is not activated.");
    else
      msg.push("The file name looks like an unencrypted file, but decryption is activated.");
  }
  if (msg.length > 0) {
    $('body').trigger('event_errorOccured', [ 'The file name does not match the expectations. Please check whether the correct file has been selected.', msg.join('\n') ]);
  }
};


/*
 * check inputs for backup file upload and start handling after upload (processed by form and php file) has finished
 */
FirmwareRestoreContent.prototype.UploadBackupFiles = function(formObj)
{
  var firmwareRestoreContent  = this;
  var status                  = SUCCESS;
  var uploadDir               = $(formObj).find('input[name=upload_directory]').val();
  var encState				  = $(formObj).find('input[name=decryption_active_state]:checked').length;
  // check input error - no package selected (or not existing either)
  if(0 === $(formObj).find('input[id^=firmware_restore_package_checkbox]:checked').length)
  {
    status  = ERROR;
    $('body').trigger('event_errorOccured', [ 'No package selected.' ] );
  }

  else
  {
    pageElements.ShowBusyScreen('Upload firmware backup...');
    
    var fileInputs = $(formObj).find('[id$=_backup_upload_file_real_input]');
    var files = [];
    $.each(fileInputs, function (key, input) {
      var name = input.id.replace('_backup_upload_file_real_input', '');
      if (input.files.length > 0 && input.files[0]) {

        var destination;
        switch (name) {
          case 'system': destination = 'firmware_backup_rootfs.tgz'; break;
          case 'codesys': destination = 'firmware_backup_codesys.tgz'; break;
          case 'settings': destination = 'firmware_backup_settings'; break;
        }
        //if encryption is enabled and the encryption frontend parameters are successfully checked -> the destination must be with extension ".enc"
        if (1 === encState)
            destination += ".enc";

		
        files.push({
          file: input.files[0],
          destination: destination
        });
        
      }
    });

    Xhr.sessionToken = deviceCommunication.csrfToken;
    var upload = new Upload(files);
    upload.execute(function(filepaths, error) {
      pageElements.RemoveBusyScreen();

      if (error) {
        $('body').trigger('event_errorOccured', [ 'File upload failed.', error.message ]);        
      } else {
        var pathComponents = filepaths[0].split('/');
        pathComponents.splice(pathComponents.length-1);
        var directory = pathComponents.join('/');

        // save upload directory to a hidden input field
        $(formObj).find('input[name=upload_directory]').val(directory);

        firmwareRestoreContent.ActivateBackup(formObj);
      }
    });

    // remember upload object for later cleanup
    firmwareRestoreContent.upload = upload;
  }

  return status;
};


/*
 * activate firmware backup
 */
FirmwareRestoreContent.prototype.ActivateBackup = function(formObj)
{
  var firmwareRestoreContent  = this;
  //Log('Activate Backup');

  var deviceMedium    = $(formObj).find('[name=source_medium]').val();
  var packageCodesys  = $(formObj).find('input[name=package-codesys]:checked').length;
  var packageSettings = $(formObj).find('input[name=package-settings]:checked').length;
  var packageSystem   = $(formObj).find('input[name=package-system]:checked').length;
  var uploadDir       = $(formObj).find('input[name=upload_directory]').val();
  var encPassphrase   = $(formObj).find('input#decryption-passphrase').val();
  // check input error - no package selected (or not existing either)
  if((0 === packageCodesys) && (0 === packageSettings) && ( 0 === packageSystem))
  {
    $('body').trigger('event_errorOccured', [ 'No package selected.' ] );
  }
  
  else
  {
    pageElements.ShowBusyScreen('Restore firmware package(s)...');

    var newValueList  = { sourceDeviceMedium: deviceMedium, uploadDir: uploadDir, packageCodesysFlag: packageCodesys, 
                          packageSettingsFlag: packageSettings, packageSystemFlag: packageSystem, encPassphrase: encPassphrase, };
    
    deviceParams.ChangeValue('process_firmware_restore', newValueList, function(status, errorText)
    {
      // errortext "error" signalizes problems with connection (because of reboot), ignore it
      if((SUCCESS != status) && ('error' != errorText))
      {
        pageElements.RemoveBusyScreen();
        $('body').trigger('event_errorOccured', [ 'Error while processing firmware restore.', errorText ]);

        // clear upload, if restore has failed for some reason
        firmwareRestoreContent.upload && firmwareRestoreContent.upload.cleanup();
        delete firmwareRestoreContent.upload;
      }
      else
      {
        // settings and/or system cause reboot of device - csrf is invalid, "reset" wbm by reload of page
        if(packageSettings || packageSystem)
        {
          var rebootFinishedTimeout  = (1000 * 90); // 90 sec
          deviceCommunication.WaitForConnection(rebootFinishedTimeout, function(status)
          {
            if(SUCCESS != status)
            {
              pageElements.RemoveBusyScreen();
              $('body').trigger('event_errorOccured', [ 'Error while reboot.', 'Timeout' ]);
            }
            else
            {
              window.location.reload();
            }
          });
        }
        else
        {
          pageElements.RemoveBusyScreen();
          $(formObj).find('input#decryption-passphrase').val('');
          // clear upload of single codesys package, if it was uploaded
          firmwareRestoreContent.upload && firmwareRestoreContent.upload.cleanup();
          delete firmwareRestoreContent.upload;
        }
      }
      
      // reset inputs (checkboxes and download file input fields) does not work with IE!!! 
      // $(formObj).find('input[id^=firmware_restore_package_checkbox]').removeAttr('checked');
      //$('#' + firmwareRestoreContent.id + '_content [id$=backup_upload_file_real_input]').val('');
      //$('#' + firmwareRestoreContent.id + '_content [id$=backup_upload_file_fake_input]').val('');
    });
  }
  
};

// check for right frontend configuration
FirmwareRestoreContent.prototype.CheckFrontendDecryptionParas = function(decryptionArea)
{
	var encState        = $(decryptionArea).find('input[name=decryption_active_state]:checked').length;
	var pass			= $(decryptionArea).find('input#decryption-passphrase').val();
	var errorTxt		= '';
	
	if(0 === encState && '' != pass)
	{
		errorTxt		= "Decryption not selected but setting passphrase.";
	}
	else if(1 === encState && '' == pass)
	{
		errorTxt		= "Decryption selected without setting passphrase.";
	}
	
	return errorTxt;
};

//check for right frontend configuration
FirmwareRestoreContent.prototype.InitDecryptionArea = function(decryptionArea)
{
    $(decryptionArea).find('input[id^=decryption_active_state]').removeAttr('checked');
    $(decryptionArea).find('input[id^=decryption_active_state]').removeAttr('disabled');
    $(decryptionArea).find('input#decryption-passphrase').val('');
};
