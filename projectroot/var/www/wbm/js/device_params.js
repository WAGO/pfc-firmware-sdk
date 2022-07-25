/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

/*------------------------------------------------------------------------------
 * Object DeviceParam / DeviceParams
 *
 * manage the list of all device(= coupler/controller) parameters
 * process read requests to get or change parameter values
 * and provide them for further processing, display in DOM, etc.
 *------------------------------------------------------------------------------
 */

var DEVICE_PARAMS_ERROR_ACCESS_NOT_ALLOWED = -2;


/*
 * initiate one single device parameter
 *
 * input: id: determined by user, must be unique
 *        exampleValue: optional, can be used for mocking
 *        configtoolReadParams: optional, specifications to read value by configtool
 *        configtoolWriteParams: optional, specifications to write value by configtool
 *
 * after read request, members value, status and errorText will be filled with response data from device
 */
function DeviceParam(initParams)
{
  var deviceParam = this;

  this.id               = initParams.id           || '';
  this.exampleValue     = initParams.exampleValue || '';
  this.invariant        = initParams.invariant    || false;
  this.format           = initParams.format       || 'text';
  this.convertFkt       = initParams.convertFkt   || false;

  this.value            = initParams.value        || '';
  this.initialized      = initParams.initialized  || false;
  this.status           = initParams.status       || SUCCESS;
  this.errorText        = initParams.errorText    || '';

  if(initParams.variables)
  {
    this.variables  = new Object(initParams.variables);
  }

  if(initParams.configtoolReadParams)
  {
    this.configtoolReadParams = new ConfigtoolRequestParams(initParams.configtoolReadParams);
  };

  if(initParams.configtoolWriteParams)
  {
    this.configtoolWriteParams = new ConfigtoolRequestParams(initParams.configtoolWriteParams);
  }
};


/*
 * list of all device parameters
 */
function DeviceParams()
{
  var deviceParams = this;

  deviceParams.list = new Object;
};


/*
 * add device params to list
 *
 * If parameter is already existing, it will not be added. In best case, this should not happen - every parameter
 * should be added by an object which is "responsible" for it. Other objects can read and use the parameter also if
 * they new its ID
 */
DeviceParams.prototype.Add = function(initParams)
{
  var deviceParams = this;
  //Log('DeviceParams.Add - ' + initParams.id);

  if(deviceParams.list[initParams.id])
  {
    Log('DeviceParams Software Warning: multible use of parameter: ' + initParams.id);
  }

  else
  {
    var newParam = new DeviceParam(initParams);
    deviceParams.list[initParams.id] = newParam;

    if(newParam.configtoolWriteParams && newParam.invariant)
    {
      alert('DeviceParams Software Error: write instructions defined for invariant parameter ' + newParam.id);
    }
  }
};


DeviceParams.prototype.Delete = function(paramId)
{
  if(deviceParams.list[paramId])
  {
    delete deviceParams.list[paramId];
  }
};


/*
 * Read a group of parameter values from device
 *
 * input: paramList: array of objects, every member includes a field "paramId"
 *        dontRenewSession: optional, indicates that a request should NOT renew the session lifetime. defaults to false
 *        callback: optional, function which is called after all request is processed and values are written to list
 *
 * Note: actually, no variables can be replaced before sending the request (like in function ReadValue)
 */
