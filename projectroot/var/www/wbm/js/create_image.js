/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function CreateImageContent(id)
{
  var createImageContent = this;
  
  createImageContent.id = id || 'create_image';
  
  createImageContent.SIZE_CHECK_OK        = 0;
  createImageContent.SIZE_CHECK_WARNING   = 13;
  createImageContent.SIZE_CHECK_ERROR     = 14;
  
  createImageContent.IN_USE_CHECK_OK      = 0;
  createImageContent.IN_USE_CHECK_WARNING = 1;
  
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var DisplayActivePartitionMedium = function(paramStatus, activePartitionMedium, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      var outputText = DeviceMediumOutputText(activePartitionMedium);
      $('#' + createImageContent.id + '_content #active_partition_medium').html(' (' + outputText + ')');
    }
  };
  

  var CreateCreateImageContentParamView = (function()
  {
    createImageContent.paramView = new DeviceParamView();
            
    createImageContent.paramView.Add(
    {
      paramId         : 'active_partition_medium',
      outputElementId : '',
      outputFkt       : DisplayActivePartitionMedium
    });
    
    createImageContent.paramView.Add(
    {
      paramId         : 'device_medium_list'
    });
    
    createImageContent.paramView.Add(
    {
      paramId         : 'memory_card_device_name'
    });
    
    createImageContent.paramView.Add(
    {
      paramId         : 'memory_card_image_min_size_mb'
    });
        
  })();

  
  $('#content_area').bind(createImageContent.id + '_create', function(event)
  {
    $('#' + createImageContent.id + '_content .area_depending_destination_memory_card').hide();

    // add event processing
    $('#' + createImageContent.id + '_content #copy_firmware_image_form').bind('submit', function(event) 
    {
      event.preventDefault();
      createImageContent.CopyFirmwareImage();
    });

    $('#' + createImageContent.id + '_content #create_image_destination_medium').bind('change', function(event)
    {
      event.preventDefault();
      createImageContent.EvaluateAndShowDestinationDeviceInfos();
    });
        
  });


  $('#content_area').bind(createImageContent.id + '_refresh', function(event)
  {
    createImageContent.Refresh();
  });

};


