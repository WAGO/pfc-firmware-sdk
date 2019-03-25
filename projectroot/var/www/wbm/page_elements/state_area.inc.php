<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

// means: don't initialize LEDs by PHP, but other values (time, date, ...) anyway! 
$initByPhp = false;

//----------------------------------------------------------------------------
// Get fileanme of images which belongs to a specified LED state
//----------------------------------------------------------------------------
function GetImgFileName($ledStateString)
{
  $aLedStateImageAssign["OFF"]           = "led_gray_12.gif";
  $aLedStateImageAssign["GRN"]           = "led_green_12.gif";
  $aLedStateImageAssign["RED"]           = "led_red_12.gif";
  $aLedStateImageAssign["YLW"]           = "led_yellow_12.gif";
  $aLedStateImageAssign["BLINK_GRN"]     = "led_green_gray_12.gif";
  $aLedStateImageAssign["BLINK_RED"]     = "led_red_gray_12.gif";
  $aLedStateImageAssign["BLINK_YLW"]     = "led_yellow_gray_12.gif";
  $aLedStateImageAssign["BLINK_GRN_RED"] = "led_green_red_12.gif";
  $aLedStateImageAssign["BLINK_GRN_YLW"] = "led_green_yellow_12.gif";
  $aLedStateImageAssign["BLINK_RED_YLW"] = "led_red_yellow_12.gif";
  $aLedStateImageAssign["BLINK_OFF_GRN"] = "led_green_gray_12.gif";
  $aLedStateImageAssign["BLINK_OFF_RED"] = "led_red_gray_12.gif";
  $aLedStateImageAssign["BLINK_OFF_YLW"] = "led_yellow_gray_12.gif";
  $aLedStateImageAssign["BLINK_RED_GRN"] = "led_red_green_12.gif";
  $aLedStateImageAssign["BLINK_YLW_GRN"] = "led_yellow_green_12.gif";
  $aLedStateImageAssign["BLINK_YLW_RED"] = "led_yellow_red_12.gif";
  
  /* not necessary to define currently - still gray
  $aLedStateImageAssign["ERR_SEQ_750"]   = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_GRN_OFF"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_RED_OFF"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_YLW_OFF"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_GRN_RED"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_GRN_YLW"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_RED_GRN"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_RED_YLW"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_YLW_GRN"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_YLW_RED"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_OFF_GRN"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_OFF_RED"] = "led_gray_12.gif";
  $aLedStateImageAssign["FLASH_OFF_YLW"] = "led_gray_12.gif";
  */
  $aLedStateImageAssign["SPECIAL_BLINK"] = "led_green_red";
  $aLedStateImageAssign["NOT_SET_LED"]   = "led_gray_12.gif";

  $imgFileName = 'led_gray_12.gif';
  if($aLedStateImageAssign[$ledStateString]) $imgFileName = $aLedStateImageAssign[$ledStateString];
  
  return $imgFileName;
}
  

//----------------------------------------------------------------------------
// Get HTML code for LED area
//----------------------------------------------------------------------------
function GetLedAreaHtml()
{
  $ledAreaHtml  = "";
  $ledNo        = 0;
  $ledName      = "";
  $ieClassText  = "";
    
  $ledName = exec(sprintf("/etc/config-tools/get_led_config name %d", $ledNo));
    
  // loop over all LEDs we can find, get complete state text, filter relevant values and transfer them to html structure
  while(strlen($ledName))
  {
    $ieClassText = ($ledNo % 2) ? ' class="right"' : '';

    $ledCompleteState = exec(sprintf("/etc/config-tools/get_led_config complete-state %s", $ledName));
    $aSubStrings      = explode(' ', $ledCompleteState, 5);
    //printf("ledCompleteState:%s<br>", $ledCompleteState);
    //var_dump($aSubStrings);

    // values not needed actually:
    //$ledName     = str_replace(':', '', $aSubStrings[0]);
    $stateString = $aSubStrings[1];
    //$frequency1  = $aSubStrings[2];
    //$frequency2  = $aSubStrings[3];

    $diagText    = str_replace('"', '\'', $aSubStrings[4]);

    $ledAreaHtml = $ledAreaHtml.sprintf("        <li %s>\n", $ieClassText);
    $ledAreaHtml = $ledAreaHtml.sprintf("          <a class=\"tooltip_link\" href=\"led_state_%s\" title=\"%s\" alt=\"off\">",
                                                    $ledName, $diagText);
    $ledAreaHtml = $ledAreaHtml.sprintf("<img height=\"12\" src=\"/wbm/images/%s\" alt=\"off\"></a>\n", GetImgFileName($stateString));
    $ledAreaHtml = $ledAreaHtml.sprintf("          %s", $ledName);
    $ledAreaHtml = $ledAreaHtml.sprintf("        </li>\n");
      
    ++$ledNo;
    $ledName = exec(sprintf("/etc/config-tools/get_led_config name %d", $ledNo));
  }
  
  return $ledAreaHtml;
}

$timeLocal          = "";
$dateLocal          = "";
$runStopReset       = "";

// get a few values always by php
$timeLocal          = exec("/etc/config-tools/get_clock_data time-local");
$dateLocal          = exec("/etc/config-tools/get_clock_data date-local");
$runStopReset       = exec("sudo /etc/config-tools/get_run_stop_switch_value");

// cut seconds from time value
$timeLocal          = preg_replace('/:..$/', '', $timeLocal);

$ledAreaHtml        = "";
    
if($initByPhp)
{
  //$ledAreaHtml  = GetLedAreaHtml();
  //printf("<div id=\"state_area_init_by_php\"></div>\n");
}

printf("\n    <div id=\"status_area\">\n");

// preload images to signalize lost connection here - they can't be loaded when connection is lost! - but don't display them anyway
printf("      <div style=\"display:none\"><img src=\"images/wait_frame_red.gif\"><img src=\"images/wait_red.gif\"></div>\n");

printf("      <h3>Status</h3>\n");
printf("      <ul>\n");
printf("        <li>WBM<span class=\"value\"><span id=\"value_wbm_state\"><img class=\"wbm_state\" src=\"images/wait_frame_green.gif\" alt=\"Status OK\"></span></span>\n");
printf("        <li>Local Time <span class=\"value\" id=\"value_status_time_local\">%s</span></li>\n", $timeLocal);
printf("        <li>Local Date <span class=\"value\" id=\"value_status_date_local\">%s</span></li>\n", $dateLocal);
printf("        <li>PLC Switch<span class=\"value\" id=\"value_run_stop_reset\">%s</span></li>\n", $runStopReset);
  
printf("          <li>\n");
printf("            LEDs\n");
printf("            <div id=\"led_area\" class=\"value\">\n");
printf("              <ul>\n");
printf("                %s\n", $ledAreaHtml);
printf("              </ul>\n");
printf("            </div>\n");
printf("          </li>\n");
 
printf("      </ul>\n");
printf("    </div>\n");

?>

