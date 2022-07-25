/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function IntegrityContent(id)
{
  var integrityContent = this;

  integrityContent.id = id || 'integrity';
  integrityContent.autoRefreshTimer     = null;
  integrityContent.autoRefreshFlag         = false;
  integrityContent.autoRefreshCycleTimeSec = -1;

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------

  var ShowIntegrityCheckState = function(status, checkState, outputElementId)
  {
    $('#' + outputElementId).removeAttr('checked');
    if('enabled' === checkState)
    {
      $('#' + outputElementId).prop('checked', 'checked');
    }
  }

  var ShowIntegrityLog = function(status, integrityLog, outputElementId)
  {
    var outputIntegrityLog = integrityLog;

    if((SUCCESS !== status) || !integrityLog || !integrityLog.length)
    {
      outputIntegrityLog = "No integrity log available.";
    }

    outputIntegrityLog = outputIntegrityLog.replace(/ /g, "&nbsp;");
    outputIntegrityLog = outputIntegrityLog.replace(/</g, "&lt;");
    outputIntegrityLog = outputIntegrityLog.replace(/>/g, "&gt;");
    outputIntegrityLog = outputIntegrityLog.replace(/"/g, "&quot;");
    outputIntegrityLog = outputIntegrityLog.replace(/\r\n/g, "\n<br>");
    outputIntegrityLog = outputIntegrityLog.replace(/\n/g, "\n<br>");

    $('div#' + outputElementId).html('<p>' + outputIntegrityLog + '</p>');
  };

  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var CreateIntegrityParamView = (function()
  {
    integrityContent.paramView = new DeviceParamView();

    integrityContent.paramView.Add(
    {
      paramId         : 'integrity_check_state',
      outputElementId : 'integrity_check_state',
      outputFkt       : ShowIntegrityCheckState
    });

    integrityContent.paramView.Add(
    {
      paramId         : 'integrity_log',
      outputElementId : 'integrity_log',
      outputFkt       : ShowIntegrityLog
    });

  })();

  var AddEvents = (function()
  {
    //------------------------------------------------------------------------------
    // first creation of area
    //------------------------------------------------------------------------------
    $('#content_area').bind(integrityContent.id + '_create', function(event)
    {
      integrityContent.Create();
    });

    //------------------------------------------------------------------------------
    // refresh of area
    //------------------------------------------------------------------------------
    $('#content_area').bind(integrityContent.id + '_refresh', function(event)
    {
      integrityContent.Refresh();
    });

    //------------------------------------------------------------------------------
    // let integrity content area "sleep" when it is not visible
    //------------------------------------------------------------------------------
    $('#content_area').bind('event_' + integrityContent.id + '_sleep', function(event)
    {
      // readlog is default
      $('#' + integrityContent.id + '_content select#action').val("readlog");
      // stop auto refresh timer if existing
      if(integrityContent.autoRefreshFlag)
      {
        window.clearTimeout(integrityContent.autoRefreshTimer);
        integrityContent.autoRefreshTimer     = null;
        integrityContent.autoRefreshFlag         = false;
        integrityContent.autoRefreshCycleTimeSec = -1;
      }
    });

  })();
};

IntegrityContent.prototype.Create = function()
{
  var integrityContent = this;
  var defaultAutoRefreshFlag          = false;
  var defaultAutoRefreshCycleTimeSec  = 20;
  var defaultLimitFlag                = true;
  var defaultLimitCount               = 80;
  var defaultAction                   = "readlog";

  //------------------------------------------------------------------------------
  // Integrity check at startup
  //------------------------------------------------------------------------------
  $('#' + integrityContent.id + '_content input#integrity_check_state').removeAttr('checked');

  $('#' + integrityContent.id + '_content #integrity_check_config_form').bind('submit', function(event)
  {
    event.preventDefault();
    integrityContent.ProcessIntegrityCheckStateSubmit(this);
  });

  //------------------------------------------------------------------------------
  // Integrity log and execute commands
  //------------------------------------------------------------------------------
  $('#' + integrityContent.id + '_content input#count').val(defaultLimitCount);
  $('#' + integrityContent.id + '_content input#automatic_refresh_cycle_time').val(defaultAutoRefreshCycleTimeSec);
  $('#' + integrityContent.id + '_content select#action').val(defaultAction);

  if(defaultLimitFlag)
  {
    $('#' + integrityContent.id + '_content #limit_count').prop('checked', 'checked');
  }
  else
  {
    $('#' + integrityContent.id + '_content #limit_none').prop('checked', 'checked');
  }

  if(defaultAutoRefreshFlag)
  {
    $('#' + integrityContent.id + '_content #automatic_refresh_flag').prop('checked', 'checked');
  }

  integrityContent.SetLimitSettingsDependencies();
  integrityContent.SetAutoRefreshSettingsDependencies();

  $('#' + integrityContent.id + '_content #limit_count, #' + integrityContent.id + '_content #limit_none').change(function(event)
  {
    integrityContent.SetLimitSettingsDependencies();
  });

  $('#' + integrityContent.id + '_content #automatic_refresh_flag').change(function(event)
  {
    integrityContent.SetAutoRefreshSettingsDependencies();

    // initiate to interrupt timer, if necessary
    if(integrityContent.autoRefreshTimer && ('checked' != $(this).prop('checked')))
    {
      $('#' + integrityContent.id + '_content #integrity_log_form').trigger('submit');
    }
  });

  // events for form processing
  $('#' + integrityContent.id + '_content #integrity_log_form').bind('submit', function(event)
  {
    event.preventDefault();
    integrityContent.ProcessRefreshIntegrityLogSubmit(this);
  });

};

IntegrityContent.prototype.ProcessIntegrityCheckStateSubmit = function(formObj)
{
  var integrityContent = this;
  
  deviceParams.ReadValue('integrity_check_state', true, function()
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != deviceParams.list['integrity_check_state'].status)
    {
      var errorText = deviceParams.list['integrity_check_state'].errorText;
      $('body').trigger('event_errorOccured', [ 'Error while reading integrity check state.', errorText ]);
    }
    else
    {
      integrityContent.paramView.ShowValues();
    }
  });
  
  $('#' + integrityContent.id + '_content #integrity_log_form').trigger('submit');
}

