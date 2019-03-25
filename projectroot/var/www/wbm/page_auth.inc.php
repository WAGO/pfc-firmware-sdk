<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

  include_once("session_lifetime.inc.php");

  //if(!ini_get("session.use_cookies")) //no authentication without cookies
  //exit;

  $requestedSource = str_replace("/wbm/page_elements/", "", htmlspecialchars($_SERVER['SCRIPT_NAME'], ENT_QUOTES));
  $accessRightsAdmin = array
  (
      'snmp_general.inc.php',
      'snmp_v1_v2c.inc.php',
      'snmp_v3.inc.php',
      'snmp_v1_v2c.inc.php',
      'users.inc.php',
      'rs232.inc.php',
      'create_image.inc.php',
      'start_reboot.inc.php',
      'firmware_backup.inc.php',
      'firmware_restore.inc.php',
      'system_partition.inc.php',
      'mass_storage.inc.php',
      'software_uploads.inc.php',
      'vpn.inc.php',
      'service_interface.inc.php',
      'security.inc.php',
      'opcua.inc.php',
      'cloud_connectivity.inc.php',
      'bacnet_diagnostic.inc.php',
      'bacnet_general_configuration.inc.php',
      'bacnet_storage_location.inc.php'
  );

  $accessRightsGuest = array(
      'information.inc.php',
      'state_area.inc.php',
      'diagnostic.inc.php',
      'rts_info.inc.php',
      'webvisu.inc.php',
      'open_source_licenses.inc.php',
      'wago_licenses.inc.php',
      'routing.inc.php'
  );


  session_start();

  // only for high security level - login always necessary
  //if(!(isset($_SESSION['username']) && $_SESSION['username']) || check_user_forbidden())

  if(check_user_forbidden($requestedSource, $accessRightsAdmin, $accessRightsGuest))
  {
    show_forbidden (ACCESS_NOT_ALLOWED);
  }

  // for higher security level - check session
  /*
  else if(SUCCESS != Session_CheckSessionLifeTime(false))
  {
    //show_forbidden (SESSION_EXPIRED);
  }
  */

  function check_user_forbidden($requestedSource, $accessRightsAdmin, $accessRightsGuest)
  {
     $isForbidden = true;

     // check if actual user has rights to get the requested ressource
     // user has login
     if(isset($_SESSION["username"]))
     {
       // user admin is allowed to request all ressources
       if($_SESSION["username"] == USER_ADMIN)
       {
         $isForbidden = false;
       }

       // user user is allowed to request all ressources except the "admin" views
       else if($_SESSION["username"] == USER_USER)
       {
         if(!in_array($requestedSource, $accessRightsAdmin))
         {
           $isForbidden = false;
         }
       }
     }

     // user has no login (guest) - can get only ressources of "guest" views
     // must be removed for higher security level
     else if(in_array($requestedSource, $accessRightsGuest))
     {
       $isForbidden = false;
     }

     return $isForbidden;
  }

  function show_forbidden($errorId)
  {
     header('Status: 403 Forbidden');
     exit;
  }

?>