DeviceParams.prototype.ReadValueGroup = function(paramList, dontRenewSession, callback)
{
  //Log('ReadValueGroup');

  // backward compatibility: second argument could be the callback
  if (typeof dontRenewSession === 'function') {
    callback = dontRenewSession;
    dontRenewSession = false;
  }
  // ensure, shouldRenewSession is boolean
  dontRenewSession = !!dontRenewSession;

  var status          = SUCCESS;
  var entireTimeout   = 0;
  var requestParamNo  = 0;

  var requestParamIdList        = new Array();
  var aConfigtoolRequestParams  = new Array();

  $.each(paramList, function(paramNo, paramObj)
  {
    var paramId = paramObj.paramId;

    // check if we already know this parameter
    if(!deviceParams.list[paramId])
    {
      status = ERROR;
      alert('DeviceParams Software Error: Unknown parameter ID: ' + paramId);
      return false;
    }

    // check if parameter does not change and was already read (-> no need to read it again)
    else if(!deviceParams.list[paramId].invariant || !deviceParams.list[paramId].initialized)
    {
      //aConfigtoolRequestParams[paramNo] = Object(deviceParams.list[paramId].configtoolReadParams);
      if(!deviceParams.list[paramId].configtoolReadParams)
      {
        status = ERROR;
        alert('DeviceParams Software Error: Unknown configtool read params for parameter ID: ' + paramId);
        return false;
      }

      aConfigtoolRequestParams[requestParamNo]  = $.extend(true, {}, deviceParams.list[paramId].configtoolReadParams);
      requestParamIdList[requestParamNo]        = paramId;

      entireTimeout += aConfigtoolRequestParams[requestParamNo].timeout;
      ++requestParamNo;
    }
  });

  if((SUCCESS === status) && requestParamNo)
  {
    deviceCommunication.SendRequest(
    {
      renewSession        : !dontRenewSession,
      timeout             : entireTimeout,
      aDeviceParams       : aConfigtoolRequestParams,
      processResponseFkt  : function(entireResponse)
      {
        var accessStatus  = SUCCESS;

        // response arrived - take over the single results (values or error infos) to parameter list
        $.each(entireResponse, function(responseParamNo, paramResponse)
        {
          var paramResponse   = this;
          var paramId         = requestParamIdList[responseParamNo];
          var paramObj        = deviceParams.list[paramId];

          deviceParams.TransferResult(paramObj, paramResponse);

          if(SUCCESS === paramObj.status) paramObj.initialized = true;

          if(DEVICE_PARAMS_ERROR_ACCESS_NOT_ALLOWED == paramObj.status) accessStatus = DEVICE_PARAMS_ERROR_ACCESS_NOT_ALLOWED;
        });

        if(DEVICE_PARAMS_ERROR_ACCESS_NOT_ALLOWED == accessStatus) $('body').trigger('event_userManagement_loginCheckRequired');
        if(callback) callback(SUCCESS);
      }
    });
  }

  // didn't send request (parameter includes no variant parameters or an error)
  else
  {
    if(callback) callback(status);
  }

  return status;
};


/*
 * Replace variable values in a request parameter string
 */
DeviceParams.prototype.ReplaceParamValuePlaceholder = function(templateParamList, valueList)
{
  var status  = SUCCESS;

  $.each(templateParamList, function(templateParamNo, templateParam)
  {
    if(templateParam.match(/\$/))
    {
      var variableName = templateParam.replace(/.*\$/, '');
      if(undefined === valueList[variableName])
      {
        status = ERROR;
      }
      else
      {
        // add a '$' to all special '$' sequences in variable value, to avoid special treatment of this character strings
        valueList[variableName] = '' + valueList[variableName];
        var variableValue = valueList[variableName].replace('$&', '$$$&').replace('$`', '$$$`').replace('$\'', '$$$\'');
        templateParamList[templateParamNo] = templateParam.replace('$' + variableName, variableValue);
      }
    }
  });

  //Log('templateParamList: ' + templateParamList);
  return status;
};


/*
 * Read single parameter value from device
 *
 * input: readParams.id: parameter ID
 *        readParams.variables: optional, array with values for variable parts in request parameter string
 *                              defined like: { variableName: variableValue }
 *                              if variable list is given, it must include exactly one value for every variable value in
 *                              parameter request string (e.g. configtool parameter string), so that all variables can be
 *                              replaced
 *        callback: callback function
 */
DeviceParams.prototype.ReadValue = function(readParams, dontRenewSession, callback)
{
  //Log('ReadValue');

  // backward compatibility: second argument could be the callback
  if (typeof dontRenewSession === 'function') {
    callback = dontRenewSession;
    dontRenewSession = false;
  }
  // ensure, shouldRenewSession is boolean
  dontRenewSession = !!dontRenewSession;

  var deviceParams  = this;

  var status    = SUCCESS;
  var paramId   = readParams.id;

  if(!deviceParams.list[paramId])
  {
    status = ERROR;
    alert('Unknown parameter ID: ' + paramId);
    return false;
  }

  else
  {
    // create a list for configtool call, including the according parts of device parameter data
    // (SendRequest function allways needs an array, even if we actually only need one value)
    var aConfigtoolRequestParams = new Array();
    aConfigtoolRequestParams[0] = $.extend(true, {}, deviceParams.list[paramId].configtoolReadParams);

    // if requested, replace variable values in configtool parameter string by actual values
    //if(deviceParams.list[paramId].variables)
    if(readParams.variables)
    {
      var templateParamList = aConfigtoolRequestParams[0].parameter;
      //var valueList         = readParams.variables;

      status = deviceParams.ReplaceParamValuePlaceholder(templateParamList, readParams.variables);
    }
  }

  if(SUCCESS === status)
  {
    deviceCommunication.SendRequest(
    {
      renewSession        : !dontRenewSession,
      timeout             : aConfigtoolRequestParams[0].timeout,
      aDeviceParams       : aConfigtoolRequestParams,
      processResponseFkt  : function(entireResponse)
      {
        // response arrived - take over result (value or error info) to parameter list, call callback
        $.each(entireResponse, function(paramNo, paramResponse)
        {
          var paramObj        = deviceParams.list[paramId];

          deviceParams.TransferResult(paramObj, paramResponse);

          if(SUCCESS === paramObj.status) paramObj.initialized = true;

          if(DEVICE_PARAMS_ERROR_ACCESS_NOT_ALLOWED == paramObj.status) $('body').trigger('event_userManagement_loginCheckRequired');
        });

        if(callback) callback();
      }
    });
  }

  return status;
};


