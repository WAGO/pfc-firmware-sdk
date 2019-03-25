// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

#include "ct_liblog.h"

#include "../config_tool_lib.h"

//TODO: make configurable
#define LAST_ERROR_FILENAME "/tmp/last_error.txt"
#define TRACE_FILENAME      "/etc/config-tools/trace_debug.txt"

char *errorMsgs[] = {
    [MISSING_PARAMETER] = "missing parameter", 
    [INVALID_PARAMETER] = "invalid parameter",  
    [FILE_OPEN_ERROR]   = "error opening file",
    [FILE_READ_ERROR]   = "error reading file",
    [FILE_WRITE_ERROR]  =  "error writing file",
    [NOT_FOUND]         = "value not found",
    [SYSTEM_CALL_ERROR] = "error during OS operation",
    [CONFIG_FILE_INCONSISTENT] = "content of config file is inconsistent",
    [TIMEOUT]           = "timeout",
    [FILE_CLOSE_ERROR]  = "error closing file",
};

static void __fillTimestamp(char *timestamp, size_t timestampLen)
{

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    (void) strftime(timestamp, timestampLen - 1, "%Y-%m-%d, %H:%M:%S", t);
}

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

int ct_liblog_reportError(enum eStatusCode errNum, const char *additionalStr)
{
    int status = SUCCESS;

    const char *_additionalStr = (NULL == additionalStr) ? "" : additionalStr;

    FILE *traceFileHandle = fopen(TRACE_FILENAME, "a");

    if(NULL == traceFileHandle)
    {
        status = FILE_OPEN_ERROR;
    }

    if(SUCCESS == status)
    {
        if(-1 == flock(fileno(traceFileHandle), LOCK_EX))
        {
            status = SYSTEM_CALL_ERROR;
        } 
    }

    if(SUCCESS == status)
    {
        char timestamp[64];
        timestamp[0] = '\0';

        __fillTimestamp(timestamp, sizeof(timestamp));

        char *errorMsg = NULL;
 
        // historically our enum is signed, but all meaningful values are unsigned.
        // => cast
        if(errNum >= 0 && (unsigned int) errNum < sizeof(errorMsgs))
        {
            errorMsg = errorMsgs[errNum];
        }

        if(NULL == errorMsg)
        {
            (void) fprintf(traceFileHandle, "Error: <%s>  Unknown error(error=%d) %s\n", timestamp, errNum, _additionalStr);
        }
        else
        {
            (void) fprintf(traceFileHandle, "Error: <%s>  %s %s\n", timestamp, errorMsg, _additionalStr);
        }
        
        if(-1 == fdatasync(fileno(traceFileHandle)))
        {
            status = SYSTEM_CALL_ERROR;
        }
    }
 
    if(NULL != traceFileHandle)
    {
        // Must release flock in any case to prevent deadlocks when
        // logging from a long-living application!
        if(-1 == flock(fileno(traceFileHandle), LOCK_UN))
        {
            status = SYSTEM_CALL_ERROR;
        }
        
        if(-1 == fclose(traceFileHandle) &&
           SUCCESS == status)
        {
            status = FILE_CLOSE_ERROR;
        }
    }

    return status;
}
