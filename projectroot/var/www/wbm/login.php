<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include("session_lifetime.inc.php");
define("MISSING_PARAMETER", 1);
define("AUTH_FAILURE", 20);

define("PASSWORD_FILENAME", "/etc/lighttpd/lighttpd-htpasswd.user");
#define("PASSWORD_FILENAME", "/etc/shadow");



function PasswordCorrect($passwordFilename, $username = '', $password = '')
{
  $pwCorrect  = false;
  //var_dump($username); var_dump($password);

  // get password file and iterate over every line
  $pwFileArray = file($passwordFilename);

  foreach($pwFileArray as $lineNo => $pwFileLine)
  {
    //var_dump($pwFileLine);
    // if username was found, filter password key
    if(preg_match("/^".$username.":/", $pwFileLine))
    {
      $pwFileKey  = str_replace($username.":", "", $pwFileLine);
      $pwFileKey  = str_replace("\n", "", $pwFileKey);

      if (preg_match('/\$(?P<algo>\d)\$(?P<salt>[a-zA-Z0-9\.\/]{2,16})\$(?<hash>.*)/', $pwFileKey, $userInfo))
      {
        $algo = $userInfo["algo"];
        $salt = $userInfo["salt"];
        //$hash = $userInfo["hash"];

        // evaluate key of password given by user and compare both
        $userKey    = crypt($password, "$".$algo."$".$salt);

        //var_dump($pwFileKey); printf("\n"); var_dump($userKey);   printf("\n");
        if($pwFileKey == $userKey)
        {
          $pwCorrect = true;
        }
      }
    }
  }

  return $pwCorrect;
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
