/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var SnmpV3Content = function(id)
{
  var snmpV3Content = this;
  
  snmpV3Content.id = id || 'snmp_v3';
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var CreateUserAreaContentCallback = function(paramStatus, userList, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      snmpV3Content.CreateUserArea(userList, outputElementId);
    }
  };

  
  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var SnmpV3Content = (function()
  {
    snmpV3Content.paramView = new DeviceParamView();
    
    snmpV3Content.paramView.Add(
    {
      paramId         : 'snmp_v3_user_list',
      outputElementId : snmpV3Content.id + '_content #v3_user',
      outputFkt       : CreateUserAreaContentCallback
    });
    
  })();
  

  //------------------------------------------------------------------------------
  // event - first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(snmpV3Content.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + snmpV3Content.id + '_content #v3-user-add-form').bind('submit', function(event)
    {
      event.preventDefault();
      snmpV3Content.AddUser(this);
    });

    // any submit event in existing user area (= delete form)
    $('#' + snmpV3Content.id + '_content #v3_user .list').bind('submit', function(event)
    {
      event.preventDefault();
      snmpV3Content.DeleteUser(event.target);
    });

  });


  //------------------------------------------------------------------------------
  // event - refresh content area
  //------------------------------------------------------------------------------
  $('#content_area').bind(snmpV3Content.id + '_refresh', function(event)
  {
    snmpV3Content.Refresh();
  });

};

SnmpV3Content.prototype.Refresh = function()
{
  var snmpV3Content = this;
  
  deviceParams.ReadValueGroup(snmpV3Content.paramView.list, function()
  {
    $('#' + snmpV3Content.id + '_content #v3_user .list').html('');
    snmpV3Content.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(snmpV3Content.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(snmpV3Content.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading SNMP configuration.', errorString ] );
      
    }
  });
  
};


SnmpV3Content.prototype.CreateUserArea = function(userList, outputElementId)
{
  var snmpV3Content  = this;

  $('#' + outputElementId + ' .count').html(userList.length);
  
  $(userList).each(function(userIndex)
  {
    var user = this;
    
    var html = snmpV3Content.UserHtml(userIndex + 1, user.authName, user.authType, user.authKey, user.privacy, 
                                      user.privacyKey, user.notificationReceiver);
    $('#' + outputElementId + ' .list').append(html);
  });
};




SnmpV3Content.prototype.UserHtml = function(userNo, authName, authType, authKey, privacy, privacyKey, notificationReceiver)
{
  var snmpV1V2cContent  = this;
  
  var userNo                  = userNo                || '';
  var authName                = authName              || '';
  var authType                = authType              || '';
  var authKey                 = authKey               || '';
  var privacy                 = privacy               || '';
  var privacyKey              = privacyKey            || '';
  var notificationReceiverIp  = notificationReceiver  || '';
  var html                    = '';

  html += '<h3>v3 User ' + userNo +'</h3>';
  
  html += '<form id="v3_user_delete_form_user_' + userNo + '"  action="javascript_requested.php" method="POST">';
  html +=   '<div class="config_form_box">';
  html +=     '<div class="config_data_list">';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Security Authentication Name:</div>';
  html +=         '<div class="value_field"><span class="auth_name">' + authName + '</span></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Authentication Type:</div>';
  html +=         '<div class="value_field"><span class="auth_type">' + authType + '</span></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Authentication Key:</div>';
  html +=         '<div class="value_field"><span class="auth_key">' + authKey + '</span></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Privacy:</div>';
  html +=         '<div class="value_field"><span class="privacy">' + privacy + '</span></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Privacy Key:</div>';
  html +=         '<div class="value_field"><span class="privacy_key">' + privacyKey + '</span></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Notification Receiver IP:</div>';
  html +=         '<div class="value_field"><span class="notification_receiver_ip">' + notificationReceiver + '</span></div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=     '<p class="button_area">';
  html +=       '<input class="button" type="submit" value="Delete" name="SUBMIT">';
  html +=     '</p>';
  html +=   '</div>';
  html += '</form>';
  
  return html;
};


SnmpV3Content.prototype.DeleteUser = function(formObj)
{
  var snmpV3Content = this;

  var userNo = $(formObj).attr('id').replace('v3_user_delete_form_user_', '');
  
  pageElements.ShowBusyScreen('Delete v3 user number ' + userNo + '...');

  // for delete, all trap config parameters must be given to identify the right trap receiver
  var authName              = $(formObj).find('.auth_name').html();
  var authType              = $(formObj).find('.auth_type').html();
  var authKey               = $(formObj).find('.auth_key').html();
  var privacy               = $(formObj).find('.privacy').html();
  var privacyKey            = $(formObj).find('.privacy_key').html();
  var notificationReceiver  = $(formObj).find('.notification_receiver_ip').html();
  
  var newValueList  = { action: 'delete', authName: authName, authType: authType, authKey: authKey, 
                        privacy: privacy, privacyKey: privacyKey, notificationReceiver: notificationReceiver };

  deviceParams.ChangeValue('snmp_edit_v3_user', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
        
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while deleting v3 user.', errorText ]);
    }

    snmpV3Content.Refresh();
  });

};




SnmpV3Content.prototype.AddUser = function(formObj)
{
  var snmpV3Content  = this;

  pageElements.ShowBusyScreen('Add new SNMP v3 user...');

  var authName              = $(formObj).find('[name=auth-name]').val();
  var authType              = $(formObj).find('[name=auth-type]:checked').val();
  var authKey               = $(formObj).find('[name=auth-key]').val();
  var privacy               = $(formObj).find('[name=privacy]:checked').val();
  var privacyKey            = $(formObj).find('[name=privacy-key]').val();
  var notificationReceiver  = $(formObj).find('[name=notification-receiver]').val();

  var newValueList  = { action: 'add', authName: authName, authType: authType, authKey: authKey, 
                        privacy: privacy, privacyKey: privacyKey, notificationReceiver: notificationReceiver };

  deviceParams.ChangeValue('snmp_edit_v3_user', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while adding new v3 user.', errorText ]);
    }
    else
    {
      snmpV3Content.Refresh();
      
      // remove input strings in form
      $(formObj).find('[name=auth-name]').val('');
      $(formObj).find('[name=auth-type]#snmp_v3_auth_type_none').prop('checked', true);
      $(formObj).find('[name=auth-key]').val('');
      $(formObj).find('[name=privacy]#snmp_v3_privacy_none').prop('checked', true);
      $(formObj).find('[name=privacy-key]').val('');
      $(formObj).find('[name=notification-receiver]').val('');
    }
  });

};


