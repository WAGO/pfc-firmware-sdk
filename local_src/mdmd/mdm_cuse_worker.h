//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     mdm_cuse_worker.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef MDM_CUSE_WORKER_H_
#define MDM_CUSE_WORKER_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <glib.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define MDM_CUSE_FIFO_REQUEST       "/var/tmp/mdm_fifo_req"
#define MDM_CUSE_FIFO_EVENT_REPORT  "/var/tmp/mdm_fifo_evr"

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern gpointer mdm_cuse_worker_main(gpointer data);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* MDM_CUSE_WORKER_H_ */
//---- End of source file ------------------------------------------------------

