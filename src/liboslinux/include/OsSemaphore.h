#ifndef D_OsSemaphore_H
#define D_OsSemaphore_H

//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
/// SPDX-License-Identifier: MPL-2.0
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \author WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <inttypes.h>
#include <sys/cdefs.h>

#include "OsTime.h"
#include "OsCommon.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

enum OsSemaphoreError
{
   OS_SEM_S_OK,
   OS_SEM_E_INVALID_PARAMETER,
   OS_SEM_E_TIMED_OUT,
   OS_SEM_E_COUNTER_OVERFLOW,
   OS_SEM_E_INTERRUPTED,
   OS_SEM_E_TRY_AGAIN,
};

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct OsSemaphore tOsSemaphore;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


tOsSemaphore* OsSemaphore_Create(void);

tOsSemaphore* OsSemaphore_CreateEx(i32 value, const char* name);

i32 OsSemaphore_Init(tOsSemaphore *sem,
                         i32 value) __nonnull((1));

i32 OsSemaphore_Wait(tOsSemaphore *sem)__nonnull((1));

i32 OsSemaphore_TimedWaitEx(tOsSemaphore *sem,
                                const tOsTime *absoluteTimeout)__nonnull((1,2));

i32 OsSemaphore_TimedWait(tOsSemaphore *sem,
                              u32 timeout_ms)__nonnull((1));

i32 OsSemaphore_Post(tOsSemaphore *sem)__nonnull((1));

i32 OsSemaphore_GetValue(tOsSemaphore *sem,
                             i32 *pValue)__nonnull((1));

i32 OsSemaphore_Deinit(tOsSemaphore *sem)__nonnull((1));

i32 OsSemaphore_Destroy(tOsSemaphore *sem)__nonnull((1));


#endif  // D_OsSemaphore_H
