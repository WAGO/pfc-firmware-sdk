/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var generalConfig =
{
  trace     : false
  //trace     : true
};


// Errornumbers
/*
 * not available in Internet Explorer :-(
 *   
 * const SUCCESS   = 0;
 * const ERROR     = -1;
 *
 */

var SUCCESS   = 0;
var ERROR     = -1;
var ACCESS_NOT_ALLOWED = -2;
var ERROR_SESSION_EXPIRED  = -3;
var ERROR_INVALID_CSRF_TOKEN  = -4;

var WBM_SESSION_KEY  = "wbm_sessId";
var WBM_USER_KEY  = "wbm_user";


var COM_TIMEOUT = -1000;
var COM_NO_AUTH = -1001;


var NOT_FOUND = 6;

var CF_CARD             = "cf-card";
var INTERNAL_FLASH      = "internal-flash";
var INTERNAL_FLASH_NAND = "internal-flash-nand";
var INTERNAL_FLASH_EMMC = "internal-flash-emmc";
var IS_INTERNAL_FLASH   = function(name) { return name == INTERNAL_FLASH_NAND || name == INTERNAL_FLASH_EMMC }
var SD_CARD             = "sd-card";
var USB1                = "usb1";
var USB2                = "usb2";
var USB3                = "usb3";
var USB4                = "usb4";
var USB5                = "usb5";
var USB6                = "usb6";
var USB7                = "usb7";
var USB8                = "usb8";
var NETWORK             = "network";
var UNKNOWN             = "unknown";


// for backup and restore
var DOWNLOAD_DIR                = '/tmp';
var UPLOAD_DIR                  = '/tmp';
var DOWNLOAD_FILENAME_FRAGMENT  = 'firmware_backup';
var CODESYS_FILENAME_FRAGMENT   = 'codesys';
var SETTINGS_FILENAME_FRAGMENT  = 'settings';
var SYSTEM_FILENAME_FRAGMENT    = 'rootfs';


//------------------------------------------------------------------------------
// Get text for device medium for output on screen 
//------------------------------------------------------------------------------
function DeviceMediumOutputText(deviceMediumText)
{
  switch(deviceMediumText)
  {
    case "cf-card":             return "Memory Card";
    case "sd-card":             return "Memory Card";
    case "internal-flash":      return "Internal Flash";
    case "internal-flash-nand": return "Internal Flash";
    case "internal-flash-emmc": return "Internal Flash";
    case "usb1":                return "USB1";
    case "usb2":                return "USB2";
    case "usb3":                return "USB3";
    case "usb4":                return "USB4";
    case "usb5":                return "USB5";
    case "usb6":                return "USB6";
    case "usb7":                return "USB7";
    case "usb8":                return "USB8";
    case "network":             return "Network";
    default:                    return deviceMediumText;
  }
}



/* Copyright (c) 2006 Mathias Bank (http://www.mathias-bank.de)
 * Dual licensed under the MIT (http://www.opensource.org/licenses/mit-license.php) 
 * and GPL (http://www.opensource.org/licenses/gpl-license.php) licenses.
 * 
 * Thanks to Hinnerk Ruemenapf - http://hinnerk.ruemenapf.de/ for bug reporting and fixing.
 */
jQuery.extend({
/**
* Returns get parameters.
*
* If the desired param does not exist, null will be returned
*
* @example value = $.getURLParam("paramName");
*/ 
 getURLParam: function(strParamName){
    var strReturn = "";
    var strHref = window.location.href;
    var bFound=false;
    
    var cmpstring = strParamName + "=";
    var cmplen = cmpstring.length;

    if ( strHref.indexOf("?") > -1 ){
      var strQueryString = strHref.substr(strHref.indexOf("?")+1);
      var aQueryString = strQueryString.split("&");
      for ( var iParam = 0; iParam < aQueryString.length; iParam++ ){
        if (aQueryString[iParam].substr(0,cmplen)==cmpstring){
          var aParam = aQueryString[iParam].split("=");
          strReturn = aParam[1];
          bFound=true;
          break;
        }
        
      }
    }
    if (bFound==false) return null;
    return strReturn;
  }
});


