/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var WEBSERVER_RESTART_TIMEOUT = 5000;

function WebvisuContent(id)
{
  var webvisuContent = this;
  
  webvisuContent.id = id || 'webvisu';
  
  webvisuContent.loginUrl = 'index.php#login';
  webvisuContent.ownUrl   = 'index.php#' + webvisuContent.id;
  
  function ShowPortStateCallback(paramStatus, portState, outputFormId)
  {
    pageElements.VisualizeStateByTextAndButton(paramStatus, portState, outputFormId);
  };
  
  function CodesysVersionCallback(paramStatus, codesysVersionNo, outputFormId)
  {
    // set default webvisu link for codesys 3
    $('#' + outputFormId).attr('href', '/codesys3visu/');

    // check if we use codesys 2 and confrom link accordingly
    if((SUCCESS === paramStatus) && ('2' === codesysVersionNo))
    {
      $('#' + outputFormId).attr('href', '/webvisu/');
    }
  };
  
  
  var CreateWebvisuParamView = (function()
  {
    webvisuContent.paramView = new DeviceParamView();
        
    webvisuContent.paramView.Add(
    {
      paramId         : 'codesys_2_webserver_port_state',
      outputElementId : webvisuContent.id + '_content #webvisu_codesys_webserver_state',
      outputFkt       : ShowPortStateCallback
    });
    
    webvisuContent.paramView.Add(
    {
      paramId         : 'codesys_3_webserver_port_state',
      outputElementId : webvisuContent.id + '_content #webvisu_codesys_3_webserver_state',
      outputFkt       : ShowPortStateCallback
    });
    
    webvisuContent.paramView.Add(
    {
      paramId         : 'default_webserver'
    });

    webvisuContent.paramView.Add(
    {
      paramId         : 'rts_available_runtimes'
    });
        
  })();

  
  $('#content_area').bind(webvisuContent.id + '_create', function(event)
  {
    //Log('Event - ' + webvisuContent.id + ' create')

    // add events for form processing
    $('#' + webvisuContent.id + '_content #webvisu_default_webserver_form').bind('submit', function(event) 
    {
      event.preventDefault();
      webvisuContent.ChangeDefaultWebserver();
    });
    
  });
        

  $('#content_area').bind(webvisuContent.id + '_refresh', function(event)
  {
    //Log('Event - ' + webvisuContent.id + ' refresh');
    webvisuContent.Refresh();
  });

};


WebvisuContent.prototype.Refresh = function()
{
  var webvisuContent = this;

  deviceParams.ReadValueGroup(webvisuContent.paramView.list, function()
  {
    if(   (SUCCESS      != deviceParams.list['rts_available_runtimes'].status)
       || (SUCCESS      != deviceParams.list['default_webserver'].status)
       || ('undefined'  == deviceParams.list['rts_available_runtimes'].value.availableVersions))
    {
      $('body').trigger('event_errorOccured', [ 'Error while reading WebVisu info.' ] );
    }
    else
    {
      var availableVersions  = deviceParams.list['rts_available_runtimes'].value.availableVersions;
      
      $('#' + webvisuContent.id + '_content #webvisu_codesys_webserver_state').hide();
      $('#' + webvisuContent.id + '_content #webvisu_codesys_3_webserver_state').hide();

      //if(-1 != availableVersions.indexOf(2)) does not work with IE
      if((availableVersions[0] == 2) ||(availableVersions[1] == 2) || (availableVersions[2] == 2))
        $('#' + webvisuContent.id + '_content #webvisu_codesys_webserver_state').show();

      if((availableVersions[0] == 3) ||(availableVersions[1] == 3) || (availableVersions[2] == 3))
        $('#' + webvisuContent.id + '_content #webvisu_codesys_3_webserver_state').show();

      webvisuContent.paramView.ShowValues();

      var defaultWebserver  = deviceParams.list['default_webserver'].value;
      switch(defaultWebserver)
      {
        case 'WBM'          : $("input[name=default-webserver][value=wbm]").prop("checked", "checked");      break;
        case 'Webvisu'      : $("input[name=default-webserver][value=webvisu]").prop("checked", "checked");  break;
        
        default           : break;
      }
    }
  });
};


WebvisuContent.prototype.ChangeDefaultWebserver = function()
{
  var webvisuContent = this;

  var defaultWebserver      = $('input[name=default-webserver]:checked').val();
  var defaultWebserverParam = '';
  
  switch(defaultWebserver)
  {
    case 'wbm'        : defaultWebserverParam = 'WBM';      break;
    case 'webvisu'    : defaultWebserverParam = 'Webvisu';  break;
    default           : break;
  }
  
  var newValueList = { defaultWebpage: defaultWebserverParam };

  // workaround for unique situation that a form can be visible without a user is logged in
  // and sending form will automatically cause authentication error for user "guest" 
  // if no user is logged in (look at username area in dom), redirect to login page and after login again to webvisu page
  if(0 === $('.user_management_area .user_name').text().length)
  {
    pageBuildup.referrer = webvisuContent.ownUrl;
    pageBuildup.FollowNavLink(webvisuContent.loginUrl);
  }
  else
  {
    pageElements.ShowBusyScreen("Changing default webserver...");
    
    deviceParams.ChangeValue('default_webserver', newValueList, function(status, errorText)
    {
      if(SUCCESS != status)
      {
        pageElements.RemoveBusyScreen();
        $('body').trigger('event_errorOccured', [ 'Error while changing default webserver.', errorText ]);
        webvisuContent.Refresh();
      }
    
      // default webserver configuration changed successful - restart of server and reload of page is necessary afterwards
      else
      {
        // initiate reload of page after webserver had time to process restart
        var timeoutObj = window.setTimeout('window.location.reload()', WEBSERVER_RESTART_TIMEOUT);
    
        // then initiate restart of webserver
        deviceParams.ChangeValue('restart_webserver', { }, function(status, errorText)
        {
          // webserver restart will always return with an error - do nothing!
        });
      }
    });
  }
};


