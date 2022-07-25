// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include "log.h"

#include <stdarg.h>
#include <stdio.h>

int _loglevel = LogLevel_Quiet;

void log_set_loglevel(int level)
{
    if (LogLevel_Quiet <= level && LogLevel_Debug >= level)
        _loglevel = level;
}

void log_printf_level(int level, const char *format, ...)
{
    if (level >= _loglevel)
        return;
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}


void log_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

int log_get_loglevel(void)
{
    return _loglevel;
}

