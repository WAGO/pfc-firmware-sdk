<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

// don't show any errors in output stream
error_reporting(0);

//------------------------------------------------------------------------------
// constants and global variables
//------------------------------------------------------------------------------

define("SUCCESS", "0");
define("ERROR", "-1");
define("ACCESS_NOT_ALLOWED", -2);

define("CF_CARD", "cf-card");
define("INTERNAL_FLASH", "internal-flash");
define("INTERNAL_FLASH_NAND", "internal-flash-nand");
define("INTERNAL_FLASH_EMMC", "internal-flash-emmc");
define("SD_CARD", "sd-card");
define("USB1", "usb1");
define("USB2", "usb2");
define("USB3", "usb3");
define("USB4", "usb4");
define("USB5", "usb5");
define("USB6", "usb6");
define("USB7", "usb7");
define("USB8", "usb8");
define("NETWORK", "network");
define("UNKNOWN", "unknown");


$status               = SUCCESS;
$errorText            = "";


function GetClientMacAddress()
{
  $clientMac = "";
  /*
  // get ip address of client via server data array, get according mac address via arp
  $resultString = exec(sprintf("arp -a %s -n", $_SERVER["REMOTE_ADDR"]), $dummyOutput, $status);

  // filter mac address from arp result line
  @preg_match("/([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})/", $resultString, $matches);
  
  $clientMac = isset($matches[0]) ? $matches[0] : "";
  */
  return $clientMac;
}


//------------------------------------------------------------------------------
// function for page design
//------------------------------------------------------------------------------

function WBM_PrintHeadArea()
{
  printf("\n    <div id=\"head_area\">\n");
      
  printf("\n      <h1>Web-based Management</h1>\n");
  
  $couplerInfoText = "";//exec("sudo /etc/config-tools/get_typelabel_value SYSDESC");
  printf("      <p id=\"value_coupler_info_text\">%s</p>\n", $couplerInfoText);
  printf("    </div>\n");
}


function WBM_PrintNavFrame()
{
  printf("\n    <div id=\"main_nav\">\n");
  printf("      <h3>Navigation</h3>\n");
  printf("        <ul>\n");
  printf("        </ul>\n");
  printf("      <!-- filled by javaScript -->\n");
  printf("    </div>\n");
}


function WBM_PrintStatusFrame()
{
  include("page_elements/state_area.inc.php");
}

function WBM_PrintHeadline()
{
  //system("cat headline.html");
  printf("    <h1>Web Based Management</h1>\n");
}


function WBM_PrintFooter()
{
  printf("\n    <div id=\"footer_area\">\n");
  printf("      <p class=\"footer\">WAGO &#8226 Hansastr. 27 &#8226 D-32423 Minden &#8226 WAGO is a registered trademark of WAGO Verwaltungsgesellschaft mbH.\n"); 
  printf("    </div>\n");
  return;
}


function DeviceMediumOutputText($deviceMediumText)
//
// Get text for device medium for output on screen
//
{ 
  $aDeviceMediumText = array(INTERNAL_FLASH       => "Internal Flash", 
                             INTERNAL_FLASH_NAND  => "Internal Flash",
                             INTERNAL_FLASH_EMMC  => "Internal Flash",
                             CF_CARD              => "CF Card",
                             SD_CARD              => "SD Card",
                             USB1                 => "USB1",
                             USB2                 => "USB2",
                             USB3                 => "USB3",
                             USB4                 => "USB4",
                             USB5                 => "USB5",
                             USB6                 => "USB6",
                             USB7                 => "USB7",
                             USB8                 => "USB8",
                             NETWORK              => "Network");

  $activePartitionText = UNKNOWN;
  if(array_key_exists($deviceMediumText, $aDeviceMediumText))
  { 
    $activePartitionText = $aDeviceMediumText[$deviceMediumText];
  }
  else
  {
    $activePartitionText = $deviceMediumText;
  }
  
  return $activePartitionText;
}


 
?>