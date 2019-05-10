/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var UsersContent = function(id)
{
  var usersContent = this;
  
  usersContent.id = id || 'users';
  
  var CreateUsersContentView = (function()
  {
    usersContent.paramView = new DeviceParamView();
                    
    usersContent.paramView.Add(
    {
      paramId         : 'default_password_flag_user'
    });
                
    usersContent.paramView.Add(
    {
      paramId         : 'default_password_flag_admin'
    });
    
  })();


  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(usersContent.id + '_create', function(event)
  {
    // add events for form processing
    $("#config_user_form").bind('submit', function(event)
    {
      event.preventDefault();
      usersContent.ChangePassword();
    });

  });


  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(usersContent.id + '_refresh', function(event)
  {
    // no actions necessary, data not changes
  });

};


UsersContent.prototype.Refresh = function()
{
  var usersContent = this;

  // read the information again, if the users use their default passwords (should have changed now)
  deviceParams.ReadValueGroup(usersContent.paramView.list, function()
  {
    //usersContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(usersContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(usersContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading user data.', errorString ] );
    }
  });
};


UsersContent.prototype.ChangePassword = function()
{
  var usersContent = this;
  //Log('Change Password');
  
  // get user input values from form
  var user            = $('#config_user_form select[name=user]').val();
  var newPassword     = $('#config_user_form input[name=new_password]').val();
  var confirmPassword = $('#config_user_form input[name=confirm_password]').val(); 
  var oldPassword     = $('#config_user_form input[name=old_password]').val();
  
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
  
  else if(newPassword.match(/[^a-zA-Z0-9!"#$%&'()*+,.\/:;<=>?@\[\\\^_`\{|\}~\-]/))
  {
    $('body').trigger('event_errorOccured', [ 'Illegal characters used. Please only use letters (a-z, A-Z), numbers, space and ASCII special characters: ]!"#$%&\'()*+,./:;<=>?@[\^_`{|}~-.' ]);
  }
  
  else
  {
    pageElements.ShowBusyScreen("Changing password...");

    var newValueList = { user: user, newPassword: newPassword, confirmPassword: confirmPassword, oldPassword: oldPassword };

    // send change request
    deviceParams.ChangeValue('config_user', newValueList, function(status, errorText)
    {
      // delete content of password input fields (we can't read the passwords anyhow)
      $('#config_user_form input[name=old_password]').val('');
      $('#config_user_form input[name=new_password]').val('');
      $('#config_user_form input[name=confirm_password]').val('');

      pageElements.RemoveBusyScreen();
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing password.', errorText ]);
      }
      else
      {
        usersContent.Refresh();
      }
    });
  }
};



