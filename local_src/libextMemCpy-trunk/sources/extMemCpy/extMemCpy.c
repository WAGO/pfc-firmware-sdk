//----------------------------------------------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
///
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
///
///  \file     extMemCpy.c
///
///  \version  $Id:
///
///  \brief    This module is the common interface of this component. It is possible to create and destroy objects of
///             the EMC functionality. Before using of the created object is possible the activation after the
///             initialisation is necessary.
///
///  \author   Wauer : WAGO GmbH & Co. KG
///
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "os_api.h"

#include "extMemCpy_SYSI.h"
#include "extMemCpy_API.h"

#include "extMemCpyGlobal.h"

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Global Variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------------------------------------------------
static status_t extMemCpy_ObjContextInitialize (extMemCpyContext_t** ppstExtMemCpyObjContext,
                                                const extMemCpySettings_t* const pstExtMemCpySettings);
static void     extMemCpy_ObjContextRelease    (extMemCpyContext_t** ppstExtMemCpyObjContext);

//----------------------------------------------------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------------------------------------------------

//-- Function: extMemCpy_Initialise ------------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
handle_t extMemCpy_Initialise (extMemCpySettings_t* pstExtMemCpySettings)
{
  extMemCpyContext_t* pstExtMemCpyObjContext = NULL;

  /* default settings of this component */
  extMemCpySettings_t stExtMemCpyDefSettings = {.uiMaxSuppCopyRules = 10,
                                                .enmMaxCopyOptimization = MAX_COPY_LEN_DWORD};

  /* if context settings aren't defined use the default settings */
  if (NULL == pstExtMemCpySettings)
  {
    pstExtMemCpySettings = (extMemCpySettings_t*) &stExtMemCpyDefSettings;
  }

  /* initialise the context of this component */
  if (EXTMEMCPY_SUCCESS == extMemCpy_ObjContextInitialize(&pstExtMemCpyObjContext, pstExtMemCpySettings))
  {
    pstExtMemCpyObjContext->stGlobal.enmState = EXTMEMCPY_OBJ_INITIALISED;
  }

  return (pstExtMemCpyObjContext);
}

//-- Function: extMemCpy_Release --------------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t extMemCpy_Release (handle_t pObjHdl)
{
  status_t status = EXTMEMCPY_SUCCESS;
  extMemCpyContext_t* pstExtMemCpyObjContext;

  /* check the object handle */
  if (NULL != pObjHdl)
  {
    /* cast the handle */
    pstExtMemCpyObjContext = (extMemCpyContext_t*) pObjHdl;

    /* check the object state */
    if (EXTMEMCPY_OBJ_INITIALISED == pstExtMemCpyObjContext->stGlobal.enmState)
    {
      /* release the component context */
      extMemCpy_ObjContextRelease(&pstExtMemCpyObjContext);
    }
    else
    {
      status = EXTMEMCPY_ERROR_INVALID_SEQUENCE;
    }
  }
  else
  {
    status = EXTMEMCPY_ERROR_NULL_POINTER;
  }

  return (status);
}

//-- Function: extMemCpy_ObjContextInitialize -------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
static status_t extMemCpy_ObjContextInitialize (extMemCpyContext_t** ppstExtMemCpyObjContext,
                                                const extMemCpySettings_t* const pstExtMemCpySettings)
{
  uint32_t status = EXTMEMCPY_SUCCESS;
  extMemCpyContext_t* pstExtMemCpyObjContext;
  uint32_t ulMemSize;
  uint8_t* pucMemPtr;

  OS_ASSERT(NULL != pstExtMemCpySettings);
  OS_ASSERT(NULL != ppstExtMemCpyObjContext);

  /* calculate the total required memory size for this object */
  ulMemSize = sizeof(extMemCpyContext_t) +
              (pstExtMemCpySettings->uiMaxSuppCopyRules * sizeof(copyRule_t)) +
              (pstExtMemCpySettings->uiMaxSuppCopyRules * sizeof(copyJob_t));

  /* allocate the memory for the object */
  pucMemPtr = os_memory_alloc(ulMemSize);

  if (NULL != pucMemPtr)
  {
    /* clear the whole data memory */
    (void) memset(pucMemPtr, 0x00, ulMemSize);

    /* initialise the context header */
    pstExtMemCpyObjContext = (extMemCpyContext_t*) ((void*) pucMemPtr);
    pstExtMemCpyObjContext->stGlobal.ulMemSize = ulMemSize;
    (void) memcpy (&pstExtMemCpyObjContext->stExtMemCpyData.stSettings, pstExtMemCpySettings,
                   sizeof(extMemCpySettings_t));
    pucMemPtr += sizeof(extMemCpyContext_t);

    /* initialise the copy rule list */
    pstExtMemCpyObjContext->stExtMemCpyData.pastCopyRuleList = (copyRule_t*) ((void*) pucMemPtr);
    pucMemPtr += pstExtMemCpySettings->uiMaxSuppCopyRules * sizeof(copyRule_t);

    /* initialise the copy job list */
    pstExtMemCpyObjContext->stExtMemCpyData.pastCopyJobList = (copyJob_t*) ((void*) pucMemPtr);

    /* create the critical section, semaphores and the threads */
    pstExtMemCpyObjContext->stExtMemCpyData.pCritSecApi = os_critical_section_create();

    if(NULL != pstExtMemCpyObjContext->stExtMemCpyData.pCritSecApi)
    {
      /* copy the context pointer */
      *ppstExtMemCpyObjContext = pstExtMemCpyObjContext;
    }
    else
    {
      /* release the component context */
      extMemCpy_ObjContextRelease(&pstExtMemCpyObjContext);

      status = EXTMEMCPY_ERROR_RESSOURCE_FAULT;
    }
  }
  else
  {
    status = EXTMEMCPY_ERROR_RESSOURCE_FAULT;
  }

  return (status);
}

//-- Function: extMemCpy_ObjContextRelease -----------------------------------------------------------------------------
///  \ingroup page_extMemCpy_sysi_functions
///
///  This function releases the context of the EMC functionality component object. The allocated
///   memory is released as well.
///
///  \param   ppstExtMemCpyObjContext    pointer to the value where the EMC functionality object context is stored
//----------------------------------------------------------------------------------------------------------------------
static void extMemCpy_ObjContextRelease (extMemCpyContext_t** ppstExtMemCpyObjContext)
{
  const extMemCpyContext_t* pstExtMemCpyObjContext;

  OS_ASSERT(NULL != ppstExtMemCpyObjContext);
  OS_ASSERT(NULL != *ppstExtMemCpyObjContext);

  pstExtMemCpyObjContext = *ppstExtMemCpyObjContext;

  /* destroy the semaphore */
  os_critical_section_destroy(pstExtMemCpyObjContext->stExtMemCpyData.pCritSecApi);

  /* deallocate the component memory */
  os_memory_dealloc(*ppstExtMemCpyObjContext);

  *ppstExtMemCpyObjContext = NULL;
}

//---- End of source file ----------------------------------------------------------------------------------------------
