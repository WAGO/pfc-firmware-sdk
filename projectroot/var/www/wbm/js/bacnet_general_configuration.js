/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var BacnetGeneralContent = function(id)
{
  var bacnetGeneralContent = this;

  bacnetGeneralContent.id    = id || 'bacnet_general_configuration';  
  bacnetGeneralContent.uploadDir    = UPLOAD_DIR + '/bacnet_upload';
  bacnetGeneralContent.newOverrideName    = 'override.xml';
  
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
   * Display BACnet states
   */
  var DisplayState = function(paramStatus, paramValue, outputElementId)
  {
    $(outputElementId).removeAttr('checked');

    if((SUCCESS === paramStatus) && ('true' === paramValue))
    {
      $('#' + outputElementId).prop('checked', true);
    }
    else
    {
      $('#' + outputElementId).prop('checked', false);
    }
  };
  
  /*
   * Display Upload Delete State
   */
  var DisplayDeleteState = function(paramStatus, paramValue, outputElementId)
  {    
    if((SUCCESS === paramStatus) && (1 == paramValue))
    {
      $('#' + bacnetGeneralContent.id + '_content #bacnet_delete_submit_button').removeAttr('disabled');
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_file_fake_input').attr('disabled', 'disabled');
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_file_fake_button').attr('disabled', 'disabled');
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_file_real_input').attr('disabled', 'disabled');
    }
    else
    {
      $('#' + bacnetGeneralContent.id + '_content #bacnet_delete_submit_button').attr('disabled', 'disabled');
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_file_fake_input').removeAttr('disabled');
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_file_fake_button').removeAttr('disabled');
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_file_real_input').removeAttr('disabled');
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
      paramId         : 'bacnet_udp_port',
      outputElementId : bacnetGeneralContent.id + '_content #bacnet_port_number'
    });

    bacnetGeneralContent.paramView.Add(
    {
      paramId         : 'bacnet_who_is_online_interval',
      outputElementId : bacnetGeneralContent.id + '_content #bacnet_who_is_online_interval_number'
    });
    
    bacnetGeneralContent.paramView.Add(
    {
      paramId         : 'bacnet_persistence_delete',
      outputElementId : bacnetGeneralContent.id + '_content #bacnet_pers_del_active_state',
      outputFkt       : DisplayState
    });
    
    bacnetGeneralContent.paramView.Add(
    {
      paramId         : 'bacnet_delete_all',
      outputElementId : bacnetGeneralContent.id + '_content #bacnet_del_all_active_state',
      outputFkt       : DisplayState
    });
    
    bacnetGeneralContent.paramView.Add(
    {
      paramId         : 'bacnet_wbm_upload',
      outputElementId : bacnetGeneralContent.id + '_content #spanWbmCopyId',
      outputFkt       : DisplayDeleteState
    });
    
  })();
  
  //------------------------------------------------------------------------------
  // creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(bacnetGeneralContent.id + '_create', function(event)
  {
    // write upload directory on device in form member
    $(('#' + bacnetGeneralContent.id + '_content #bacnet_upload_form')).append('<input type="hidden" name="upload_type" value="' + bacnetGeneralContent.uploadDir + '">');
    //$('#' + bacnetGeneralContent.id + '_content #upload_bacnet_iframe').show();
    
    // add events for form processing
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
    
    $("#bacnet_pers_del_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetGeneralContent.ChangeState(event, "bacnet_persistence_delete", "persDelState", "bacnet_pers_del_active_state", "persistence delete");
    });
    
    $("#bacnet_del_all_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetGeneralContent.ChangeState(event, "bacnet_delete_all", "delAllState", "bacnet_del_all_active_state", "reset all BACnet");
    });
    
    // upload events
    $("#bacnet_upload_file_real_input").bind('change', function(event)
    {
      // get filename of files array, because reading fileupload value is not the correct way and adds funny strings to filename ("fakepath etc.")
      // respect, that filearray is not always existing
      var filename = "";
      if($(event.target)[0].files[0] && $(event.target)[0].files[0].name) 
      {
        filename = $(event.target)[0].files[0].name;
      }
      // show filename in fake input field
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_form').find('.file_browse_fake_input').val(filename);
      // enable update button if really one file was selected
      if(filename.length)
      {
        $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_submit_button').removeAttr('disabled');
      }
      else
      {
        $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_submit_button').attr('disabled', 'disabled');
      }
    });
    
    $("#bacnet_upload_form").bind('submit', function(event) 
    {
      // don't prevent default!!! File upload is handled in iframe, not via ajax
      //event.preventDefault();
      bacnetGeneralContent.Upload(this);
    });
    
    $("#bacnet_delete_form").bind('submit', function(event) 
    {
      event.preventDefault();
      bacnetGeneralContent.Delete();
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
 * Change state
 */
BacnetGeneralContent.prototype.ChangeState = function(event, paramId, paramName, paramInput, paramText)
{
  var bacnetGeneralContent = this;
  var newValueList = {};
  
  pageElements.ShowBusyScreen("Changing BACnet " + paramText + "...");
  newValueList[paramName] = 'false';
  if( ('true' === $("input[name=" + paramInput +"]").val()) && 
      ($("input[name=" + paramInput +"]").prop("checked")) )
  {
    newValueList[paramName] = 'true';
  }
   
  deviceParams.ChangeValue(paramId, newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing ' + paramText, errorText ]);
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

/*
 * BACnet Delete
 */
BacnetGeneralContent.prototype.Delete = function()
{
  var bacnetGeneralContent = this;
  
  pageElements.ShowBusyScreen('Delete file...');
  
  var newValueList  = { uploadFunc: 'delete' };
  
  deviceParams.ChangeValue('bacnet_wbm_upload', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while delete BACnet Upload', errorText ]);
    }
    
    bacnetGeneralContent.Refresh();
  });
};

/*
 * BACnet Upload
 */
BacnetGeneralContent.prototype.Upload = function(formObj)
{
  pageElements.ShowBusyScreen('Upload file...');
  
  var bacnetGeneralContent = this;
  var fileInput = $(formObj).find('[id$=bacnet_upload_file_real_input]')[0];
  var file = fileInput.files[0];

  Xhr.sessionToken = deviceCommunication.csrfToken;
  var upload = new Upload([{
    file: file, 
    destination: bacnetGeneralContent.uploadDir + '/' + bacnetGeneralContent.newOverrideName
  }]);

  upload.execute(function(filepaths, error) {
    pageElements.RemoveBusyScreen();
    if (error) {
      $('body').trigger('event_errorOccured', [ 'File upload failed.', error.message ]);
    } else {
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_file_real_input').val('');
      $('#' + bacnetGeneralContent.id + '_content #bacnet_upload_file_real_input').trigger('change');
      
      var newValueList  = { uploadFunc: 'copy' };
      pageElements.ShowBusyScreen('Copy BACnet Upload...');
      deviceParams.ChangeValue('bacnet_wbm_upload', newValueList, function(status, errorText)
      {
        pageElements.RemoveBusyScreen();
        if(SUCCESS != status) {
          $('body').trigger('event_errorOccured', [ 'Error while copy BACnet Upload', errorText ]);
        } else {
          upload.cleanup();
          bacnetGeneralContent.Refresh();
        }
      });
    }
  });
};