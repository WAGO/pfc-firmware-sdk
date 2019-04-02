<?php namespace ini_utils;

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function max_post_size() {
    $maxPostSize = ini_get('post_max_size');
    if (preg_match('/K$/', $maxPostSize)) {
        $maxPostSize = intval($maxPostSize) * 1024;
    } else if (preg_match('/M$/', $maxPostSize)) {
        $maxPostSize = intval($maxPostSize) * 1024 * 1024;
    } else if (preg_match('/G$/', $maxPostSize)) {
        $maxPostSize = intval($maxPostSize) * 1024 * 1024 * 1024;
    }
    return $maxPostSize;
}
