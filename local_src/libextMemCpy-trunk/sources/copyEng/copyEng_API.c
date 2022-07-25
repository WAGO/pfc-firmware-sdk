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
///  \file     copyEng_API.c
///
///  \version  $Id:
///
///  \brief    This module is the application interface for the Copy-Engine functionality. All available
///             functions can only be accessed by using this interface. The copy engine copies data where the data
///             locations are specified by a copy job list.
///
///  \author   Wauer : WAGO GmbH & Co. KG
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "os_api.h"

#include "copyEng_API.h"
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
static void copyEng_PrepareCopyJobOffsetToOffset      (void* pDstData, void* pSrcData, copyJob_t* pstCopyJob);
static void copyEng_PrepareCopyJobOffsetToAbsolute    (void* pDstData, void* pSrcData, copyJob_t* pstCopyJob);
static void copyEng_PrepareCopyJobAbsoluteToOffset    (void* pDstData, void* pSrcData, copyJob_t* pstCopyJob);
static void copyEng_PrepareCopyJobAbsoluteToAbsolute  (void* pDstData, void* pSrcData, copyJob_t* pstCopyJob);
static void copyEng_CopyOneBit                        (void* const pDstData, void* const pSrcData,
                                                       copyJob_t* const pstCopyJob);
static void copyEng_CopyBit                           (void* const pDstData, void* const pSrcData,
                                                       copyJob_t* const pstCopyJob);
static void copyEng_CopyOneByte                       (void* pDstData, void* const pSrcData,
                                                       copyJob_t* const pstCopyJob);
static void copyEng_CopyByte                          (void* pDstData, void* pSrcData,
                                                       copyJob_t* const pstCopyJob);
static void copyEng_CopyWord                          (void* pDstData, void* pSrcData, copyJob_t* const pstCopyJob);
static void copyEng_CopyOneWord                       (void* pDstData, void* const pSrcData,
                                                       copyJob_t* const pstCopyJob);
static void copyEng_CopyDWord                         (void* pDstData, void* const pSrcData,
                                                       copyJob_t* const pstCopyJob);
static void copyEng_CopyOneDWord                      (void* pDstData, void* const pSrcData,
                                                       copyJob_t* const pstCopyJob);
static void copyEng_CopyArcSpec                       (void* pDstData, void* const pSrcData,
                                                       copyJob_t* const pstCopyJob);

//----------------------------------------------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------------------------------------------

//-- Function: copyEng_PrcCopyJobList ----------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
void copyEng_PrcCopyJobList (void* pDstData,
                             void* pSrcData,
                             copyJob_t* pastCopyJobList,
                             uint_t ulNoOfCopyJobs)
{
  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  //OS_ASSERT(NULL != pastCopyJobList);

  /* process each specified copy job */
  for (; 0 != ulNoOfCopyJobs; ulNoOfCopyJobs--)
  {
    OS_ASSERT(NULL != pastCopyJobList);
    OS_ASSERT(NULL != pastCopyJobList->pfCopyJobPrepFunc);

    /* call the copy job prepare function */
    pastCopyJobList->pfCopyJobPrepFunc (pDstData, pSrcData, pastCopyJobList);

    /* process the next copy job */
    pastCopyJobList++;
  }
}

