/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var BacnetStorageContent = function(id)
{
  var bacnetStorageContent = this;

  bacnetStorageContent.id    = id || 'bacnet_storage_location';
   
  /*
   * Highlight selection
   */
  var DisplaySelectedStorageLocation = function(paramStatus, paramValue, outputElementId)
  {
    if((SUCCESS === paramStatus) && ('internal-flash' === paramValue))
    {
      $('[id=' + outputElementId + '_sd_card]').removeAttr('checked');
      $('[id=' + outputElementId + '_internal_flash]').prop('checked', true);
    }
    
    if((SUCCESS === paramStatus) && ('sd-card' === paramValue))
    {
      $('[id=' + outputElementId + '_internal_flash]').removeAttr('checked');
      $('[id=' + outputElementId + '_sd_card]').prop('checked', true);
    }
  }; 
  
  
  /*
   * Disable and greyed the SD Card if this option not available
   */
  var DisplayStorageState =  function(paramStatus, paramValue, outputElementId)
  {
    // default value
    $('[for=sd_card]').removeClass('grayed');
    $('[id=bacnet_persist_sd_card]').removeAttr('disabled');
    $('[id=bacnet_trend_sd_card]').removeAttr('disabled');
    $('[id=bacnet_event_sd_card]').removeAttr('disabled');
    
    // sd-card insert ?
    if((SUCCESS === paramStatus) && ('0' === paramValue))
    {
      $('[for=sd_card]').addClass('grayed');
      
      // persistence      
      $('[id=bacnet_persist_sd_card]').attr('disabled', true);
      
      // trendlog      
      $('[id=bacnet_trend_sd_card]').attr('disabled', true);
      
      // eventlog      
      $('[id=bacnet_event_sd_card]').attr('disabled', true);
    }
  };
  
  /*
   * Disable and greyed all if the controller boot from sd card 
   */  
  var DisplayActivePartitionMedium = function(paramStatus, activePartitionMedium, outputElementId)
  {
    if((SUCCESS === paramStatus) &&('sd-card' === activePartitionMedium))
    {
      $('[for=sd_card]').addClass('grayed');
      $('[for=internal_flash]').addClass('grayed');
    	
	  // persistence      
      $('[id=bacnet_persist_sd_card]').attr('disabled', true);
      $('[id=bacnet_persist_internal_flash]').attr('disabled', true);
    
      // trendlog      
      $('[id=bacnet_trend_sd_card]').attr('disabled', true);
      $('[id=bacnet_trend_internal_flash]').attr('disabled', true);
      
      // eventlog      
      $('[id=bacnet_event_sd_card]').attr('disabled', true);
      $('[id=bacnet_event_internal_flash]').attr('disabled', true);
    }
  };
  
  //------------------------------------------------------------------------------
  // reference bacnet content device parameter
  //------------------------------------------------------------------------------
  var CreateBacnetParamView = (function()
  {
    bacnetStorageContent.paramView = new DeviceParamView();

    bacnetStorageContent.paramView.Add(
    {
      paramId         : 'bacnet_persistence_storage',
      outputElementId : 'bacnet_persist',
      outputFkt       : DisplaySelectedStorageLocation
    });
    
    bacnetStorageContent.paramView.Add(
    {
      paramId         : 'bacnet_trendlog_storage',
      outputElementId : 'bacnet_trend',
      outputFkt       : DisplaySelectedStorageLocation
    });
    
    bacnetStorageContent.paramView.Add(
    {
      paramId         : 'bacnet_eventlog_storage',
      outputElementId : 'bacnet_event',
      outputFkt       : DisplaySelectedStorageLocation
    });
    
    bacnetStorageContent.paramView.Add(
    {
      paramId         : 'bacnet_storage_state',
      outputElementId : '',
      outputFkt       : DisplayStorageState
    });
    
    bacnetStorageContent.paramView.Add(
    {
      paramId         : 'active_partition_medium',
      outputElementId : '',
      outputFkt       : DisplayActivePartitionMedium
    });
    
  })();
  
  //------------------------------------------------------------------------------
  // creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(bacnetStorageContent.id + '_create', function(event)
  {
	// add events for form processing    
    $("#bacnet_persist_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetStorageContent.ConfirmChangeStorage(event);
    });
    
    $("#bacnet_trend_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetStorageContent.ConfirmChangeStorage(event);
    });
    
    $("#bacnet_event_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetStorageContent.ConfirmChangeStorage(event);
    });
  });
  
  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(bacnetStorageContent.id + '_refresh', function(event)
  {
    bacnetStorageContent.Refresh();
  });
  
};

/*
 * Refresh values
 */
BacnetStorageContent.prototype.Refresh = function()
{
  var bacnetStorageContent = this;
  
  deviceParams.ReadValueGroup(bacnetStorageContent.paramView.list, function()
  {
        
    if(deviceParams.ReadErrorOccured(bacnetStorageContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(bacnetStorageContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading BACnet services configuration.', errorString ] );
    }
    else
    {
      bacnetStorageContent.paramView.ShowValues();
    }
  });
};


/*
 * Confirm Info Screen 
 */
BacnetStorageContent.prototype.ConfirmChangeStorage = function(event)
{
  var bacnetStorageContent = this;
  var eventFormId = event.target.id;
  
  switch(eventFormId)
  {
  case "bacnet_persist_form":
    if(confirm("Really want to change persistence storage location? \n" +
               "Important note: Please do not turn off or restart the controller\n" +
               "after changing the storage location within the following 30s.\n" +
               "Otherwise data will be lost."))
    {
      bacnetStorageContent.ChangeStorage("persistence", "persist");
    }
    break;
  case "bacnet_trend_form":
    if(confirm("Really want to change trendlog storage location? \n" +
               "Important note: Data will be lost"))
    {
      bacnetStorageContent.ChangeStorage("trendlog", "trend");
    }
    
    break;
  case "bacnet_event_form":
    if(confirm("Really want to change eventlog storage location? \n" +
               "Important note: Data will be lost"))
    {
      bacnetStorageContent.ChangeStorage("eventlog", "event");
    }
    break;
  default:
    $('body').trigger('event_errorOccured', [ 'Error while changing storage location.', errorText ]);
    return;
    break;
  }
  
  bacnetStorageContent.Refresh();
};

/*
 * Change storage location
 */
BacnetStorageContent.prototype.ChangeStorage = function(name, idName)
{
  pageElements.ShowBusyScreen("Changing BACnet " + name + "...");
  
  // check selected location
  if($('[id=bacnet_' + idName + '_internal_flash]').prop('checked'))
  {
    var newValueList = {};
    newValueList[name +"Storage"] = $('[id=bacnet_' + idName + '_internal_flash]').val()
  }
  else if($('[id=bacnet_' + idName + '_sd_card]').prop('checked'))
  {
    var newValueList = {};
    newValueList[name +"Storage"] = $('[id=bacnet_' + idName + '_sd_card]').val()
  } 
  // call config tool
  deviceParams.ChangeValue('bacnet_' + name + '_storage', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing ' + name + ' storage location.', errorText ]);
    }
  });
};

