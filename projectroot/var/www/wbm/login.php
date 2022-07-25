<?php

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include("session_lifetime.inc.php");
define("MISSING_PARAMETER", 1);
define("AUTH_FAILURE", 20);

define("PASSWORD_FILENAME", "/etc/lighttpd/lighttpd-htpasswd.user");

function PasswordCorrect($passwd_file, $username, $passwd)
{
  $passwd_correct = false;

  $passwd_fd = fopen( $passwd_file, 'r' );
  if($passwd_fd)
  {
    while(($line = fgets($passwd_fd)) !== false)
    {
      $passwd_fields = explode(':', trim($line));

      if($passwd_fields[0] === $username)
      {
        $passwd_correct = password_verify( $passwd, $passwd_fields[1]);
        break;
      }
    }

    fclose($passwd_fd);
  }

  return $passwd_correct;
}

function Login()
{
  $status     = ERROR;
  $errorText  = "";
  $csrfId   = "";
  $username = "";
  $isDefaultPW = "";
  // get request json string by stdin
  $requestString  = file_get_contents("php://input");
  //echo "requestString:".$requestString;

  if($requestString)
  {
    // decode string to php array/object
    $request  = json_decode($requestString, true);
    //var_dump($request);

    if(isset($request["username"]))
      $username = htmlspecialchars($request["username"], ENT_QUOTES);

    if(!isset($username) || !isset($request["password"]))
    {
      $status     = MISSING_PARAMETER;
      $errorText  = "Invalid input";
    }

    else if(!PasswordCorrect(PASSWORD_FILENAME, $username, $request["password"]))
    {
      $status    = AUTH_FAILURE;
      //$errorText = "Wrong username or password";
    }
    else
    {
      StartLoginSession($username);
      $isDefaultPW = isDefaultSessionUserPW($username);
      $csrfId = $_SESSION["csrf_token"];
      $status                = SUCCESS;
    }
  }

  // encode responses to json string and send it to stdout
  $resultObj = array('status'    	 => $status,
                     'csrfToken' 	 => $csrfId,
                     'username'  	 => $username,
                     'isDefaultPW' => $isDefaultPW,
                     'errorText' 	 => $errorText);

  echo json_encode($resultObj);
}

Login();

?>
