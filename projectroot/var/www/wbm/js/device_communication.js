/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var NO_TIMEOUT      = -1;
var DEFAULT_TIMEOUT = 12000;

var CONNECTION_CHECK_TIMEOUT    = 3000;
var CONNECTION_CHECK_CYCLETIME  = 5000;


//--------------------------------------------------------------------------------------
// Object DeviceCommunication based on "configtools"
// read values from device (coupler/controller) by using configtool interface via AJAX
//--------------------------------------------------------------------------------------


var ConfigtoolRequestParams = function(initParams)
{
  this.name       = initParams.name       || '';
  this.parameter  = initParams.parameter  || '';
  this.sudo       = initParams.sudo       || true;
  this.multiline  = initParams.multiline  || false;
  this.timeout    = initParams.timeout    || DEFAULT_TIMEOUT;
  this.dataId     = initParams.dataId     || 0;
};


var ConfigtoolResponseParams = function(initParams)
{
  this.status       = initParams.status       || SUCCESS;
  this.resultString = initParams.resultString || '';
  this.errorText    = initParams.errorText    || '';
  this.callString   = initParams.callString   || '';
};



var DeviceCommunication = function()
{
  var deviceCommunication = this;

  this.ressourceName      = 'configtools.php';
  this.activeRequestCount = 0;
  this.csrfToken          = false;
  deviceCommunication.doConnectionCheck       = false;
  deviceCommunication.globalConnectionOkFlag  = true;
  deviceCommunication.InitCyclicConnectionCheck();
};


DeviceCommunication.prototype.SetCSRFToken = function(csrfToken)
{
  this.csrfToken  = csrfToken;

};

DeviceCommunication.prototype.GetCSRFToken = function()
{
  return this.csrfToken;
};

//--------------------------------------------------------------------------------------
// functions for start and complete of requests
// show special gif to indicate that still ajax requests are busy
//--------------------------------------------------------------------------------------

DeviceCommunication.prototype.RequestStart = function(jqXHR)
{
  //Log("RequestStart" + configtools.activeRequestCount);
  if(0 === this.activeRequestCount)
  {
    $('body').trigger('event_deviceCommunicationStart');
  }

  this.activeRequestCount++;
};


DeviceCommunication.prototype.RequestComplete = function(jqXHR)
{
  //Log("RequestComplete " + configtools.activeRequestCount);
  this.activeRequestCount--;
  if(0 === this.activeRequestCount)
  {
    $('body').trigger('event_deviceCommunicationResting');
  }
};



DeviceCommunication.prototype.CreateErrorResponse = function(request, status, errorText)
{
  var aResponse = new Array;

  if("timeout" === errorText)
  {
    status    = COM_TIMEOUT;
    errorText = "Timeout\nHint: Application in STOP leads to a faster reaction time in WBM.";
  }

  $(request.aDeviceParams).each(function(paramNo)
  {
    aResponse[paramNo] = new ConfigtoolResponseParams({ status: status, errorText: errorText });
  });

  return aResponse;
};


/*
 * send request to device with configtool query
 */
