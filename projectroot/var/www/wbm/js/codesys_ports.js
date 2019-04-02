/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var CodesysPortsContent = function(id)
{
  var codesysPortsContent = this;
  
  codesysPortsContent.id = id || 'codesys_ports';
  
  
  //------------------------------------------------------------------------------
  // private functions to display (port) states - we use central function, 
  // but some responses must be convertet to the fitting strings 
  //------------------------------------------------------------------------------
  function Codesys2StateCallback(paramStatus, codesysVersion, outputFormId)
  {
    var codesys2State = ('2' === codesysVersion) ? 'enabled' : 'disabled';
    pageElements.VisualizeStateByTextAndButton(paramStatus, codesys2State, outputFormId);
  };

  function Codesys3StateCallback(paramStatus, codesysVersion, outputFormId)
  {
    var codesys3State = ('3' === codesysVersion) ? 'enabled' : 'disabled';
    pageElements.VisualizeStateByTextAndButton(paramStatus, codesys3State, outputFormId);
  };


  //------------------------------------------------------------------------------
  // define view elements 
  //------------------------------------------------------------------------------
  var CreateCodesysPortsParamView = (function()
  {
    codesysPortsContent.paramView = new DeviceParamView();
            
    codesysPortsContent.paramView.Add(
     {
       paramId         : 'codesys_version',
       outputElementId : codesysPortsContent.id + '_content #codesys_services_codesys_2_state',
       outputFkt       : Codesys2StateCallback
     });

    codesysPortsContent.paramView.Add(
    {
      paramId         : 'codesys_2_webserver_port_state',
      outputElementId : codesysPortsContent.id + '_content #codesys_services_webserver_enabled'
    });
    
    codesysPortsContent.paramView.Add(
    {
      paramId         : 'codesys_version',
      outputElementId : codesysPortsContent.id + '_content #codesys_services_codesys_3_state',
      outputFkt       : Codesys3StateCallback
    });

    codesysPortsContent.paramView.Add(
    {
      paramId         : 'codesys_3_webserver_port_state',
      outputElementId : codesysPortsContent.id + '_content #codesys_services_webserver_3_enabled'
    });
    
    codesysPortsContent.paramView.Add(
    {
      paramId         : 'codesys_2_communication_state',
      outputElementId : codesysPortsContent.id + '_content #codesys_communication_active_state'
    });
        
    codesysPortsContent.paramView.Add(
    {
      paramId         : 'codesys_communication_port',
      outputElementId : codesysPortsContent.id + '_content #codesys_port'
    });
            
    codesysPortsContent.paramView.Add(
    {
      paramId         : 'codesys_2_port_2455_auth_state',
      outputElementId : codesysPortsContent.id + '_content #codesys_services_port_authentication_enabled'
    });

    codesysPortsContent.paramView.Add(
    {
      paramId         : 'codesys_3_port_2455_auth_state',
      outputElementId : codesysPortsContent.id + '_content #codesys_services_codesys3_port_authentication_enabled'
    });

    codesysPortsContent.paramView.Add(
    {
      paramId         : 'rts_available_runtimes'
    });
            
  })();

  
  //------------------------------------------------------------------------------
  // first creation of content area
  //------------------------------------------------------------------------------
  $('#content_area').bind(codesysPortsContent.id + '_create', function(event)
  {
    //------------------------------------------------------------------------------
    // add events for form processing
    //------------------------------------------------------------------------------
    //$('#' + codesysPortsContent.id + '_content input#change_state_button_codesys_webserver').bind('click', function(event)
    $('#' + codesysPortsContent.id + '_content #codesys_webserver_port_state_form').bind('submit', function(event)
    {
      event.preventDefault();
      var newState = $(event.target).find('[name=service_state]').prop('checked') ? 'enabled' : 'disabled';
      codesysPortsContent.ChangeCodesys2WebserverState(newState);
    });

    $('#' + codesysPortsContent.id + '_content #codesys_webserver_3_port_state_form').bind('submit', function(event)
    {
      event.preventDefault();
      var newState = $(event.target).find('[name=service_state]').prop('checked') ? 'enabled' : 'disabled';
      codesysPortsContent.ChangeCodesys3WebserverState(newState);
    });

    $('#' + codesysPortsContent.id + '_content #codesys_communication_active_state_form').bind('submit', function(event) 
    {
      event.preventDefault();
      var newState = $(event.target).find('[name=service_active_state]').prop('checked') ? 'enabled' : 'disabled';
      codesysPortsContent.ChangeCodesysCommunicationState(newState);
    });

    $('#' + codesysPortsContent.id + '_content form#change_codesys_communication_port_form').bind('submit', function(event)
    {
      event.preventDefault();
      codesysPortsContent.ChangeCodesysPort(this);
    });

    $('#' + codesysPortsContent.id + '_content #codesys_port_2455_auth_state_form').bind('submit', function(event)
    {
      event.preventDefault();
      var newState = $(event.target).find('[name=service_state]').prop('checked') ? 'enabled' : 'disabled';
      codesysPortsContent.ChangePort2455AuthState(newState);
    });

    $('#' + codesysPortsContent.id + '_content form#change_port_2455_password_form').bind('submit', function(event) 
    {
      event.preventDefault();
      codesysPortsContent.ChangePort2455Password(this);
    });
  
    $('#' + codesysPortsContent.id + '_content #codesys_port_codesys3_port_2455_authentication_form').bind('submit', function(event)
    {
      event.preventDefault();
      var newState = $(event.target).find('[name=service_state]').prop('checked') ? 'enabled' : 'disabled';
      codesysPortsContent.ChangeCodesys3Port2455AuthState(newState);
    });

  });


  //------------------------------------------------------------------------------
  // refresh of complete content
  //------------------------------------------------------------------------------
  $('#content_area').bind(codesysPortsContent.id + '_refresh', function(event)
  {
    codesysPortsContent.Refresh();
  });

};


