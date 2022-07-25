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
///  \file     copyJobGen_API.c
///
///  \version  $Id:
///
///  \brief    This module is the application interface for the Copy-Job-Generator functionality. All available
///             functions can only be accessed by using this interface. This module generates an optimized copy
///             job list from a specified copy rule list.
///
///  \author   Wauer : WAGO GmbH & Co. KG
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------------------------------
#include <stdint.h>

#include "os_api.h"

#include "extMemCpy_SYSI.h"

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
static status_t copyJobGen_GenCopyJobOffsetToOffset     (uint_t* puiCpyRuleIdx,
                                                         copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                         extMemCpyData_t* const pstObjData);
static status_t copyJobGen_GenCopyJobOffsetToAbsolute   (uint_t* const puiCpyRuleIdx,
                                                         copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                         extMemCpyData_t* const pstObjData);
static status_t copyJobGen_GenCopyJobAbsoluteToOffset   (uint_t* const puiCpyRuleIdx,
                                                         copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                         extMemCpyData_t* const pstObjData);
static status_t copyJobGen_GenCopyJobAbsoluteToAbsolute (uint_t* const puiCpyRuleIdx,
                                                         copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                         extMemCpyData_t* const pstObjData);
static status_t copyJobGen_GetOptimizedCopyFunction     (copyJob_t* pstCopyJob,
                                                         const uint_t uiBitSize,
                                                         copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                         extMemCpyData_t* const pstObjData);

//----------------------------------------------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------------------------------------------

//-- Function: copyJobGen_GenCopyJobList -------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
status_t copyJobGen_GenCopyJobList (extMemCpyData_t* pstObjData)
{
  status_t status = EXTMEMCPY_SUCCESS;
  const copyRule_t* pstCurCopyRule;
  uint_t uiCurCpyRuleIdx = 0;
  copyEngInterfaceDesc_t stCopyEngInterfaceDesc;

  OS_ASSERT(NULL != pstObjData);
  OS_ASSERT(NULL != pstObjData->pastCopyRuleList);
  OS_ASSERT(NULL != pstObjData->pastCopyJobList);

  /* get the interface description of the copy engine */
  copyEng_GetInterfaceDesc(&stCopyEngInterfaceDesc);

  /* reset the copy job information */
  pstObjData->uiNoOfCopyJobsInList = 0;

  /* process each copy rule in the list */
  while (   (EXTMEMCPY_SUCCESS == status)
         && (uiCurCpyRuleIdx < pstObjData->uiNoOfCopyRulesInList))
  {
    pstCurCopyRule = &pstObjData->pastCopyRuleList[uiCurCpyRuleIdx];

    /* select the copy rule type */
    switch(pstCurCopyRule->enmRuleTypeId)
    {
      case OFFSET_OFFSET_RULE:
        status = copyJobGen_GenCopyJobOffsetToOffset(&uiCurCpyRuleIdx, &stCopyEngInterfaceDesc, pstObjData);
        break;

      case OFFSET_ABSOLUTE_RULE:
        status = copyJobGen_GenCopyJobOffsetToAbsolute(&uiCurCpyRuleIdx, &stCopyEngInterfaceDesc, pstObjData);
        break;

      case ABSOLUTE_OFFSET_RULE:
        status = copyJobGen_GenCopyJobAbsoluteToOffset(&uiCurCpyRuleIdx, &stCopyEngInterfaceDesc, pstObjData);
        break;

      case ABSOLUTE_ABSOLUTE_RULE:
        status = copyJobGen_GenCopyJobAbsoluteToAbsolute(&uiCurCpyRuleIdx, &stCopyEngInterfaceDesc, pstObjData);
        break;

      default:
        status = EXTMEMCPY_ERROR_INVALID_COPY_RULE_TYPE;
        break;
    }
  }

  return (status);
}

