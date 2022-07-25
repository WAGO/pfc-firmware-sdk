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
///  \file     testCases.c
///
///  \version  $Id:
///
///  \brief    This module collects different functions which are necessary for the PROFIBUS slave application
///             component functionality.
///
///  \author   Wauer : WAGO GmbH & Co. KG
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------------------------------
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

#include "os_api.h"

#include "extMemCpy_SYSI.h"
#include "extMemCpy_API.h"

#include "trace_SYSI.h"
#include "trace_API.h"

#include "testCases.h"

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  handle_t pExtMemCpyHdl;
  handle_t pTraceHdl;
}extMemCpyTestData_t;

/** test function description */
typedef status_t (*testFunction_f) (extMemCpyTestData_t*);

//----------------------------------------------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------------------------------------------------
static status_t testCases_InitialiseTestData                      (extMemCpyTestData_t* pstTestData);
static status_t testCases_InitialiseAndActivateTraceInstance      (extMemCpyTestData_t* pstTestData);
static status_t testCases_InitialiseExtMemCpyInstance             (extMemCpyTestData_t* pstTestData);
static status_t testCases_CopySpeedCheck                          (extMemCpyTestData_t* pstTestData);
static status_t testCases_CopyDataCheck1                          (extMemCpyTestData_t* pstTestData);
static status_t testCases_ReleaseExtMemCpyInstance                (extMemCpyTestData_t* pstTestData);
static status_t testCases_DeactivateAndReleaseTraceInstance       (extMemCpyTestData_t* pstTestData);

//----------------------------------------------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------------------------------------------

//-- Function: testCases_Start -----------------------------------------------------------------------------------------
///
///  This function ...
///
///  \param
///
//----------------------------------------------------------------------------------------------------------------------
status_t testCases_Start (void)
{
  status_t status = EXTMEMCPY_SUCCESS;
  uint32_t ulTestCaseIdx = 0;
  uint32_t ulNoOfTestCases;
  extMemCpyTestData_t stPbdpSlvApplTestData;

  testFunction_f astTestFunction[] =
  {
   testCases_InitialiseTestData,
   testCases_InitialiseAndActivateTraceInstance,
   testCases_InitialiseExtMemCpyInstance,
   testCases_CopySpeedCheck,
   testCases_CopyDataCheck1,
   testCases_ReleaseExtMemCpyInstance,
   testCases_DeactivateAndReleaseTraceInstance
  };

  ulNoOfTestCases = OS_MAX_COUNT(astTestFunction);

  /* process the test cases until a error will occur */
  while (   (EXTMEMCPY_SUCCESS == status)
         && (ulTestCaseIdx < ulNoOfTestCases))
  {
    /* call the test case */
    sleep(1);
    printf ("\n");
    printf ("Call test case %d...\n",ulTestCaseIdx + 1);
    status = astTestFunction[ulTestCaseIdx](&stPbdpSlvApplTestData);

    /* process the next test case */
    ulTestCaseIdx++;
  }

  return (status);
}

//-- Function: testCases_InitialiseTestData ----------------------------------------------------------------------------
///
///  This function ...
///
///  \param
///
//----------------------------------------------------------------------------------------------------------------------
status_t testCases_InitialiseTestData(extMemCpyTestData_t* pstTestData)
{
  status_t status = EXTMEMCPY_SUCCESS;

  /* reset the settings */
  (void) memset(&pstTestData, 0x00, sizeof(pstTestData));

  printf ("Test of the dpc31Driver software component has been started\n");

  return (status);
}

//-- Function: testCases_InitialiseAndActivateTraceInstance ------------------------------------------------------------
///
///  This function ...
///
///  \param
///
//----------------------------------------------------------------------------------------------------------------------
status_t testCases_InitialiseAndActivateTraceInstance(extMemCpyTestData_t* pstTestData)
{
  status_t status = EXTMEMCPY_ERROR_UNDEFINED;

  OS_ARGUMENT_USED(pstTestData);

  /* get an instance of the PROFIBUS slave application component */
  if (   (TRACE_SUCCESS == trace_Initialise(NULL))
      && (TRACE_SUCCESS == trace_Activate()))
  {
    printf ("The initializing and activating of the Trace functionality was successfully\n");
    TRACE (1, INF, "Trace-Test 1\n");
    TRACE (1, INF, "Trace-Test 2\n");
    TRACE (1, INF, "Trace-Test 3\n");
    TRACE (1, INF, "Trace-Test 4\n");
    status = EXTMEMCPY_SUCCESS;
  }
  else
  {
    printf ("The initializing and activating of the Trace functionality has failed\n");
  }

  return (status);
}

