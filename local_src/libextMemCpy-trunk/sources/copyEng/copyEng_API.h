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
///  \file     copyEng_API.h
///
///  \defgroup page_copyEng_api copyEng_API
///  \ingroup  page_extMemCpy_internal
///
///  \version  $Id:
///
///  \brief    This module is the application interface for the Copy-Engine functionality. All available
///             functions can only be accessed by using this interface. The copy engine copies data where the data
///             locations are specified by a copy job list.
///
///  \author   Wauer : WAGO GmbH & Co. KG
//----------------------------------------------------------------------------------------------------------------------
#ifndef COPYENG_API_H
#define COPYENG_API_H

//----------------------------------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------------------------------
#include <stdint.h>

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
///  \defgroup page_copyEng_api_data_types Data Types
///  \ingroup page_copyEng_api
///
///  These are the data types of this module.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_copyEng_api_data_types
///
/// The \ref copyJob_t structure describes the properties of one copy job.
//----------------------------------------------------------------------------------------------------------------------
typedef struct copyJob_tag copyJob_t;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_copyEng_api_data_types
///
/// The \ref copyJobPrepFunc_t function pointer is used to specify a preparing function of a copy job.
//----------------------------------------------------------------------------------------------------------------------
typedef void (*copyJobPrepFunc_t) (void* const pDstData, void* const pSrcData, copyJob_t* pstCopyJob);

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_copyEng_api_data_types
///
/// The \ref copyJobFunc_t function pointer is used to specify a data copy function of a copy job.
//----------------------------------------------------------------------------------------------------------------------
typedef void (*copyJobFunc_t) (void* pDstData, void* const pSrcData, copyJob_t* const pstCopyJob);

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_copyEng_api_data_types
///
/// The \ref copyJob_tag structure describes the properties of one copy job.
//----------------------------------------------------------------------------------------------------------------------
struct copyJob_tag
{
  /** copy job preparation function */
  copyJobPrepFunc_t pfCopyJobPrepFunc
  /** copy job copy function */;
  copyJobFunc_t pfCopyJobFunc;
  /** source data memory pointer */
  void* pSrcData;
  /** destination data memory pointer */
  void* pDstData;
  /** source data byte offset */
  uint_t uiSrcDataByteOffset;
  /** source data bit offset */
  uint_t uiSrcDataBitPos;
  /** destination data byte offset */
  uint_t uiDstDataByteOffset;
  /** destination data bit offset */
  uint_t uiDstDataBitPos;
  /** number of elements to be copied */
  uint_t uiNoOfElements;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup page_copyEng_api_data_types
///
/// The \ref copyEngInterfaceDesc_t structure describes the usable functions of the copy engine API.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /** preparation function for offset to offset copy jobs */
  copyJobPrepFunc_t pfPrepareCopyJobOffsetToOffset;
  /** preparation function for offset to absolute copy jobs */
  copyJobPrepFunc_t pfPrepareCopyJobOffsetToAbsolute;
  /** preparation function for absolute to offset copy jobs */
  copyJobPrepFunc_t pfPrepareCopyJobAbsoluteToOffset;
  /** preparation function for absolute to absolute copy jobs */
  copyJobPrepFunc_t pfPrepareCopyJobAbsoluteToAbsolute;
  /** bit copy function */
  copyJobFunc_t pfCopyBit;
  /** one bit copy function */
  copyJobFunc_t pfCopyOneBit;
  /** byte copy function */
  copyJobFunc_t pfCopyByte;
  /** one byte copy function */
  copyJobFunc_t pfCopyOneByte;
  /** word copy function */
  copyJobFunc_t pfCopyWord;
  /** one word copy function */
  copyJobFunc_t pfCopyOneWord;
  /** double word copy function */
  copyJobFunc_t pfCopyDWord;
  /** one double word copy function */
  copyJobFunc_t pfCopyOneDWord;
  /** architecture specific copy function */
  copyJobFunc_t pfCopyArcSpec;
}copyEngInterfaceDesc_t;

//----------------------------------------------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_copyEng_api_functions Functions
///  \ingroup page_copyEng_api
///
///   These are the functions of this module.
///
//----------------------------------------------------------------------------------------------------------------------

//-- Function: copyEng_PrcCopyJobList ----------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function processes the specified job list. The pointer to the source and destination data memories are used
///   for the with offset specified copy jobs.
///
///  \param pDstData          pointer to the destination data memory
///  \param pSrcData          pointer to the source data memory
///  \param pastCopyJobList   pointer to the copy job list
///  \param ulNoOfCopyJobs    number of copy jobs in the list
//----------------------------------------------------------------------------------------------------------------------
void copyEng_PrcCopyJobList (void* pDstData,
                             void* pSrcData,
                             copyJob_t* pastCopyJobList,
                             uint_t ulNoOfCopyJobs);

//-- Function: copyEng_GetInterfaceDesc --------------------------------------------------------------------------------
///  \ingroup page_copyEng_api_functions
///
///  This function returns the interface of the Copy-Engine module API.
///
///  \param pstCopyEngInterfaceDesc   pointer where the interface description should be stored.
//----------------------------------------------------------------------------------------------------------------------
void copyEng_GetInterfaceDesc (copyEngInterfaceDesc_t* pstCopyEngInterfaceDesc);

#endif
//---- End of header ---------------------------------------------------------------------------------------------------
