/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

//------------------------------------------------------------------------------
// CODESYS common configuration data / project details
//------------------------------------------------------------------------------
var RtsCommonConfig = function()
{
  var rtsCommonConfig = this;

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var DisplayCodesysProjectDescription = function(paramStatus, paramValue, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      var description = '';
      var description = paramValue ? paramValue.replace(/\r\n/g, "<br>") : '';
      $('#' + outputElementId).html(description);
    }
  };

  var ShowNumberOfTasks = function(paramStatus, rtsTaskList, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      var taskCount = rtsTaskList.length;
      $('#' + outputElementId).text(taskCount);
    }
  };

  //------------------------------------------------------------------------------
  // initialisation
  //------------------------------------------------------------------------------
  var CreateRtsCommonConfigParamView = (function()
  {
    rtsCommonConfig.paramView = new DeviceParamView();
            
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_webserver_version',
      outputElementId : 'rts_info_content #codesys_webserver_version'
      //defaultContent  : 'unknown'
    });
    
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_state',
      outputElementId : 'rts_info_content #rts_state'
      //defaultContent  : 'unknown'
    });
        
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'rts_task_list',
      outputElementId : 'rts_info_content #number_of_tasks',
      outputFkt       : ShowNumberOfTasks
      
    });
    /*
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_task_count',
      outputElementId : 'rts_info_content #number_of_tasks'
      //defaultContent  : 'unknown'
    });
    */
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_project_date',
      outputElementId : 'rts_info_content #project_date'
      //defaultContent  : 'unknown',
    });
        
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_project_title',
      outputElementId : 'rts_info_content #project_title'
      //defaultContent  : 'unknown',
    });
        
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_project_version',
      outputElementId : 'rts_info_content #project_version',
      //defaultContent  : 'unknown'
    });
        
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_project_author',
      outputElementId : 'rts_info_content #project_author',
      //defaultContent  : 'unknown'
    });
        
    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_project_description',
      outputElementId : 'rts_info_content #project_description',
      outputFkt       : DisplayCodesysProjectDescription
      //defaultContent  : 'unknown',
    });

    rtsCommonConfig.paramView.Add(
    {
      paramId         : 'codesys_project_checksum',
      outputElementId : 'rts_info_content #project_checksum',
      //defaultContent  : 'unknown',
    });
    
  })();

};


RtsCommonConfig.prototype.Refresh = function()
{
  var rtsCommonConfig = this;

  rtsCommonConfig.paramView.ShowDefaultContent();
  
  deviceParams.ReadValueGroup(rtsCommonConfig.paramView.list, function()
  {
    rtsCommonConfig.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(rtsCommonConfig.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(rtsCommonConfig.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading rts information.', errorString ] );
    }
  });
};



//------------------------------------------------------------------------------
// CODESYS task configuration
//------------------------------------------------------------------------------
var RtsTaskConfig = function()
{
  var rtsTaskConfig = this;

  rtsTaskConfig.paramView   = new DeviceParamView();
  rtsTaskConfig.openImgUrl  = 'images/plus_green_18x18.png';
  rtsTaskConfig.closeImgUrl = 'images/minus_green_18x18.png';

  var CreateRtsTaskConfigView = (function()
  {
    rtsTaskConfig.paramView = new DeviceParamView();
                
    rtsTaskConfig.paramView.Add(
    {
      paramId         : 'rts_task_list'
    });
    
  })();
  
};

RtsTaskConfig.prototype.CreateArea = function()
{
  var rtsTaskConfig = this;
  
  // add event - toggle display of task info 
  $("[href^=toggle_task_info_]").bind('click', function(event)
  {
    // don't follow link
    event.preventDefault();
    rtsTaskConfig.ToggleTaskInfo($(this).attr('href').replace('toggle_task_info_', ''));
    return false;
  });
};


RtsTaskConfig.prototype.ResetArea = function()
{
  $('#rts_info_no_task_area #rts_info_no_task_info_text').html('Unknown');
  $('#rts_info_no_task_area').show();
  
  $('[id^=rts-task-table-]').hide();
};


RtsTaskConfig.prototype.Refresh = function()
{
  var rtsTaskConfig  = this;

  rtsTaskConfig.ResetArea();
  
  //deviceParams.ReadValue(rtsTaskConfig.paramView.list, function()
  deviceParams.ReadValue({ id: 'rts_task_list' }, function()
  {
    if(deviceParams.ReadErrorOccured(rtsTaskConfig.paramView.list)) 
    {
      $('body').trigger('event_errorOccured', [ 'Error while reading task information.', deviceParams.list['rts_task_list'].errorText ] );
    }
    else
    {
      rtsTaskConfig.RefreshDisplay(); 
    }
  });

};


/**
 * Refresh display of codesys tasks - WITHOUT requesting values again
 */
RtsTaskConfig.prototype.RefreshDisplay = function()
{
  var rtsTaskConfig = this;

  rtsTaskConfig.ResetArea();
  
  if(SUCCESS != deviceParams.list['rts_task_list'].status)
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading task information.', deviceParams.list['rts_task_list'].errorText ] );

    $('#rts_info_no_task_area #rts_info_no_task_info_text').html('Unknown');
    $('#rts_info_no_task_area').show();
  }
  
  else
  {
    var rtsTaskList   = deviceParams.list['rts_task_list'].value;
    
    if(!rtsTaskList.length)
    {
      $('#rts_info_no_task_area #rts_info_no_task_info_text').html('No tasks existing.');
      $('#rts_info_no_task_area').show();
    }
    
    else 
    {
      $('#rts_info_no_task_area').hide();
      
      $.each(rtsTaskList, function(rtsTaskIndex, rtsTaskInfo)
      {
        $('#task_name_'             + rtsTaskIndex).html(rtsTaskInfo.taskName);
        $('#task_id_'               + rtsTaskIndex).html(rtsTaskInfo.taskId);
        $('#task_cycle_count_'      + rtsTaskIndex).html(rtsTaskInfo.cycleCount);
        $('#task_cycletime_'        + rtsTaskIndex).html(rtsTaskInfo.cycleTime);
        $('#task_cycletime_min_'    + rtsTaskIndex).html(rtsTaskInfo.cycleTimeMin);
        $('#task_cycletime_max_'    + rtsTaskIndex).html(rtsTaskInfo.cycleTimeMax);
        $('#task_cycleletime_acc_'  + rtsTaskIndex).html(rtsTaskInfo.cycleTimeAcc);
        $('#task_status_'           + rtsTaskIndex).html(rtsTaskInfo.status);
        $('#task_mode_'             + rtsTaskIndex).html(rtsTaskInfo.mode);
        $('#task_priority_'         + rtsTaskIndex).html(rtsTaskInfo.priority);
        $('#task_interval_'         + rtsTaskIndex).html(rtsTaskInfo.interval);
        $('#task_interval_unit_'    + rtsTaskIndex).html(rtsTaskInfo.snUnit);

        $('#rts-task-table-' + rtsTaskIndex).show();
      });
    }
  }

};


