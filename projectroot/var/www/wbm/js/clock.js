/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

/*
 * functions for read and write data for WBM content "clock"
 */

function ClockContent(id)
{
  var clockContent = this;

  clockContent.id = id || 'clock';

  /*
   * local function for display of special values
   */
  var DisplayTimeFormat = function(paramStatus, paramValue, outputElementId)
  {
    $(outputElementId).removeAttr('checked');

    if((SUCCESS === paramStatus) && ('12-hour-format' === paramValue))
    {
      $('#' + outputElementId).prop('checked', true);
    }
  };

  var DisplayTimezone = function(paramStatus, paramValue, outputElementId)
  {
    // remove possible "unknown" entries
    $('[name=timezone] option[value=unknown]').remove();

    // if timezone string is existing in selection menu, show according line selected
    if((SUCCESS === paramStatus) && $('[name=timezone] option[value="' + paramValue + '"]').length)
    {
      $('[name=timezone] option[value="' + paramValue + '"]').prop('selected', 'selected');
      // this is not working with jQuery 1.7 in IE $('[name=timezone]').val(paramValue);
    }

    // not existing - create "unknown" line
    else
    {
      $('[name=timezone]').append('<option selected value="unknown" >Unknown</option>');
      $('[name=timezone]').val('unknown');
    }
  };

  var CreateTimezoneDropdown = function(paramStatus, paramValue, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      $('#' + outputElementId).find('option').remove();

      $.each(paramValue, function(i, item)
      {
        $('#' + outputElementId).append($('<option>', {
          value: item,
          text: item
        }));
      });
    }
  };


  /*
   * definitions for display of parameters
   */
  var CreateClockParamView = (function()
  {
    clockContent.paramView = new DeviceParamView();
    clockContent.paramView.Add(
    {
      paramId         : 'date_local',
      outputElementId : clockContent.id + '_content #date_local'
    });

    clockContent.paramView.Add(
    {
      paramId         : 'time_local',
      outputElementId : clockContent.id + '_content #time_local'
    });


    clockContent.paramView.Add(
    {
      paramId         : 'time_utc',
      outputElementId : clockContent.id + '_content #time_utc'
    });

    clockContent.paramView.Add(
    {
      paramId         : 'clock_display_mode',
      outputElementId : clockContent.id + '_content #display_mode',
      outputFkt       : DisplayTimeFormat
    });

    clockContent.paramView.Add(
    {
      paramId         : 'tz_string',
      outputElementId : clockContent.id + '_content #tz_string'
    });

    clockContent.paramView.Add(
    {
      //paramId         : 'all-timezone-strings',
      paramId         : 'timezone-list',
      outputElementId : clockContent.id + '_content #timezone',
      outputFkt       : CreateTimezoneDropdown
    });

    clockContent.paramView.Add(
    {
      paramId         : 'timezone',
      outputElementId : clockContent.id + '_content #timezone',
      outputFkt       : DisplayTimezone
    });



  })();

  /*
   * event - first creation of clock content area
   */
  $('#content_area').bind(clockContent.id + '_create', function(event)
  {
    //Log("Event - clock create");

    // actually implemented directly in html, because foreseeable, no changes will be made here
    //CreateTimezoneSelection();

    // add events for form processing
    $("#date_local_form").bind('submit', function(event)
    {
      event.preventDefault();
      clockContent.ChangeDate();
    });

    $("#time_local_form").bind('submit', function(event)
    {
      event.preventDefault();
      clockContent.ChangeTimeLocal();
    });

    $("#time_utc_form").bind('submit', function(event)
    {
      event.preventDefault();
      clockContent.ChangeTimeUtc();
    });

    $("#display_mode_form").bind('submit', function(event)
    {
      event.preventDefault();
      clockContent.ChangeDisplayMode();
    });

    $("#timezone_form").bind('submit', function(event)
    {
      event.preventDefault();
      clockContent.ChangeTimezone();
    });

    $("#tz_string_form").bind('submit', function(event)
    {
      event.preventDefault();
      clockContent.ChangeTzString();
    });

  });


  /*
   * event - refresh of clock content area
   */
  $('#content_area').bind(clockContent.id + '_refresh', function(event)
  {
    //Log("Event - clock refresh");
    clockContent.Refresh();
  });

};