//-- Function: testCases_InitialiseExtMemCpyInstance -------------------------------------------------------------------
///
///  This function ...
///
///  \param
///
//----------------------------------------------------------------------------------------------------------------------
status_t testCases_InitialiseExtMemCpyInstance(extMemCpyTestData_t* pstTestData)
{
  status_t status = EXTMEMCPY_ERROR_UNDEFINED;
  extMemCpySettings_t stExtMemCpySettings;

  stExtMemCpySettings.enmMaxCopyOptimization  = MAX_COPY_LEN_DWORD;
  stExtMemCpySettings.uiMaxSuppCopyRules      = 1024;

  /* get an instance of the PROFIBUS slave application component */
  if (NULL != (pstTestData->pExtMemCpyHdl = extMemCpy_Initialise(&stExtMemCpySettings)))
  {
    TRACE (1, INF, "The initializing of the Extended-Memory-Copy was successfully\n");
    status = EXTMEMCPY_SUCCESS;
  }
  else
  {
    TRACE (1, INF, "The initializing of the Extended-Memory-Copy has failed\n");
  }

  return (status);
}

//-- Function: testCases_CopySpeedCheck --------------------------------------------------------------------------------
///
///  This function ...
///
///  \param
///
//----------------------------------------------------------------------------------------------------------------------
status_t testCases_CopySpeedCheck(extMemCpyTestData_t* pstTestData)
{
  uint8_t aucSrcTestData[1024];
  uint8_t aucDstTestData[1024];
  uint8_t* pucSrcTestData;
  uint8_t* pucDstTestData;
  uint32_t ulCnt = 1024;

  OS_ARGUMENT_USED(pstTestData);

  pucSrcTestData = &aucSrcTestData[0];
  pucDstTestData = &aucDstTestData[0];
  TRACE (1, INF, "Memcpy (8bit) test started\n");
  while (ulCnt -= 2)
  {
    (void) memcpy(pucDstTestData + 1, pucSrcTestData, 1);
    (void) memcpy(pucDstTestData, pucSrcTestData + 1, 1);
    pucSrcTestData += 2;
    pucDstTestData += 2;
  }
  TRACE (1, INF, "Memcpy (8bit) test finished\n");

  TRACE (1, INF, "Memcpy (32bit) test started\n");
  (void) memcpy(aucDstTestData, aucSrcTestData, 1024);
  TRACE (1, INF, "Memcpy (32bit) test finished\n");

  return (EXTMEMCPY_SUCCESS);
}



