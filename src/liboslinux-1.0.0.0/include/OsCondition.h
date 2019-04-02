#ifndef D_OsCondition_H
#define D_OsCondition_H

//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// \file ${file_name}
//
// \version <Revision>: $Rev$
//
// \brief short description of the file contents
//
// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <inttypes.h>

#include <OsCommon.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct _OsCondition tOsCondition;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

/**
 *
 * @return
 */
tOsCondition *OsCondition_Create(void);

/**
 *
 * @param cond
 */
void OsCondition_Destroy(tOsCondition *self);

/**
 *
 * @param self
 * @return
 */
i32 OsCondition_Lock(tOsCondition *self);

/**
 *
 * @param self
 * @return
 */
i32 OsCondition_Unlock(tOsCondition *self);

/**
 *
 * @param self
 * @return
 */
i32 OsCondition_Broadcast(tOsCondition *self);

/**
 *
 * @param self
 * @return
 */
i32 OsCondition_Signal(tOsCondition *self);

/**
 *
 * @param self
 * @return
 */
i32 OsCondition_Wait(tOsCondition *self);

#endif  // D_OsCondition_H
