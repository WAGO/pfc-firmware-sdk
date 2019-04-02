<?php 

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include_once 'session_lifetime.inc.php';

include_once 'http/http_header.php';
include_once 'response/response.php';
include_once 'transfer/file_transfer.php';
include_once 'transfer/file_upload.php';

use function http\request_header;
use function response\die_with_response;
use transfer\FileTransfer;
use transfer\FileUpload;


$csrfToken = request_header('Com-Wago-Session-Token');
if (!$csrfToken) {
    die_with_response(200, [
        'status' => -1,
        'statusMessage' => 'No CSRF token provided'
    ]);
}

$sessionStatus = Session_HandleSessionLifeTime($csrfToken, true);
if (0 != $sessionStatus) {
    die_with_response(200, [
        'status' => $sessionStatus,
        'statusMessage' => 'Invalid CSRF token'
    ]);
}

if ('admin' != $_SESSION['username']) {
    die_with_response(200, [
        'status' => -2,
        'statusMessage' => 'Access not allowed'
    ]);
}


$transfer;
$transferToken = request_header('Com-Wago-Transfer-Token');
if ($transferToken) {
    $transfer = FileTransfer::reopen($transferToken);
    if (!$transfer) {
        die_with_response(200, [
            'status' => -1,
            'statusMessage' => 'No Transfer for provided token'
        ]);
    }
} else {
    die_with_response(200, [
        'status' => -1,
        'statusMessage' => 'No transfer token provided'
    ]);
}


$upload;
$uploadToken = request_header('Com-Wago-Upload-Token');
if ($uploadToken) {
    $upload = FileUpload::resume($uploadToken);
    if (!$upload) {
        die_with_response(200, [
            'status' => -1,
            'statusMessage' => 'No upload for provided token'
        ]);
    }
} else {
    $name = request_header('Com-Wago-Upload-Name');
    if (!$name) {
        die_with_response(200, [
            'status' => -1,
            'statusMessage' => 'No upload name provided'
        ]);
    }
    $size = request_header('Com-Wago-Upload-Size');
    if (!$size) {
        die_with_response(200, [
            'status' => -1,
            'statusMessage' => 'No upload size provided'
        ]);
    }

    $destination = request_header('Com-Wago-Upload-Destination'); // optional
    
    $upload = FileUpload::create($transfer, $name, $size, $destination);
    if (!$upload) {
        die_with_response(200, [
            'status' => -1,
            'statusMessage' => 'Upload could not be created (too large?)'
        ]);
    }
}



$bytes = file_get_contents('php://input');
if (false == $upload->process($bytes)) {
    if ($upload->finalize() != false) {
        die_with_response(200, [
            'status' => -1,
            'statusMessage' => 'Upload already completed'
        ]);
    }
    die_with_response(200, [
        'status' => -1,
        'statusMessage' => 'Upload cannot be processed'
    ]);
}


if ($path = $upload->finalize()) {
    die_with_response(200, [
        'status' => 0,
        'uploadPath' => $path
    ]);
} else {
    die_with_response(200, [
        'status' => 0,
        'uploadToken' => $upload->token()
    ]);
}