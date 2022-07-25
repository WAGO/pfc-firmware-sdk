//----------------------------------------------------------------------------------------------------------------------
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
///  \file     extMemCpy_SYSI.h
///
///  \defgroup page_extMemCpy_sysi System Interface (SYSI)
///  \ingroup  page_extMemCpy_component
///
///  \version  $Id:
///
///  \brief    This module is the common system interface of the Expended-Memory-Copy component. It is possible to
///              create and destroy objects of the Extended-Memory-Copy functionality (EMC functionality).
///             ATTENTION: Do not modify this file! Changes can lead to undefined component behavior!
///
///  \author   Wauer : WAGO GmbH & Co. KG
///
//----------------------------------------------------------------------------------------------------------------------
#ifndef EXTMEMCPY_SYSI_H
#define EXTMEMCPY_SYSI_H

//----------------------------------------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------------------------------------
#include "os_api.h"

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_extMemCpy_sysi_definitions Definitions
///  \ingroup page_extMemCpy_sysi
///
///  These are the definitions of this module.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup extMemCpyStatus Status codes
///  \ingroup page_extMemCpy_sysi_definitions
///
/// The return value defines are 32 bit values and have the following format:
///
/// \code
///   3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0
///   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
///  +---+---------------------------+--------------------------------+
///  |Sev|      extended Code        |               Code             |
///  +---+---------------------------+--------------------------------+
/// \endcode
///
///  - Sev is the severity code
///   - 00 - Success
///   - 01 - Informational
///   - 10 - Warning
///   - 11 - Error
///  - extended Code describes the code more detailed (for further information see error code definitions)
/// \{
//----------------------------------------------------------------------------------------------------------------------
/** the processing of the function has been successfully finished */
#define EXTMEMCPY_SUCCESS                                         0x00000000U
/** an undefined error has occurred during the processing of the function */
#define EXTMEMCPY_ERROR_UNDEFINED                                 0xC0000000U
/** the pointer of a parameter is invalid */
#define EXTMEMCPY_ERROR_NULL_POINTER                              0xC0000001U
/** the object state is not valid */
#define EXTMEMCPY_ERROR_INVALID_OBJ_STATE                         0xC0000002U
/** the function was called in the wrong sequence */
#define EXTMEMCPY_ERROR_INVALID_SEQUENCE                          0xC0000003U
/** the requested resource is not available */
#define EXTMEMCPY_ERROR_RESSOURCE_FAULT                           0xC0000004U
/** the specified settings are invalid */
#define EXTMEMCPY_ERROR_INVALID_SETTINGS                          0xC0000005U
/** the requested functionality is not supported */
#define EXTMEMCPY_ERROR_FUNC_NOT_SUPPORTED                        0xC0000006U
/** the specified copy rule type is invalid */
#define EXTMEMCPY_ERROR_INVALID_COPY_RULE_TYPE                    0xC0000007U
/** the specified copy rule parameters are invalid */
#define EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM                   0xC0000008U
/** a rescan of the specified rule list is required */
#define EXTMEMCPY_ERROR_RULE_LIST_RESCAN_REQUIRED                 0xC0000009U
/** the requested data are not available */
#define EXTMEMCPY_ERROR_DATA_NOT_AVAILABLE                        0xC000000AU
/** the requested copy size is invalid */
#define EXTMEMCPY_ERROR_INVALID_COPY_SIZE                         0xC000000BU
/** the requested copy size is invalid */
#define EXTMEMCPY_ERROR_INVCOMPATIBLE_MAX_COPY_LEN                0xC000000CU
/** there is not enough free memory available */
#define EXTMEMCPY_ERROR_NO_FREE_MEM_AVAIL                         0xC000000DU
///  \}

//----------------------------------------------------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_extMemCpy_sysi_data_types Data Types
///  \ingroup page_extMemCpy_sysi
///
///  These are the data types of this module.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_sysi_data_types
///
/// The \ref maxCopyOptimization_t describes the maximum data length to be copied in one step.
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  /** the maximum copy data length is one bit */
  MAX_COPY_LEN_BIT = 1,
  /** the maximum copy data length is one byte */
  MAX_COPY_LEN_BYTE = 2,
  /** the maximum copy data length is one word */
  MAX_COPY_LEN_WORD = 3,
  /** the maximum copy data length is one double word */
  MAX_COPY_LEN_DWORD = 4
}maxCopyOptimization_t;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_sysi_data_types
///
/// The \ref extMemCpySettings_t structure describes the Extended-Memory-Copy object settings.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /** number of maximum supported copy rules */
  uint_t uiMaxSuppCopyRules;
  /** maximum copy optimization */
  maxCopyOptimization_t enmMaxCopyOptimization;
}extMemCpySettings_t;

//----------------------------------------------------------------------------------------------------------------------
// Global Variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_extMemCpy_sysi_functions Functions
///  \ingroup page_extMemCpy_sysi
///
///   These are the functions of this module.
//----------------------------------------------------------------------------------------------------------------------

//-- Function: extMemCpy_Initialise ------------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_sysi_functions
///
///  This function creates a new object of the EMC functionality. The memory is allocated here. Additionally all
///   necessary internal initialisation are processed in this function. If something went wrong the handle
///   is NULL and no memory has been allocated.
///
///  \param   pstExtMemCpySettings    settings of the component context
///
///  \return  object handle
//----------------------------------------------------------------------------------------------------------------------
handle_t extMemCpy_Initialise (extMemCpySettings_t* pstExtMemCpySettings);


//-- Function: extMemCpy_Release --------------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_sysi_functions
///
///  This function destroys the object of the EMC functionality. The memory is released as well.
///
///  \param   pObjHdl                     handle of the EMC functionality object
///
///  \return  \ref EXTMEMCPY_SUCCESS                 no error has occurred
///  \return  \ref EXTMEMCPY_ERROR_INVALID_SEQUENCE  the current state of the object is invalid
///  \return  \ref EXTMEMCPY_ERROR_NULL_POINTER      the object handle is invalid
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_Release (handle_t pObjHdl);

#endif
//---- End of header ---------------------------------------------------------------------------------------------------
