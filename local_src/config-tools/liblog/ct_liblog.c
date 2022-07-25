// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

#include "ct_liblog.h"

#include "../config_tool_lib.h"

//TODO: make configurable
#define LAST_ERROR_FILENAME "/tmp/last_error.txt"

int ct_liblog_setLastError(const char *errorStr)
{
    int status = SUCCESS;

    FILE *lastErrorFileHandle = fopen(LAST_ERROR_FILENAME, "w+");

    if(NULL == lastErrorFileHandle)
    {
        status = FILE_OPEN_ERROR;
    }

    if(SUCCESS == status)
    {
        if(-1 == flock(fileno(lastErrorFileHandle), LOCK_EX))
        {
            status = SYSTEM_CALL_ERROR;
        } 
    }

    if(SUCCESS == status)
    {
        (void) fprintf(lastErrorFileHandle, "%s", errorStr);
        
        if(-1 == fdatasync(fileno(lastErrorFileHandle)))
        {
            status = SYSTEM_CALL_ERROR;
        }
    }
    
    if(NULL != lastErrorFileHandle)
    {
        // Must release flock in any case to prevent deadlocks when
        // logging from a long-living application!
        if(-1 == flock(fileno(lastErrorFileHandle), LOCK_UN))
        {
            status = SYSTEM_CALL_ERROR;
        }
        
        if(-1 == fclose(lastErrorFileHandle) &&
           SUCCESS == status)
        {
            status = FILE_CLOSE_ERROR;
        }
    }

    return status;
}

