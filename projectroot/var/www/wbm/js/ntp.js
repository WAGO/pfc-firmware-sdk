/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var NtpClientContent = function(id)
{
  var ntpClientContent = this;
  
  ntpClientContent.id = id || 'ntp';
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var ShowNtpStateCallback = function(paramStatus, ntpState, outputElementId)
  {
    $('#' + outputElementId).removeAttr('checked');
    if('enabled' === ntpState) $('#' + outputElementId).prop('checked', 'checked');
  };

 
  var NtpClientParamView = (function()
  {
    ntpClientContent.paramView = new DeviceParamView();
    
    ntpClientContent.paramView.Add(
    {
      paramId         : 'ntp_client_configuration'
    });
    
    ntpClientContent.paramView.Add(
    {
      paramId         : 'ntp_client_time_servers_currently_used'
    });
        
  })();
  
  
  $('#content_area').bind(ntpClientContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + ntpClientContent.id + '_content #config_ntp').bind('submit', function(event) 
    {
      event.preventDefault();
      ntpClientContent.ProcessConfigChangeSubmit(event.target);
    });
    
    $('#' + ntpClientContent.id + '_content #ntp_trigger_button').bind('click', function(event) 
    {
      event.preventDefault();
      ntpClientContent.TriggerActualization();
    });
    
  });

  $('#content_area').bind(ntpClientContent.id + '_refresh', function(event)
  {
    ntpClientContent.Refresh();
  });
  
};


NtpClientContent.prototype.Refresh = function()
{
  var ntpClientContent = this;
  
  deviceParams.ReadValueGroup(ntpClientContent.paramView.list, function()
  {
    if(   deviceParams.ReadErrorOccured(ntpClientContent.paramView.list) 
       || !deviceParams.list['ntp_client_configuration'].value.timeServerList
       || !deviceParams.list['ntp_client_time_servers_currently_used'].value.timeServerList)
    {
      var errorString = deviceParams.CollectedErrorStrings(ntpClientContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading NTP configuration.', errorString ] );
    }
    else
    {
      // for TEST
      //deviceParams.list['ntp_client_time_servers_currently_used'].value.timeServerList.push("123.321.123.321");
      //deviceParams.list['ntp_client_time_servers_currently_used'].value.timeServerList.push("192.168.001.051");
      //deviceParams.list['ntp_client_time_servers_currently_used'].value.timeServerList = [];

      ntpClientContent.ShowConfigFromObject();
      
      var additionalUsedList = $.getUniqueEntriesOfExtendedList(deviceParams.list['ntp_client_configuration'].value.timeServerList,
                                                                deviceParams.list['ntp_client_time_servers_currently_used'].value.timeServerList,
                                                                'ip');
      ntpClientContent.ShowTimeServersCurrentlyUsed(additionalUsedList);
    }
  });

};

NtpClientContent.prototype.ShowConfigFromObject = function()
{
  var ntpClientContent  = this;
  
  if(SUCCESS === deviceParams.list['ntp_client_configuration'].status)
  {
    var ntpConfig = deviceParams.list['ntp_client_configuration'].value;
    var ntpArea   = $('#' + ntpClientContent.id + '_content #config_ntp');
    
    $(ntpArea).find('#ntp_client_state').removeAttr('checked');
    $(ntpArea).find('#ntp_client_active_state_area').hide();
    $(ntpArea).find('#ntp_client_active_state').html('');
    
    if('enabled' === ntpConfig.state)
    {
      $(ntpArea).find('#ntp_client_state').prop('checked', 'checked');
      $(ntpArea).find('#ntp_client_active_state_area').show();
      //$(ntpArea).find('#ntp_client_active_state').html(' (' + ntpConfig.activeState + ')');
      
      var activeStateOutputText = ('active' === ntpConfig.activeState) ? 'Time server available' : 'Time server not available until now';
      $(ntpArea).find('#ntp_client_active_state').html(activeStateOutputText);
    }
    
    $(ntpArea).find('#ntp_client_update_time').val(ntpConfig.updateTime);

    $(ntpArea).find('[name^=time_server]').val('');
    $.each(ntpConfig.timeServerList, function(index, timeServer)
    {
      $(ntpArea).find('[name=time_server_' + (index + 1) + ']').val(timeServer);
    });
  }
};


NtpClientContent.prototype.ShowTimeServersCurrentlyUsed = function(timeServerList)
{
  var ntpClientContent  = this;
  var outputText        = '';
  
  if(!timeServerList || !timeServerList.length)
  {
    outputText = 'none';
  }
  else 
  {
    $.each(timeServerList, function(timeServerIndex, timeServerValue)
    {
      outputText = outputText + timeServerValue + '<br>';
    });
  }

  $('#ntp_client_time_servers_currently_used').html(outputText);
};

/*
 * set config fields to disabled, if ntp itself is disabled
 * - not necessary by now, because we need to configure ntp also if it is disabled (e.g. for one-time trigger)
 */
NtpClientContent.prototype.ConformInputAccessStates = function()
{
  var ntpClientContent = this;

  if($('#' + ntpClientContent.id + '_content #ntp_client_state:checked').length)
  {
    $('#' + ntpClientContent.id + '_content #config_ntp input[name!=ntp_client_state][type!=submit]').removeAttr('disabled');
  }
  else
  {
    $('#' + ntpClientContent.id + '_content #config_ntp input[name!=ntp_client_state][type!=submit]').attr('disabled', 'disabled');
  }
};


NtpClientContent.prototype.ChangeState = function(newState)
{
  var ntpClientContent = this;
  
  pageElements.ShowBusyScreen('Changing NTP state...');

  var newValueList  = { state: newState };

  deviceParams.ChangeValue('ntp_client_state', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing NTP state.', errorText ]);
    }
    
    // show actual configuration
    ntpClientContent.Refresh();
  });

};