DeviceCommunication.prototype.SendRequest = function(request)
{
  var deviceCommunication = this;

  var communicationParams =
  {
    csrfToken:      deviceCommunication.csrfToken,
    renewSession:   request.renewSession ? true : false,
    aDeviceParams:  $.extend(true, {}, request.aDeviceParams)
  }

  $.ajax(
  {
    url:        deviceCommunication.ressourceName,
    data:       JSON.stringify(communicationParams),
    timeout:    request.timeout,
    type:       "POST",
    async:      true,
    success:    function(jsonResponse)
                {
                  // parse json response into response array, check error
                  try { response = jQuery.parseJSON(jsonResponse); }
                  catch(e)
                  {
                    var response =
                    {
                      csrfToken:        0,
                      aDeviceResponse:  deviceCommunication.CreateErrorResponse(request, ERROR, 'Invalid response data format')
                    };
                  }

                  // check response for session errors
                  $.each(response.aDeviceResponse, function(responseIndex, singleResponse)
                  {
                    // session has expired
                    if(ERROR_SESSION_EXPIRED === singleResponse.status)
                    {
                      // stop cyclic reading only in case of stronger security (audit)
                      //stateArea.StopCyclicReading();

                      // show error message
                      $('body').trigger('event_errorOccured', [ 'Your session has expired!', ' Please log in again.' ]);

                      // if user ist logged in - log him out and show start page
                      if('Logout' == $("#head_area .user_management_area").find('a').text())
                      {
                        $(document).trigger('event_userManagement_logout');
                      }
                      return false;
                    }

                    // other session errors - show error message and reload website
                    else if((ERROR_INVALID_CSRF_TOKEN === singleResponse.status) || (ACCESS_NOT_ALLOWED === singleResponse.status))
                    {
                      $('body').trigger('event_errorOccured', [ 'Session Error', singleResponse.errorText ]);
                      //$(document).trigger('event_userManagement_logout');
                      window.location.reload();
                      return false;
                    }
                  });

                  if(request.processResponseFkt) request.processResponseFkt(response.aDeviceResponse);
                },

    error:      function(jqXHR, textStatus)
                {
                  //Log('Response arrived error ');
                  // set error data in response object
                  var response = deviceCommunication.CreateErrorResponse(request, ERROR, textStatus);

                  if(request.processResponseFkt) request.processResponseFkt(response);
                },

                // call functions directly, to have the right "this" context:
    beforeSend: function(jqXHR) { deviceCommunication.RequestStart(jqXHR); },
    complete:   function(jqXHR) { deviceCommunication.RequestComplete(jqXHR); }
  });
};


/*
 * send post request to device with variable data and ressource
 */
DeviceCommunication.prototype.PostRequest = function(requestParams)
{
  var deviceCommunication   = this;
  var ressource             = requestParams.ressource || '';
  var params                = requestParams.params    || '';
  var timeout               = requestParams.timeout   || DEFAULT_TIMEOUT;
  var callback              = requestParams.callback;

  $.ajax(
  {
    url:        ressource,
    data:       JSON.stringify(params),
    timeout:    timeout,
    type:       "POST",
    async:      true,
    success:    function(jsonResponse)
                {
                  //Log('Response arrived: ' + jsonResponse);
                  // parse json response into response array, check error
                  try { response = jQuery.parseJSON(jsonResponse); }
                  catch(e)
                  {
                    var response = { status: ERROR, errorText: 'Invalid response data format' };
                  }

                  if(callback) callback(response);
                },

    error:      function(jqXHR, textStatus)
                {
                  //Log('Response arrived error ');
                  // set error data in response object
                  var response = { status: ERROR, errorText: textStatus };

                  if(callback) callback(response);
                },

                // call functions directly, to have the right "this" context:
    beforeSend: function(jqXHR) { deviceCommunication.RequestStart(jqXHR); },
    complete:   function(jqXHR) { deviceCommunication.RequestComplete(jqXHR); }
  });
};


DeviceCommunication.prototype.GetRessource = function(fktParams)
{
  var deviceCommunication   = this;
  var ressource             = fktParams.ressource || '';
  var params                = fktParams.params    || '';
  var timeout               = fktParams.timeout   || DEFAULT_TIMEOUT;
  var callback              = fktParams.callback;

  $.ajax(
  {
    url:      ressource,
    data:     params,
    timeout:  timeout,
    type:     "GET",
    cache:    false,
    success:  function(response, successCode, jqXHR)
              {
                if(callback) callback(SUCCESS, response, successCode, jqXHR);
              },

    error:    function(jqXHR, textStatus)
              {
                if(callback) callback(ERROR, null, jqXHR.statusText);
              },

                // call functions directly, to have the right "this" context:
    beforeSend: function(jqXHR) { deviceCommunication.RequestStart(jqXHR); },
    complete:   function(jqXHR) { deviceCommunication.RequestComplete(jqXHR); }
  });
};