DeviceParams.prototype.CallConfigtool = function(params)
{
  var dontRenewSession = !!params.dontRenewSession;

  if(!params.requestParams) return ERROR;

  var aConfigtoolRequestParams  = new Array();
  aConfigtoolRequestParams[0]   = $.extend(true, {}, params.requestParams);

  deviceCommunication.SendRequest(
  {
    renewSession        : !dontRenewSession,
    timeout             : aConfigtoolRequestParams[0].timeout,
    aDeviceParams       : aConfigtoolRequestParams,
    processResponseFkt  : function(entireResponse)
    {
      // if((params.format) && ('text' != params.format))
      //   transform json or xml response

      if(params.callback) params.callback(entireResponse[0]);
    }
  });
};




DeviceParams.prototype.TransferResult = function(paramObj, paramResponse)
{
  var status     = paramResponse.status;
  var errorText  = paramResponse.errorText;
  var value      = paramResponse.resultString;

//  paramObj.status     = paramResponse.status;
//  paramObj.errorText  = paramResponse.errorText;
//  paramObj.value      = paramResponse.resultString;

  if((SUCCESS === paramObj.status) && ('json' === paramObj.format))
  {
    try { value = jQuery.parseJSON(paramResponse.resultString); }
    catch(e)
    {
      status    = ERROR;
      errorText = 'Error while parsing JSON format for Parameter ' + paramObj.id;
    }
  }

  // especially for IE - use active X to parse XML document really as XML structure...
  else if('xml' === paramObj.format)
  {
    if(!value || !value.length)
    {
      status    = NOT_FOUND;
      errorText = "Response is empty.";
    }

    else if(window.ActiveXObject)
    {
      var xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(paramResponse.resultString);

      value = xml;
    }

    try { value = $.xml2json(value); }
    catch(e)
    {
      status    = ERROR;
      errorText = "Can't convert response XML.";
    }
  }

  if((SUCCESS === status) && (paramObj.convertFkt))
  {
    value = paramObj.convertFkt(value);
    if(null === value)
    {
      status    = ERROR;
      errorText = "Can't convert response data";
    }
  }

  // transfer (converted) response string and error data into param object
  paramObj.status     = status;
  paramObj.errorText  = errorText;
  paramObj.value      = value;

  // send global event, that deviceParam has been updated - actually not needed
  // $('body').trigger('event_deviceParams_updated_' + paramObj.id);
};



/*
 * Change single parameter value on device
 *
 * input: paramId: parameter ID
 *        valueList: array with values for variable parts in request parameter string (mostly values that shall be written)
 *                   defined like: { variableName: variableValue }
 *                   list must include exactly one value for every variable value in
 *                   parameter request string (e.g. configtool parameter string), so that all variables can be
 *                   replaced
 *        callback: callback function
 */
