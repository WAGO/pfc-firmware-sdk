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
///  \file     extMemCpy_API.c
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
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------------------------------
#include <stdint.h>

#include "os_api.h"

#include "extMemCpy_SYSI.h"
#include "extMemCpy_API.h"

#include "extMemCpyGlobal.h"
#include "copyJobGen_API.h"
#include "extMemCpyTrace.h"

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------------------------------------------------
static status_t extMemCpy_GetAndCheckObjContext (handle_t pObjHandle, extMemCpyData_t** ppstExtMemCpyData);

//----------------------------------------------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------------------------------------------

//-- Function: extMemCpy_CopyData --------------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_CopyData (void* pDstData,
                             void* pSrcData,
                             handle_t pObjHandle)
{
  status_t status;

  extMemCpyData_t* pstExtMemCpyData;

  /* get and check the object */
  status = extMemCpy_GetAndCheckObjContext(pObjHandle, &pstExtMemCpyData);

  if (EXTMEMCPY_SUCCESS == status)
  {
    /* if the copy job list is active */
    if (false != pstExtMemCpyData->bCopyJobListActive)
    {
      copyEng_PrcCopyJobList(pDstData, pSrcData, pstExtMemCpyData->pastCopyJobList,
                             pstExtMemCpyData->uiNoOfCopyJobsInList);
    }
    else
    {
      status = EXTMEMCPY_ERROR_RULE_LIST_RESCAN_REQUIRED;
    }
  }

  return (status);
}

//-- Function: extMemCpy_GetCopyRule -----------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_GetCopyRule (const uint_t uiCopyRuleIdx,
                                copyRule_t* pstCopyRule,
                                handle_t pObjHandle)
{
  status_t status;

  extMemCpyData_t* pstExtMemCpyData;

  /* get and check the object */
  status = extMemCpy_GetAndCheckObjContext(pObjHandle, &pstExtMemCpyData);

  if (EXTMEMCPY_SUCCESS == status)
  {
    if (NULL != pstCopyRule)
    {
      /* if the copy rule is available */
      if (uiCopyRuleIdx < pstExtMemCpyData->uiNoOfCopyRulesInList)
      {
        /* copy the copy rule */
        (void) memcpy(pstCopyRule, &pstExtMemCpyData->pastCopyRuleList[uiCopyRuleIdx], sizeof(copyRule_t));
      }
      else
      {
        status = EXTMEMCPY_ERROR_DATA_NOT_AVAILABLE;
      }
    }
    else
    {
      status = EXTMEMCPY_ERROR_NULL_POINTER;
    }
  }

  return (status);
}

//-- Function: extMemCpy_AddCopyRuleList -------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_AddCopyRuleList (copyRule_t* const pastCopyRuleList,
                                    const uint_t uiNoOfCopyRulesInList,
                                    handle_t pObjHandle)
{
  status_t status;
  extMemCpyData_t* pstExtMemCpyData;

  /* get and check the object */
  status = extMemCpy_GetAndCheckObjContext(pObjHandle, &pstExtMemCpyData);

  if (EXTMEMCPY_SUCCESS == status)
  {
    if (NULL != pastCopyRuleList)
    {
      /* if the number of copy rules are supported */
      if (uiNoOfCopyRulesInList <= pstExtMemCpyData->stSettings.uiMaxSuppCopyRules)
      {
        /* copy the copy rule list */
        (void) memcpy(pstExtMemCpyData->pastCopyRuleList, pastCopyRuleList, sizeof(copyRule_t) * uiNoOfCopyRulesInList);
        pstExtMemCpyData->uiNoOfCopyRulesInList = uiNoOfCopyRulesInList;
        /* set the copy job list inactive */
        pstExtMemCpyData->bCopyJobListActive = false;

        TRACE (EXT_MEM_CPY_TRACE_SRC_API, EXT_MEM_CPY_TRACE_SEV_INFO,
               "Copy rule list has been added successfully (Elements: %u)\n", uiNoOfCopyRulesInList);
      }
      else
      {
        status = EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM;
      }
    }
    else
    {
      status = EXTMEMCPY_ERROR_NULL_POINTER;
    }
  }

  return (status);
}

//-- Function: extMemCpy_AddCopyRule -----------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_AddCopyRule (copyRule_t* const pstCopyRule,
                                handle_t const pObjHandle)
{
  status_t status;
  extMemCpyData_t* pstExtMemCpyData;

  /* get and check the object */
  status = extMemCpy_GetAndCheckObjContext(pObjHandle, &pstExtMemCpyData);

  if (EXTMEMCPY_SUCCESS == status)
  {
    /* check the pointer */
    if (NULL != pstCopyRule)
    {
      /* if the number of copy rules are supported */
      if (pstExtMemCpyData->uiNoOfCopyRulesInList < pstExtMemCpyData->stSettings.uiMaxSuppCopyRules)
      {
        /* copy the copy rule */
        (void) memcpy(&pstExtMemCpyData->pastCopyRuleList[pstExtMemCpyData->uiNoOfCopyRulesInList],
                      pstCopyRule, sizeof(copyRule_t));

        pstExtMemCpyData->uiNoOfCopyRulesInList++;

        TRACE (EXT_MEM_CPY_TRACE_SRC_API, EXT_MEM_CPY_TRACE_SEV_INFO,
               "Copy rule has been added successfully (avail. copy rules: %u)\n",
               pstExtMemCpyData->uiNoOfCopyRulesInList);

        /* set the copy job list inactive */
        pstExtMemCpyData->bCopyJobListActive = false;
      }
      else
      {
        status = EXTMEMCPY_ERROR_NO_FREE_MEM_AVAIL;
      }
    }
    else
    {
      status = EXTMEMCPY_ERROR_NULL_POINTER;
    }
  }

  return (status);
}

