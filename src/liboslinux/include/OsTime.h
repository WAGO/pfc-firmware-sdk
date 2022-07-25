#ifndef D_OsTime_H
#define D_OsTime_H

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

#include <sys/cdefs.h>
#include <stdbool.h>
#include <stdlib.h>

#include "OsTypedefs.h"
#include "OsCommon.h"



//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define OS_TIME_NS_PER_US (1000U)
#define OS_TIME_US_PER_MS (1000U)

#define OS_TIME_MS_PER_SEC (1000U)

#define OS_TIME_NS_PER_SEC (OS_TIME_MS_PER_SEC * \
                            OS_TIME_US_PER_MS * \
                            OS_TIME_NS_PER_US)

#define OS_TIME_US_PER_SEC (OS_TIME_MS_PER_SEC * \
                            OS_TIME_US_PER_MS)

#define OS_TIME_MS_TO_US(x) ((x)*OS_TIME_US_PER_MS)

#define OS_TIME_MS_TO_NS(x) (OS_TIME_MS_TO_US(x) * OS_TIME_NS_PER_US)

#define OS_TIME_MS_TO_SEC(x) ((x)/OS_TIME_MS_PER_SEC)

#define OS_TIME_NORM_MS_TO_NS(x) ({u32 a = OS_TIME_MS_TO_NS(x);\
                                   a < OS_TIME_NS_PER_SEC ?\
                                         a : a % OS_TIME_NS_PER_SEC;})

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

#define OsTime_CreateStack() OsTime_Init((tOsTime*)alloca(kOsTimeStructSize))

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct stOsTime tOsTime;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

extern const size_t kOsTimeStructSize;

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------
tOsTime* OsTime_Create(void);

void OsTime_Destroy(tOsTime* self);


tOsTime* OsTime_Init(tOsTime *self);



/**
 * Reads the current monotonic time.
 */
i32 OsTime_GetTimeMonotonic(tOsTime *self) OS_ARG_NONNULL();

/**
 * Reads the current real time.
 */
i32 OsTime_GetTime(tOsTime *self) OS_ARG_NONNULL();

/**
 * Gets the second fraction of the tOsTime.
 */
i32 OsTime_GetSeconds(tOsTime const *self) OS_ARG_NONNULL();

/**
 * Gets the nanosecond fraction of the tOsTime
 */
i32 OsTime_GetNanoSeconds(tOsTime const *self) OS_ARG_NONNULL();

/**
 * Sets the second fraction of the tOsTime.
 */
void OsTime_SetSeconds( tOsTime *self, i32 seconds) OS_ARG_NONNULL();

/**
 * Sets the nano second fraction of the tOsTime.
 */
void OsTime_SetNanoSeconds( tOsTime *self, i32 nanoSeconds) OS_ARG_NONNULL();

/**
 * Adds seconds seconds to the supplied time.
 * @param time The to add the seconds to.
 * @param seconds The seconds to add.
 */
void OsTime_AddSeconds( tOsTime *self, i32 seconds) OS_ARG_NONNULL();

void OsTime_AddNanoSeconds( tOsTime *self, i32 nanoSeconds) OS_ARG_NONNULL();

void OsTime_SubSeconds( tOsTime *time, i32 seconds) OS_ARG_NONNULL();

void OsTime_SubNanoSeconds( tOsTime *time, i32 nanoSeconds) OS_ARG_NONNULL();

void OsTime_Subtract(tOsTime const *minued, tOsTime const *subtrahend, tOsTime *result) OS_ARG_NONNULL();

void OsTime_Add(tOsTime const *add1, tOsTime const * add2, tOsTime *result) OS_ARG_NONNULL();

void OsTime_Clear(tOsTime *time) OS_ARG_NONNULL();

void OsTime_Set(tOsTime const *in, tOsTime *out) OS_ARG_NONNULL();

/**
 * Compares t1 and t2 by subtracting t2 from t1.
 * @param t1
 * @param t2
 * @return 0 on equal.
 *         <0 on t1 < t2
 *         >0 on t1 > t2
 */
i32 OsTime_Compare(tOsTime const *t1, tOsTime const *t2) OS_ARG_NONNULL();

/**
 * \retruns true if time lies in the past.
 */
bool OsTime_IsElapsed( tOsTime const * time) OS_ARG_NONNULL();

void OsTime_SleepRelative(tOsTime const *time) OS_ARG_NONNULL();

void OsTime_SleepAbsolute(tOsTime const *time) OS_ARG_NONNULL();

void const * OsTime_GetOsHandle(tOsTime const *time) OS_ARG_NONNULL();

void OsTime_ConvertToMonotonic(tOsTime const *realtime, tOsTime *monotonic) OS_ARG_NONNULL();
void OsTime_ConvertToRealtime(tOsTime const *monotonic, tOsTime *realtime) OS_ARG_NONNULL();


#endif  // D_OsTime_H
