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
      bacnetStorageContent.ConfirmPersistence();
    });
    
    $("#bacnet_trend_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetStorageContent.ChangeTrendlog();
    });
    
    $("#bacnet_event_form").bind('submit', function(event)
    {
      event.preventDefault();
      bacnetStorageContent.ChangeEventlog();
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
BacnetStorageContent.prototype.ConfirmPersistence = function()
{
  var bacnetStorageContent = this;
  
  pageElements.ShowConfirmScreen("Important note: <br/>After changing the storage location, " +
  		"problems with persistence data may occur until the next complete persistence cycle is finished.", 
		"The persistence is performed every 30s. Please do not turn off or restart the controller " +
		"after changing the storage location within the following 30s. " +
		"Otherwise persistence data will be lost.", 
		function() { bacnetStorageContent.ChangePersistence(); } );
};

/*
 * Change persistence location
 */
BacnetStorageContent.prototype.ChangePersistence = function()
{
  var bacnetStorageContent = this;
  
  pageElements.ShowBusyScreen("Changing BACnet persistence...");
  
  // check selected location
  if($('[id=bacnet_persist_internal_flash]').prop('checked'))
  {
    var newValueList = { persistenceStorage: $('[id=bacnet_persist_internal_flash]').val() };
  }
  else if($('[id=bacnet_persist_sd_card]').prop('checked'))
  {
    var newValueList = { persistenceStorage: $('[id=bacnet_persist_sd_card]').val() };
  } 
  
  deviceParams.ChangeValue('bacnet_persistence_storage', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing persistence storage location.', errorText ]);
    }
    
    bacnetStorageContent.Refresh();
  });
};

/*
 * Change trendlog location
 */
BacnetStorageContent.prototype.ChangeTrendlog = function()
{
  var bacnetStorageContent = this;
  
  pageElements.ShowBusyScreen("Changing BACnet trendlog...");    

  //check selected location
  if($('[id=bacnet_trend_internal_flash]').prop('checked'))
  {
    var newValueList = { trendlogStorage: $('[id=bacnet_trend_internal_flash]').val() };
  }
  else if($('[id=bacnet_trend_sd_card]').prop('checked'))
  {
    var newValueList = { trendlogStorage: $('[id=bacnet_trend_sd_card]').val() };
  } 
  
  deviceParams.ChangeValue('bacnet_trendlog_storage', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing trendlog storage location.', errorText ]);
    }
    
    bacnetStorageContent.Refresh();
  });
};

/*
 * Change eventlog location
 */
BacnetStorageContent.prototype.ChangeEventlog = function()
{
  var bacnetStorageContent = this;
  
  pageElements.ShowBusyScreen("Changing BACnet eventlog...");    

  //check selected location
  if($('[id=bacnet_event_internal_flash]').prop('checked'))
  {
    var newValueList = { eventlogStorage: $('[id=bacnet_event_internal_flash]').val() };
  }
  else if($('[id=bacnet_event_sd_card]').prop('checked'))
  {
    var newValueList = { eventlogStorage: $('[id=bacnet_event_sd_card]').val() };
  } 
  
  deviceParams.ChangeValue('bacnet_eventlog_storage', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing eventlog storage location.', errorText ]);
    }
    
    bacnetStorageContent.Refresh();
  });
};

