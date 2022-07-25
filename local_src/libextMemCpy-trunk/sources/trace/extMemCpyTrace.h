//---------------------------------------------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
///
///  \file     extMemCpyTrace.h
///
///  \defgroup page_extMemCpy_trace extMemCpyTrace
///  \ingroup  page_extMemCpy_internal
///
///  \version  $Id:
///
///  \brief    Diagnostics trace messages management.
///
///  \author   Wauer : WAGO GmbH & Co. KG
///
//----------------------------------------------------------------------------------------------------------------------
#ifndef EXTMEMCPYTRACE_H
#define EXTMEMCPYTRACE_H

//----------------------------------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>

#include "os_api.h"

//----------------------------------------------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_extMemCpy_trace_definitions Definitions
///  \ingroup page_extMemCpy_trace
///
///  These are the definitions of this module.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup extMemCpyTraceDefines Common defines
///  \ingroup page_extMemCpy_trace_definitions
///
///  The following defines are used in this module.
///  \{
//----------------------------------------------------------------------------------------------------------------------

/** maximum trace message length definition */
#define MAX_TRACE_MESSAGE_LEN 120
/** maximum severity string length */
#define MAX_TRACE_SEV_STR_LEN 10
/** software component identification string definition */
#define SW_COMP_ID_STRING         "EXT_MEM_CPY"
/** software component identification string length definition */
#define SW_COMP_ID_STRING_LEN     (sizeof(SW_COMP_ID_STRING) - 1)
/** software component severity string length definition */
#define MSG_SEV_STRING_LEN        6
/** trace severity string definition for errors */
#define TRACE_SEV_ERR   "_ERR: "
/** trace severity string definition for warnings */
#define TRACE_SEV_WAR   "_WAR: "
/** trace severity string definition for informations */
#define TRACE_SEV_INF   "_INF: "
/** trace severity string definition for debug messages */
#define TRACE_SEV_DBG   "_DBG: "
///  \}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_trace_definitions
///
/// \def  TRACE(SRC, SEV, MSG, ...)
///
/// This macro specifies the trace functionality. The parameters for the trace source, trace severity and
/// the trace message are specified separately. This macro can interpret these parameter and builds
/// the accordingly trace message. Depending on the source, severity related to the make file definition the message
/// will be processed and send to the specified destination.
///
/// \param  SRC   trace source (see \ref psaTraceSource_t)
/// \param  SEV   trace severity (see \ref psaTraceSeverity_t)
/// \param  MSG   additional arguments following MSG are formatted and inserted in the resulting string replacing
///               their respective specifiers
//----------------------------------------------------------------------------------------------------------------------

/*lint -emacro(506, TRACE) */
/*lint -emacro(670, TRACE) */
/*lint -emacro(774, TRACE) */
/*lint -emacro(778, TRACE) */
/*lint -emacro(960, TRACE) */

#ifdef TRACE_ENABLE

  #define TRACE(SRC, SEV, MSG, ...)                                                                   \
  {                                                                                                   \
    /* if the message source is activated */                                                          \
    if (0 != (TRACE_SRC_BIT_FIELD & (1 << ((int) SRC))))                                              \
    {                                                                                                 \
      /* if the severity is activated */                                                              \
      if (0 != (TRACE_SEV_BIT_FIELD & (1 << ((int) SEV))))                                            \
      {                                                                                               \
        char acMessage[MAX_TRACE_MESSAGE_LEN] = SW_COMP_ID_STRING;                                    \
        uint32_t ulMsgLen = sizeof(MSG);                                                              \
                                                                                                      \
        if ((SW_COMP_ID_STRING_LEN + MSG_SEV_STRING_LEN + ulMsgLen) > MAX_TRACE_MESSAGE_LEN)          \
        {                                                                                             \
          ulMsgLen = MAX_TRACE_MESSAGE_LEN - (SW_COMP_ID_STRING_LEN + MSG_SEV_STRING_LEN);            \
        }                                                                                             \
                                                                                                      \
        (void) memcpy(&acMessage[SW_COMP_ID_STRING_LEN], &acSevString[SEV][0], MSG_SEV_STRING_LEN);   \
        (void) memcpy(&acMessage[SW_COMP_ID_STRING_LEN + MSG_SEV_STRING_LEN], MSG, ulMsgLen);         \
                                                                                                      \
        if (0 != TRACE_DST_PRINTF)                                                                    \
        {                                                                                             \
          printf(MSG, ##__VA_ARGS__);                                                                 \
        }                                                                                             \
      }                                                                                               \
    }                                                                                                 \
  }                                                                                                   \

#else
  /* trace nothing */
  #define TRACE(SRC, SEV, MSG, ...)
#endif

//----------------------------------------------------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_extMemCpy_trace_data_types Data Types
///  \ingroup page_extMemCpy_trace
///
///  These are the data types of this module.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_trace_data_types
///
/// The \ref psaTraceSeverity_t enumeration specifies the available trace severities.
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  /** trace severity ERROR */
  EXT_MEM_CPY_TRACE_SEV_ERROR     = 0,
  /** trace severity WARNING */
  EXT_MEM_CPY_TRACE_SEV_WARNING   = 1,
  /** trace severity INFO */
  EXT_MEM_CPY_TRACE_SEV_INFO      = 2,
  /** trace severity DEBUG */
  EXT_MEM_CPY_TRACE_SEV_DEBUG     = 3,
}psaTraceSeverity_t;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_trace_data_types
///
/// The \ref psaTraceSource_t enumeration specifies the available trace sources.
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  /** trace source API */
  EXT_MEM_CPY_TRACE_SRC_API           = 0,
  /** trace source COPY_JOB_GEN */
  EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN  = 1,
  /** trace source COPY_ENG */
  EXT_MEM_CPY_TRACE_SRC_COPY_ENG      = 2
}psaTraceSource_t;

//----------------------------------------------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------------------------------------------

static const char acSevString[][MAX_TRACE_SEV_STR_LEN] = {TRACE_SEV_ERR,
                                                          TRACE_SEV_WAR,
                                                          TRACE_SEV_INF,
                                                          TRACE_SEV_DBG};

//----------------------------------------------------------------------------------------------------------------------
// Global function prototypes
//----------------------------------------------------------------------------------------------------------------------

#endif
//---- End of header file ----------------------------------------------------------------------------------------------
