//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     diagnostic_API.h
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef DIAGNOSTIC_API_H_
#define DIAGNOSTIC_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/time.h>
#include <communication_API.h>
#include <LedCtl_API.h>
#include <ledserver_API.h>
#include "diagnostic_min_API.h"

typedef void(*log_tEventHandler)(com_tConnection*, com_tComMessage*, log_tEventId, void*);


#define LOG_TYPE_INVALID COM_TYPE_INVALID
#define LOG_TYPE_BYTE    COM_TYPE_BYTE
#define LOG_TYPE_INT16   COM_TYPE_INT16
#define LOG_TYPE_UINT16  COM_TYPE_UINT16
#define LOG_TYPE_INT32   COM_TYPE_INT32
#define LOG_TYPE_UINT32  COM_TYPE_UINT32
#define LOG_TYPE_BOOL    COM_TYPE_BOOL

#define LOG_PATH          "/wago/diagnose/ID"
#define LOG_INTERFACE     "wago.diagnose"
#define LOG_PARSER_NAME   "wago.diagnostic"
#define LOG_DIAG_PATH     "/wago/diagnose"
#define LOG_GET_INFO      "getInfo"


#define DIAGNOSTIG_API 2
//#define openlog    diag_ConnectToLog
//#define closelog   diag_DisonnectFromLog
//#define setlogmask diag_SetLogMask

//void diag_ConnectToLog (const char *ident, int logstat, int logfac);
//void diag_DisonnectFromLog (void);
//int  diag_SetLogMask(int pmask);

typedef union
{
    tLedStatic staticVar;
    tLedBlink  blinkVar;
    tLedFlash  flashVar;
    tLed750    err750Var;
    tLedCan    canVar;
}tLedVars;

typedef struct
{
    char name[16];
    tLedStateClass state;
    struct timeval timestamp;
    tLedVars vars;
    tIdCtrlInfo   info;
}tLedStatus;

void log_EVENT_Init(          const char       * name);

void log_EVENT_LogIdParam(    log_tEventId           id,
                              bool               set,
                              int                first_arg_type, ...);

/*void log_EVENT_LogDev(        int priority,
                              const char         format, ...);*/
#define log_EVENT_LogDev    syslog
#define log_EVENT_VLogDev   vsyslog

//void log_EVENT_LogDev(int pri, const char *fmt, ...);
//void log_EVENT_VLogDev(int __pri, const char *__fmt, va_list __ap);

com_tSignalHandle log_EVENT_RegisterForId(  log_tEventId           eventId,
                                        log_tEventHandler      callback,
                                        void             * user_data);

bool log_EVENT_HandlerIdSet(com_tConnection * con, com_tComMessage * msg);

int log_EVENT_DeregisterId(com_tSignalHandle  handle);

bool log_EVENT_GetState(     log_tEventId           eventId,
                             struct timeval   * timestamp,
                             int                first_arg_type, ...);

/*int led_GetLedState(         const char       * ledName,
                             uint8_t          * state,
                             uint16_t         * var1,
                             uint16_t         * var2);
int led_GetLedStateWithId(   const char       * ledName,
                             uint8_t          * state,
                             uint16_t         * var1,
                             uint16_t         * var2,
                             uint32_t         * id,
                             struct timeval   * timestamp);

int led_GetLedStateWithIdInfo(   const char       * ledName,
                             uint8_t          * state,
                             uint16_t         * var1,
                             uint16_t         * var2,
                             tIdCtrlInfo      * id,
                             struct timeval   * timestamp);*/

int led_GetLedStatus(const char * led_name,
                     tLedStatus * status);

void led_GetDbusInfoByState(DBusMessageIter * iter,tLedStatus * status);


#endif /* DIAGNOSTIC_API_H_ */
//---- End of source file ------------------------------------------------------