/*
 * read and show all parameters necessary for whole clock content
 * because all clock paramters are addicted to each other, it is necessary to read them all
 * even if only one has changed
 */
ClockContent.prototype.Refresh = function()
{
  var clockContent = this;

  deviceParams.ReadValueGroup(clockContent.paramView.list, function()
  {
    clockContent.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(clockContent.paramView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(clockContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading clock data.', errorString ] );
    };
  });
};


/*
 * functions for changing clock data
 */
ClockContent.prototype.ChangeDate = function()
{
  var clockContent = this;

  pageElements.ShowBusyScreen("Changing date...");

  var newValueList = { date: $('input[name=date_local]').val() };

  deviceParams.ChangeValue('date_local', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing date.', errorText ]);
    }

    setTimeout(function() { clockContent.Refresh(); }, 1000);
    setTimeout(function() { $('#status_area').trigger('event_refresh'); }, 1000);
  });
};

ClockContent.prototype.ChangeTimeLocal = function()
{
  var clockContent = this;

  pageElements.ShowBusyScreen("Changing time...");

  var newValueList = { time: $('input[name=time_local]').val() };

  deviceParams.ChangeValue('time_local', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing time.', errorText ]);
    }

    setTimeout(function() { clockContent.Refresh(); }, 1000);
    setTimeout(function() { $('#status_area').trigger('event_refresh'); }, 1000);
  });
};


ClockContent.prototype.ChangeTimeUtc = function()
{
  var clockContent = this;

  pageElements.ShowBusyScreen("Changing time...");

  var newValueList = { time: $("input[name=time_utc]").val() };

  deviceParams.ChangeValue('time_utc', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing time.', errorText ]);
    }

    setTimeout(function() { clockContent.Refresh(); }, 1000);
    setTimeout(function() { $('#status_area').trigger('event_refresh'); }, 1000);
  });
};

ClockContent.prototype.ChangeDisplayMode = function()
{
  var clockContent = this;

  pageElements.ShowBusyScreen("Changing display mode...");

  var newValueList = { displayMode: '24-hour-format' };
  if(('12-hour-format' === $("input[name=display_mode]").val()) && $("input[name=display_mode]").prop("checked") )
  {
    newValueList = { displayMode: '12-hour-format' };
  }

  deviceParams.ChangeValue('clock_display_mode', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing display mode.', errorText ]);
    }

    setTimeout(function() { clockContent.Refresh(); }, 1000);
  });
};


ClockContent.prototype.ChangeTimezone = function()
{
  var clockContent = this;

  pageElements.ShowBusyScreen("Changing timezone...");

  var newValueList = { timezone: $('[name=timezone]').val() };

  deviceParams.ChangeValue('timezone', newValueList, function(status, errorText)
  {
     pageElements.RemoveBusyScreen();
     if(SUCCESS != status)
     {
       $('body').trigger('event_errorOccured', [ 'Error while changing timezone.', errorText ]);
     }

     setTimeout(function() { clockContent.Refresh(); }, 1000);
     setTimeout(function() { $('#status_area').trigger('event_refresh'); }, 1000);
   });
};


ClockContent.prototype.ChangeTzString = function()
{
  var clockContent = this;

  pageElements.ShowBusyScreen("Changing TZ string...");

  var newValueList = { tzString: $('[name=tz_string]').val() };

  deviceParams.ChangeValue('tz_string', newValueList, function(status, errorText)
  {
     pageElements.RemoveBusyScreen();
     if(SUCCESS != status)
     {
       $('body').trigger('event_errorOccured', [ 'Error while changing TZ string.', errorText ]);
     }

     setTimeout(function() { clockContent.Refresh(); }, 1000);
     setTimeout(function() { $('#status_area').trigger('event_refresh'); }, 1000);
   });
};