//-- Function: copyJobGen_GenCopyJobOffsetToOffset ---------------------------------------------------------------------
///  \ingroup page_copyJobGen_api_functions
///
///  This function generates a copy job for copy rules of the type "Offset-To-Offset". If it is possible more than
///   one rule will be packed together in one copy job to decrease the copy time.
///
///  \param puiCpyRuleIdx             pointer to the currently processed copy rule index
///  \param pstCopyEngInterfaceDesc   pointer to the copy engine interface description
///  \param pstObjData                pointer to the Expended-Memory-Copy object context data
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM indicates failure.
//----------------------------------------------------------------------------------------------------------------------
static status_t copyJobGen_GenCopyJobOffsetToOffset(uint_t* puiCpyRuleIdx,
                                                    copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                    extMemCpyData_t* const pstObjData)
{
  status_t status;
  const copyRule_t* pstCurCopyRule;
  copyJob_t* pstCurCopyJob;
  uint_t uiGapFreeSrcDataBitOffset;
  uint_t uiGapFreeDstDataBitOffset;
  bool bRuleValid = true;
  uint_t uiBitSizeSum = 0;

  OS_ASSERT(NULL != pstCopyEngInterfaceDesc);
  OS_ASSERT(NULL != pstObjData);
  OS_ASSERT(NULL != puiCpyRuleIdx);

  TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO,
         "Start processing Offset-To-Offset rule (Index: %u)\n", *puiCpyRuleIdx);

  /* get the initial pointer */
  pstCurCopyRule = &pstObjData->pastCopyRuleList[*puiCpyRuleIdx];
  pstCurCopyJob = &pstObjData->pastCopyJobList[pstObjData->uiNoOfCopyJobsInList];

  /* initialise the variables */
  uiGapFreeSrcDataBitOffset = pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiSrcDataBitOffset;
  uiGapFreeDstDataBitOffset = pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiDstDataBitOffset;

  /* store the data start conditions */
  pstCurCopyJob->uiSrcDataByteOffset  = pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiSrcDataBitOffset >> 3;
  pstCurCopyJob->uiSrcDataBitPos      = pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiSrcDataBitOffset & 0x07U;
  pstCurCopyJob->uiDstDataByteOffset  = pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiDstDataBitOffset >> 3;
  pstCurCopyJob->uiDstDataBitPos      = pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiDstDataBitOffset & 0x07U;

  /* try to pack several rules into on job */
  while (   (bRuleValid)
         && (*puiCpyRuleIdx < pstObjData->uiNoOfCopyRulesInList))
  {
    bRuleValid = false;

    /* check the rule type */
    if (OFFSET_OFFSET_RULE == pstObjData->pastCopyRuleList[*puiCpyRuleIdx].enmRuleTypeId)
    {
      /* get the pointer to the current list entries */
      pstCurCopyRule = &pstObjData->pastCopyRuleList[*puiCpyRuleIdx];

      /* check if there is no gap in the source and destination data bit offset */
      if (   (pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiSrcDataBitOffset == uiGapFreeSrcDataBitOffset)
          && (pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiDstDataBitOffset == uiGapFreeDstDataBitOffset))
      {
        bRuleValid = true;

        /* add the data length to be copied */
        uiBitSizeSum += pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiBitSize;

        /* check the next data copy rule */
        uiGapFreeSrcDataBitOffset = pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiSrcDataBitOffset +
            pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiBitSize;
        uiGapFreeDstDataBitOffset = pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiDstDataBitOffset +
            pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiBitSize;
            
        TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO,
               "Rule properties (Index: %u, SrcBitOffset: %u, DstBitOffset: %u, BitSize: %u)\n", *puiCpyRuleIdx,
               pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiSrcDataBitOffset,
               pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiDstDataBitOffset,
               pstCurCopyRule->unRuleTypes.stOffsetToOffsetRule.uiBitSize);
            

        (*puiCpyRuleIdx)++;
      }
    }
  }

  pstCurCopyJob->pSrcData            = NULL;
  pstCurCopyJob->pDstData            = NULL;
  pstCurCopyJob->pfCopyJobPrepFunc   = pstCopyEngInterfaceDesc->pfPrepareCopyJobOffsetToOffset;

  /* select the optimized copy function */
  status = copyJobGen_GetOptimizedCopyFunction(pstCurCopyJob, uiBitSizeSum, pstCopyEngInterfaceDesc, pstObjData);

  pstObjData->uiNoOfCopyJobsInList++;

  TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO,
         "Generate copy job (SrcByteOffset: %u, SrcBitPos: %u, DstByteOffset: %u, DstBitPos: %u, Elements: %u)\n",
         pstCurCopyJob->uiSrcDataByteOffset, pstCurCopyJob->uiSrcDataBitPos, pstCurCopyJob->uiDstDataByteOffset,
         pstCurCopyJob->uiDstDataBitPos, pstCurCopyJob->uiNoOfElements);

  return (status);
}