DeviceParams.prototype.ChangeValue = function(paramId, valueList, dontRenewSession, callback)
{
  // backward compatibility: second argument could be the callback
  if (typeof dontRenewSession === 'function') {
    callback = dontRenewSession;
    dontRenewSession = false;
  }
  dontRenewSession = !!dontRenewSession;

  var deviceParams  = this;
  var status        = SUCCESS;
  if(!deviceParams.list[paramId])
  {
    status = ERROR;
    callback(ERROR, 'WBM software error - unknown parameter ID ' + paramId);
    return false;
  }
  else if(!deviceParams.list[paramId].configtoolWriteParams)
  {
    callback(ERROR, 'WBM software error - no write instructions specified for parameter ' + paramId);
  }
  else
  {
    // create a list for configtool call, including the according parts of device parameter data
    var aConfigtoolRequestParams  = new Array();
    aConfigtoolRequestParams[0]   = $.extend(true, {}, deviceParams.list[paramId].configtoolWriteParams);

    // replace variable values in configtool parameter string by actual values
    var templateParamList = aConfigtoolRequestParams[0].parameter;
    if(SUCCESS != deviceParams.ReplaceParamValuePlaceholder(templateParamList, valueList))
    {
      callback(ERROR, 'WBM software error - missing write instructions for parameter list');
    }
    else
    {
      deviceCommunication.SendRequest(
      {
        renewSession        : !dontRenewSession,
        timeout             : 2 * aConfigtoolRequestParams[0].timeout, //20000,
        aDeviceParams       : aConfigtoolRequestParams,
        processResponseFkt  : function(aResponse)
        {
          if(DEVICE_PARAMS_ERROR_ACCESS_NOT_ALLOWED == aResponse[0].status) $('body').trigger('event_userManagement_loginCheckRequired');

          // response arrived - call callback function
          if(callback) callback(aResponse[0].status, aResponse[0].errorText);
        }
      });
    }
  }
};


DeviceParams.prototype.ChangeValueGroup = function(aParamData, dontRenewSession, callback)
{
  // backward compatibility: second argument could be the callback
  if (typeof dontRenewSession === 'function') {
    callback = dontRenewSession;
    dontRenewSession = false;
  }
  dontRenewSession = !!dontRenewSession;

  var status          = SUCCESS;
  var entireTimeout   = 0;

  var requestParamIdList        = new Array();
  var aConfigtoolRequestParams  = new Array();

  $.each(aParamData, function(requestParamIndex, paramObj)
  {
    var paramId = paramObj.paramId;

    // check if we already know this parameter
    if(!deviceParams.list[paramId])
    {
      status = ERROR;
      alert('DeviceParams Software Error: Unknown parameter ID: ' + paramId);
      return false;
    }

    if(!deviceParams.list[paramId].configtoolWriteParams)
    {
      status = ERROR;
      alert('DeviceParams Software Error: Unknown configtool write params for parameter ID: ' + paramId);
      return false;
    }

    // transfer instruction configtool params data to request param list
    aConfigtoolRequestParams[requestParamIndex]  = $.extend(true, {}, deviceParams.list[paramId].configtoolWriteParams);

    // replace variable values in configtool parameter string by actual values
    var templateParamList = aConfigtoolRequestParams[requestParamIndex].parameter;
    if(SUCCESS != deviceParams.ReplaceParamValuePlaceholder(templateParamList, paramObj.valueList))
    {
      callback(ERROR, 'WBM software error - missing write instructions for parameter list');
    }

    entireTimeout += aConfigtoolRequestParams[requestParamIndex].timeout;
  });

  if(SUCCESS === status)
  {
    deviceCommunication.SendRequest(
    {
      renewSession        : !dontRenewSession,
      timeout             : entireTimeout,
      aDeviceParams       : aConfigtoolRequestParams,
      processResponseFkt  : function(entireResponse)
      {
        var entireState   = SUCCESS;
        var lastErrorText = '';

        // response arrived - take over the single results (values or error infos) to parameter list
        $.each(entireResponse, function(responseParamIndex, paramResponse)
        {
          var paramId         = aParamData[responseParamIndex].paramId;
          var paramObj        = deviceParams.list[paramId];

          // tranfer status informations of write process
          paramObj.status     = paramResponse.status;
          paramObj.errorText  = paramResponse.errorText;

          if(SUCCESS != paramObj.status)
          {
            // access error is always top priority and should not be overwritten with less important errors
            if(DEVICE_PARAMS_ERROR_ACCESS_NOT_ALLOWED != entireState)
            {
              entireState   = paramResponse.status;
              lastErrorText = paramResponse.errorText;
            }
          }
        });

        if(DEVICE_PARAMS_ERROR_ACCESS_NOT_ALLOWED == entireState) $('body').trigger('event_userManagement_loginCheckRequired');
        if(callback) callback(entireState, lastErrorText);
      }
    });
  }

  // didn't send request (parameter includes no variant parameters or an error)
  else
  {
    if(callback) callback(status);
  }

  return status;
};




/*
 * check if an error is existen in at least one parameter of a parameter list
 */
DeviceParams.prototype.ReadErrorOccured = function(paramList)
{
  var errorOccured = false;

  $.each(paramList, function(paramIndex, paramListObj)
  {
    var paramObj      = deviceParams.list[paramListObj.paramId];

    if(SUCCESS != paramObj.status)
    {
      errorOccured = true;
      return false;
    }
  });

  return errorOccured;
};


