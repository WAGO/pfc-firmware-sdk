#ifndef D_OsCondition_H
#define D_OsCondition_H

//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
/// SPDX-License-Identifier: MPL-2.0
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// \author WAGO GmbH & Co. KG
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