//-- Function: copyJobGen_GenCopyJobOffseteToAbsolute ------------------------------------------------------------------
///  \ingroup page_copyJobGen_api_functions
//
///  This function generates a copy job for copy rules of the type "Offset-To-Absolute". If it is possible more than
///   one rule will be packed together in one copy job to decrease the copy time.
///
///  \param puiCpyRuleIdx             pointer to the currently processed copy rule index
///  \param pstCopyEngInterfaceDesc   pointer to the copy engine interface description
///  \param pstObjData                pointer to the Expended-Memory-Copy object context data
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM indicates failure.
//----------------------------------------------------------------------------------------------------------------------
static status_t copyJobGen_GenCopyJobOffsetToAbsolute(uint_t* const puiCpyRuleIdx,
                                                      copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                      extMemCpyData_t* const pstObjData)
{
  status_t status = EXTMEMCPY_SUCCESS;
  const copyRule_t* pstCurCopyRule;
  copyJob_t* pstCopyJob;

  OS_ASSERT(NULL != pstCopyEngInterfaceDesc);
  OS_ASSERT(NULL != pstObjData);
  OS_ASSERT(NULL != puiCpyRuleIdx);

  pstCurCopyRule = &pstObjData->pastCopyRuleList[*puiCpyRuleIdx];
  pstCopyJob = &pstObjData->pastCopyJobList[*puiCpyRuleIdx];

  /* check the bit values */
  if (   (0 == (pstCurCopyRule->unRuleTypes.stOffsetToAbsoluteRule.uiDstDataBitPos))
      && (0 == (pstCurCopyRule->unRuleTypes.stOffsetToAbsoluteRule.uiSrcDataBitOffset & (0x07U)))
      && (0 == (pstCurCopyRule->unRuleTypes.stOffsetToAbsoluteRule.uiBitSize & (0x07U))))
  {
    pstCopyJob->pSrcData            = NULL;
    pstCopyJob->pDstData            = NULL;
    pstCopyJob->uiSrcDataByteOffset = pstCurCopyRule->unRuleTypes.stOffsetToAbsoluteRule.uiSrcDataBitOffset >> 3;
    pstCopyJob->uiSrcDataBitPos     = 0;
    pstCopyJob->uiDstDataByteOffset = 0;
    pstCopyJob->uiDstDataBitPos     = pstCurCopyRule->unRuleTypes.stOffsetToAbsoluteRule.uiDstDataBitPos;
    pstCopyJob->uiNoOfElements      = pstCurCopyRule->unRuleTypes.stOffsetToAbsoluteRule.uiBitSize >> 3;
    pstCopyJob->pfCopyJobPrepFunc   = pstCopyEngInterfaceDesc->pfPrepareCopyJobAbsoluteToOffset;
    pstCopyJob->pfCopyJobFunc       = pstCopyEngInterfaceDesc->pfCopyArcSpec;

    pstObjData->uiNoOfCopyJobsInList++;
  }
  else
  {
    status = EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM;
  }

  return (status);
}

//-- Function: copyJobGen_GenCopyJobAbsoluteToOffset -------------------------------------------------------------------
///  \ingroup page_copyJobGen_api_functions
///
///  This function generates a copy job for copy rules of the type "Absolute-To-Offset". If it is possible more than
///   one rule will be packed together in one copy job to decrease the copy time.
///
///  \param puiCpyRuleIdx             pointer to the currently processed copy rule index
///  \param pstCopyEngInterfaceDesc   pointer to the copy engine interface description
///  \param pstObjData                pointer to the Expended-Memory-Copy object context data
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM indicates failure.
//----------------------------------------------------------------------------------------------------------------------
static status_t copyJobGen_GenCopyJobAbsoluteToOffset(uint_t* const puiCpyRuleIdx,
                                                      copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                      extMemCpyData_t* const pstObjData)
{
  status_t status = EXTMEMCPY_SUCCESS;
  const copyRule_t* pstCurCopyRule;
  copyJob_t* pstCopyJob;

  OS_ASSERT(NULL != pstCopyEngInterfaceDesc);
  OS_ASSERT(NULL != pstObjData);
  OS_ASSERT(NULL != puiCpyRuleIdx);

  pstCurCopyRule = &pstObjData->pastCopyRuleList[*puiCpyRuleIdx];
  pstCopyJob = &pstObjData->pastCopyJobList[*puiCpyRuleIdx];

  /* check the bit values */
  if (   (0 == (pstCurCopyRule->unRuleTypes.stAbsoluteToOffsetRule.uiSrcDataBitPos))
      && (0 == (pstCurCopyRule->unRuleTypes.stAbsoluteToOffsetRule.uiDstDataBitOffset & (0x07U)))
      && (0 == (pstCurCopyRule->unRuleTypes.stAbsoluteToOffsetRule.uiBitSize & (0x07U))))
  {
    pstCopyJob->pSrcData            = NULL;
    pstCopyJob->pDstData            = NULL;
    pstCopyJob->uiSrcDataByteOffset = 0;
    pstCopyJob->uiSrcDataBitPos     = pstCurCopyRule->unRuleTypes.stAbsoluteToOffsetRule.uiSrcDataBitPos;
    pstCopyJob->uiDstDataByteOffset = pstCurCopyRule->unRuleTypes.stAbsoluteToOffsetRule.uiDstDataBitOffset >> 3;
    pstCopyJob->uiDstDataBitPos     = 0;
    pstCopyJob->uiNoOfElements      = pstCurCopyRule->unRuleTypes.stAbsoluteToOffsetRule.uiBitSize >> 3;
    pstCopyJob->pfCopyJobPrepFunc   = pstCopyEngInterfaceDesc->pfPrepareCopyJobAbsoluteToOffset;
    pstCopyJob->pfCopyJobFunc       = pstCopyEngInterfaceDesc->pfCopyArcSpec;

    pstObjData->uiNoOfCopyJobsInList++;
  }
  else
  {
    status = EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM;
  }

  return (status);
}