//-- Function: extMemCpy_ModifyCopyRule --------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_ModifyCopyRule (copyRule_t* const pstCopyRule,
                                   const uint_t uiCopyRuleIdx,
                                   handle_t const pObjHandle)
{
  OS_ARGUMENT_USED(pstCopyRule);
  OS_ARGUMENT_USED(uiCopyRuleIdx);
  OS_ARGUMENT_USED(pObjHandle);

  return (EXTMEMCPY_ERROR_FUNC_NOT_SUPPORTED);
}

//-- Function: extMemCpy_RemoveAllCopyRules ----------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_RemoveAllCopyRules (handle_t const pObjHandle)
{
  status_t status;
  extMemCpyData_t* pstExtMemCpyData;

  /* get and check the object */
  status = extMemCpy_GetAndCheckObjContext(pObjHandle, &pstExtMemCpyData);

  if (EXTMEMCPY_SUCCESS == status)
  {
    pstExtMemCpyData->uiNoOfCopyRulesInList = 0;

    /* set the copy job list inactive */
    pstExtMemCpyData->bCopyJobListActive = false;
  }

  return (status);
}

//-- Function: extMemCpy_RemoveCopyRule --------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_RemoveCopyRule (const uint_t uiCopyRuleIdx,
                                   handle_t const pObjHandle)
{
  OS_ARGUMENT_USED(uiCopyRuleIdx);
  OS_ARGUMENT_USED(pObjHandle);

  return (EXTMEMCPY_ERROR_FUNC_NOT_SUPPORTED);
}

//-- Function: extMemCpy_SetCopyRuleActivation -------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_SetCopyRuleActivation (const uint_t uiCopyRuleIdx,
                                          const  bool bSetActive,
                                          handle_t const pObjHandle)
{
  OS_ARGUMENT_USED(uiCopyRuleIdx);
  OS_ARGUMENT_USED(pObjHandle);

  if (bSetActive){}

  return (EXTMEMCPY_ERROR_FUNC_NOT_SUPPORTED);
}

//-- Function: extMemCpy_RefreshCopyRule -------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_RefreshCopyRuleList (handle_t pObjHandle)
{
  status_t status;
  extMemCpyData_t* pstExtMemCpyData;

  /* get and check the object */
  status = extMemCpy_GetAndCheckObjContext(pObjHandle, &pstExtMemCpyData);

  if (EXTMEMCPY_SUCCESS == status)
  {
    /* generate the copy job list */
    status = copyJobGen_GenCopyJobList(pstExtMemCpyData);

    if (EXTMEMCPY_SUCCESS == status)
    {
      /* set the copy job list active */
      pstExtMemCpyData->bCopyJobListActive = true;
    }
    else
    {
      status = EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM;
    }
  }

  return (status);
}

//-- Function: extMemCpy_GetAndCheckObjContext -------------------------------------------------------------------------
///  \ingroup page_extMemCpy_api_functions
///
///  This function checks and returns the object context data specified by the object handle.
///
///  \param   pObjHandle                object handle
///  \param   ppstExtMemCpyData         pointer to the object data pointer
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_NULL_POINTER indicates failure.
//----------------------------------------------------------------------------------------------------------------------
static status_t extMemCpy_GetAndCheckObjContext (handle_t pObjHandle,
                                                 extMemCpyData_t** ppstExtMemCpyData)
{
  status_t status = EXTMEMCPY_SUCCESS;
  extMemCpyContext_t* pstDpc31StackObjContext;

  OS_ASSERT(NULL != ppstExtMemCpyData);

  /* check the handle pointer */
  if (NULL != pObjHandle)
  {
    pstDpc31StackObjContext = (extMemCpyContext_t*) pObjHandle;

    /* check the object state */
    if (EXTMEMCPY_OBJ_INITIALISED == pstDpc31StackObjContext->stGlobal.enmState)
    {
      *ppstExtMemCpyData = &pstDpc31StackObjContext->stExtMemCpyData;
    }
    else
    {
      status = EXTMEMCPY_ERROR_INVALID_OBJ_STATE;
    }
  }
  else
  {
    status = EXTMEMCPY_ERROR_NULL_POINTER;
  }

  return (status);
}

//---- End of source file ----------------------------------------------------------------------------------------------
