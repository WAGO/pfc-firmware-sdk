//--------------------------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
/// \file       mdmd_log.h
///
/// \version    $Revision: 38420 $
///
/// \par        <b>Software Package</b>:
///             Modem Management Daemon (mdmd)
///
/// \brief      <b>Logging Interface</b>
///
/// \details    This global interface provides all resources and functionalities
///             allowing debug prints
///
/// \author     K.Nuenthel : WAGO Kontakttechnik GmbH & Co. KG
//--------------------------------------------------------------------------------------------------
#ifndef MDMD_LOG_H_
#define MDMD_LOG_H_

#include <stdint.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/time.h>

//--------------------------------------------------------------------------------------------------
//  DEFINES
//--------------------------------------------------------------------------------------------------

#define FUN                     (__FUNCTION__)
#define SYS_LOG_OFF             0xFF 
enum {
    /// MDMD logging off
    MD_LOG_OFF      = 0,

    /// Fatal error, MDMD can not continue its process and must be restarted
    MD_LOG_ERR      = 1,

    /// Warning, unsatisfied condition occurs, MDMD process continues
    MD_LOG_WRN      = 2,

    /// Information only
    MD_LOG_INF      = 3,

    /// Common debug information
    MD_LOG_DBG      = 4,

    /// Detailed debug information
    MD_LOG_DBG2     = 5,

    /// Default Trace mask
    MD_LOG_DEFAULT  = MD_LOG_INF,
};


//--------------------------------------------------------------------------------------------------
//  Macros
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//  Typedefs
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Globals
//--------------------------------------------------------------------------------------------------
extern unsigned long  _mdmdLogLevel;
int mdmd_GetLogLevel( void );
bool mdmd_SetLogLevel( int logLevel );
unsigned int mdmd_GetSyslogLogLevel(unsigned int mdmd_logLevel);

//--------------------------------------------------------------------------------------------------
//  function implementation
//--------------------------------------------------------------------------------------------------
#define mdmd_Log(__lvl, __fmt, ...)  \
        do\
        {\
            if(_mdmdLogLevel < __lvl) break;\
            const unsigned int syslogLevel = mdmd_GetSyslogLogLevel(__lvl);\
            if(syslogLevel != SYS_LOG_OFF) syslog((int)syslogLevel, __fmt , ##__VA_ARGS__);\
        } while(0)

#define mdmd_LogAlways(__fmt, ...)  \
        do\
        {\
          const unsigned int syslogLevel = LOG_INFO;\
          syslog((int)syslogLevel, __fmt , ##__VA_ARGS__);\
        } while(0)


#endif /* MDMD_LOG_H_ */
//---- End of source file ------------------------------------------------------
