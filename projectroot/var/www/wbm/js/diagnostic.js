/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function DiagnosticContent(id)
{
  var diagnosticContent = this;

  diagnosticContent.id = id || 'diagnostic';

  diagnosticContent.autoRefreshTimerObj     = null;
  diagnosticContent.autoRefreshFlag         = false;
  diagnosticContent.autoRefreshCycleTimeSec = -1;

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
  
  
  var CreateDiagnosticParamView = (function()
  {
    diagnosticContent.paramView = new DeviceParamView();
            
    diagnosticContent.paramView.Add(
    {
      paramId         : 'diaglist',
      outputElementId : 'diag_list',
      outputFkt       : ShowDiagList
    });
    
  })();

  var AddEvents = (function()
  {
    // fist creation of diagnostic content
    $('#content_area').bind(diagnosticContent.id + '_create', function(event)
    {
      diagnosticContent.Create();
    });
    
    // refresh of diagnostic content area
    $('#content_area').bind(diagnosticContent.id + '_refresh', function(event)
    {
      //Log("Event - diagnostic refresh");
      $('#' + diagnosticContent.id + '_content #refresh_diag_list').trigger('submit');
    });

    // let diagnostic content area "sleep" when it is not visible
    $('#content_area').bind('event_' + diagnosticContent.id + '_sleep', function(event)
    {
      //Log("Event - diagnostic sleep");

      // stop auto refresh timer if existing
      if(diagnosticContent.autoRefreshFlag)
      {
        window.clearTimeout(diagnosticContent.autoRefreshTimerObj);
        diagnosticContent.autoRefreshTimerObj     = null;
        diagnosticContent.autoRefreshFlag         = false;
        diagnosticContent.autoRefreshCycleTimeSec = -1;
      }
    });
    
  })();

};


/*
 * set according fields in view enabled/disabled depending on limit settings
 */
DiagnosticContent.prototype.SetLimitSettingsDependencies = function()
{
  var diagnosticContent = this;

  if($("#limit_count:checked").length)
  {
    $('#' + diagnosticContent.id + '_content input#count').removeAttr('disabled');
  }

  else if($("#limit_none:checked").length)
  {
    $('#' + diagnosticContent.id + '_content input#count').attr('disabled', true);
  }
};


/*
 * set according fields in view enabled/disabled depending on automatic refresh settings
 */
DiagnosticContent.prototype.SetAutoRefreshSettingsDependencies = function()
{
  var diagnosticContent = this;
  
  if($("#automatic_refresh_flag:checked").length)
  {
    $('#' + diagnosticContent.id + '_content input#automatic_refresh_cycle_time').removeAttr('disabled');
    $('#' + diagnosticContent.id + '_content input[type=submit]').val('Start');
  }
  else
  {
    $('#' + diagnosticContent.id + '_content input#automatic_refresh_cycle_time').attr('disabled', true);
    $('#' + diagnosticContent.id + '_content input[type=submit]').val('Refresh');
  }
};



/*
 * first creation of diagnostic content area
 */
DiagnosticContent.prototype.Create = function()
{
  var diagnosticContent = this;
  
  //Log("Event - diagnostic create");
  var defaultAutoRefreshFlag          = false;
  var defaultAutoRefreshCycleTimeSec  = 20;
  var defaultLimitFlag                = true;
  var defaultLimitCount               = 20;
  
  // show object default values in input fields
  $('#' + diagnosticContent.id + '_content input#count').val(defaultLimitCount);
  $('#' + diagnosticContent.id + '_content input#automatic_refresh_cycle_time').val(defaultAutoRefreshCycleTimeSec);
    
  if(defaultLimitFlag)
  {
    $('#' + diagnosticContent.id + '_content #limit_count').prop('checked', 'checked');
  }
  else
  {
    $('#' + diagnosticContent.id + '_content #limit_none').prop('checked', 'checked');
  }

  if(defaultAutoRefreshFlag)
  {
    $('#' + diagnosticContent.id + '_content #automatic_refresh_flag').prop('checked', 'checked');
  }

  diagnosticContent.SetLimitSettingsDependencies();
  diagnosticContent.SetAutoRefreshSettingsDependencies();
    
  // events for click on specified view elements
  $('#' + diagnosticContent.id + '_content #limit_count, #' + diagnosticContent.id + '_content #limit_none').change(function(event)
  {
    diagnosticContent.SetLimitSettingsDependencies();
  });
    
  $('#' + diagnosticContent.id + '_content #automatic_refresh_flag').change(function(event)
  {
    diagnosticContent.SetAutoRefreshSettingsDependencies();
      
    // initiate to interrupt timer, if necessary
    if(diagnosticContent.autoRefreshTimerObj && ('checked' != $(this).prop('checked')))
    {
      $('#' + diagnosticContent.id + '_content #refresh_diag_list').trigger('submit');
    }
  });
    
  // events for form processing
  $('#' + diagnosticContent.id + '_content #refresh_diag_list').bind('submit', function(event)
  {
    event.preventDefault();
    diagnosticContent.ProcessRefreshSubmit(this);
  });
  
};


DiagnosticContent.prototype.RefreshDiagList = function(limitFlag, limitCount)
{
  //Log('Refresh diaglist');
  var diagnosticContent = this;

  if(!limitFlag) limitCount = '';

  if(limitCount && (NaN == parseInt(limitCount, 10)))
  {
    $('body').trigger('event_errorOccured', [ 'Wrong input value for limit count (only integer values).' ] );
  }
  else
  {
    pageElements.ShowBusyScreen("Reading diagnostic information...");

    deviceParams.ReadValue({ id: 'diaglist', variables: { diag_count: limitCount } }, function()
    {
      pageElements.RemoveBusyScreen();

      if(SUCCESS != deviceParams.list['diaglist'].status)
      {
        var errorText = deviceParams.list['diaglist'].errorText;
        $('body').trigger('event_errorOccured', [ 'Error while changing diagnostic information.', errorText ]);
      }
      
      diagnosticContent.paramView.ShowValues();
      
      if(diagnosticContent.autoRefreshFlag)
      {
        diagnosticContent.autoRefreshTimerObj = window.setTimeout(function() { diagnosticContent.RefreshDiagList(limitFlag, limitCount) }, 
          diagnosticContent.autoRefreshCycleTimeSec * 1000);
      }
    });
  }

};
  

DiagnosticContent.prototype.ProcessRefreshSubmit = function(formObj)
{
  var diagnosticContent = this;
  
  // if cyclic timer is startet, interrupt 
  if(diagnosticContent.autoRefreshFlag)
  {
    window.clearTimeout(diagnosticContent.autoRefreshTimerObj);
    diagnosticContent.autoRefreshTimerObj     = null;
    diagnosticContent.autoRefreshFlag         = false;
    diagnosticContent.autoRefreshCycleTimeSec = -1;

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
        diagnosticContent.autoRefreshFlag         = true;
        diagnosticContent.autoRefreshCycleTimeSec = autoRefreshCycleTimeSec;
        $(formObj).find('.button[name=refresh]').val('Stop');
      }
      
      diagnosticContent.RefreshDiagList(limitFlag, limitCount);
    }
  }
  
};

