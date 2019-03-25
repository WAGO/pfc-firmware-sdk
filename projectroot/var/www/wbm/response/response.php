<?php namespace response;

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function die_with_response($code, $object) {
    \http_response_code(200);
    echo json_encode($object, JSON_UNESCAPED_SLASHES);
    die;
}