//-- Function: copyJobGen_GenCopyJobAbsoluteToAbsolute -----------------------------------------------------------------
///  \ingroup page_copyJobGen_api_functions
///
///  This function generates a copy job for copy rules of the type "Absolute-To-Absolute". If it is possible more than
///   one rule will be packed together in one copy job to decrease the copy time.
///
///  \param puiCpyRuleIdx             pointer to the currently processed copy rule index
///  \param pstCopyEngInterfaceDesc   pointer to the copy engine interface description
///  \param pstObjData                pointer to the Expended-Memory-Copy object context data
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM indicates failure.
//----------------------------------------------------------------------------------------------------------------------
static status_t copyJobGen_GenCopyJobAbsoluteToAbsolute(uint_t* const puiCpyRuleIdx,
                                                        copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                        extMemCpyData_t* const pstObjData)
{
  status_t status = EXTMEMCPY_SUCCESS;
  const copyRule_t* pstCurCopyRule;
  copyJob_t* pstCopyJob;

  OS_ASSERT(NULL != pstCopyEngInterfaceDesc);
  OS_ASSERT(NULL != pstObjData);
  OS_ASSERT(NULL != puiCpyRuleIdx);

  pstCurCopyRule = &pstObjData->pastCopyRuleList[*puiCpyRuleIdx];
  pstCopyJob = &pstObjData->pastCopyJobList[*puiCpyRuleIdx];

  /* check the bit values */
  if (   (0 == (pstCurCopyRule->unRuleTypes.stAbsoluteToAbsoluteRule.uiSrcDataBitPos))
      && (0 == (pstCurCopyRule->unRuleTypes.stAbsoluteToAbsoluteRule.uiDstDataBitPos))
      && (0 == (pstCurCopyRule->unRuleTypes.stAbsoluteToAbsoluteRule.uiBitSize & (0x07U))))
  {
    pstCopyJob->pSrcData            = pstCurCopyRule->unRuleTypes.stAbsoluteToAbsoluteRule.pSrcData;
    pstCopyJob->pDstData            = pstCurCopyRule->unRuleTypes.stAbsoluteToAbsoluteRule.pDstData;
    pstCopyJob->uiSrcDataByteOffset = 0;
    pstCopyJob->uiSrcDataBitPos     = pstCurCopyRule->unRuleTypes.stAbsoluteToAbsoluteRule.uiSrcDataBitPos;
    pstCopyJob->uiDstDataByteOffset = 0;
    pstCopyJob->uiDstDataBitPos     = pstCurCopyRule->unRuleTypes.stAbsoluteToAbsoluteRule.uiDstDataBitPos;
    pstCopyJob->uiNoOfElements      = pstCurCopyRule->unRuleTypes.stAbsoluteToAbsoluteRule.uiBitSize >> 3;
    pstCopyJob->pfCopyJobPrepFunc   = pstCopyEngInterfaceDesc->pfPrepareCopyJobAbsoluteToAbsolute;
    pstCopyJob->pfCopyJobFunc       = pstCopyEngInterfaceDesc->pfCopyArcSpec;

    pstObjData->uiNoOfCopyJobsInList++;
  }
  else
  {
    status = EXTMEMCPY_ERROR_INVALID_COPY_RULE_PARAM;
  }

  return (status);
}

