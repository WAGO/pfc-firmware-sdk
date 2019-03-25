/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function BacnetDiagnosticContent(id)
{
  var bacDiagContent = this;

  bacDiagContent.id = id || 'bacnet_diagnostic';

  bacDiagContent.autoRefreshTimerObj     = null;
  bacDiagContent.autoRefreshFlag         = false;
  bacDiagContent.autoRefreshCycleTimeSec = -1;

  var ShowDiagList = function(status, diagList, outputElementId)
  {
    var outputDiagList = diagList;

    if((SUCCESS !== status) || !diagList || !diagList.length) 
    {
      outputDiagList = "No diagnostic information available.";
    }
    
    outputDiagList = outputDiagList.replace(/ /g, "&nbsp;");
    outputDiagList = outputDiagList.replace(/</g, "&lt;");
    outputDiagList = outputDiagList.replace(/>/g, "&gt;");
    outputDiagList = outputDiagList.replace(/"/g, "&quot;");
    outputDiagList = outputDiagList.replace(/\r\n/g, "\n<br>");
    outputDiagList = outputDiagList.replace(/\n/g, "\n<br>");
  
    $('div#' + outputElementId).html('<p>'+ outputDiagList + '</p>');
  };
  
  
  var CreateBacnetDiagnosticParamView = (function()
  {
    bacDiagContent.paramView = new DeviceParamView();
            
    bacDiagContent.paramView.Add(
    {
      paramId         : 'bacnet_diaglist',
      outputElementId : 'bacnet_diag_list',
      outputFkt       : ShowDiagList
    });
    
  })();

  var AddBacnetDiagnosticEvents = (function()
  {
    // fist creation of diagnostic content
    $('#content_area').bind(bacDiagContent.id + '_create', function(event)
    {
      bacDiagContent.Create();
    });
    
    // refresh of diagnostic content area
    $('#content_area').bind(bacDiagContent.id + '_refresh', function(event)
    {
      //Log("Event - diagnostic refresh");
      $('#' + bacDiagContent.id + '_content #refresh_bacnet_diag_list').trigger('submit');
    });

    // let diagnostic content area "sleep" when it is not visible
    $('#content_area').bind('event_' + bacDiagContent.id + '_sleep', function(event)
    {
      //Log("Event - diagnostic sleep");

      // stop auto refresh timer if existing
      if(bacDiagContent.autoRefreshFlag)
      {
        window.clearTimeout(bacDiagContent.autoRefreshTimerObj);
        bacDiagContent.autoRefreshTimerObj     = null;
        bacDiagContent.autoRefreshFlag         = false;
        bacDiagContent.autoRefreshCycleTimeSec = -1;
      }
    });
    
  })();

};


/*
 * set according fields in view enabled/disabled depending on limit settings
 */
BacnetDiagnosticContent.prototype.SetLimitSettingsDependencies = function()
{
  var bacDiagContent = this;

  if($("#limit_count:checked").length)
  {
    $('#' + bacDiagContent.id + '_content input#count').removeAttr('disabled');
  }

  else if($("#limit_none:checked").length)
  {
    $('#' + bacDiagContent.id + '_content input#count').attr('disabled', true);
  }
};


/*
 * set according fields in view enabled/disabled depending on automatic refresh settings
 */
BacnetDiagnosticContent.prototype.SetAutoRefreshSettingsDependencies = function()
{
  var bacDiagContent = this;
  
  if($("#automatic_refresh_flag:checked").length)
  {
    $('#' + bacDiagContent.id + '_content input#automatic_refresh_cycle_time').removeAttr('disabled');
    $('#' + bacDiagContent.id + '_content input[type=submit]').val('Start');
  }
  else
  {
    $('#' + bacDiagContent.id + '_content input#automatic_refresh_cycle_time').attr('disabled', true);
    $('#' + bacDiagContent.id + '_content input[type=submit]').val('Refresh');
  }
};



/*
 * first creation of diagnostic content area
 */
BacnetDiagnosticContent.prototype.Create = function()
{
  var bacDiagContent = this;
  
  //Log("Event - diagnostic create");
  var defaultAutoRefreshFlag          = false;
  var defaultAutoRefreshCycleTimeSec  = 20;
  var defaultLimitFlag                = true;
  var defaultLimitCount               = 20;
  
  // show object default values in input fields
  $('#' + bacDiagContent.id + '_content input#count').val(defaultLimitCount);
  $('#' + bacDiagContent.id + '_content input#automatic_refresh_cycle_time').val(defaultAutoRefreshCycleTimeSec);
    
  if(defaultLimitFlag)
  {
    $('#' + bacDiagContent.id + '_content #limit_count').prop('checked', 'checked');
  }
  else
  {
    $('#' + bacDiagContent.id + '_content #limit_none').prop('checked', 'checked');
  }

  if(defaultAutoRefreshFlag)
  {
    $('#' + bacDiagContent.id + '_content #automatic_refresh_flag').prop('checked', 'checked');
  }

  bacDiagContent.SetLimitSettingsDependencies();
  bacDiagContent.SetAutoRefreshSettingsDependencies();
    
  // events for click on specified view elements
  $('#' + bacDiagContent.id + '_content #limit_count, #' + bacDiagContent.id + '_content #limit_none').change(function(event)
  {
    bacDiagContent.SetLimitSettingsDependencies();
  });
    
  $('#' + bacDiagContent.id + '_content #automatic_refresh_flag').change(function(event)
  {
    bacDiagContent.SetAutoRefreshSettingsDependencies();
      
    // initiate to interrupt timer, if necessary
    if(bacDiagContent.autoRefreshTimerObj && ('checked' != $(this).prop('checked')))
    {
      $('#' + bacDiagContent.id + '_content #refresh_bacnet_diag_list').trigger('submit');
    }
  });
    
  // events for form processing
  $('#' + bacDiagContent.id + '_content #refresh_bacnet_diag_list').bind('submit', function(event)
  {
    event.preventDefault();
    bacDiagContent.ProcessRefreshSubmit(this);
  });
  
};


BacnetDiagnosticContent.prototype.RefreshDiagList = function(limitFlag, limitCount)
{
  //Log('Refresh bacnet_diaglist');
  var bacDiagContent = this;

  if(!limitFlag) limitCount = '';

  if(limitCount && (NaN == parseInt(limitCount, 10)))
  {
    $('body').trigger('event_errorOccured', [ 'Wrong input value for limit count (only integer values).' ] );
  }
  else
  {
    pageElements.ShowBusyScreen("Reading diagnostic information...");

    deviceParams.ReadValue({ id: 'bacnet_diaglist', variables: { diag_count: limitCount } }, function()
    {
      pageElements.RemoveBusyScreen();

      if(SUCCESS != deviceParams.list['bacnet_diaglist'].status)
      {
        var errorText = deviceParams.list['bacnet_diaglist'].errorText;
        $('body').trigger('event_errorOccured', [ 'Error while changing diagnostic information.', errorText ]);
      }
      
      bacDiagContent.paramView.ShowValues();
      
      if(bacDiagContent.autoRefreshFlag)
      {
        bacDiagContent.autoRefreshTimerObj = window.setTimeout(function() { bacDiagContent.RefreshDiagList(limitFlag, limitCount) }, 
          bacDiagContent.autoRefreshCycleTimeSec * 1000);
      }
    });
  }

};
  

BacnetDiagnosticContent.prototype.ProcessRefreshSubmit = function(formObj)
{
  var bacDiagContent = this;
  
  // if cyclic timer is startet, interrupt 
  if(bacDiagContent.autoRefreshFlag)
  {
    window.clearTimeout(bacDiagContent.autoRefreshTimerObj);
    bacDiagContent.autoRefreshTimerObj     = null;
    bacDiagContent.autoRefreshFlag         = false;
    bacDiagContent.autoRefreshCycleTimeSec = -1;

    $(formObj).find('.button[name=refresh]').val('Refresh');
  }
  
  // now cyclic timer by now - start it
  else
  {
    // read form values
    var limitFlag               = $(formObj).find('#limit_count:checked').length ? true : false;
    var limitCount              = $(formObj).find('input#count').val();
    var autoRefreshFlag         = $(formObj).find('#automatic_refresh_flag:checked').length ? true : false;
    var autoRefreshCycleTimeSec = $(formObj).find('#automatic_refresh_cycle_time').val();
      
    // check input parameter
    if(limitCount && !limitCount.match(/^[0-9]+$/))
    {
      $('body').trigger('event_errorOccured', [ 'Invalid input for notification count (only integer values allowed).' ] );
    }
    
    else if(autoRefreshFlag  && !autoRefreshCycleTimeSec.match(/^[1-9]+[0-9]*$/))
    {
      $('body').trigger('event_errorOccured', [ 'Invalid input for refresh cycle (only positive integer values allowed).' ] );
    }
    else
    {
      if(autoRefreshFlag)
      {
        bacDiagContent.autoRefreshFlag         = true;
        bacDiagContent.autoRefreshCycleTimeSec = autoRefreshCycleTimeSec;
        $(formObj).find('.button[name=refresh]').val('Stop');
      }
      
      bacDiagContent.RefreshDiagList(limitFlag, limitCount);
    }
  }
  
};

