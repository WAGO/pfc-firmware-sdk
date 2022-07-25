/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var SshContent = function(id)
{
  var sshContent = this;
  
  sshContent.id = id || 'ssh';
  
  var ShowStateInCheckbox = function(paramStatus, state, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      if('enabled' === state) $('#' + outputElementId).prop('checked', 'checked');
      else $('#' + outputElementId).removeAttr('checked');
    }
  };
  
  
  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var SshParamView = (function()
  {
    sshContent.paramView = new DeviceParamView();
                
    sshContent.paramView.Add(
    {
      paramId         : 'ssh_state',
      outputElementId : sshContent.id + '_content #ssh_active_state'
    });
            
    sshContent.paramView.Add(
    {
      paramId         : 'ssh_root_access_state',
      outputElementId : sshContent.id + '_content #ssh_root_access_state',
      outputFkt       : ShowStateInCheckbox
    });
        
    sshContent.paramView.Add(
    {
      paramId         : 'ssh_password_request_state',
      outputElementId : sshContent.id + '_content #ssh_password_request_state',
      outputFkt       : ShowStateInCheckbox
    });

    sshContent.paramView.Add(
    {
      paramId         : 'ssh_port_number',
      outputElementId : sshContent.id + '_content #ssh_port_number'
    });

  })();
      

  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(sshContent.id + '_create', function(event)
  {
    $('#' + sshContent.id + '_content #ssh_config_form').bind('submit', function(event) 
    {
      event.preventDefault();
      sshContent.ChangeParameter(this);
    });

  });


  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(sshContent.id + '_refresh', function(event)
  {
    sshContent.Refresh();
  });

};


SshContent.prototype.Refresh = function()
{
  var sshContent = this;
  
  deviceParams.ReadValueGroup(sshContent.paramView.list, function()
  {
    sshContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(sshContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(sshContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading SSH configuration.', errorString ] );
    }
  });

};


SshContent.prototype.ChangeParameter = function(formObj)
{
  var sshContent          = this;  
  var activeState         = ($(formObj).find('[name=service_active_state]:checked').length)   ? 'enabled' : 'disabled';
  var portNumber          = $(formObj).find('[name=port_number]').val();
  var rootLoginState      = ($(formObj).find('[name=root_access_state]:checked').length)      ? 'enabled' : 'disabled';
  var passwordLoginState  = ($(formObj).find('[name=password_request_state]:checked').length) ? 'enabled' : 'disabled';
  
  var isNotANumber = isNaN(portNumber);
  var iPortNumber = parseInt(portNumber);
  if (isNotANumber || (iPortNumber < 1) || (iPortNumber > 65535))
  {
    $('body').trigger('event_errorOccured', [ 'Port parameter must be in the range of [1 - 65535].' ]);
  }
  else
  {
  pageElements.ShowBusyScreen('Changing SSH parameter...');

  var newValueList  = { state: activeState, rootAccessState: rootLoginState, passwordRequestState: passwordLoginState, portNumber: iPortNumber };

  deviceParams.ChangeValue('ssh_parameter', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
  
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing SSH parameter.', errorText ]);
    }
  
    // show actual configuration
    sshContent.Refresh();
  });
  }

};