CodesysPortsContent.prototype.Refresh = function()
{
  var codesysPortsContent = this;
  var status              = SUCCESS;
  
  deviceParams.ReadValueGroup(codesysPortsContent.paramView.list, function()
  {
    // check general errors
    if(   (SUCCESS != deviceParams.list['rts_available_runtimes'].status)
       || (SUCCESS != deviceParams.list['codesys_version'].status)
       || ('undefined' == deviceParams.list['rts_available_runtimes'].value.availableVersions))
    {
      status = ERROR;
    }
    else
    {
      var availableVersions  = deviceParams.list['rts_available_runtimes'].value.availableVersions;
      
      // initially hide both codesys areas
      $('#' + codesysPortsContent.id + '_content #codesys_ports_codesys_2_area').hide();
      $('#' + codesysPortsContent.id + '_content #codesys_ports_codesys_3_area').hide();

      // codesys 2 is available - show page area and check codesys 2 errors
      //if(-1 != availableVersions.indexOf(2)) does not work with IE
      if((availableVersions[0] == 2) ||(availableVersions[1] == 2) || (availableVersions[2] == 2))
      {
        $('#' + codesysPortsContent.id + '_content #codesys_ports_codesys_2_area').show();
        
        if(   (SUCCESS != deviceParams.list['codesys_2_webserver_port_state'].status)
           || (SUCCESS != deviceParams.list['codesys_2_communication_state'].status)
           || (SUCCESS != deviceParams.list['codesys_communication_port'].status)
           || (SUCCESS != deviceParams.list['codesys_2_port_2455_auth_state'].status))
        {
          status = ERROR;
        }
      }

      // codesys 3 is available - show page area and check codesys 3 errors
      if((availableVersions[0] == 3) ||(availableVersions[1] == 3) || (availableVersions[2] == 3))
      {
        $('#' + codesysPortsContent.id + '_content #codesys_ports_codesys_3_area').show();

        if(   (SUCCESS != deviceParams.list['codesys_3_webserver_port_state'].status)
           || (SUCCESS != deviceParams.list['codesys_3_port_2455_auth_state'].status))
        {
          status = ERROR;
        }
      }
    }
    
    if(SUCCESS != status) 
    {
      $('body').trigger('event_errorOccured', [ 'Error while reading CODESYS port configuration.' ] );
    }

    codesysPortsContent.paramView.ShowValues();
  });

};



CodesysPortsContent.prototype.ChangeCodesys2WebserverState = function(newState)
{
  var codesysPortsContent = this;

  if(('enabled' != newState) && ('disabled' != newState))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for CODESYS webserver state.', formObj.value ] );
  }
  
  else
  {
    pageElements.ShowBusyScreen('Changing port state...');
  
    var newValueList = { state: newState };

    deviceParams.ChangeValue('codesys_2_webserver_port_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing port state.', errorText ]);
        codesysPortsContent.Refresh();
      }
    });
  }
};


