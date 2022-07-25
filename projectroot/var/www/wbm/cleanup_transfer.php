<?php

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include_once 'session_lifetime.inc.php';

include_once 'http/http_header.php';
include_once 'response/response.php';
include_once 'transfer/file_transfer.php';

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
        'statusMessage' => 'Invalid session token'
    ]);
}

if ('admin' != $_SESSION['username']) {
    die_with_response(200, [
        'status' => -2,
        'statusMessage' => 'Access Not Allowed'
    ]);
}

    
$token = request_header('Com-Wago-Transfer-Token');
if (!$token) {
    die_with_response(200, [
        'status' => -1,
        'statusMessage' => 'No transfer token provided'
    ]);
}
$transfer = FileTransfer::reopen($token);
if (!$transfer) {
    die_with_response(200, [
        'status' => -1,
        'statusMessage' => 'No transfer for provided token'
    ]);
}

$transfer->cleanup();

die_with_response(200, [
    'status' => 0
]);