//-- Function: copyEng_GetInterfaceDesc --------------------------------------------------------------------------------
///  \skip For detailed description see the corresponding header file.
//----------------------------------------------------------------------------------------------------------------------
void copyEng_GetInterfaceDesc (copyEngInterfaceDesc_t* pstCopyEngInterfaceDesc)
{
  OS_ASSERT(NULL != pstCopyEngInterfaceDesc);

  /* copy preparing functions */
  pstCopyEngInterfaceDesc->pfPrepareCopyJobOffsetToOffset       = &copyEng_PrepareCopyJobOffsetToOffset;
  pstCopyEngInterfaceDesc->pfPrepareCopyJobOffsetToAbsolute     = &copyEng_PrepareCopyJobOffsetToAbsolute;
  pstCopyEngInterfaceDesc->pfPrepareCopyJobAbsoluteToOffset     = &copyEng_PrepareCopyJobAbsoluteToOffset;
  pstCopyEngInterfaceDesc->pfPrepareCopyJobAbsoluteToAbsolute   = &copyEng_PrepareCopyJobAbsoluteToAbsolute;
  /* copy functions */
  pstCopyEngInterfaceDesc->pfCopyBit                            = &copyEng_CopyBit;
  pstCopyEngInterfaceDesc->pfCopyOneBit                         = &copyEng_CopyOneBit;
  pstCopyEngInterfaceDesc->pfCopyByte                           = &copyEng_CopyByte;
  pstCopyEngInterfaceDesc->pfCopyOneByte                        = &copyEng_CopyOneByte;
  pstCopyEngInterfaceDesc->pfCopyWord                           = &copyEng_CopyWord;
  pstCopyEngInterfaceDesc->pfCopyOneWord                        = &copyEng_CopyOneWord;
  pstCopyEngInterfaceDesc->pfCopyDWord                          = &copyEng_CopyDWord;
  pstCopyEngInterfaceDesc->pfCopyOneDWord                       = &copyEng_CopyOneDWord;
  pstCopyEngInterfaceDesc->pfCopyArcSpec                        = &copyEng_CopyArcSpec;
}

//-- Function: copyEng_PrepareCopyJobOffsetToOffset --------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies the data specified by the copy job from offset address to offset address. The offsets
///   relating to the specified data memory pointer.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_PrepareCopyJobOffsetToOffset(void* pDstData,
                                                 void* pSrcData,
                                                 copyJob_t* pstCopyJob)
{
  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);
  OS_ASSERT(NULL != pstCopyJob->pfCopyJobFunc);

  /* call the specified copy function */
  pstCopyJob->pfCopyJobFunc(((uint8_t*) pDstData) + pstCopyJob->uiDstDataByteOffset,
                            ((uint8_t*) pSrcData) + pstCopyJob->uiSrcDataByteOffset,
                            pstCopyJob);
}

//-- Function: copyEng_PrepareCopyJobOffsetToAbsolute ------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies the data specified by the copy job from offset address to absolute address. The offsets
///   relating to the specified data memory pointer. The destination data pointer is not used by this function.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_PrepareCopyJobOffsetToAbsolute(void* pDstData,
                                                   void* pSrcData,
                                                   copyJob_t* pstCopyJob)
{
  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);
  OS_ASSERT(NULL != pstCopyJob->pfCopyJobFunc);

  /* call the specified copy function */
  pstCopyJob->pfCopyJobFunc(pDstData, ((uint8_t*) pSrcData) + pstCopyJob->uiSrcDataByteOffset, pstCopyJob);
}

//-- Function: copyEng_PrepareCopyJobAbsoluteToAbsolute ----------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies the data specified by the copy job from absolute address to offset address. The offsets
///   relating to the specified data memory pointer. The source data pointer is not used by this function.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_PrepareCopyJobAbsoluteToOffset(void* pDstData,
                                                   void* pSrcData,
                                                   copyJob_t* pstCopyJob)
{
  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);
  OS_ASSERT(NULL != pstCopyJob->pfCopyJobFunc);

  pstCopyJob->pfCopyJobFunc(((uint8_t*) pDstData) + pstCopyJob->uiDstDataByteOffset, pSrcData, pstCopyJob);
}

//-- Function: copyEng_PrepareCopyJobAbsoluteToAbsolute ----------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies the data specified by the copy job from absolute address to absolute address.
///   The source and destination data pointer are not used by this function.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_PrepareCopyJobAbsoluteToAbsolute(void* const pDstData,
                                                     void* const pSrcData,
                                                     copyJob_t* pstCopyJob)
{
  OS_ASSERT(NULL != pstCopyJob);
  OS_ASSERT(NULL != pstCopyJob->pfCopyJobFunc);

  OS_ARGUMENT_USED(pDstData);
  OS_ARGUMENT_USED(pSrcData);

  pstCopyJob->pfCopyJobFunc(pstCopyJob->pDstData, pstCopyJob->pSrcData, pstCopyJob);
}

