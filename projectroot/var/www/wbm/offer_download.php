<?php

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include_once "session_lifetime.inc.php";
include_once "wbm_lib.inc.php";

include_once 'transfer/file_transfer.php';
use transfer\FileTransfer;

// define constants

/*
define("DOWNLOAD_DIR", "/tmp");
define("UPLOAD_DIR", "/tmp");
*/
define("DOWNLOAD_FILENAME_FRAGMENT", "firmware_backup");

// Offer backup download file. Function takes the first file in download directory (there should be only one),
// transfers it to browser and deletes the file afterwards.
function OfferDownload()
{
  $status     = SUCCESS;
  $errorText  = "";

  $transfer; // optinal, of type FileTransfer

  // check for correct session and reset session timeout
  $status = Session_HandleSessionLifeTime($_GET["csrf_id"],false);

  if(SUCCESS != $status)
  {
    $errorText = Session_GetErrorTxt($status);
  }
  else if(!isset($_SESSION["username"]) || USER_ADMIN != $_SESSION["username"])
  {
    $status    = ACCESS_NOT_ALLOWED;
    $errorText = "Access not allowed.";
  }

  // check if folder is given
  else
  {
    if(!isset($_GET["download_dir"]))
    {
 
      // check if a valid transfer token is provided
      if (isset($_GET['transfer']) && isset($_SESSION['file_transfer'][$_GET['transfer']])) {
        // transfer exists for provided token. in this case get the download directory from session cache
        $transfer = FileTransfer::reopen($_GET['transfer']);
        if (!$transfer) {
          $status = ERROR;
          $errorText = "Invalid download.";  
        }
        $_GET["download_dir"] = $transfer->path();
      } 
      
      else {
        $status = ERROR;
        $errorText = "Missing input parameter.";
      }
    }

    // check if download directory is allowed - more valid directorys can be added in future if necessary
    else if(("/tmp/firmware_backup" != $_GET["download_dir"]) && ("/tmp/vpncfg-out" != $_GET["download_dir"]))
    {
      $status    = ERROR;
      $errorText = "Invalid parameter for download directory";
    }
  }


  if(SUCCESS == $status)
  {
    $downloadDir = $_GET["download_dir"];
    //printf("downloadDir:%s\n", $downloadDir);

    // open directory, check for success
    if(!$hDir = @opendir($downloadDir))
    {
      $status    = ERROR;
      $errorText = "Can't open download directory.";
    }
    else
    {
      $status     = ERROR;
      $errorText  = "No upload file found.";

      // security: filenames may only contain numbers, letters, '.', '-' and '_'
      //           to prevent code injection via filename
      $filename_regex = '/^[.a-zA-Z0-9_-]*$/';

      // loop over files in download directory to find the first backup file (there always should be exactly one!)
      while((SUCCESS != $status) && ($filename = readdir($hDir)))
      {
        // ignore invalid filenames
        if (0 == preg_match($filename_regex, $filename)) continue;

        // filenames "." and ".." are nor interesting anyway
        if(($filename == ".") || ($filename == "..")) continue;

        // found file with indicator for download file in name?
        if((false !== strpos($filename, DOWNLOAD_FILENAME_FRAGMENT)) || ("/tmp/vpncfg-out" == $downloadDir))
        {
          // check access rights
          if(!is_readable($downloadDir."/".$filename))
          {
            $status = ERROR;
            $errorText = "Error while reading download file.";
          }
          else
          {
            //printf("            <p>path:%s filename:%s </p>\n", $downloadDir, $filename);
            // put fitting filetype to header
            header("Content-Type: application/octet-stream");

            // send header with proposal of fitting filename
            header(sprintf("Content-Disposition: attachment; filename=\"%s\"", basename($filename)));

            // send file size
            header('Content-Length: '.filesize($downloadDir."/".$filename));

            // read and send file chunkwise
            flush();
            $file = fopen($downloadDir."/".$filename, 'r');
            while (!feof($file)) {
              print fread($file, 1024*1024); // read 1MiB...
              flush();                       // ... and send to browser
            }
            fclose($file);

            // note that we ware successfull
            $status     = SUCCESS;
            $errorText  = "";
          }

        }
      }

      closedir($hDir);
    }
  }

  // afterwards delete file
  if($status == SUCCESS)
  {

    if ($transfer) {
      // if the transfer was used, use its cleanup function to remove the downloaded file
      $transfer->cleanup();
    } else {
      //printf("<p>filename:%s</p>\n", $filename);
      system(sprintf("sudo /bin/rm %s/%s > /dev/null 2> /dev/null", $downloadDir, $filename));
    }
  }

  // only do error outputs in case of error - otherwise the text will turn up in download file!
  else
  {
    printf("<span class=\"status\">%s</span>\n", $status);
    printf("<span class=\"error_text\">%s</span>\n", $errorText);
  }
}

OfferDownload();

?>