IntegrityContent.prototype.Refresh = function(event)
{
  var integrityContent = this;
  
  deviceParams.ReadValue({id: 'integrity_check_state'}, true, function()
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != deviceParams.list['integrity_check_state'].status)
    {
      var errorText = deviceParams.list['integrity_check_state'].errorText;
      $('body').trigger('event_errorOccured', [ 'Error while reading integrity check state.', errorText ]);
    }
    else
    {
      integrityContent.paramView.ShowValues();
    }
  });
  
  $('#' + integrityContent.id + '_content #integrity_log_form').trigger('submit');
}

IntegrityContent.prototype.ProcessIntegrityCheckStateSubmit = function(form)
{
  var integrityContent = this;
  var checkState = $(form).find('#integrity_check_state:checked').length ? true : false;

  integrityContent.SetIntegrityCheckState(checkState)
}

IntegrityContent.prototype.SetIntegrityCheckState = function(checkState)
{
  var integrityContent = this;

  pageElements.ShowBusyScreen("Changing integrity check state...");

  deviceParams.ChangeValue('integrity_check_state', { integrity_check_state: (checkState) ? 'enable' : 'disable' }, function()
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != deviceParams.list['integrity_check_state'].status)
    {
      var errorText = deviceParams.list['integrity_check_state'].errorText;
      $('body').trigger('event_errorOccured', [ 'Error while changing integrity check state.', errorText ]);
    }
  });
}

//------------------------------------------------------------------------------
// set fields in view enabled/disabled depending on limit settings
//------------------------------------------------------------------------------
IntegrityContent.prototype.SetLimitSettingsDependencies = function()
{
  var integrityContent = this;

  if($("#limit_count:checked").length)
  {
    $('#' + integrityContent.id + '_content input#count').removeAttr('disabled');
  }
  else if($("#limit_none:checked").length)
  {
    $('#' + integrityContent.id + '_content input#count').attr('disabled', true);
  }
};

//------------------------------------------------------------------------------
// set fields in view enabled/disabled depending on automatic refresh settings
//------------------------------------------------------------------------------
IntegrityContent.prototype.SetAutoRefreshSettingsDependencies = function()
{
  var integrityContent = this;

  if($("#automatic_refresh_flag:checked").length)
  {
    $('#' + integrityContent.id + '_content input#automatic_refresh_cycle_time').removeAttr('disabled');
    $('#' + integrityContent.id + '_content input#refresh').val('Start');
  }
  else
  {
    $('#' + integrityContent.id + '_content input#automatic_refresh_cycle_time').attr('disabled', true);
    $('#' + integrityContent.id + '_content input#refresh').val('Refresh');
  }
};

IntegrityContent.prototype.ProcessRefreshIntegrityLogSubmit = function(form)
{
  var integrityContent = this;

  // if cyclic timer is startet, interrupt 
  if(integrityContent.autoRefreshFlag)
  {
    window.clearTimeout(integrityContent.autoRefreshTimer);
    integrityContent.autoRefreshTimer     = null;
    integrityContent.autoRefreshFlag         = false;
    integrityContent.autoRefreshCycleTimeSec = -1;
    $(form).find('.button[name=refresh]').val('Refresh');
  }
  // now cyclic timer by now - start it
  else
  {
    var limitFlag               = $(form).find('#limit_count:checked').length ? true : false;
    var limitCount              = $(form).find('input#count').val();
    var autoRefreshFlag         = $(form).find('#automatic_refresh_flag:checked').length ? true : false;
    var autoRefreshCycleTimeSec = $(form).find('#automatic_refresh_cycle_time').val();
    var action                  = $('#integrity_log_form select[name=action]').val();

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
        integrityContent.autoRefreshFlag         = true;
        integrityContent.autoRefreshCycleTimeSec = autoRefreshCycleTimeSec;
        $(form).find('.button[name=refresh]').val('Stop');
      }

      integrityContent.RefreshIntegrityLog(limitFlag, limitCount, action);
    }
  }
};

IntegrityContent.prototype.RefreshIntegrityLog = function(limitFlag, limitCount, action)
{
  var integrityContent = this;

  if(!limitFlag)
  {
    limitCount = '';
  }
  
  if(limitCount && (NaN == parseInt(limitCount, 10)))
  {
    $('body').trigger('event_errorOccured', [ 'Invalid input value for limit count (only numeric values are allowed).' ] );
  }
  else
  {
    pageElements.ShowBusyScreen("Reading integrity information...");
    deviceParams.ReadValue({ id: 'integrity_log', variables: { integrity_count: limitCount, integrity_action: action } }, function()
    {
      pageElements.RemoveBusyScreen();
      if(SUCCESS != deviceParams.list['integrity_log'].status)
      {
        var errorText = deviceParams.list['integrity_log'].errorText;
        $('body').trigger('event_errorOccured', [ 'Error while changing integrity information.', errorText ]);
      }
      else
      {
        integrityContent.paramView.ShowSingleValue(1);
        if(integrityContent.autoRefreshFlag)
        {
          integrityContent.autoRefreshTimer = window.setTimeout(function()
          {
            integrityContent.RefreshIntegrityLog(limitFlag, limitCount, action)
          },
            integrityContent.autoRefreshCycleTimeSec * 1000);
        }
      }
    });
  }
};

