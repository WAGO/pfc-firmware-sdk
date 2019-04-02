/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function UserManagement(username, csrfToken)
{

  var userManagement = this;
  userManagement.actionArea       = $("#head_area .user_management_area");
  userManagement.currentUser      = username  || '';
  userManagement.csrfToken        = csrfToken || false;
  userManagement.defaultReferrer  = 'index.php#information';
  userManagement.authFailureId    = '20';
  userManagement.loginPageId      = 'login';
  userManagement.ownReferrer      = 'index.php#' + userManagement.loginPageId;
  /**
   * internal functions
   */

  /**
   * create and actualize area with output of current user and link to login/logout
   */
  var ShowActionAreaHtmlFrame = function()
  {
    $(userManagement.actionArea).append('<span class="user_name"></span>');
    $(userManagement.actionArea).append('<a href="#"></a>');
  };

  var UpdateActionArea = function()
  {
    if(userManagement.currentUser)
    {
      $(userManagement.actionArea).find('a').attr('href', 'logout.php');
      $(userManagement.actionArea).find('a').text('Logout');
      $(userManagement.actionArea).find('.user_name').text('Username: ' + userManagement.currentUser);
      // for stronger security
      //$('#status_area').trigger('event_startCyclicReading')
    }
    else
    {
      $(userManagement.actionArea).find('a').attr('href', 'login.php');
      $(userManagement.actionArea).find('a').text('Login');
      $(userManagement.actionArea).find('.user_name').text('');
      // for stronger security
      //$("#head_area").find('p').text('');
      //$('#status_area').trigger('event_stopCyclicReading');
    }
  };

  /**
   * create area with login form (on a single content screen)
   */
  var CreateLoginArea = function()
  {
    var html = '';

    html += '<h2>Authentification</h2>';
    html += '<h3>Login</h3>';
    html += '<form id="login_form" action="javascript_requested.php" method="POST">';
    html +=   '<div class="config_form_box">';
    html +=     '<div class="config_data_list">';
    html +=       '<div class="label_value_pair">';
    html +=         '<div class="label_field"><label for="login_username">Username:</label></div>';
    html +=         '<div class="value_field"><input autocomplete="off" type="text" class="input_field" id="login_username" name="username"></div>';
    html +=       '</div>';
    html +=       '<div class="label_value_pair">';
    html +=         '<div class="label_field"><label for="login_password">Password:</label></div>';
    html +=         '<div class="value_field">';
    html +=           '<input autocomplete="off" id="login_password" class="input_field" type="password" name="password" maxlength="32">';
    html +=         '</div>';
    html +=       '</div>';
    html +=     '</div>';
    html +=     '<p class="button_area">';
    html +=       '<input class="button" type="submit" value="Submit" name="submit">';
    html +=     '</p>';
    html +=   '</div>';
    html += '</form>';

    pageBuildup.AddContentArea(userManagement.loginPageId, html);

    // workaround for IE: IE does not trigger submit event after return in input field in case if
    // - form is added dynamically via javascript or
    // - form is added static in html but is "display: none" at the moment, when the submit event handler is added
    // so here we trigger submit on ourselfs at return in input field (WAT12115)
    if("Microsoft Internet Explorer" === window.navigator.appName)
    {
      $('#login_form input').bind('keypress', function(event)
      {
        if('13' == event.keyCode)
          $('#login_form').trigger('submit');
      });
    }

    $('#' + userManagement.loginPageId + '_content').bind('submit', function(event)
    {
      event.preventDefault();
      ProcessLoginRequest();
    });
  };

  /**
   * correlate username and level
   */
  var UserlevelByUsername = function(username)
  {
    switch(username)
    {
      case 'admin': var userlevel = 3;  break;
      case 'user':  var userlevel = 2;  break;
      default:      var userlevel = 0;  break;
    }
    return userlevel;
  };
  UserManagement.prototype.GetCurrentUserlevel = function () {
    return UserlevelByUsername(this.currentUser);
  };

  /**
   * process complete login after form has been send
   */
  var ProcessLoginRequest = function()
  {
    var username = $('#' + userManagement.loginPageId + '_content').find('form#login_form [name=username]').val();
    var password = $('#' + userManagement.loginPageId + '_content').find('form#login_form [name=password]').val();

    // check input values formal (password will be checked severside of course)
    if(!username || !password)
    {
      status  = ERROR;
      $('body').trigger('event_errorOccured', [ 'Please enter username and password.' ]);
    }

    // if someone is logged in actually - log him out first and call login function again via callback
    else if(userManagement.currentUser)
    {
      ProcessLogoutRequest(ProcessLoginRequest);
    }

    // send login request
    else
    {
      deviceCommunication.PostRequest(
      {
        ressource:  'login.php',
        params:     { username: username, password: password },
        callback:   ProcessLoginResponse
      });
    }
  };

  /**
   * Process response of login request - change internal data accordingly and inform depending objects
   */
  var ProcessLoginResponse = function(response)
  {
    if(response.status == userManagement.authFailureId)
    {
      $('body').trigger('event_errorOccured', [ 'Wrong username or password.' ]);
    }
    else if(SUCCESS != response.status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while login.', response.errorText ]);
    }
    else
    {
      userManagement.csrfToken    = response.csrfToken;
      userManagement.currentUser  = response.username; //$('#login_content').find('form#login_form [name=username]').val();
      SetDefaultPasswordFlag(response.isDefaultPW);
      UpdateActionArea();

      deviceCommunication.SetCSRFToken(response.csrfToken);
      pageBuildup.SetCurrentUserlevel(UserlevelByUsername(userManagement.currentUser));
    headStationInfoContent.Init();

      // if referrer is existing and it's not login page itself - go there, otherwise to default page
      if(pageBuildup.referrer && (pageBuildup.referrer != userManagement.ownReferrer))
      {
        pageBuildup.FollowNavLink(pageBuildup.referrer);
      }
      else pageBuildup.FollowNavLink(userManagement.defaultReferrer);
    }

    // reset input values in login form
    $('#' + userManagement.loginPageId + '_content').find('form#login_form [name=username]').val('');
    $('#' + userManagement.loginPageId + '_content').find('form#login_form [name=password]').val('');
  };

  /**
   * process logout - send request to device
   */
  var ProcessLogoutRequest = function(callback)
  {
    deviceCommunication.PostRequest(
    {
      ressource:  'logout.php',
      params:     { csrfToken: userManagement.csrfToken },
      callback:   function(response) { ProcessLogoutResponse(response, callback); }
    });
  };

  /**
   * process response of logout request - change internal data accordingly, inform depending objects
   */
  var ProcessLogoutResponse = function(response, callback)
  {
    if(SUCCESS != response.status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while logout.' ]);
    }
    else if('' != userManagement.currentUser)
    {
      userManagement.csrfToken    = response.csrfToken;
      SetDefaultPasswordFlag("");
      userManagement.currentUser  = '';
      UpdateActionArea();
      deviceCommunication.SetCSRFToken(response.csrfToken);
      pageBuildup.SetCurrentUserlevel(UserlevelByUsername(userManagement.currentUser));

      // go to callback function, if it is given (for example, direct login with another user) or jump to information page
      if(callback) callback();
      else pageBuildup.FollowNavLink(userManagement.defaultReferrer);
    }
  };

  var SecurityDefaultPasswordCheck = function()
  {
    if(userManagement.currentUser && deviceParams.list['default_password_flag_' + userManagement.currentUser])
    {
      // if actual user has not changed his password (via WBM, other changes are not recognized without reload!)
      var defaultPasswordFlag = deviceParams.list['default_password_flag_' + userManagement.currentUser].value;
      if('1' === defaultPasswordFlag)
      {
        //$('body').trigger('event_errorOccured', [ 'Security message: please change your password!' ] );
        if (userManagement.currentUser == 'admin') {
          alert("Security message: please change your password!");
        } else {
          alert("Security message: you are using the default password! Please log in as \"admin\" and change the password.");
        }
      }
    }
  };

  var SetDefaultPasswordFlag = function(value)
  {
    if(userManagement.currentUser && deviceParams.list['default_password_flag_' + userManagement.currentUser])
    {
      deviceParams.list['default_password_flag_' + userManagement.currentUser].value = value;
    }
  };


  /**
   * check login
   */
  var ProcessLoginCheckRequest = function(callback)
  {
    /*deviceCommunication.PostRequest(
    {
      ressource:  'get_session_data.php',
      params:     { csrfToken: userManagement.csrfToken },
      callback:   function(response) { ProcessLoginCheckResponse(response, callback); }
    });*/
  };

  /**
   * process response of login check
   */
  var ProcessLoginCheckResponse = function(response, callback)
  {
    if(SUCCESS != response.status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while checking login data.' ]);
    }
    else
    {

    if(   userManagement.currentUser != response.userName
       || userManagement.csrfToken != response.csrfToken
       || (userManagement.csrfToken != deviceCommunication.GetCSRFToken()))
      {
        //$('body').trigger('event_errorOccured', [ 'Detected invalid session - initiate reload.' ]);
        alert('Detected invalid csrf - reload is necessary.');
        window.location.reload();
        /*
        userManagement.csrfToken    = response.csrfToken;
        userManagement.currentUser  = response.userName;
        deviceCommunication.SetCSRFToken(response.csrfToken);
        UpdateActionArea();
        pageBuildup.SetCurrentUserlevel(UserlevelByUsername(userManagement.currentUser));

        pageBuildup.Reload();
        if(callback) callback();
        */
      }

    }
  };

  /**
   * Set focus on username input when login page is displayed
   */
  var ContentRefresh = function()
   {
     $('#' + userManagement.loginPageId + '_content form#login_form input[name=username]').focus();
   };

  /**
   * Initialisation (via constructor)
   */
  var Init = (function()
  {
    deviceCommunication.SetCSRFToken(userManagement.csrfToken);

    ShowActionAreaHtmlFrame();
    UpdateActionArea();
    CreateLoginArea();
    pageBuildup.SetCurrentUserlevel(UserlevelByUsername(userManagement.currentUser));

    $('#content_area').bind(userManagement.loginPageId + '_refresh', function(event)
    {
      ContentRefresh();
    });

    $(userManagement.actionArea).find('a[href^=log]').bind('click', function(event)
    {
      event.preventDefault();
      if($(event.target).attr('href').match('login'))
      {
        pageBuildup.referrer = location.pathname.replace('/wbm/', '') + location.hash;
        pageBuildup.ChangeContent(userManagement.loginPageId);
      }
      else if($(event.target).attr('href').match('logout'))
      {
        ProcessLogoutRequest();
      }
    });

    $('body').bind('event_pageBuildup_contentChange', function(event)
    {
      SecurityDefaultPasswordCheck();
    });

  //no get_session_data
    //$('body').bind('event_userManagement_loginCheckRequired', function(event)
    //{

      //ProcessLoginCheckRequest();
    //});

  $(document).bind('event_userManagement_logout', function(event)
    {
      ProcessLogoutRequest();
    });

  })();

};