NtpClientContent.prototype.ChangeParameter = function(timeServer, updateTime)
{
  var ntpClientContent = this;

  pageElements.ShowBusyScreen('Changing NTP parameter...');

  var newValueList  = { timeServer: timeServer, updateTime: updateTime };

  deviceParams.ChangeValue('ntp_client_parameter', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
  
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing NTP parameter.', errorText ]);
    }
  
    // show actual configuration
    ntpClientContent.Refresh();
  });
  
};


NtpClientContent.prototype.ProcessConfigChangeSubmit = function(formObj)
{
  var ntpClientContent = this;

  var state       = $(formObj).find('[name=ntp_client_state]:checked').length ? 'enabled' : 'disabled';
  var updateTime  = $(formObj).find('[name=update_time]').val();
  var timeServer1 = $(formObj).find('[name=time_server_1]').val();
  var timeServer2 = $(formObj).find('[name=time_server_2]').val();
  var timeServer3 = $(formObj).find('[name=time_server_3]').val();
  var timeServer4 = $(formObj).find('[name=time_server_4]').val();
  
  pageElements.ShowBusyScreen('Changing NTP configuration...');
  
  var newValueList  = { state: state, updateTime: updateTime, timeServer1: timeServer1, timeServer2: timeServer2, timeServer3: timeServer3, timeServer4: timeServer4 };

  deviceParams.ChangeValue('ntp_client_configuration', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
  
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing NTP configuration.', errorText ]);
    }
  
    ntpClientContent.Refresh();
  });

};



NtpClientContent.prototype.TriggerActualization = function()
{
  var ntpClientContent = this;

  pageElements.ShowBusyScreen('Update time...');
  
  var newValueList  = { };

  deviceParams.ChangeValue('ntp_client_trigger', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
  
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while updating time.', errorText ]);
    }
    else
    {
      setTimeout(function() { $('#status_area').trigger('event_refresh'); }, 1000);
    }
  });

};

