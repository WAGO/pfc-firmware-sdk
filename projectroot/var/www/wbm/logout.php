<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include("session_lifetime.inc.php");

function Logout()
{
  $status     = SUCCESS;
  $errorText  = "";
  $csrfToken   = "";
  $sessionStatus = '';
  // get request json string by stdin
  $requestString  = file_get_contents("php://input");
  //echo "requestString:".$requestString;

  if($requestString)
  {
    // decode string to php array/object
    $request = json_decode($requestString, true);
    //var_dump($request);
  
    if(isset($request["csrfToken"]))
    {
      $csrfToken = htmlspecialchars($request["csrfToken"], ENT_QUOTES);
    }

    $sessionStatus = Session_HandleSessionLifeTime($csrfToken, false);
    if( SUCCESS == $sessionStatus || SESSION_EXPIRED == $sessionStatus)
    {
      Session_DestroySession();
    }
    /*
    else
    {
      $status = $sessionStatus;
      $errorText = Session_GetErrorTxt($status);
    }
    */
  }

  //echo json_encode($aResponse);

  // encode responses to json string and send it to stdout
  $resultObj = array("status"  => $status, "errorText" => $errorText, "csrfToken" => "");
  echo json_encode($resultObj);
}

Logout();
  
?>