/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var MassStorageContent = function(id)
{
  var massStorageContent = this;
  
  massStorageContent.id = id || 'mass_storage';
  
  var CreateMassStorageParamView = (function()
  {
    massStorageContent.paramView = new DeviceParamView();
            
    massStorageContent.paramView.Add(
    {
      paramId         : 'active_partition_medium',
      outputElementId : ''
    });
        
    massStorageContent.paramView.Add(
    {
      paramId         : 'device_data_list'
    });
        
  })();
        
  $('#content_area').bind(massStorageContent.id + '_create', function(event)
  {
    // nothing to do once at creation of area...
  });


  $('#content_area').bind(massStorageContent.id + '_refresh', function(event)
  {
    massStorageContent.Refresh();
  });

};


MassStorageContent.prototype.Refresh = function()
{
  var massStorageContent = this;

  // remove old contend
  $('#' + massStorageContent.id + '_content #mass_storage_list').html('');
  $('#' + massStorageContent.id + '_content #no_device_area').html('<p>Unknown</p>');

  // get data and create content
  deviceParams.ReadValueGroup(massStorageContent.paramView.list, function()
  {
    massStorageContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(massStorageContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(massStorageContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading device data.', errorString ] );
    }
    
    massStorageContent.CreateContent();
  });
  
};
  

MassStorageContent.prototype.CreateContent = function()
{
  var massStorageContent    = this;
  var activePartitionMedium = deviceParams.list['active_partition_medium'].value;

  if(!deviceParams.list['device_data_list'].value || !deviceParams.list['device_data_list'].value.length)
  {
    $('#' + massStorageContent.id + '_content #no_device_area').html('<p>No device found.</p>');
  }
  
  else
  {
    $('#' + massStorageContent.id + '_content #no_device_area').html('');
 
    // create and show content for every single device
    $(deviceParams.list['device_data_list'].value).each(function(deviceNo)
    {
      var deviceData  = deviceParams.list['device_data_list'].value[deviceNo];
    
      var deviceHtml  = massStorageContent.DeviceHtml(deviceData.deviceMedium, deviceData.deviceName, 
                                                      activePartitionMedium,  deviceData.deviceLabel);
      
      $('#' + massStorageContent.id + '_content #mass_storage_list').append(deviceHtml);
      
      // event processing for form
      $('#' + massStorageContent.id + '_content #format_medium_form_' + deviceData.deviceMedium).bind('submit', function(event)
      {
        event.preventDefault();
        var device  = $(this).find('input[name=device]').val();
        var label   = $(this).find('input[name=volume-name]').val();
        var fsType  = "";
        if ($(this).find('#radio_fs_type_fat').is(":checked"))
        {
          fsType = "fat";
        }
        else if ($(this).find('#radio_fs_type_ext4').is(":checked"))
        {
          fsType = "ext4";
        }
        massStorageContent.FormatDevice(device, label, fsType);
      });
    });
  }
  
};


MassStorageContent.prototype.DeviceHtml = function(deviceMedium, deviceName, activePartitionMedium, volumeName)
{
  var massStorageContent = this;
  
  var html = '';

  html += '<h3>' + DeviceMediumOutputText(deviceMedium);
  if(deviceMedium === activePartitionMedium) html += ' - Active Partition';
  html += '</h3>';

  html += '<div class="config_form_box" >';
  html +=   '<div class="config_data_list">';
  
  // show device infos 
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Device:</div>';
  html +=       '<div class="value_field">' + deviceName + '</div>';
  html +=     '</div>';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Volume name:</div>';
  html +=       '<div class="value_field">' + volumeName + '</div>';
  html +=     '</div>';
  html +=   '</div>';
  html += '</div>';
  
  //------------------------------------------------------------
  // show format form 
  //------------------------------------------------------------
  if((deviceMedium != activePartitionMedium) && (INTERNAL_FLASH != deviceMedium) && !IS_INTERNAL_FLASH(deviceMedium))
  {
    html += '<h3>' + DeviceMediumOutputText(deviceMedium) + ' - create new filesystem</h3>';
    html += '<form id="format_medium_form_' + deviceMedium + '" action="javascript_requested.php" method="POST">';
    html +=   '<input type="hidden" name="device" value="' + deviceName + '">';
    html +=   '<div class="config_form_box" >';
    html +=     '<div class="config_data_list">';
    html +=       '<div class="label_value_pair">';
    html +=         '<div class="label_field">Filesystem type:</div>';
    html +=         '<div class="value_field">';
    html +=           '<ul class="check_list_important filesystem_type">';
    html +=             '<li>';
    html +=               '<input id="radio_fs_type_ext4" type="radio" name="filesystem" value="ext4" checked="checked" />';
    html +=               '<label for="radio_fs_type_ext4">Ext4</label>';
    html +=             '</li>';
    html +=             '<li>';
    html +=               '<input id="radio_fs_type_fat" type="radio" name="filesystem" value="fat" />';
    html +=               '<label for="radio_fs_type_fat">FAT</label>';
    html +=             '</li>';
    html +=           '</ul>';
    html +=         '</div>';
    html +=       '</div>';
    html +=     '</div>';
    html +=     '<div class="config_data_list">';
    html +=       '<div class="label_value_pair">';
    html +=         '<div class="label_field"><label for="volume_name">Volume name:</label></div>';
    html +=         '<div class="value_field"><input class="input_field" type="input" name="volume-name" value="" ></div>';
    html +=       '</div>';
    html +=     '</div>';
    html +=     '<p class="button_area">';
    html +=       '<input class="button" type="SUBMIT" value="Start Formatting" name="action">';
    html +=     '</p>';
    html +=   '</div>';
    html += '</form>';
  }

  return html;
};


MassStorageContent.prototype.FormatDevice = function(deviceName, volumeName, fsType)
{
  var massStorageContent  = this;
  var volumeName          = volumeName || '';
  
  if(!volumeName || !volumeName.length)
  {
    $('body').trigger('event_errorOccured', [ 'Volume name is missing.' ]);
  }
  
  else if(!deviceName || !deviceName.length)
  {
    $('body').trigger('event_errorOccured', [ 'Software error', 'Device name is missing.' ]);
  }

  else if (!fsType || !fsType.length)
  {
    $('body').trigger('event_errorOccured', [ 'Filesystem type is missing']);
  }
  
  else
  {
    if(confirm('All data will be deleted. Do you want to continue?'))
    {
      pageElements.ShowBusyScreen('Formatting device...');
      
      var newValueList = { deviceName: deviceName, volumeName: volumeName, fsType: fsType };

      deviceParams.ChangeValue('format_medium', newValueList, function(status, errorText)
      {
        pageElements.RemoveBusyScreen();
        if(SUCCESS != status)
        {
          $('body').trigger('event_errorOccured', [ 'Error while formatting device.', errorText ]);
        }
        
        massStorageContent.Refresh();
      });
    }
  }
  
  return false;
};