jQuery.extend({
  /**
  * Returns a list with an array of entries, which are unique elements of only the second given (extended) array 
  * and not the first given array
  *
  * @example array = $.getUniqueEntriesOfExtendedList(array original list, array extended list);
  */
   ip2Number: function(ipString) 
   {
     var d = ipString.split('.');
     return((((((+d[0])*256)+(+d[1]))*256)+(+d[2]))*256)+(+d[3]);
   },
   
   getUniqueEntriesOfExtendedList: function(originalList, extendedList, format)
   {
     var format             = format || 'string';
     var extendedEntryList  = new Array();
     
     // examine all entries of extended list
     $.each(extendedList, function(extendedListEntryIndex, extendedListEntry)
     {
       var entryFoundInOriginalList = false;
       
       // check if extended list entry is also included in original list
       $.each(originalList, function(originalListEntryIndex, originalListEntry)
       {
         if('ip' === format)
         {
           if($.ip2Number(originalListEntry) === $.ip2Number(extendedListEntry))
           {
             entryFoundInOriginalList = true;
           }
         }
         else if('string' === format)
         {
           if(originalListEntry === extendedListEntry)
           {
             entryFoundInOriginalList = true;
           }
         }
       });
       
       // if entry of extended list is unique - put it to output list
       if(false === entryFoundInOriginalList)
       {
         extendedEntryList.push(extendedListEntry);
       }
     });

     return extendedEntryList;
    }

    /*
    getUniqueEntriesOfExtendedList: function(originalList, extendedList)
    {
      var extendedEntryList = new Array();
     
      // examine all entries of extended list
      $.each(extendedList, function(extendedListEntryIndex, extendedListEntry)
      {
        var entryFoundInOriginalList = false;
       
        // check if extended list entry is also included in original list
        $.each(originalList, function(originalListEntryIndex, originalListEntry)
        {
          if(originalListEntry === extendedListEntry)
          {
            entryFoundInOriginalList = true;
          }
        });
       
        // if entry of extended list is unique - put it to output list
        if(false === entryFoundInOriginalList)
        {
          extendedEntryList.push(extendedListEntry);
        }
      });

      return extendedEntryList;
    }
    */
  });


Log = function(message)
{
  if((generalConfig.trace) && (typeof(console) != 'undefined'))
  {
    if(typeof(message) != 'object')
    {
      console.log(message);
    }
    else
    {
      console.dir(message);
    }
  }
}
  
  
/*
function GetPageSize()
{
  var xScroll, yScroll;

  if (window.innerHeight && window.scrollMaxY)
  {  
    xScroll = window.innerWidth + window.scrollMaxX;
    yScroll = window.innerHeight + window.scrollMaxY;
  } 
  else if (document.body.scrollHeight > document.body.offsetHeight)
  { // all but Explorer Mac
    xScroll = document.body.scrollWidth;
    yScroll = document.body.scrollHeight;
  } 
  else 
  { // Explorer Mac...would also work in Explorer 6 Strict, Mozilla and Safari
    xScroll = document.body.offsetWidth;
    yScroll = document.body.offsetHeight;
  }

  var windowWidth, windowHeight;

  if (self.innerHeight) 
  { // all except Explorer
    if(document.documentElement.clientWidth)  
    {
      windowWidth = document.documentElement.clientWidth; 
    } 
    else 
    {
      windowWidth = self.innerWidth;
    }
    windowHeight = self.innerHeight;
  } 
  else if (document.documentElement && document.documentElement.clientHeight) 
  { // Explorer 6 Strict Mode
    windowWidth = document.documentElement.clientWidth;
    windowHeight = document.documentElement.clientHeight;
  } 
  else if (document.body) 
  { // other Explorers
    windowWidth = document.body.clientWidth;
    windowHeight = document.body.clientHeight;
  } 

  // for small pages with total height less then height of the viewport
  if(yScroll < windowHeight)
  {
    pageHeight = windowHeight;
  } 
  else 
  { 
    pageHeight = yScroll;
  }

  // for small pages with total width less then width of the viewport
  if(xScroll < windowWidth)
  {  
    pageWidth = xScroll;    
  }
  else 
  {
    pageWidth = windowWidth;
  }

  return [pageWidth,pageHeight];
}
*/