DeviceCommunication.prototype.InitCyclicConnectionCheck = function()
{
  //this.connectionCheckIntervalObj  = window.setInterval(function() { deviceCommunication.CheckCommunication(); }, CONNECTION_CHECK_CYCLETIME);
  this.CheckCommunication();
};


DeviceCommunication.prototype.StopCyclicConnectionCheck = function()
{
  deviceCommunication.doCyclicConnectionCheck = false;
};


DeviceCommunication.prototype.CheckCommunication = function()
{
  var deviceCommunication = this;

  if(deviceCommunication.doConnectionCheck)
  {
    $.ajax(
    {
      url:      '../index.html',
      timeout:  CONNECTION_CHECK_TIMEOUT,
      type:     "HEAD",
      cache:    false,
      success:  function(response)
                {
                  //Log('Commuication ok');
                  deviceCommunication.globalConnectionOkFlag = true;
                  $('body').trigger('event_deviceConnectionOk');
                  window.setTimeout(function() { deviceCommunication.CheckCommunication(); }, CONNECTION_CHECK_CYCLETIME);
                },

      error:    function(jqXHR, textStatus)
                {
                  //Log('Commuication failed');
                  deviceCommunication.globalConnectionOkFlag = false;
                  $('body').trigger('event_deviceConnectionLost');
                  window.setTimeout(function() { deviceCommunication.CheckCommunication(); }, CONNECTION_CHECK_CYCLETIME);
                }
    });
  }
};


/*
 * Check connection to device, retry cyclic, and call callback function if connection is ok or after max retries/timeout
 */
DeviceCommunication.prototype.WaitForConnection = function(timeout, callback)
{
  var deviceCommunication       = this;
  var connectionCheckCycletime  = CONNECTION_CHECK_CYCLETIME;
  var entireRetryCount          = Math.ceil(timeout / connectionCheckCycletime);
  var retryCount                = entireRetryCount;

  var SingleConnectionCheck = function()
  {
    deviceCommunication.ConnectionOk(function(connectionOk)
    {
      //Log('ConnectionCheck - retry ' + retryCount);
      if(connectionOk)
      {
        callback(SUCCESS);
      }

      else if(0 === retryCount)
      {
        callback(ERROR);
      }
      else
      {
        --retryCount;
        window.setTimeout(function() { SingleConnectionCheck() }, connectionCheckCycletime );
      }
    });
  }

  window.setTimeout(function() { SingleConnectionCheck() }, connectionCheckCycletime );
};


/*
 * Check connection to device, retry cyclic, and call callback function if connection is lost or after max retries/timeout
 */
DeviceCommunication.prototype.WaitForLostConnection = function(timeout, callback)
{
  var deviceCommunication       = this;
  var connectionCheckCycletime  = CONNECTION_CHECK_CYCLETIME;
  var entireRetryCount          = Math.ceil(timeout / connectionCheckCycletime);
  var retryCount                = entireRetryCount;

  var SingleConnectionCheck = function()
  {
    deviceCommunication.ConnectionOk(function(connectionOk)
    {
      //Log('ConnectionCheck - retry ' + retryCount);
      if(!connectionOk)
      {
        callback(SUCCESS);
      }

      else if(0 === retryCount)
      {
        callback(ERROR);
      }
      else
      {
        --retryCount;
        window.setTimeout(function() { SingleConnectionCheck() }, connectionCheckCycletime );
      }
    });
  }

  window.setTimeout(function() { SingleConnectionCheck() }, connectionCheckCycletime );
};



/*
 * Check connection to device via head request to index.html. Transfer status via callback.
 */
DeviceCommunication.prototype.ConnectionOk = function(callback)
{
  var deviceCommunication   = this;

  $.ajax(
  {
    url:      '../index.html',
    timeout:  CONNECTION_CHECK_TIMEOUT,
    type:     "HEAD",
    cache:    false,
    success:  function(response)
              {
                //Log('Commuication ok');
                if(callback) callback(true);
              },

    error:    function(jqXHR, textStatus)
              {
                //Log('Commuication failed');
                if(callback) callback(false);
              }
  });
};


var deviceCommunication = new DeviceCommunication();
