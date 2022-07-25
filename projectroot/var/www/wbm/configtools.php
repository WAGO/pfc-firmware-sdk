<?php

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include("session_lifetime.inc.php");
include("ctperm.inc.php");

//define("MOCK", true);
define("MOCK", false);
define("USERLEVEL_GUEST", 0);
define("USERLEVEL_USER",  2);
define("USERLEVEL_ADMIN", 3);

function MockConfigtool($callString, &$dummy, &$status)
{
  $status       = 0;
  $resultString = "";

  switch($callString)
  {
    // read tools

    case "/etc/config-tools/get_coupler_details order-number":

      $resultString = "0758-0874-0000-0111";
      break;

    case "/etc/config-tools/get_actual_eth_config eth0 ip-address":

      $resultString = "192.168.1.17";
      break;

    // write tools

    case "/etc/config-tools/change_hostname hostname=test":

      $resultString = "";
      break;

    case "/etc/config-tools/change_hostname hostname=te+st":

      $resultString = "";
      break;

    case "/etc/config-tools/config_interfaces interface=eth0 ip-address=192.168.1.18":

      $resultString = "";
      break;

    // provocated fails

    case "/etc/config-tools/timeout":

      sleep(2);
      $resultString = "output after long time";
      break;

    case "/etc/config-tools/empty_string":

       $resultString = "";
       break;

     case "/etc/config-tools/time":

       $resultString = sprintf("%d", time());
       break;

    case "/etc/config-tools/no_jason_return":

      echo "bla";
      die();

    case "/etc/config-tools/get_touchscreen_config display-state":

      $resultString = "off";
      break;

    default: $status = -1;
  }

  return $resultString;
}

function ConfigtoolNameInvalid($name)
{
  $nameInvalid = false;

  if(strpos($name, "/") !== false)
  {
    $nameInvalid = true;
  }

  return $nameInvalid;
}

function CallConfigtool($configtoolObj, &$resultObj, $username = "", $cmdStatus )
{
  $status       = SUCCESS;
  $resultString = "";
  $errorText    = "";
  $callString   = "";
  $dataId       = isset($configtoolObj["dataId"]) ? $configtoolObj["dataId"] : "";

  if(!isset($configtoolObj))
  {
    $status     = ERROR;
    $errorText  = "Missing input";
  }
  else if(ConfigtoolNameInvalid($configtoolObj["name"]))
  {
    $status     = ERROR;
    $errorText  = "Invalid configtool name (".$configtoolObj["name"].")";
  }
  else if(!file_exists(sprintf("/etc/config-tools/%s", $configtoolObj["name"])))
  {
    $status     = ERROR;
    $errorText  = "Unknown configtool (".$configtoolObj["name"].")";
  }

  // check if access is allowed in related to user level
  else if(!AccessAllowed($configtoolObj["name"], $username))
  {
    $status     = ACCESS_NOT_ALLOWED;
    $errorText  = "Access not allowed.";
  }

  // check, if cfrs token is ok
  // NOTE: actually, user "guest" (= username is empty, no session is existing) is allowed to read a few params, so
  // we don't check crsr here, if access was basically allowed for this configtool
  // we will change that later on in case of stronger security configuration
  else if(strlen($username) && (SUCCESS != $cmdStatus))
  {
    $status    = $cmdStatus;
    $errorText = Session_GetErrorTxt($status);
  }
  else
  {
    // by now, generally call all configtools with sudo
    $callString = "sudo ";

    // create string to call configtool by linux shell - first directory and configtool name
    $callString = $callString."/etc/config-tools/".$configtoolObj["name"];

    // now all configtool parameters, one after the other
    $paramNo = 0;
    while(isset($configtoolObj["parameter"][$paramNo]))
    {
      $paramString = $configtoolObj["parameter"][$paramNo];
      
      // When data is sent to the client we replace special chars with HTML
      // entities. Most Config-Tools do not expect such masking of characters
      // and thus we have to revert the changes here, else we end up with
      // data encoded twice.
      $paramString = htmlspecialchars_decode($paramString, ENT_QUOTES);
      
      // decode parameter in url format (to save spaces eg.), but '=' (changed to "%3d") must be decoded again
      // decode '%' and '+', because they have special meanings in the encoded characters
      $paramString = str_replace("%", "%25", $paramString);
      $paramString = str_replace("+", "%2b", $paramString);

      // decode characters in url format, which otherwise confuse the bash
      $paramString = str_replace(" ", "%20", $paramString);
      $paramString = str_replace("'", "%27", $paramString);
      $paramString = str_replace('"', "%22", $paramString);
      $paramString = str_replace('`', "%60", $paramString);
      $paramString = str_replace('*', "%2a", $paramString);
      $paramString = str_replace('$', "%24", $paramString);

      // escape parameter string for bash
      // (add (single!) quotation marks around parameter string to let the bash don't interprete "(", ")", and so on
      // and mask quotation mark itself inside string)
      // "escapeshellarg" does not work, because it strippes UTF8 characters from string and setlocale does not help
      $paramString = "'".str_replace("'", "'\\''", $paramString)."'";

      // add parameter to call string
      $callString = $callString." ".$paramString;

      ++$paramNo;
    }

    // multiline output expected - use special shell call, which transfers more than one output line
    if(isset($configtoolObj["multiline"]) && $configtoolObj["multiline"])
    {
      $resultString = shell_exec($callString);
    }
    // normal call of configtool
    else
    {
      $dummyOutput = '';
      $resultString = exec($callString, $dummyOutput, $status);
    }

    if($resultString == NULL)
    {
      $resultString = "";
    }
    else
    {
      // In all there comes data in four different formats:
      //   - Text ("lorem ipsum")
      //   - XML ("<?xml ...> ... </ ...>")
      //   - JSON ("{ ... }")
      //   - JSON Array ("[ ... ]")
      //
      // In case of Text we replace all special characters with the
      // corresponding HTML entities, in case of XML we do nothing
      // as we expect to be valid and for JSON/JSON Array we do not
      // escape quotes.
      if(isJSON($resultString) || isJSONArray($resultString))
      {
        // JSON [Array]
        $resultString = htmlspecialchars($resultString, ENT_NOQUOTES);
      }
      else if(!isXML($resultString))
      {
        // text
        $resultString = htmlspecialchars($resultString, ENT_QUOTES);
      }
    }

    if(0 != $status)
    {
      $lastErrorFilename = "/tmp/last_error.txt";
      if(file_exists($lastErrorFilename))
      {
        $errorText = file_get_contents($lastErrorFilename);
        exec(sprintf("sudo /bin/rm %s", $lastErrorFilename));
      }
    }
  }

  // to return call string (only!) for debugging, comment out next line meanwhile
  $callString = "";

  $resultObj = array('status'       => $status,
                     'resultString' => $resultString,
                     'errorText'    => htmlspecialchars($errorText, ENT_QUOTES),
                     'dataId'       => htmlspecialchars($dataId, ENT_QUOTES),
                     'callString'   => htmlspecialchars($callString, ENT_QUOTES) );
}