//-- Function: copyEng_CopyOneBit --------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies one of bit from the specified source memory to the specified destination
///   memory.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyOneBit (void* const pDstData,
                                void* const pSrcData,
                                copyJob_t* const pstCopyJob)
{
  uint8_t* pucDstData;
  const uint8_t* pucSrcData;
  uint8_t ucSrcDataMask;

  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);

  pucDstData = (uint8_t*) pDstData;
  pucSrcData = (uint8_t*) pSrcData;

  ucSrcDataMask = (((uint8_t) (0x01U << pstCopyJob->uiNoOfElements)) - 1);

  *pucDstData |= (uint8_t)  (((uint8_t) ((*pucSrcData >> pstCopyJob->uiSrcDataBitPos) & ucSrcDataMask)) <<
                             pstCopyJob->uiSrcDataBitPos);
}

//-- Function: copyEng_CopyBit -----------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies the specified number of bits from the specified source memory to the specified destination
///   memory.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyBit (void* const pDstData,
                             void* const pSrcData,
                             copyJob_t* const pstCopyJob)
{
  uint8_t* pucDstData;
  const uint8_t* pucSrcData;
  uint_t uiCurSrcBitPos;
  uint_t uiCurDstBitPos;
  uint8_t ucSrcPrcDataByte;
  uint_t uiNoOfPendElem;
  int_t iSrcDstBitOffsetDiff;
  uint32_t ulPrcBits;
  uint8_t ucDestDataByteMask;

  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);

  pucDstData = (uint8_t*) pDstData;
  pucSrcData = (uint8_t*) pSrcData;

  /* initialise the number of pending elements */
  uiNoOfPendElem = pstCopyJob->uiNoOfElements;

  /* initialise the current bit position */
  uiCurSrcBitPos = pstCopyJob->uiSrcDataBitPos;
  uiCurDstBitPos = pstCopyJob->uiDstDataBitPos;

  while (0 != uiNoOfPendElem)
  {
    /* get the processing data bytes */
    ucSrcPrcDataByte = pucSrcData[uiCurSrcBitPos >> 3];

    /* calculate the offset differences */
    iSrcDstBitOffsetDiff = (uiCurSrcBitPos & 0x07U) - (uiCurDstBitPos & 0x07U);

    /* check the offset differences */
    if (iSrcDstBitOffsetDiff > 0)
    {
      ucSrcPrcDataByte = ucSrcPrcDataByte >> iSrcDstBitOffsetDiff;
      ulPrcBits = 8 - (uiCurSrcBitPos & 0x07U);
    }
    else
    {
      /*lint --e(504) (Unusual shift operation) */
      ucSrcPrcDataByte = (uint8_t) (ucSrcPrcDataByte << (-iSrcDstBitOffsetDiff));
      ulPrcBits = 8 - (uiCurDstBitPos & 0x07U);
    }

    /* correct the number of processed elements */
    if (ulPrcBits > uiNoOfPendElem)
    {
      ulPrcBits = uiNoOfPendElem;
    }

    /* calculate the byte mask for the destination data byte */
    ucDestDataByteMask = (uint8_t) (((uint8_t) (0x01U << ulPrcBits) - 0x01U) << uiCurDstBitPos);
    /* clear the bit in the destination data */
    pucDstData[uiCurDstBitPos >> 3] &= ~(ucDestDataByteMask);
    /* copy the bit from prepared source data to destination data */
    pucDstData[uiCurDstBitPos >> 3] |= (ucSrcPrcDataByte & ucDestDataByteMask);

    /* update the processing information */
    uiCurSrcBitPos += ulPrcBits;
    uiCurDstBitPos += ulPrcBits;
    uiNoOfPendElem -= ulPrcBits;
  }
}

