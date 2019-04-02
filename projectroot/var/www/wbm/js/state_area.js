/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

//--------------------------------------------------------------------------------------
// object including whole status area - creation and (cyclic) request and display of values
//--------------------------------------------------------------------------------------
var StateArea = function()
{
  var stateAreaObj  = this;
  var ledAreaList;
  var ledAreaStatus = ERROR;
  var refreshIntervalObj;

  stateAreaObj.cycleTime    = 30000;
  stateAreaObj.ajaxTimout   = 20000;
  stateAreaObj.cycleActive  = true;
  stateAreaObj.timeoutObj;

  stateAreaObj.incrementTimeIntervall     = 1 * 1000;  // 1 second
  stateAreaObj.incrementTimeIntervallObj  = undefined;

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  /*
   * function to increment time value ongoing to "simulate" contact to device
   * currently not used, because "dithering" second value makes to much distraction
   * instead, seconds value is faded out
   */
  var IncrementTime = function()
  {
    var actualTime = $('#status_area #value_status_time_local').text();

    if(actualTime)
    {
      var aTime = actualTime.split(':');
      hours   = parseInt(aTime[0], 10);
      minutes = parseInt(aTime[1], 10);
      seconds = parseInt(aTime[2], 10);

      ++seconds;
      if(seconds > 59)
      {
        seconds = 0;
        ++minutes;

        if(minutes > 59)
        {
          minutest = 0;
          ++hours;

          if(hours > 23)
          {
            hours = 0;
          }
        }
      }

      var newHours    = '' + ((hours < 10)    ? '0' + hours : hours);
      var newMinutes  = '' + ((minutes < 10)  ? '0' + minutes : minutes);
      var newSeconds  = '' + ((seconds < 10)  ? '0' + seconds : seconds);
      var newTime     = newHours + ':' + newMinutes + ':' + newSeconds;

      $('#status_area #value_status_time_local').text(newTime);
    }
  };


  var ShowRunStopSwitchValueCallback = function(paramStatus, runStopSwitchValue, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      $('#' + outputElementId).html(runStopSwitchValue.toUpperCase());
    }
  };

  var ShowTimeCallback = function(paramStatus, timeValue, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      var outputTimeValue = timeValue.replace(/:..$/, '');
      $('#' + outputElementId).text(outputTimeValue);
    }
  };


  var CreateStateAreaParamView = (function()
  {
    stateAreaObj.paramView = new DeviceParamView();

    stateAreaObj.paramView.Add(
    {
      paramId         : 'time_local',
      defaultValue    : '-',
      outputElementId : 'status_area #value_status_time_local',
      outputFkt       : ShowTimeCallback
    });

    stateAreaObj.paramView.Add(
    {
      paramId         : 'date_local',
      defaultValue    : '-',
      outputElementId : 'status_area #value_status_date_local'
    });

    stateAreaObj.paramView.Add(
    {
      paramId         : 'run_stop_switch_value',
      defaultValue    : '-',
      outputElementId : 'status_area #value_run_stop_reset',
      outputFkt       : ShowRunStopSwitchValueCallback
    });

    stateAreaObj.paramView.Add(
    {
      paramId         : 'complete_led_states',
      outputFkt       : LedValueOutput
    });

  })();





  //--------------------------------------------------------------------------------------
  // assignment of LED state strings and images to show
  //--------------------------------------------------------------------------------------
  var aLedStateImageAssign = new Array();
  aLedStateImageAssign["OFF"]           = "led_gray_12.gif";
  aLedStateImageAssign["GRN"]           = "led_green_12.gif";
  aLedStateImageAssign["RED"]           = "led_red_12.gif";
  aLedStateImageAssign["YLW"]           = "led_yellow_12.gif";
  aLedStateImageAssign["BLINK_GRN"]     = "led_green_gray_12.gif";
  aLedStateImageAssign["BLINK_RED"]     = "led_red_gray_12.gif";
  aLedStateImageAssign["BLINK_YLW"]     = "led_yellow_gray_12.gif";
  aLedStateImageAssign["BLINK_GRN_RED"] = "led_green_red_12.gif";
  aLedStateImageAssign["BLINK_GRN_YLW"] = "led_green_yellow_12.gif";
  aLedStateImageAssign["BLINK_RED_YLW"] = "led_red_yellow_12.gif";
  aLedStateImageAssign["BLINK_OFF_GRN"] = "led_green_gray_12.gif";
  aLedStateImageAssign["BLINK_OFF_RED"] = "led_red_gray_12.gif";
  aLedStateImageAssign["BLINK_OFF_YLW"] = "led_yellow_gray_12.gif";
  aLedStateImageAssign["BLINK_RED_GRN"] = "led_red_green_12.gif";
  aLedStateImageAssign["BLINK_YLW_GRN"] = "led_yellow_green_12.gif";
  aLedStateImageAssign["BLINK_YLW_RED"] = "led_yellow_red_12.gif";

  aLedStateImageAssign["ERR_SEQ_750"]   = "led_red_gray_12.gif";
  aLedStateImageAssign["FLASH_GRN_OFF"] = "led_green_gray_12.gif";
  aLedStateImageAssign["FLASH_RED_OFF"] = "led_red_gray_12.gif";
  aLedStateImageAssign["FLASH_YLW_OFF"] = "led_yellow_gray_12.gif";
  aLedStateImageAssign["FLASH_GRN_RED"] = "led_green_red_12.gif";
  aLedStateImageAssign["FLASH_GRN_YLW"] = "led_green_yellow_12.gif";
  aLedStateImageAssign["FLASH_RED_GRN"] = "led_red_green_12.gif";
  aLedStateImageAssign["FLASH_RED_YLW"] = "led_red_yellow_12.gif";
  aLedStateImageAssign["FLASH_YLW_GRN"] = "led_yellow_green_12.gif";
  aLedStateImageAssign["FLASH_YLW_RED"] = "led_yellow_red_12.gif";
  aLedStateImageAssign["FLASH_OFF_GRN"] = "led_green_gray_12.gif";
  aLedStateImageAssign["FLASH_OFF_RED"] = "led_red_gray_12.gif";
  aLedStateImageAssign["FLASH_OFF_YLW"] = "led_yellow_gray_12.gif";

  aLedStateImageAssign["SPECIAL_BLINK"] = "led_green_red";
  aLedStateImageAssign["NOT_SET_LED"]   = "led_gray_12.gif";
  aLedStateImageAssign["LED_ERROR"]   	= "led_gray_12.gif";


  //--------------------------------------------------------------------------------------
  // ask for LED names and create LED area
  //--------------------------------------------------------------------------------------
  CreateLedArea = function(callback)
  {
    // no multiple entries for newly created area after logout
    $(ledAreaList).empty();

    //--------------------------------------------------------------------------------------
    // add one LED to DOM
    //--------------------------------------------------------------------------------------
    AddLedToDom = function(ledNo, ledName)
    {
      // IE8 needs a special class to detect every second (right placed) li entry (nth-child does not work...)
      var ieClassText = (ledNo % 2) ? ' class="right"' : '';

      var html = '';
      html += '<li' + ieClassText + '>';
      html +=   '<a class="tooltip_link" href="led_state_' + ledName + '" title="Unknown" alt="off">';
      html +=     '<img height="12" src="images/led_gray_12.gif" alt="off">';
      html +=   '</a>';
      html +=   '<span class="led_name">' + ledName + '</span>';
      html += '</li>';

      $(ledAreaList).append(html);
    }

    //--------------------------------------------------------------------------------------
    // get XML file with LED structure and insert single LEDs to DOM
    //--------------------------------------------------------------------------------------
    CreateLedAreaViaXML = function(callback)
    {
      var ledStruct = false;
      var ledName   = '';

      deviceCommunication.GetRessource(
      {
        ressource:  "led.xml",
        callback:   function(status, ledXmlFile, textStatus, ledXmlXhr)
        {
          var ledXml;
          if(SUCCESS === status)
          {
            try { ledStruct = $.xml2json(ledXmlXhr.responseText); }
            catch(e)
            {
              status = ERROR;
            }

            if(ledStruct)
            {
              // loop over all LEDs
              $.each(ledStruct.led, function(ledIndex, singleLed)
              {
                ledName = singleLed.name || '???';
                AddLedToDom(ledIndex, ledName);
              });
              status = SUCCESS;
            }
          }

          if(status != SUCCESS)
          {
            $('body').trigger('event_errorOccured', [ 'Can\'t read LED structure' ]);
          }

          callback && callback(status);
        }
      });
    }

    //--------------------------------------------------------------------------------------

    CreateLedAreaViaXML(function(status)
    {
      // error handling if we found no LED
      if(0 === $(ledAreaList).find('li').length)
      {
        $(ledAreaList).append('<li>-</li>');
      }
      callback && callback(status);
    });
  }


  //--------------------------------------------------------------------------------------
  // return fitting file name of image representing a specified led state
  //--------------------------------------------------------------------------------------
  function GetImgFileName(ledStateString)
  {
    var imgFileName = 'led_gray_12.gif';
    if(aLedStateImageAssign[ledStateString]) imgFileName = aLedStateImageAssign[ledStateString];
    return imgFileName;
  }

  //--------------------------------------------------------------------------------------
  // return object with with the relevant split parts of complete LED state string
  //--------------------------------------------------------------------------------------
  function SplitLedStateString(ledStateString)
  {
    // format example complete state string
    //RUN: RED 0 0 Wed Nov 21 14:04:11 2012: "Undefined ID: 00010017"

    var aSubStrings = ledStateString.split(' ', 4);
    var ledName     = aSubStrings[0].replace(':', '');
    var stateString = aSubStrings[1];
    var frequency1  = aSubStrings[2];
    var frequency2  = aSubStrings[3];

    var ledInfo =
    {
      ledName:      aSubStrings[0].replace(':', ''),
      stateString:  stateString,
      frequency1:   frequency1,
      frequency2:   frequency2,
      diagText:     ledStateString.replace(ledName + ': ' + stateString + ' ' + frequency1 + ' ' + frequency2 + ' ', '')
    };

    return ledInfo;
  }

  //--------------------------------------------------------------------------------------
  // display LED state with data from configtool response
  // used for processing a single config tool request for each LED
  // actually unused
  //--------------------------------------------------------------------------------------
  function DisplayLedState(response, ledName)
  {
    //Log("display state " + GetLedName());
    if(SUCCESS != response.status)
    {
      //Log("error " + response.errorText + " " + ledName);
      $("#led_area a[href=led_state_" + ledName + "]").attr('title', 'WBM - ' + response.errorText);
      $("#led_area a[href=led_state_" + ledName + "]").trigger('change');
    }
    else
    {
      // format example complete state string
      //RUN: RED 0 0 Wed Nov 21 14:04:11 2012: "Undefined ID: 00010017"

      // split complete string in its relevant single parts
      var aSubStrings = response.resultString.split(' ', 4);
      //var ledName     = aSubStrings[0].replace(':', '');
      var stateString = aSubStrings[1];
      var frequency1  = aSubStrings[2];
      var frequency2  = aSubStrings[3];
      var diagText    = response.resultString.replace(ledName + ': ' + stateString + ' ' + frequency1 + ' ' + frequency2 + ' ', '');

      // change data in DOM
      //var titleDiagText = diagText.replace(/"/g, '');
      $("#led_area a[href=led_state_" + ledName + "] img").attr('src', 'images/' + GetImgFileName(stateString));
      $("#led_area a[href=led_state_" + ledName + "]").attr('title', diagText);
      $("#led_area a[href=led_state_" + ledName + "]").trigger('change');
    }
  }


  function LedValueOutput(status, ledStateString, outputElementId)
  {
    // an error occured - set error text for all LEDs
    if(SUCCESS != status)
    {
      $("#led_area a[href^=led_state_] img").attr('src', 'images/' + GetImgFileName('NOT_SET_LED'));
      $("#led_area a[href^=led_state_]").attr('title', 'WBM - Error'); // + response.errorText);
      $("#led_area a[href^=led_state_]").trigger('change');
    }

    else
    {
      // split response in single lines - one for every LED - and process them one after the other
      var aStateStrings = ledStateString.split("\n");
      $(aStateStrings).each(function()
      {
        if(0 < this.length)
        {
          // filter single state informations from string and change data in DOM accordingly
          var ledInfo = SplitLedStateString(this);
          //ledInfo.diagText.replace(/"/g, '&quot;');
          $("#led_area a[href=led_state_" + ledInfo.ledName + "] img").attr('src', 'images/' + GetImgFileName(ledInfo.stateString));
          $("#led_area a[href=led_state_" + ledInfo.ledName + "]").attr('title', ledInfo.diagText);
          $("#led_area a[href=led_state_" + ledInfo.ledName + "]").trigger('change');
        }
      });
    }
  };


  //--------------------------------------------------------------------------------------
  // create parts of status area whose configuration must be evaluated at runtime
  //--------------------------------------------------------------------------------------
  stateAreaObj.Create = function(callback)
  {
    //default values
    $('#status_area #value_status_time_local').text("-");
    $('#status_area #value_status_date_local').text("-");
    $('#status_area #value_run_stop_reset').text("-");

    CreateLedArea(callback);
  }

  StateArea.prototype.Refresh = function(singleRequest)
  {
    var stateArea = this;

    // functionality to increment time every second, not implemented yet
    //if(stateAreaObj.incrementTimeIntervallObj)  clearInterval(stateAreaObj.incrementTimeIntervallObj);

    //Log('StateArea - ReadValueGroup');
    deviceParams.ReadValueGroup(stateArea.paramView.list, true, function()
    {
      // if cycle should be still active and read was no single request - initiate next read
      if(stateArea.cycleActive && !singleRequest)
      {
        stateAreaObj.timeoutObj = window.setTimeout(function() { stateArea.Refresh(); }, stateArea.cycleTime);
      }

      stateArea.paramView.ShowValues();
      //stateAreaObj.incrementTimeIntervallObj = setInterval(IncrementTime, stateAreaObj.incrementTimeIntervall);

      if(deviceParams.ReadErrorOccured(stateArea.paramView.list))
      {
        var errorString = deviceParams.CollectedErrorStrings(stateArea.paramView.list);
        //$('body').trigger('event_errorOccured', [ 'Error while reading controller details info.', errorString ] );
      };

    });

  };


  //--------------------------------------------------------------------------------------
  // ask and show complete data of state area cyclic
  //--------------------------------------------------------------------------------------
  stateAreaObj.StartCyclicReading = function(cycleTime)
  {
    var stateArea = this;

    //Log('StartCyclicReading');
    if(cycleTime) stateArea.cycleTime = cycleTime;

    // read data the first time after one second (to give rest of page time to be read complete)
    stateAreaObj.cycleActive = true;
    window.setTimeout(function() { stateAreaObj.Refresh(); }, 1000 );

    // change for security audit
    //stateArea.StopCyclicReading();

    // start cyclic read interval
    //refreshIntervalObj = window.setInterval(function() { stateAreaObj.Refresh(); }, stateAreaObj.cycleTime);
  };

  stateAreaObj.StopCyclicReading = function()
  {
    var stateArea = this;
    stateArea.cycleActive = false;
    window.clearTimeout(stateArea.timeoutObj);
    //window.clearInterval(refreshIntervalObj);
  };


  //--------------------------------------------------------------------------------------
  // start working - look where state area is located in DOM
  // start reading after command (not till important parts of the page are loaded...)
  //--------------------------------------------------------------------------------------
  $(document).ready(function()
  {
    ledAreaList = $("#led_area ul")
    //stateAreaObj.Create();
  });


  //------------------------------------------------------------------------------
  // refresh of area via external event
  //------------------------------------------------------------------------------
  $('#status_area').bind('event_refresh', function(event)
  {
    stateAreaObj.Refresh(true);
  });

  //------------------------------------------------------------------------------
  // stop of cyclic reading via external event
  //------------------------------------------------------------------------------
  $('#status_area').bind('event_stopCyclicReading', function(event)
  {
    stateAreaObj.StopCyclicReading();
  });

  //------------------------------------------------------------------------------
  // start of cyclic reading via external event
  //------------------------------------------------------------------------------
  $('#status_area').bind('event_startCyclicReading', function(event)
  {
    stateAreaObj.StartCyclicReading();
  });

}

/*
StateAreaObj.prototype.Refresh = function(param)
{
  // refresh whole status area or single value of it
};
*/


var stateArea = new StateArea();
