<?php namespace http;

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function request_header($name) {
    $name = strtoupper($name);
    $name = str_replace('-', '_', $name);
    $name = "HTTP_$name";
    return urldecode($_SERVER[$name]);
}
