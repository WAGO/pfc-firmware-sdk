/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var SnmpV1V2cContent = function(id)
{
  var snmpV1V2cContent = this;
  
  snmpV1V2cContent.id = id || 'snmp_v1_v2c';
  

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var CreateTrapReceiverAreaContentCallback = function(paramStatus, trapReceiverList, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      snmpV1V2cContent.CreateTrapReceiverArea(trapReceiverList, outputElementId);
    }
  };

  
  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var SnmpV1V2cContentlView = (function()
  {
    snmpV1V2cContent.configParamView        = new DeviceParamView();
    
    snmpV1V2cContent.configParamView.Add(
    {
      paramId         : 'snmp_v1_v2c_state',
      outputElementId : snmpV1V2cContent.id + '_content #v1-v2c-protocol-state'
    });
    
    snmpV1V2cContent.configParamView.Add(
    {
      paramId         : 'snmp_v1_v2c_community_name',
      outputElementId : snmpV1V2cContent.id + '_content #v1-v2c-community-name'
    });
    
    
    snmpV1V2cContent.trapReceiverParamView  = new DeviceParamView();
    
    snmpV1V2cContent.trapReceiverParamView.Add(
    {
      paramId         : 'snmp_v1_v2c_trap_receiver_list',
      outputElementId : snmpV1V2cContent.id + '_content #v1_v2c_trap_receiver',
      outputFkt       : CreateTrapReceiverAreaContentCallback
    });
    
    snmpV1V2cContent.completeParamList = 
      deviceParams.SummarizeParameterLists(snmpV1V2cContent.configParamView.list, snmpV1V2cContent.trapReceiverParamView.list);
    
  })();

  
  //------------------------------------------------------------------------------
  // event - first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(snmpV1V2cContent.id + '_create', function(event)
  {
    // add events for form processing
    
    $('#' + snmpV1V2cContent.id + '_content form#change-v1-v2c-config-form').bind('submit', function(event) 
    {
      event.preventDefault();
      snmpV1V2cContent.ChangeConfig(this);
    });

    // any submit event in existing trap receiver area (= delete form)
    $('#' + snmpV1V2cContent.id + '_content #v1_v2c_trap_receiver .list').bind('submit', function(event)
    {
      event.preventDefault();
      snmpV1V2cContent.DeleteTrapReceiver(event.target);
    });

    // add events for form processing
    $('#' + snmpV1V2cContent.id + '_content #v1-v2c-trap-receiver-add-form').bind('submit', function(event)
    {
      event.preventDefault();
      snmpV1V2cContent.AddTrapReceiver(this);
    });
  });


  //------------------------------------------------------------------------------
  // event - refresh content area
  //------------------------------------------------------------------------------
  $('#content_area').bind(snmpV1V2cContent.id + '_refresh', function(event)
  {
    snmpV1V2cContent.Refresh();
  });


};


SnmpV1V2cContent.prototype.Refresh = function(paramList)
{
  var snmpV1V2cContent = this;
  
  // if no special param list is given, read parameters from entire page - otherwise read only the given list
  var readParamList = paramList || snmpV1V2cContent.completeParamList;
  
  deviceParams.ReadValueGroup(readParamList, function()
  {
    // if we can'f find community name, it's no error in this context, so reset it and set value to empty string
    if(NOT_FOUND === deviceParams.list['snmp_v1_v2c_community_name'].status)
    {
      deviceParams.list['snmp_v1_v2c_community_name'].status  = SUCCESS;
      deviceParams.list['snmp_v1_v2c_community_name'].value   = '';
    }
    
    // show values of requested parameter list(s) in page
    if(!paramList || (paramList && (snmpV1V2cContent.configParamView.list === paramList)))
    {
      snmpV1V2cContent.configParamView.ShowValues();
    }
    if(!paramList || (paramList && (snmpV1V2cContent.trapReceiverParamView.list === paramList)))
    {
      $('#' + snmpV1V2cContent.id + '_content #v1_v2c_trap_receiver .list').html('');
      snmpV1V2cContent.trapReceiverParamView.ShowValues();
    }
    
    // check errors for all read parameter
    if(deviceParams.ReadErrorOccured(readParamList)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(readParamList);
      $('body').trigger('event_errorOccured', [ 'Error while reading SNMP configuration.', errorString ] );
    }
  });
  
};