CodesysPortsContent.prototype.ChangeCodesys3WebserverState = function(newState)
{
  var codesysPortsContent = this;

  if(('enabled' != newState) && ('disabled' != newState))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for CODESYS webserver state.', formObj.value ] );
  }
  
  else
  {
    pageElements.ShowBusyScreen('Changing port state...');
  
    var newValueList = { state: newState };

    deviceParams.ChangeValue('codesys_3_webserver_port_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing port state.', errorText ]);
        codesysPortsContent.Refresh();
      }
    });
  }
};


CodesysPortsContent.prototype.ChangeCodesysCommunicationState = function(newState)
{
  var codesysPortsContent = this;

  // check input parameter
  if(('enabled' != newState) && ('disabled' != newState))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for CODESYS port state.', newState ] );
  }
  
  else
  {
    pageElements.ShowBusyScreen('Changing communication state...');
  
    var newValueList = { state: newState };

    deviceParams.ChangeValue('codesys_2_communication_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing port state.', errorText ]);
        codesysPortsContent.Refresh();
      }
      else
      {
        // no error while processing - new state is already shown in checkbox
      }
    });
  }

};


CodesysPortsContent.prototype.ChangeCodesysPort = function(formObj)
{
  var codesysPortsContent = this;

  pageElements.ShowBusyScreen('Changing CODESYS port...');
  
  var newPortNumber = $(formObj).find('#codesys_port').val();
  var newValueList  = { portNumber: newPortNumber };

  deviceParams.ChangeValue('codesys_communication_port', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing CODESYS port.', errorText ]);
    }
    
    codesysPortsContent.Refresh();
  });
};


CodesysPortsContent.prototype.ChangePort2455AuthState = function(codesys_2_webserver_port_state)
{
  var codesysPortsContent = this;

  // get and check input parameter
  if(('disabled' != codesys_2_webserver_port_state) && ('enabled' != codesys_2_webserver_port_state))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for port authentication state.', codesys_2_webserver_port_state ] );
  }
   
  else
  {
    pageElements.ShowBusyScreen('Changing port authentication state...');
  
    var newValueList = { state: codesys_2_webserver_port_state };

    deviceParams.ChangeValue('codesys_2_port_2455_auth_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing port authentication state.', errorText ]);
        codesysPortsContent.Refresh();
      }
    });
  }
};



CodesysPortsContent.prototype.ChangePort2455Password = function(formObj)
{
  // get user input values from form
  var newPassword     = $(formObj).find('input[name=port_2455_new_password]').val();
  var confirmPassword = $(formObj).find('input[name=port_2455_confirm_password]').val(); 
  
  // check if new password is given
  if(0 == newPassword.length) 
  {
    $('body').trigger('event_errorOccured', [ 'Password is missing.' ]);
  }
  
  // check if new password and its confirmation are the same
  else if(newPassword != confirmPassword) 
  {
    $('body').trigger('event_errorOccured', [ 'Confirmed password is different.' ]);
  }
  
  else
  {
    pageElements.ShowBusyScreen("Changing password...");

    var newValueList = { newPassword: newPassword, confirmPassword: confirmPassword };

    // send change request
    deviceParams.ChangeValue('codesys_port_auth_password', newValueList, function(status, errorText)
    {
      // delete content of password input fields (we can't read the passwords anyhow)
      $(formObj).find('input[name=port_2455_new_password]').val('');
      $(formObj).find('input[name=port_2455_confirm_password]').val('');

      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing password.', errorText ]);
      }
    });
  }
};



CodesysPortsContent.prototype.ChangeCodesys3Port2455AuthState = function(newState)
{
  var codesysPortsContent = this;

  // get and check input parameter
  if(('enabled' != newState) && ('disabled' != newState))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for port authentication state.', codesys_2_webserver_port_state ] );
  }
   
  else
  {
    pageElements.ShowBusyScreen('Changing port authentication state...');
  
    var newValueList = { state: newState };

    deviceParams.ChangeValue('codesys_3_port_2455_auth_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing port authentication state.', errorText ]);
        codesysPortsContent.Refresh();
      }
    });
  }
};