//-- Function: copyEng_CopyByte ----------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function the specified number of bytes from the source memory to the destination memory.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyByte (void* pDstData,
                              void* pSrcData,
                              copyJob_t* const pstCopyJob)
{
  uint uiNoOfBytes;

  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);

  uiNoOfBytes = pstCopyJob->uiNoOfElements;

  for (;0 != uiNoOfBytes; uiNoOfBytes--)
  {
    *((uint8_t*) pDstData) = *((uint8_t*) pSrcData);
    pSrcData = (void*) (((uint8_t*) pSrcData) + 1);
    pDstData = (void*) (((uint8_t*) pDstData) + 1);
  }
}

//-- Function: copyEng_CopyOneByte -------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies one byte from the source memory to the destination memory.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyOneByte (void* pDstData,
                                 void* const pSrcData,
                                 copyJob_t* const pstCopyJob)
{
  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);

  OS_ARGUMENT_USED(pstCopyJob);

  *((uint8_t*) pDstData) = *((uint8_t*) pSrcData);
}

//-- Function: copyEng_CopyWord ----------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function the specified number of words from the source memory to the destination memory.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyWord (void* pDstData,
                              void* const pSrcData,
                              copyJob_t* const pstCopyJob)
{
  uint16_t* pusDstData;
  const uint16_t* pusSrcData;
  uint uiNoOfWords;

  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);

  pusDstData = (uint16_t*) pDstData;
  pusSrcData = (uint16_t*) pSrcData;
  uiNoOfWords = pstCopyJob->uiNoOfElements;

  for(; 0 != uiNoOfWords; uiNoOfWords--)
  {
    *pusDstData++ = *pusSrcData++;
  }
}

//-- Function: copyEng_CopyOneWord -------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies one word from the source memory to the destination memory.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyOneWord (void* pDstData,
                                 void* const pSrcData,
                                 copyJob_t* const pstCopyJob)
{
  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);

  OS_ARGUMENT_USED(pstCopyJob);

  *((uint16_t*) pDstData) = *((uint16_t*) pSrcData);
}

//-- Function: copyEng_CopyDWord ---------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function the specified number of double words from the source memory to the destination memory.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyDWord (void* pDstData,
                               void* const pSrcData,
                               copyJob_t* const pstCopyJob)
{
  uint32_t* pulDstData;
  const uint32_t* pulSrcData;
  uint_t uiNoOfDWords;

  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);

  pulDstData = (uint32_t*) pDstData;
  pulSrcData = (uint32_t*) pSrcData;
  uiNoOfDWords = pstCopyJob->uiNoOfElements;

  for (; 0 != uiNoOfDWords; uiNoOfDWords--)
  {
    *pulDstData++ = *pulSrcData++;
  }
}

//-- Function: copyEng_CopyOneDWord ------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies one double word from the source memory to the destination memory.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyOneDWord (void* pDstData,
                                  void* const pSrcData,
                                  copyJob_t* const pstCopyJob)
{
  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);

  OS_ARGUMENT_USED(pstCopyJob);

  *((uint32_t*) pDstData) = *((uint32_t*) pSrcData);
}

//-- Function: copyEng_CopyArcSpec -------------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function copies the specified number of bytes from the source memory to the destination memory by
///   using the architecture specific maximum copy data length.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pstCopyJob        pointer to the copy job information
//----------------------------------------------------------------------------------------------------------------------
static void copyEng_CopyArcSpec (void* pDstData,
                                 void* const pSrcData,
                                 copyJob_t* const pstCopyJob)
{
  OS_ASSERT(NULL != pDstData);
  OS_ASSERT(NULL != pSrcData);
  OS_ASSERT(NULL != pstCopyJob);

  (void) memcpy(pDstData, pSrcData, pstCopyJob->uiNoOfElements);
}

//---- End of source file ----------------------------------------------------------------------------------------------