//------------------------------------------------------------------------------
// Show or hide area with extended task informations, 
// display toggle image accordingly
//------------------------------------------------------------------------------
RtsTaskConfig.prototype.ToggleTaskInfo = function(taskNo)
{
  //Log("toggle task info - " + taskNo);
  var rtsTaskConfig   = this;
  var taskInfoObj     = $('#rts-task-table-' + taskNo + ' .config_form_box');
  var toggleImageObj  = $('a[href=toggle_task_info_' + taskNo + '] img');
  
  $(taskInfoObj).toggle();
  if($(taskInfoObj).is(":visible"))
  {
    $(toggleImageObj).attr({ 'src': rtsTaskConfig.closeImgUrl, 'alt': 'Hide task informations', 'title': 'Hide task information' });
  }
  else
  {
    $(toggleImageObj).attr({ 'src': rtsTaskConfig.openImgUrl, 'alt': 'Show task informations', 'title': 'Show task information' });
  }
};




//------------------------------------------------------------------------------
// contekt of page CODESYS: common rts configuration (+ project details) and task configuration
//------------------------------------------------------------------------------
function RtsInfoContent(id)
{
  var rtsInfoContent = this;
  
  rtsInfoContent.id = id || 'rts_info';
  
  var CreateRtsInfoParamView = (function()
  {
    rtsInfoContent.paramView = new DeviceParamView();
    
    rtsInfoContent.paramView.Add(
    {
      paramId         : 'codesys_version',
      outputElementId : 'rts_info_content #codesys_version'
    });
        
  })();
  
  
      
  rtsInfoContent.rtsCommonConfig  = new RtsCommonConfig;
  rtsInfoContent.rtsTaskConfig    = new RtsTaskConfig;
  
  $('#content_area').bind(rtsInfoContent.id + '_create', function(event)
  {
    //Log("Event - rts_info create")
    rtsInfoContent.rtsTaskConfig.CreateArea();
  });

  
  $('#content_area').bind(rtsInfoContent.id + '_refresh', function(event)
  {
    //Log("Event - rts_info refresh");
    rtsInfoContent.Refresh();
  });

};


RtsInfoContent.prototype.Refresh = function()
{
  var rtsInfoContent = this;

  deviceParams.ReadValueGroup(rtsInfoContent.paramView.list, function()
  {
    if(SUCCESS != deviceParams.list['codesys_version'].status)
    {
      var errorString = deviceParams.list['codesys_version'].errorText;
      $('body').trigger('event_errorOccured', [ 'Error while reading rts information.', errorString ] );
    }
    
    else
    {
      if('0' === deviceParams.list['codesys_version'].value)
      {
        $('#codesys_info_version_3_area').addClass('existing');
        $('#codesys_info_version_2_area').removeClass('existing');
        
        $('#codesys_info_version_3_area #codesys_info_version_3').html('None');
      }
      
      else if('2' === deviceParams.list['codesys_version'].value)
      {
        $('#codesys_info_version_2_area').addClass('existing');
        $('#codesys_info_version_3_area').removeClass('existing');
        
        $('#codesys_info_version_2_area #codesys_info_version_2').html('CODESYS 2');
        rtsInfoContent.rtsCommonConfig.Refresh();
        rtsInfoContent.rtsTaskConfig.Refresh();
      }
      
      else if('3' === deviceParams.list['codesys_version'].value)
      {
        $('#codesys_info_version_3_area').addClass('existing');
        $('#codesys_info_version_2_area').removeClass('existing');
        
        $('#codesys_info_version_3_area #codesys_info_version_3').html('e!RUNTIME');
      }
    }
  });
};

