//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     config_tool_lib.h
///
///  \version  $Revision: 65689 $1
///
///  \brief    
///
///  \author   Stefanie Meihöfer : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//TODO: use global config-tools header

#include <stdio.h>
#include <string.h>

#ifndef __CT_ERROR_HANDLING_H__
#define __CT_ERROR_HANDLING_H__


#define UNUSED_PARAMETER(x) ((void)(x))

// status- and error-codes
enum eStatusCode
{
  ERROR                           = -1,
  SUCCESS                         = 0,

  // all error values combined with a msg string have to have positive values! 

  MISSING_PARAMETER,
  INVALID_PARAMETER,
  FILE_OPEN_ERROR,
  FILE_READ_ERROR,
  FILE_CLOSE_ERROR,
  NOT_FOUND = 6, // value interpreted by WBM
  SYSTEM_CALL_ERROR,
  CONFIG_FILE_INCONSISTENT,
  TIMEOUT,
  FILE_WRITE_ERROR,
  NARROW_SPACE_WARNING = 13,   // value interpreted by WBM
  NOT_ENOUGH_SPACE_ERROR = 14, // value interpreted by WBM
};

static inline void printValidArgs(const char *argList[])
{
    const char **next = argList;

    while(*next != NULL)
    {
        printf("%s", *next);
        ++next;
        if(NULL != *next)
        {
            printf(", ");
        }
    }

    printf("\n");
}

static inline int isValidArg(const char *arg, const char *argList[])
{
    const char **next = argList;

    while(*next != NULL)
    {
        if(0 == strcmp(*next, arg))
        {
            return SUCCESS;
        }

        ++next;
    }

    return INVALID_PARAMETER;
}

#endif