//-- Function: testCases_CopyDataCheck1 --------------------------------------------------------------------------------
///
///  This function ...
///
///  \param
///
//----------------------------------------------------------------------------------------------------------------------
status_t testCases_CopyDataCheck1(extMemCpyTestData_t* pstTestData)
{
  status_t status = EXTMEMCPY_ERROR_UNDEFINED;
  uint32_t ulIdx;
  copyRule_t astCopyRuleList[1024];
  uint8_t aucSrcTestData[1024];
  uint8_t aucDstTestData[1024];
  uint8_t aucDstTestDataRef[1024];

  TRACE (1, INF, "Generate the copy rule list...\n");

  /* generate the test data */
  for (ulIdx = 0; ulIdx < 1024; ulIdx++)
  {
    aucSrcTestData[ulIdx] = ulIdx;
  }

  /* generate the copy rule list and the test data (byte x -> byte x + 1, byte x + 1 -> byte x, ...) */
  for (ulIdx = 0; ulIdx < 512; ulIdx++)
  {
    astCopyRuleList[ulIdx].enmRuleType = OFFSET_OFFSET_RULE;
    astCopyRuleList[ulIdx].stOffsetToOffsetRule.uiSrcDataBitOffset = ulIdx * 8;
    astCopyRuleList[ulIdx].stOffsetToOffsetRule.uiDstDataBitOffset = ulIdx * 8;
    astCopyRuleList[ulIdx].stOffsetToOffsetRule.uiBitSize = 8;

    aucDstTestDataRef[ulIdx] = (uint8_t) ulIdx;
  }

  /* generate the copy rule list and the test data (byte x -> byte x + 1, byte x + 1 -> byte x, ...) */
  for (ulIdx = 512; ulIdx < 1024; ulIdx += 2)
  {
    astCopyRuleList[ulIdx].enmRuleType = OFFSET_OFFSET_RULE;
    astCopyRuleList[ulIdx].stOffsetToOffsetRule.uiSrcDataBitOffset = ulIdx * 8;
    astCopyRuleList[ulIdx].stOffsetToOffsetRule.uiDstDataBitOffset = (ulIdx + 1) * 8;
    astCopyRuleList[ulIdx].stOffsetToOffsetRule.uiBitSize = 8;

    astCopyRuleList[ulIdx + 1].enmRuleType = OFFSET_OFFSET_RULE;
    astCopyRuleList[ulIdx + 1].stOffsetToOffsetRule.uiSrcDataBitOffset = (ulIdx + 1) * 8;
    astCopyRuleList[ulIdx + 1].stOffsetToOffsetRule.uiDstDataBitOffset = ulIdx * 8;;
    astCopyRuleList[ulIdx + 1].stOffsetToOffsetRule.uiBitSize = 8;

    aucDstTestDataRef[ulIdx]      = (uint8_t) ulIdx + 1;
    aucDstTestDataRef[ulIdx + 1]  = (uint8_t) ulIdx;
  }

  TRACE (1, INF, "Generating the copy rule list finished\n");

  /* set the copy rule */
  if (EXTMEMCPY_SUCCESS == (status = extMemCpy_AddCopyRuleList(&astCopyRuleList[0], OS_MAX_COUNT(astCopyRuleList),
                                                               pstTestData->pExtMemCpyHdl)))
  {
    TRACE (1, INF, "The adding of a copy rule list was successful\n");

    /* refresh copy rule list */
    if (EXTMEMCPY_SUCCESS == (status = extMemCpy_RefreshCopyRuleList(pstTestData->pExtMemCpyHdl)))
    {
      TRACE (1, INF, "The refreshing of the copy rule list was successful\n");

      /* copy test data */
      if(EXTMEMCPY_SUCCESS == (status = extMemCpy_CopyData(aucDstTestData, aucSrcTestData,
                                                           pstTestData->pExtMemCpyHdl)))
      {
        TRACE (1, INF, "Copying the data was successful\n");

        /* check test data */
        if (0 == memcmp (aucDstTestData,aucDstTestDataRef,sizeof(aucDstTestDataRef)))
        {
          TRACE (1, INF, "Copied data were equal\n");
        }
        else
        {
          TRACE (1, INF, "Copied data were not equal\n");
        }
      }
      else
      {
        TRACE (1, INF, "Copying the data has failed\n");
      }
    }
    else
    {
      TRACE (1, INF, "The refreshing of the copy rule list has failed\n");
    }
  }
  else
  {
    TRACE (1, INF, "The adding of a copy rule list has failed\n");
  }

  return (status);
}

//-- Function: testCases_ReleaseExtMemCpyInstance ----------------------------------------------------------------------
///
///  This function ...
///
///  \param
///
//----------------------------------------------------------------------------------------------------------------------
status_t testCases_ReleaseExtMemCpyInstance(extMemCpyTestData_t* pstTestData)
{
  status_t status = EXTMEMCPY_ERROR_UNDEFINED;

  /* get an instance of the PROFIBUS slave application component */
  if (EXTMEMCPY_SUCCESS == (status = extMemCpy_Release(pstTestData->pExtMemCpyHdl)))
  {
    TRACE (1, INF, "The releasing of the Extended-Memory-Copy was successfully\n");
  }
  else
  {
    TRACE (1, INF, "The releasing of the Extended-Memory-Copy has failed\n");
  }

  return (status);
}

//-- Function: testCases_DeactivateAndReleaseTraceInstance -------------------------------------------------------------
///
///  This function ...
///
///  \param
///
//----------------------------------------------------------------------------------------------------------------------
status_t testCases_DeactivateAndReleaseTraceInstance(extMemCpyTestData_t* pstTestData)
{
  status_t status = EXTMEMCPY_ERROR_UNDEFINED;

  OS_ARGUMENT_USED(pstTestData);

  /* get an instance of the PROFIBUS slave application component */
  if (   (TRACE_SUCCESS == trace_Deactivate())
      && (TRACE_SUCCESS == trace_Release()))
  {
    printf ("The deactivating and releasing of the Trace functionality was successfully\n");
    status = EXTMEMCPY_SUCCESS;
  }
  else
  {
    printf ("The deactivating and releasing of the Trace functionality has failed\n");
  }

  return (status);
}

//---- End of source file ----------------------------------------------------------------------------------------------
