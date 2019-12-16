<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

//------------------------------------------------------------------------------
// constants and global variables
//------------------------------------------------------------------------------
define("SUCCESS", 0);
define("ERROR", -1);
define("ACCESS_NOT_ALLOWED", -2);
define("SESSION_EXPIRED", -3);
define("CSRF_TOKEN_INVALID", -4);
define("SESSION_LIFETIME", 900); //in seconds
define("USER_USER", "user");
define("USER_ADMIN", "admin");
const CSRF_TOKEN_LENGTH = 48;

function system_uptime ()
{
  $proc_uptime = file_get_contents( '/proc/uptime' );
  $uptime = explode( ' ', $proc_uptime )[0];
  return $uptime;
}

// Handle Session Lifetime
function Session_HandleSessionLifeTime($csrfToken, $shouldResetSessionTimeout)
{
  $status     = SUCCESS;
  @session_start();

  if(Session_CheckClientInformation())
  {
    if(Session_CheckCSRFToken($csrfToken))
      $status = Session_CheckSessionLifeTime($shouldResetSessionTimeout);
    else // invalid csrfToken
      $status = CSRF_TOKEN_INVALID;
  }
  else // no active session for client
  {
    $status = ACCESS_NOT_ALLOWED;
  }

  return $status;
}

function StartLoginSession($username)
{
  session_start();
  session_unset();
  session_regenerate_id(true);
  $_SESSION['username']  = $username;
  $_SESSION['timeout'] = system_uptime() + SESSION_LIFETIME;
  $_SESSION["csrf_token"] = Session_GenerateCSRFToken();
  $_SESSION['userAgent'] = $_SERVER['HTTP_USER_AGENT'];
}

function Session_DestroySession()
{
  session_unset();
  if (ini_get("session.use_cookies")) // delete cookie
  {
    $params = session_get_cookie_params();
    setcookie(  session_name(), '', time() - 42000,
          $params["path"],
          $params["domain"]
          //$params["secure"],
          //$params["httponly"]
         );
  }
  session_destroy();
  unset($_SESSION['username']);
  unset($_SESSION['userlevel']);
  unset($_SESSION['timeout']);
  unset($_SESSION['csrf_token']);
  unset($_SESSION['userAgent']);
}

function Session_CheckSessionLifeTime($shouldResetSessionTimeout)
{
  $status = SUCCESS;
  $uptime = system_uptime();

  if($uptime > $_SESSION['timeout'])
    $status = SESSION_EXPIRED;
  else if ($shouldResetSessionTimeout)
    $_SESSION['timeout'] = $uptime+ SESSION_LIFETIME; // 15 min

  return $status;
}

function Session_GenerateCSRFToken()
{
    $csrfTokenChars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    $maxRand = strlen($csrfTokenChars) - 1;
    $csrfToken='';

    for ($i = 0; $i < CSRF_TOKEN_LENGTH; $i++)
    {
        $csrfToken .= $csrfTokenChars{intval(mt_rand(0.0, $maxRand))};
    }

    return $csrfToken;
}

function Session_CheckCSRFToken($csrfToken)
{
  $status = false;

  if(isset($_SESSION["csrf_token"]))
  {
     if($_SESSION["csrf_token"] == $csrfToken)
      $status = true;
  }
  else  // no login
  {
     if(!ini_get("session.use_cookies"))
      Session_DestroySession();
  }

  return $status;
}

function Session_CheckClientInformation()
{
  $isAccessAllowed = false;
  if(isset($_SESSION['timeout']) && Session_CheckUserAgent())
    $isAccessAllowed = true;

  return $isAccessAllowed;
}

function Session_CheckUserAgent()
{
  $isAccessAllowed = false;

  if($_SESSION['userAgent'] == $_SERVER['HTTP_USER_AGENT'])
    $isAccessAllowed = true;

  return $isAccessAllowed;
}

function Session_GetErrorTxt($cmdStatus)
{
  $errorText = "";
  switch($cmdStatus)
  {
    case ACCESS_NOT_ALLOWED:  // no session
      $errorTxt = "Access not allowed.";
      break;

    case SESSION_EXPIRED:    // session active and valid csrf token but session expired
      $errorTxt = "Session Expired";
      break;

    case CSRF_TOKEN_INVALID:  // session active but invalid csrf token
      $errorTxt = "CSRF token is invalid";
      break;

    default:
      $errorTxt = "";
      break;
  }
  return $errorTxt;
}

function isDefaultSessionUserPW($username)
{
  $isDefaultPW = "";
  if(!empty(session_id()))
  {
    if( "" != $_SESSION['username'])
    {
      if($_SESSION['username'] == $username)
      {
        $execString = "sudo /etc/config-tools/get_user_info --is-default-pwd '" . $username . "'";
        $isDefaultPW = exec($execString);
      }
    }
  }
  return $isDefaultPW;
}

?>