function CallConfigtoolByJson()
{
  // get request json string by stdin
  $requestString    = file_get_contents("php://input");
  $username         = "";
  $csrfId = "";
  $cmdStatus = "";

  if($requestString)
  {
    $configtoolNo = 0;
    // decode string to php array/object
    $aRequest = json_decode($requestString, true);

    if(isset($aRequest["csrfToken"]))
    {
      $csrfId = htmlspecialchars($aRequest["csrfToken"], ENT_QUOTES);
      $aDeviceParams = isset($aRequest["aDeviceParams"]) ? $aRequest["aDeviceParams"] : array();
      // cyclic clock_data command has to be ignored because of session lifetime
      $cmdStatus = Session_HandleSessionLifeTime($csrfId, $aRequest["renewSession"]);
    }

    $username = isset($_SESSION["username"]) ? $_SESSION["username"] : "";

    while(isset($aDeviceParams[$configtoolNo]))
    {
      CallConfigtool($aDeviceParams[$configtoolNo], $aDeviceResponse[$configtoolNo], $username, $cmdStatus);
      ++$configtoolNo;
    }
  }

  if(isset($aRequest["csrfToken"]))
  {
    //test_csrf
    $aResponse  = array("csrfToken" => $csrfId, "aDeviceResponse" => $aDeviceResponse);
  }
  else
  {
    $aResponse = isset($aDeviceResponse) ? $aDeviceResponse : false;
  }

  // encode responses to json string and send it to stdout
  echo json_encode($aResponse);
}

// for session lifetime
function isCycClockCommand($aDeviceParams)
{
  // pattern for controller infos (cyclic command pattern)
  $cycClockCmds = array('get_clock_data','get_clock_data','get_run_stop_switch_value','get_led_config');
  $cycClockCmdMax = count($cycClockCmds);
  $cycClockCmdCount = 0;
  $isCycClockCmd = false;
  while(isset($aDeviceParams[$cycClockCmdCount]))
  {
    $configToolObj = $aDeviceParams[$cycClockCmdCount];
    if($cycClockCmdMax > $cycClockCmdCount)
    {
      if($configToolObj["name"] == $cycClockCmds[$cycClockCmdCount])
      {
        ++$cycClockCmdCount;
      }
      else
        break;
    }
    else
    {
      $cycClockCmdCount = 0;
      break;
    }
  }

  if($cycClockCmdMax == $cycClockCmdCount)
    $isCycClockCmd = true;

  return $isCycClockCmd;
}

function startsWith($haystack, $needle)
{
     $length = strlen($needle);
     return (substr($haystack, 0, $length) === $needle);
}

function endsWith($haystack, $needle)
{
    $length = strlen($needle);
    return $length === 0 ||
    (substr($haystack, -$length) === $needle);
}

// detect if text is in JSON format
function isJSON($text)
{
  // without parsing we cannot be sure it is JSON,
  // but it should be sufficient if text starts and
  // ends with curly brackets
  return startsWith(trim($text), '{') && endsWith(trim($text), '}');
}

// detect if text is a JSON array
function isJSONArray($text)
{
  // some Config-Tools return their data in JSON array
  // notation, then we also may not escape quotes. We
  // assume if text starts and ends with square brackets
  // it is a JSON array.
  return startsWith(trim($text), '[') && endsWith(trim($text), ']');
}

// detect if text is in XML format
function isXML($text)
{
  // if the text starts with '<?xml' and ends with '>'
  // we assume this is some data in XML format
  return startsWith(trim($text), '<?xml') && endsWith(trim($text), '>');
}


CallConfigtoolByJson();

?>
