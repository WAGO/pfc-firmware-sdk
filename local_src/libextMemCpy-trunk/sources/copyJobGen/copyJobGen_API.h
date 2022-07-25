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
///  \file     copyJobGen_API.h
///
///  \defgroup page_copyJobGen_api copyJobGen_API
///  \ingroup  page_extMemCpy_internal
///
///  \version  $Id:
///
///  \brief    This module is the application interface for the Copy-Job-Generator functionality. All available
///             functions can only be accessed by using this interface. This module generates an optimized copy
///             job list from a specified copy rule list.
///
///  \author   Wauer : WAGO GmbH & Co. KG
//----------------------------------------------------------------------------------------------------------------------
#ifndef COPYJOBGEN_API_H
#define COPYJOBGEN_API_H

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
// Global variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Global functions
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
///  \defgroup page_copyJobGen_api_functions Functions
///  \ingroup page_copyJobGen_api
///
///   These are the functions of this module.
///
//----------------------------------------------------------------------------------------------------------------------

//-- Function: copyJobGen_GenCopyJobList -------------------------------------------------------------------------------
///  \ingroup page_copyJobGen_api_functions
///
///  This function generates an optimized copy job list from the specified copy rule list.
///
///  \param pstObjData  pointer to the Expended-Memory-Copy object context data
///
///  \return  The routine returns \ref EXTMEMCPY_SUCCESS on success. Another value such like
///            \ref EXTMEMCPY_ERROR_INVALID_COPY_RULE_TYPE indicates failure.
//----------------------------------------------------------------------------------------------------------------------
status_t copyJobGen_GenCopyJobList (extMemCpyData_t* pstObjData);

#endif
//---- End of header ---------------------------------------------------------------------------------------------------
