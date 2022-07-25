//----------------------------------------------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
///
///  \file     extMemCpyGlobal.h
///
///  \defgroup page_extMemCpy_global extMemCpyGlobal
///  \ingroup  page_extMemCpy_internal
///
///  \version  $Id:
///
///  \brief    This module collects different functions which are necessary for the EMC functionality
///            component functionality.
///
///  \author   Wauer : WAGO GmbH & Co. KG
///
//----------------------------------------------------------------------------------------------------------------------
#ifndef EXTMEMCPYGLOBAL_H
#define EXTMEMCPYGLOBAL_H

//----------------------------------------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------------------------------------
#include <stdbool.h>
#
#include "extMemCpy_SYSI.h"
#include "extMemCpy_API.h"

#include "copyEng_API.h"

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_extMemCpy_global_data_types Data Types
///  \ingroup page_extMemCpy_global
///
///  These are the data types of this module.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_global_data_types
///
///  The \ref extMemCpyCompState_t enumeration describes the component object states.
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  /** The component is released. */
  EXTMEMCPY_OBJ_RELEASED = 0,
  /** The component is initialized. */
  EXTMEMCPY_OBJ_INITIALISED
}extMemCpyCompState_t;

//----------------------------------------------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_global_data_types
///
///  The \ref extMemCpyData_t structure describes the component object data.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /** object user settings */
  extMemCpySettings_t stSettings;
  /** pointer copy rule list */
  copyRule_t* pastCopyRuleList;
  /** number of copy rules in the copy rule list */
  uint_t uiNoOfCopyRulesInList;
  /** activation state of the copy rule list */
  bool bCopyJobListActive;
  /** pointer to the copy job list */
  copyJob_t* pastCopyJobList;
  /** pointer to the copy job list */
  uint_t uiNoOfCopyJobsInList;
  /** critical section of the component API */
  handle_t pCritSecApi;
}extMemCpyData_t;

//----------------------------------------------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_global_data_types
///
///  The \ref extMemCpyObjGlobal_t structure describes the general component object data.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /* current object state */
  extMemCpyCompState_t enmState;
  /* size of the allocated memory for the object */
  uint32_t ulMemSize;
}extMemCpyObjGlobal_t;

//----------------------------------------------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_global_data_types
///
///  The \ref extMemCpyContext_t structure describes the component object context data.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /* global component object data */
  extMemCpyObjGlobal_t stGlobal;
  /* specific component object data */
  extMemCpyData_t stExtMemCpyData;
}extMemCpyContext_t;

//----------------------------------------------------------------------------------------------------------------------
// Global Variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------------------------------------------------

#endif
//---- End of header ---------------------------------------------------------------------------------------------------
