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
///  \file     extMemCpy_API.h
///
///  \defgroup page_extMemCpy_api Application Programming Interface (API)
///  \ingroup  page_extMemCpy_component
///
///  \version  $Id:
///
///  \brief    This module is the application interface for the Extended-Copy-Engine. All available
///             functions can only be accessed by using this interface. This module copies data depending on
///             the specified copy rules.
///             There are different types of copy rules usable:
///             - Offset-to-Offset
///               - Copies data from a bit offset related to defined source memory to a bit offset
///                 related to a defined destination memory. The bit offsets are specified in the copy rule. The
///                 source and destination memories are specified by the parameters of the copy function call.
///             - Offset-to-Absolute
///               - Copies data from a bit offset related to defined source memory to a defined destination memory.
///                 The source data bit offset and the destination memory are specified in the copy rule. The
///                 source memory is specified by the parameter of the copy function call.
///             - Absolute-to-Offset
///               - Copies data from a defined source memory to a bit offset related to a defined destination memory.
///                 The destination data bit offset and the source memory are specified in the copy rule. The
///                 destination memory is specified by the parameter of the copy function call.
///             - Absolute-to-Absolute
///               - Copies data from a defined source memory to a defined destination memory.
///                 The source and destination data memory are specified in the copy rule.
///             There are several functions to modify the copy rule list. An internal optimizer can put different
///             copy rules together to decrease the copy time. The order of the copy rules doesn't matter.
///
///  \author   Wauer : WAGO GmbH & Co. KG
///
//----------------------------------------------------------------------------------------------------------------------
#ifndef EXTMEMCPY_API_H
#define EXTMEMCPY_API_H

//----------------------------------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

#include "extMemCpy_SYSI.h"

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_extMemCpy_api_data_types Data Types
///  \ingroup page_extMemCpy_api
///
///  These are the data types of this module.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_api_data_types
///
/// The \ref copyRuleOffsetToOffset_t structure describes the rule to copy data from
///  offset memory location to offset memory location.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /** source data bit offset */
  uint_t uiSrcDataBitOffset;
  /** destination data bit offset */
  uint_t uiDstDataBitOffset;
  /** bit size */
  uint_t uiBitSize;
}copyRuleOffsetToOffset_t;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_api_data_types
///
/// The \ref copyRuleAbsoluteToOffset_t structure describes the rule to copy data from
///  offset memory location to absolute memory location.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /* source data memory pointer */
  void* pSrcData;
  /** source data bit position */
  uint_t uiSrcDataBitPos;
  /** destination data bit offset */
  uint_t uiDstDataBitOffset;
  /** bit size */
  uint_t uiBitSize;
}copyRuleAbsoluteToOffset_t;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_api_data_types
///
/// The \ref copyRuleOffsetToAbsolute_t structure describes the rule to copy data from
///  absolute memory location to offset memory location.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /** destination data memory pointer */
  void* pDstData;
  /** destination data bit position */
  uint_t uiDstDataBitPos;
  /** source data bit position */
  uint_t uiSrcDataBitOffset;
  /** bit size */
  uint_t uiBitSize;
}copyRuleOffsetToAbsolute_t;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_api_data_types
///
/// The \ref copyRuleAbsoluteToAbsolute_t structure describes the rule to copy data from
///  absolute memory location to absolute memory location.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /** source data memory pointer */
  void* pSrcData;
  /** source data bit position */
  uint_t uiSrcDataBitPos;
  /* destination data memory pointer */
  void* pDstData;
  /** destination data bit position */
  uint_t uiDstDataBitPos;
  /** bit size */
  uint_t uiBitSize;
}copyRuleAbsoluteToAbsolute_t;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_api_data_types
///
/// The \ref ruleType_t describes the type of the specified rule.
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  /** the rule specifies an offset to offset copy action  */
  OFFSET_OFFSET_RULE = 1,
  /** the rule specifies an offset to absolute copy action  */
  OFFSET_ABSOLUTE_RULE = 2,
  /** the rule specifies an absolute to offset copy action  */
  ABSOLUTE_OFFSET_RULE = 3,
  /** the rule specifies an absolute to absolute copy action  */
  ABSOLUTE_ABSOLUTE_RULE = 4
}ruleType_t;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_extMemCpy_api_data_types
///
/// The \ref copyRule_t structure describes a copy rule.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /** type identification of the copy rule */
  ruleType_t enmRuleTypeId;

  /** copy rule type union */
  union
  {
    /** offset to offset location copy rule */
    copyRuleOffsetToOffset_t stOffsetToOffsetRule;
    /** offset to absolute location copy rule */
    copyRuleOffsetToAbsolute_t stOffsetToAbsoluteRule;
    /** absolute to offset location copy rule */
    copyRuleAbsoluteToOffset_t stAbsoluteToOffsetRule;
    /** absolute to absolute location copy rule */
    copyRuleAbsoluteToAbsolute_t stAbsoluteToAbsoluteRule;
  }unRuleTypes;
}copyRule_t;

