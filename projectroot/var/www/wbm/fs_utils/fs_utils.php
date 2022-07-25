<?php namespace fs_utils;

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function rmkdir($path) {
    if ( \file_exists($path) ) 
        return false;
    else 
        return \mkdir($path, 0777, true);
}

function rrmdir($path) {
    if ( \is_dir($path)) {

        // recursively remove subdirectories and files
        $objects = \scandir($path);
        foreach ($objects as $object) { 
            if ($object != "." && $object != "..") {
                rrmdir("$path/$object");
            }
        }
        \reset($objects);

        // lastly try to remove the given path itself
        return \rmdir($path);

    } else {
        \unlink($path);
    }
}

function dirsize($path) {
    $total = 0;
    if ( \is_dir($path)) {
        // recursively remove subdirectories and files
        $objects = \scandir($path);
        foreach ($objects as $object) { 
            if ($object != "." && $object != "..") {
                $total += dirsize("$path/$object");
            }
        }
        \reset($objects);
    } else {
        $total = \filesize($path);
    }
    return $total;
}