SnmpV1V2cContent.prototype.TrapReceiverHtml = function(trapReceiverNo, ipAddress, communityName, version)
{
  var snmpV1V2cContent  = this;
  
  var trapReceiverNo    = trapReceiverNo  || '';
  var ipAddress         = ipAddress       || '';
  var communityName     = communityName   || '';
  var version           = version         || '';
  var html              = '';

  html += '<h3>Trap Receiver ' + trapReceiverNo +'</h3>';
  
  html += '<form id="v1-v2c-trap-delete-form-' + trapReceiverNo + '"  action="javascript_requested.php" method="POST">';
  html +=   '<div class="config_form_box">';
  html +=     '<div class="config_data_list">';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">IP Address:</div>';
  html +=         '<div class="value_field"><span class="address">' + ipAddress + '</span></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Community Name:</div>';
  html +=         '<div class="value_field"><span class="community-name">' + communityName + '</span></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Version:</div>';
  html +=         '<div class="value_field"><span class="version">' + version + '</span></div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=     '<p class="button_area">';
  html +=       '<input class="button" type="submit" value="Delete" name="SUBMIT">';
  html +=     '</p>';
  html +=   '</div>';
  html += '</form>';
  
  return html;
};


SnmpV1V2cContent.prototype.CreateTrapReceiverArea = function(trapReceiverList, outputElementId)
{
  var snmpV1V2cContent  = this;

  $('#' + outputElementId + ' .count').html(trapReceiverList.length);
  
  $(trapReceiverList).each(function(receiverIndex)
  {
    var trapReceiver = this;
    
    var html = snmpV1V2cContent.TrapReceiverHtml(receiverIndex + 1, trapReceiver.address, trapReceiver.communityName, 
                                                 trapReceiver.version);
    $('#' + outputElementId + ' .list').append(html);
  });
};


SnmpV1V2cContent.prototype.ChangeConfig = function(formObj)
{
  var snmpV1V2cContent  = this;
  var newState          = $(formObj).find('input#v1-v2c-protocol-state:checked').length ? 'enabled' : 'disabled';
  var newCommunityName  = $(formObj).find('#v1-v2c-community-name').val();
  
  // we can't write a community name when state is disabled, so remove it in anyway
  if('disabled' === newState) newCommunityName = '';
  
  pageElements.ShowBusyScreen('Changing SNMP v1/v2c configuration...');

  var newValueList  = { state: newState, communityName: newCommunityName };

  deviceParams.ChangeValue('snmp_v1_v2c_configuration', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing SNMP v1/v2c manager configuration.', errorText ]);
    }
    else
    {
      // show actual configuration
      snmpV1V2cContent.Refresh(snmpV1V2cContent.configParamView.list);
    }
  });

};



SnmpV1V2cContent.prototype.DeleteTrapReceiver = function(formObj)
{
  var snmpV1V2cContent = this;

  var trapNo = $(formObj).attr('id').replace('v1-v2c-trap-delete-form-', '');
  
  pageElements.ShowBusyScreen('Delete SNMP v1/v2c trap receiver number ' + trapNo + '...');

  // for delete, all trap config parameters must be given to identify the right trap receiver
  var address       = $(formObj).find('.address').html();
  var communityName = $(formObj).find('.community-name').html();
  var version       = $(formObj).find('.version').html();
  
  var newValueList  = { action: 'delete', address: address, communityName: communityName, version: version };

  deviceParams.ChangeValue('snmp_edit_v1_v2c_trap_receiver', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
        
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while deleting trap receiver.', errorText ]);
    }

    snmpV1V2cContent.Refresh(snmpV1V2cContent.trapReceiverParamView.list);
  });

};


SnmpV1V2cContent.prototype.AddTrapReceiver = function(formObj)
{
  var snmpV1V2cContent = this;
  
  pageElements.ShowBusyScreen('Add new SNMP v1/v2c trap receiver...');

  var address       = $(formObj).find('[name=address]').val();
  var communityName = $(formObj).find('[name=community_name]').val();
  var version       = $(formObj).find('[name=version]:checked').val();
  
  var newValueList  = { action: 'add', address: address, communityName: communityName, version: version };

  deviceParams.ChangeValue('snmp_edit_v1_v2c_trap_receiver', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while adding new trap receiver.', errorText ]);
    }
    else
    {
      snmpV1V2cContent.Refresh(snmpV1V2cContent.trapReceiverParamView.list);
      
      // remove input strings in form
      $(formObj).find('[name=address]').val('');
      $(formObj).find('[name=community_name]').val('');
      $(formObj).find('[name=version][value=v1]').prop('checked', 'checked');
    }
    
  });


};


