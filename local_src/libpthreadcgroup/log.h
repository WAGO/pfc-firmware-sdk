// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef __LOG_H
#define __LOG_H 1

enum
{
    LogLevel_Quiet,
    LogLevel_Error,
    LogLevel_Warning,
    LogLevel_Info,
    LogLevel_Debug,
};

void log_printf_level(int level, const char *format, ...);
void log_set_loglevel(int level);
void log_printf(const char *format, ...);
int log_get_loglevel(void);

#define DBG(X, ...)                                                            \
    log_printf_level(LogLevel_Debug, "Debug: " X "\n", __VA_ARGS__)

#define INFO(X, ...)                                                           \
    log_printf_level(LogLevel_Info, "Info: " X "\n", __VA_ARGS__)

#define WARN(X, ...)                                                           \
    log_printf_level(LogLevel_Warning, "Warning: " X "\n", __VA_ARGS__)

#define ERROR(X, ...)                                                          \
    log_printf_level(LogLevel_Error, "Error: " X "\n", __VA_ARGS__)

#endif
