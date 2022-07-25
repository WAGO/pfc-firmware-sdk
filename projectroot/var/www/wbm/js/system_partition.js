/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var SystemPartitionContent = function(id)
{
  var systemPartitionContent = this;
  
  systemPartitionContent.id = id || 'system_partition';
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------

  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var CreateSystemPartitionContentParamView = (function()
  {
    systemPartitionContent.paramView = new DeviceParamView();
            
    // no output handling specified, because output must consider other coupler values
    systemPartitionContent.paramView.Add(
    {
      paramId         : 'active_partition'
    });
        
    systemPartitionContent.paramView.Add(
    {
      paramId         : 'active_partition_medium'
    });

    systemPartitionContent.paramView.Add(
    {
      paramId:          'systeminfo'
    });

  })();

  
  //------------------------------------------------------------------------------
  // event - first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(systemPartitionContent.id + '_create', function(event)
  {
    $('#' + systemPartitionContent.id + '_content #start_backup_system_button').bind('click', function(event)
    {
      event.preventDefault();
      systemPartitionContent.SwitchBootloader();
    });
  });


  //------------------------------------------------------------------------------
  // event - refresh content area
  //------------------------------------------------------------------------------
  $('#content_area').bind(systemPartitionContent.id + '_refresh', function(event)
  {
    systemPartitionContent.Refresh();
  });

};


/**
 * Filter number of active partition of systeminfo.
 * 
 * @param systeminfo object array of infos for every system (must be correct)
 * @return int number of active partition
 */
SystemPartitionContent.prototype.GetActivePartitionNo = function(syteminfo)
{
  var activePartitionNo = 'unknown';

  $.each(deviceParams.list['systeminfo'].value, function(systemIndex, singleSystemInfo)
  {
    if(singleSystemInfo.active)
    {
      activePartitionNo = parseInt(singleSystemInfo['system-number'], 10);
      return;
    }
  });
  return activePartitionNo;
}


SystemPartitionContent.prototype.Refresh = function()
{
  var systemPartitionContent = this;
  
  deviceParams.ReadValueGroup(systemPartitionContent.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(systemPartitionContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(systemPartitionContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading filesystem data.', errorString ] );
    }
    else
    {
      var activePartitionMedium = deviceParams.list['active_partition_medium'].value;
      var activePartition       = deviceParams.list['active_partition'].value;
      var activePartitionNo     = systemPartitionContent.GetActivePartitionNo(deviceParams.list['systeminfo'].value);
      
      $('#' + systemPartitionContent.id + '_content .active_partition').html('Partition ' + activePartitionNo);
      $('#' + systemPartitionContent.id + '_content .active_partition_medium').html('on ' + DeviceMediumOutputText(activePartitionMedium));
    }
  });

};


SystemPartitionContent.prototype.SwitchBootloader = function() 
{
  // as a precaution recheck
  if(confirm('Really want to switch boot partition?'))
  {
    pageElements.ShowBusyScreen('Switch bootloader...');
 
    var newValueList = { };

    deviceParams.ChangeValue('switch_bootloader', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while switching bootloader.', errorText ]);
      }
        
    });
  }
};