/*
 * Get error text of last parameter of a parameter group
 * (before: collect all error strings of a list of parameters, seperatet by linefeed - can get very largen and normally not neccesary)
 */
DeviceParams.prototype.CollectedErrorStrings = function(paramList)
{
  var errorString = '';

  $.each(paramList, function(paramIndex, paramListObj)
  {
    var paramObj  = deviceParams.list[paramListObj.paramId];

    if(SUCCESS != paramObj.status)
    {
      //errorString = errorString + paramObj.errorText + '\n';
      errorString = paramObj.errorText;
    }
  });

  return errorString;
};


DeviceParams.prototype.CollectedErrorNumbers = function(paramList)
{
  var errorString = '';

  $.each(paramList, function(paramIndex, paramListObj)
  {
    var paramObj  = deviceParams.list[paramListObj.paramId];

    if(SUCCESS != paramObj.status)
    {
      errorString = errorString + 'parameter: ' + paramListObj.paramId + ', status: '+ paramObj.status + '\n';
    }
  });

  return errorString;
};


/*
 * delete al paramters from global device parameter list, given by an array of parameter IDs
 */
DeviceParams.prototype.DeleteValueGroup = function(paramList, callback)
{
  Log('DeleteValueGroup');

  $.each(paramList, function(paramNo, paramListObj)
  {
    if(paramListObj.paramId) delete deviceParams.list[paramListObj.paramId];
  });
};


DeviceParams.prototype.SummarizeParameterLists = function(parameterLists)
{
  var deviceParams = this;

  if(arguments.length) var summaryParamList = Array();

  $.each(arguments, function(listIndex, singleList)
  {
    $(singleList).each(function(paramIndex)
    {
      var newParam = { paramId: this.paramId };
      summaryParamList.push(newParam);
    });
  });

  return summaryParamList;
};



// create at once exactly one device param object
var deviceParams = new DeviceParams();




/*------------------------------------------------------------------------------
 * Object DeviceParamView / SingleParamView
 *
 * holds the data which is necessary to deal with specified parameters in web application
 * - show them in DOM of further processing
 *
 * every part of WBM defines its own paramView with the requested parameters
 * The parameter IDs refer to the DevideParam IDs
 *------------------------------------------------------------------------------
 */
function SingleParamView(initParams)
{
  var singleParamView = this;

  this.paramId          = initParams.paramId          || '';
  this.outputElementId  = initParams.outputElementId  || '';
  this.outputFkt        = initParams.outputFkt;
  this.defaultContent   = initParams.defaultContent;
  this.errorContent     = initParams.errorContent;
};


function DeviceParamView()
{
  var deviceParamView  = this;
  deviceParamView.list = Array();
};


DeviceParamView.prototype.Add = function(paramInitValues)
{
  var deviceParamView     = this;
  var newSingleParamView  = new SingleParamView(paramInitValues);

  deviceParamView.list.push(newSingleParamView);
};

/*
 * Further processing of the parameters in view:
 * show them in the specified DOM element or call callback function for futher processing
 *
 */
DeviceParamView.prototype.ShowValues = function()
{
  var deviceParamView = this;
  for (var i = 0; i < deviceParamView.list.length; i++) {
      deviceParamView.ShowSingleValue(i);
  }
};

/*
 * Further processing of a single parameter in view:
 * show it in the specified DOM element or call callback function for futher processing
 *
 */
DeviceParamView.prototype.ShowSingleValue = function(index)
{
  var deviceParamView = this;
  var paramView = deviceParamView.list[index];

  if (!paramView) {
    throw new Error('No Parameter View at index' + index)
  };

  var paramId   = paramView.paramId;
  var paramObj  = deviceParams.list[paramId];

  if(paramView.outputFkt)
  {
    paramView.outputFkt(paramObj.status, paramObj.value, paramView.outputElementId);
  }

  else if(paramView.outputElementId)
  {
    var paramValue = paramObj.value;

    if((SUCCESS != paramObj.status) && (paramView.defaultValue)) paramValue = paramView.defaultValue;
    $('body').trigger('event_elementContentChange', [ paramView.outputElementId, paramValue ] );
  }
};

DeviceParamView.prototype.ShowDefaultContent = function()
{
  var deviceParamView = this;

  $.each(deviceParamView.list, function(paramIndex, paramView)
  {
    if(paramView.outputElementId && paramView.defaultContent)
    {
      $('body').trigger('event_elementContentChange', [ paramView.outputElementId, paramView.defaultContent ] );
    }
  });
};