//----------------------------------------------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Global functions
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_extMemCpy_api_functions Functions
///  \ingroup page_extMemCpy_api
///
///   These are the functions of this module.
///
//----------------------------------------------------------------------------------------------------------------------

//-- Function: extMemCpy_CopyData --------------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function copies the data depending on the specified copy rule list. The data pointer are used for the
///   offset defined copy rules.
///
///  \param pDstData    destination data memory used for offset copy rules
///  \param pSrcData    source data memory used for offset copy rules
///  \param pObjHandle  handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_CopyData (void* pDstData,
                             void* pSrcData,
                             handle_t pObjHandle);

//-- Function: extMemCpy_GetCopyRule -----------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function returns the copy rule depending on the specified copy rule list index.
///
///  \param uiCopyRuleIdx   requested copy rule list index
///  \param pstCopyRule     pointer to the memory where the copy rule should be stored
///  \param pObjHandle      handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_GetCopyRule (const uint_t uiCopyRuleIdx,
                                copyRule_t* pstCopyRule,
                                handle_t pObjHandle);

//-- Function: extMemCpy_AddCopyRuleList -------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function adds a copy rule list. An existing copy list will be replaced by the new one. To set the new list
///   active it is necessary to call the function \ref extMemCpy_RefreshCopyRuleList.
///
///  \param pastCopyRuleList        pointer to a list of copy rules
///  \param uiNoOfCopyRulesInList   number of specified copy rules in the list
///  \param pObjHandle              handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_AddCopyRuleList (copyRule_t* const pastCopyRuleList,
                                    const uint_t uiNoOfCopyRulesInList,
                                    handle_t pObjHandle);

//-- Function: extMemCpy_AddCopyRule -----------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function adds a copy rules at the end of the copy rule list in the Expended-Memory-Copy object.
///   To set the new list active it is necessary to call the function \ref extMemCpy_RefreshCopyRuleList.
///
///  \param pstCopyRule  pointer to a copy rule to be added to the internal copy rule list
///  \param pObjHandle   handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_AddCopyRule (copyRule_t* const pstCopyRule,
                                handle_t const pObjHandle);

//-- Function: extMemCpy_ModifyCopyRule --------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function modifies the copy rule specified by the copy rule list index. To set the new list active it is
///   necessary to call the function \ref extMemCpy_RefreshCopyRuleList.
///
///  \param pstCopyRule             pointer to the new copy rule witch replaces the old one
///  \param uiCopyRuleIdx           list index of the copy rule which should be replaced
///  \param pObjHandle              handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_ModifyCopyRule (copyRule_t* const pstCopyRule,
                                   const uint_t uiCopyRuleIdx,
                                   handle_t const pObjHandle);

//-- Function: extMemCpy_RemoveAllCopyRules ----------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function removes all stored copy rules.
///
///  \param pObjHandle              handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_RemoveAllCopyRules (handle_t const pObjHandle);

//-- Function: extMemCpy_RemoveCopyRule --------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function removes the copy rule specified by the copy rule list index. To set the new list active it is
///   necessary to call the function \ref extMemCpy_RefreshCopyRuleList.
///
///  \param uiCopyRuleIdx           list index of the copy rule which should be removed
///  \param pObjHandle              handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_RemoveCopyRule (const uint_t uiCopyRuleIdx,
                                   handle_t const pObjHandle);

//-- Function: extMemCpy_SetCopyRuleActivation -------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function activates or deactivates the copy rule specified by the copy rule list index. To set the new list
///   active it is necessary to call the function \ref extMemCpy_RefreshCopyRuleList.
///
///  \param uiCopyRuleIdx           list index of the copy rule which should be modified
///  \param bSetActive              true/false = set the specified copy rule active/inactive
///  \param pObjHandle              handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_SetCopyRuleActivation (const uint_t uiCopyRuleIdx,
                                          const bool bSetActive,
                                          handle_t const pObjHandle);

//-- Function: extMemCpy_RefreshCopyRule -------------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function refreshes the copy rule list. After this call the data will be copied by the new list.
///
///  \param pObjHandle              handle of the the Expended-Memory-Copy object
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_RefreshCopyRuleList (handle_t pObjHandle);

#endif
//---- End of header ---------------------------------------------------------------------------------------------------
