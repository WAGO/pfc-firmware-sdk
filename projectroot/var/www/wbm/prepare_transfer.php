<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include_once 'session_lifetime.inc.php';

include_once 'ini_utils/ini_utils.php';
include_once 'http/http_header.php';
include_once 'response/response.php';
include_once 'transfer/file_transfer.php';

use function ini_utils\max_post_size;
use function http\request_header;
use function response\die_with_response;
use transfer\FileTransfer;


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

    
$size = request_header('Com-Wago-Transfer-Size');
if (!$size) {
    $size = 0;
} 
$transfer = FileTransfer::create($size);
if (!$transfer) {
    die_with_response(200, [
        'status' => -1,
        'statusMessage' => 'Not enough memory available'
    ]);
}

die_with_response(200, [
    'status' => 0,
    'uploadChunkSize' => max_post_size(),
    'transferToken' => $transfer->token(),
    'transferPath' => $transfer->path()
]);