<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include("wbm_lib.inc.php");


function GetSessionData()
{
  $status     = SUCCESS;
  $errorText  = "";

  // get request json string by stdin
  $requestString  = file_get_contents("php://input");
  //echo "requestString:".$requestString;

  if($requestString)
  {
    // decode string to php array/object
    $request = json_decode($requestString, true);
    //var_dump($request);
  
    $sessionId = isset($request["sessionId"]) ? $request["sessionId"] : '';
  
    session_id($sessionId);
    @session_start();
    
    $userName  = isset($_SESSION["username"])  ? $_SESSION["username"]  : "";
    $userLevel = isset($_SESSION["userlevel"]) ? $_SESSION["userlevel"] : "0";
  }
                           
  //echo json_encode($aResponse);

  // encode responses to json string and send it to stdout
  $resultObj = array('status'  => $status, 'sessionId' => session_id(), 'errorText' => $errorText, 'userName' => $userName, 'userLevel' => $userLevel);
  echo json_encode($resultObj);
}

GetSessionData();
  
?>