CreateImageContent.prototype.Refresh = function()
{
  var createImageContent = this;

  deviceParams.ReadValueGroup(createImageContent.paramView.list, function()
  {
    createImageContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(createImageContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(createImageContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading device data.', errorString ] );
    }
    else
    {
      createImageContent.CreateContent();
    }
  });
  
};


CreateImageContent.prototype.CreateContent = function()
{
  var createImageContent    = this;
  var activePartitionMedium = deviceParams.list['active_partition_medium'].value;
  
  var copyImageForm = $('#' + createImageContent.id + '_content #copy_firmware_image_form');

  // delete (possible) existing selection menu entries
  $('#' + createImageContent.id + '_content [name=destination_medium] option').remove();

  var optionCount = 0;
  $.each(deviceParams.list['device_medium_list'].value, function(mediumIndex, medium)
  {
    // don't offer active partition (we can't copy to ourselfes...)
    if(medium != activePartitionMedium)
    {
      var optionHtml = '<option value="' + medium + '" >' + DeviceMediumOutputText(medium) + '</option>';
      $(copyImageForm).find('select[name=destination_medium]').append(optionHtml);
      ++optionCount;
    }
  });
  
  // if no device was found, display this error and set form disabled
  if(0 === optionCount)
  {
    status = ERROR;
    $(copyImageForm).find('#create_image_read_only_destination_medium').html('No device found');
    $(copyImageForm).find('select[name=destination_medium]').append('<option value="error" >No device found</option>');
    $(copyImageForm).find('select[name=destination_medium]').attr('disabled', 'disabled');
    $(copyImageForm).find('p.button_area').hide();
    $('#' + createImageContent.id + '_content #device_check_note').hide();
    $('#' + createImageContent.id + '_content .area_depending_destination_memory_card').hide();
  }
  
  // no error - set first entry in selection menu and check depending device
  else
  {
    $(copyImageForm).find('p.button_area').show();

    // set first entry selected by using first element of options array - just unsing val(1) does not longer work after jQuery upgrade
    var destinationMenuOptions = $(copyImageForm).find('select[name=destination_medium] option');
    $(destinationMenuOptions[0]).prop('selected', true);
    createImageContent.EvaluateAndShowDestinationDeviceInfos();

    // exactly one device was found - offer selection menu to user is needless
    // - set data accordingly, but show value only on "read only" area
    if(1 === optionCount)
    {
      var selectedOptionText = $(copyImageForm).find('select[name=destination_medium] option:selected').text();
      $('#' + createImageContent.id + '_content #create_image_read_only_destination_medium').html(selectedOptionText);
    }
    // more than one device was found - show selection menu and hide "read only" area
    else
    {
      $(copyImageForm).find('#create_image_read_only_destination_medium').hide();
      $(copyImageForm).find('select[name=destination_medium]').removeClass('initial_invisible');
    }
  }
};


//------------------------------------------------------------------------------
// Get informations about device to decide if write access is allowed/advisible
// and display them to user.
//------------------------------------------------------------------------------
CreateImageContent.prototype.EvaluateAndShowDestinationDeviceInfos = function(destinationMedium)
{
  var createImageContent    = this;
  var activePartitionMedium = deviceParams.list['active_partition_medium'].value;
  var destinationMedium     = $('#' + createImageContent.id + '_content #copy_firmware_image_form select[name=destination_medium]').val();
  
  var variablesList         = { sourceMedium: activePartitionMedium, destinationMedium: destinationMedium };

  // do the necessary stuff if destination is sd card
  $('#' + createImageContent.id + '_content .area_depending_destination_memory_card').hide();
  if('sd-card' === destinationMedium)
  {
    // hide or show sd-card depending area
    $('#' + createImageContent.id + '_content .area_depending_destination_memory_card').show();
    
    // get size of sd card
    var sdCardDeviceName = deviceParams.list['memory_card_device_name'].value;
    deviceParams.ReadValue({ id: 'memory_card_size_kb', variables: { sdCardDeviceName: sdCardDeviceName } }, function()
    {
      if(SUCCESS != deviceParams.list['memory_card_size_kb'].status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while checking destination device.' ]);
      }
    });
  }
  
  pageElements.ShowBusyScreen('Checking destination device...');
  
  // get device check infos one after the other
  deviceParams.ReadValue({ id: 'fw_copy_nand_size_check', variables: variablesList }, function()
  {
    deviceParams.ReadValue({ id: 'fw_copy_in_use_check', variables: variablesList }, function()
    {
      pageElements.RemoveBusyScreen();
      // mocking result for test
      //deviceParams.list['fw_copy_nand_size_check'].status = createImageContent.SIZE_CHECK_ERROR; 
      //deviceParams.list['fw_copy_nand_size_check'].status = createImageContent.SIZE_CHECK_OK; 
      //deviceParams.list['fw_copy_nand_size_check'].status = createImageContent.SIZE_CHECK_WARNING; 
      //deviceParams.list['fw_copy_in_use_check'].status    = createImageContent.IN_USE_CHECK_WARNING; 

      // transfer the check results into according text components
      var checkResultObj = createImageContent.EvaluateCheckResult(deviceParams.list['fw_copy_nand_size_check'].status,
                                                                  deviceParams.list['fw_copy_in_use_check'].status);
      
      // prepare note text for display in html content
      var noteText  = checkResultObj.noteText.replace(/\n/g, '<br>');

      // make device check info area visible, show image and text
      $('#' + createImageContent.id + '_content #device_check_note').html(checkResultObj.imgTag + '<p>' + noteText + '</p>');
      $('#' + createImageContent.id + '_content #device_check_note').show();
    });
    
  });
};


//------------------------------------------------------------------------------
// Evaluate device info about destination device
// decide if state is ok, warning or error, accordingly assign image, output text, 
// enable/disable form button etc.
//------------------------------------------------------------------------------
CreateImageContent.prototype.EvaluateCheckResult = function(sizeCheckResult, inUseCheckResult)
{
  var createImageContent  = this;
  var checkResultObj      = new Object();
  
  checkResultObj.sizeCheckResult  = sizeCheckResult;
  checkResultObj.inUseCheckResult = inUseCheckResult;
  checkResultObj.noteText         = '';
  checkResultObj.imgTag           = ''; 
  
  // size check and rts check in apple pie order
  if((createImageContent.SIZE_CHECK_OK === sizeCheckResult) && (createImageContent.IN_USE_CHECK_OK === inUseCheckResult))
  {
    checkResultObj.noteText = "Enough memory is available on destination device,\ndevice is not in use by CODESYS.";
    checkResultObj.imgTag   = '<img class="type" alt="OK" title="OK" src="images/check_mark_circle_transp.gif">';
  }

  // not enough memory on destination device
  else if(createImageContent.SIZE_CHECK_ERROR === sizeCheckResult)
  {
    checkResultObj.noteText = 'Not enough memory available on destination device.\nWrite access not allowed.';
    checkResultObj.imgTag   = '<img class="type" alt="Error" title="Error" src="images/exclamation_mark_transp.gif">';
  }
  
  // less memory on destination device and/or destination device in use - warning
  else if((createImageContent.SIZE_CHECK_WARNING == sizeCheckResult) || (createImageContent.IN_USE_CHECK_WARNING == inUseCheckResult))
  {
    if(createImageContent.SIZE_CHECK_WARNING == sizeCheckResult)
        checkResultObj.noteText = 'Possibly not enough memory available on destination device.\n Reason: the space requirement for this operation can only be approximately estimated.';
    if(createImageContent.IN_USE_CHECK_WARNING == inUseCheckResult)  checkResultObj.noteText = checkResultObj.noteText + 'Device is in use by CoDeSys.';
    
    checkResultObj.imgTag = '<img class="type" alt="Warning" title="Warning" src="images/exclamation_mark_transp.gif">';
  }
  
  // any other problems on destination device
  else
  {
    checkResultObj.noteText = 'Error while checking destination device.\nWrite access not allowed.';
    checkResultObj.imgTag   = '<img class="type" alt="Error" title="Error" src="images/exclamation_mark_transp.gif">';
  }

  return checkResultObj;
};


//------------------------------------------------------------------------------
// Evaluate if copy access is allowed, if it is not, show info about in popup 
// or - in case of warning - get permission by user.
//------------------------------------------------------------------------------
CreateImageContent.prototype.GetAccessAdmission = function(sizeCheckResult, inUseCheckResult, noteText)
{
  var createImageContent  = this;
  var admission           = false;
  
  // size check and rts check in apple pie order
  if((createImageContent.SIZE_CHECK_OK === sizeCheckResult) && (createImageContent.IN_USE_CHECK_OK === inUseCheckResult))
  {
    admission = true;
  }

  // not enough memory on destination device - error
  else if(createImageContent.SIZE_CHECK_ERROR === sizeCheckResult)
  {
    $('body').trigger('event_errorOccured', [ noteText ]);
    admission  = false;
  }
  
  // less memory on destination device and/or destination device in use - warning
  else if((createImageContent.SIZE_CHECK_WARNING == sizeCheckResult) || (createImageContent.IN_USE_CHECK_WARNING == inUseCheckResult))
  {
    noteText  = noteText + "\nDo you want to continue?";
    admission = confirm(noteText);
  }
  
  // any other problems on destination device
  else
  {
    $('body').trigger('event_errorOccured', [ noteText ]);
    admission  = false;
  }
  
  return admission;
};


CreateImageContent.prototype.CopyFirmwareImage = function()
{
  var createImageContent    = this;
  var activePartitionMedium = deviceParams.list['active_partition_medium'].value;
  var destinationMedium     = $('#' + createImageContent.id + '_content select[name=destination_medium]').val();

  var sizeCheckResult   = deviceParams.list['fw_copy_nand_size_check'].status;
  var inUseCheckResult  = deviceParams.list['fw_copy_in_use_check'].status;
  var status            = SUCCESS;
  
  var newValueList = { sourceMedium: activePartitionMedium, destinationMedium: destinationMedium, size: '0' };

  // if image should be created on sd card, we must determin which size image shall have
  if('sd-card' === destinationMedium)
  {
    // default: as small as posible
    newValueList.size = deviceParams.list['memory_card_image_min_size_mb'].value;
    
    // user wants to use whole memory card size
    var selectCreateImageSize = $('#' + createImageContent.id + '_content select[name=create_image_size]').val();
    if('memory_card_size' === selectCreateImageSize)
    {
      // convert size from kB to MB, to next lower integer and to string
      newValueList.size = '' + Math.floor(parseInt(deviceParams.list['memory_card_size_kb'].value, 10) / 1024);

      if(SUCCESS != deviceParams.list['memory_card_size_kb'].status)
      {
        status = ERROR;
        $('body').trigger('event_errorOccured', [ 'Error while copying firmware image.', 'Can\'t evaluate memory card size' ]);
      }
    }
  }


  if(SUCCESS === status)
  {
    // check if copy is allowed and reinsure at user if necessary
    var checkResultObj  = createImageContent.EvaluateCheckResult(sizeCheckResult, inUseCheckResult);
    var accessAdmitted  = createImageContent.GetAccessAdmission(sizeCheckResult, inUseCheckResult, checkResultObj.noteText);
    
    if(accessAdmitted)
    {
      pageElements.ShowBusyScreen('Copy process is active\n- can take a few minutes!');
    
      deviceParams.ChangeValue('copy_firmware_image', newValueList, function(status, errorText)
      {
        pageElements.RemoveBusyScreen();
        if(SUCCESS != status)
        {
          $('body').trigger('event_errorOccured', [ 'Error while copying firmware image.', errorText ]);
        }
      });
    }
  }
};