//-- Function: copyJobGen_GetOptimizedCopyFunction ---------------------------------------------------------------------
///  \ingroup page_copyJobGen_api_functions
///
///  This function selects the optimized copy function depending on the bit size to be copied and the specified
///   maximum copy data length. The results are stored in the specified copy job.
///
///  \param pstCopyJob                pointer to the currently processed copy job
///  \param uiBitSize                 bit size to be copied by the copy function
///  \param pstCopyEngInterfaceDesc   pointer to the copy engine interface description
///  \param pstObjData                pointer to the Expended-Memory-Copy object context data
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_INVCOMPATIBLE_MAX_COPY_LEN indicates failure.
//----------------------------------------------------------------------------------------------------------------------
static status_t copyJobGen_GetOptimizedCopyFunction (copyJob_t* pstCopyJob,
                                                     const uint_t uiBitSize,
                                                     copyEngInterfaceDesc_t* const pstCopyEngInterfaceDesc,
                                                     extMemCpyData_t* const pstObjData)
{
  status_t status = EXTMEMCPY_SUCCESS;

  OS_ASSERT (NULL != pstCopyJob);
  OS_ASSERT (NULL != pstCopyEngInterfaceDesc);
  OS_ASSERT (NULL != pstObjData);

  /* if coping bitwise is not necessary */
  if (   (0 == (pstCopyJob->uiSrcDataBitPos & 0x07U))
      && (0 == (pstCopyJob->uiDstDataBitPos & 0x07U))
      && (0 == (uiBitSize & 0x07U)))
  {
    /* select the max copy length */
    switch (pstObjData->stSettings.enmMaxCopyOptimization)
    {
      case MAX_COPY_LEN_BIT:
        pstCopyJob->uiNoOfElements = uiBitSize;
        pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyBit;
        TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <Bit>\n");
        break;

      case MAX_COPY_LEN_BYTE:

        /* select the copy function */
        switch (uiBitSize >> 3)
        {
          case 1:
           pstCopyJob->uiNoOfElements = 1;
           pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyOneByte;
           TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <OneByte>\n");
           break;

          default:
            pstCopyJob->uiNoOfElements = uiBitSize >> 3;
            pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyByte;
            TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <Byte>\n");
            break;
        }
        break;


      case MAX_COPY_LEN_WORD:

        /* select the copy function */
        switch (uiBitSize >> 3)
        {
          case 1:
           pstCopyJob->uiNoOfElements = 1;
           pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyOneByte;
           TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <OneByte>\n");
           break;

          case 2:
            pstCopyJob->uiNoOfElements = 1;
            pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyOneWord;
            TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <OneWord>\n");
            break;

          default:
            /* is word a wise copy possible */
            if (0 == (uiBitSize & 0x0FU))
            {
              pstCopyJob->uiNoOfElements = uiBitSize >> 4;
              pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyWord;
              TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <Word>\n");
            }
            else
            {
              pstCopyJob->uiNoOfElements = uiBitSize >> 3;
              pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyByte;
              TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <Byte>\n");
            }
            break;
          }
        break;

      case MAX_COPY_LEN_DWORD:

        /* select the copy function */
        switch (uiBitSize >> 3)
        {
          case 1:
            pstCopyJob->uiNoOfElements = 1;
            pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyOneByte;
            TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <OneByte>\n");
            break;
          case 2:
            pstCopyJob->uiNoOfElements = 1;
            pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyOneWord;
            TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <OneWord>\n");
            break;
          case 4:
            pstCopyJob->uiNoOfElements = 1;
            pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyOneDWord;
            TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <OneDWord>\n");
            break;
          default:
            pstCopyJob->uiNoOfElements = uiBitSize >> 3;
            pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyArcSpec;
            TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <ArcSpec>\n");
            break;
          }
        break;

      default:
        status = EXTMEMCPY_ERROR_INVCOMPATIBLE_MAX_COPY_LEN;
        break;
    }
  }
  else
  {
    pstCopyJob->uiNoOfElements = uiBitSize;
    pstCopyJob->pfCopyJobFunc = pstCopyEngInterfaceDesc->pfCopyBit;
    TRACE (EXT_MEM_CPY_TRACE_SRC_COPY_JOB_GEN, EXT_MEM_CPY_TRACE_SEV_INFO, "Select copy function <Bit>\n");
  }

  return (status);
}

//---- End of source file ----------------------------------------------------------------------------------------------
