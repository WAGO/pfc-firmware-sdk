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

#include "OsTime.h"

#include <errno.h>
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#define __need_timespec
#include <time.h>

#include <OsMemory.h>
//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define __TIME(t) (t->Time)
#define __TIME_NSEC(t) (__TIME(t).tv_nsec)
#define __TIME_SEC(t) (__TIME(t).tv_sec)


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef struct stOsTime
{
	struct timespec Time;
	i32 ClockId;
}_stOsTime;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

const size_t kOsTimeStructSize = sizeof(_stOsTime);

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000
#endif


static void TimeNormalize(tOsTime *time)
{
   while( time->Time.tv_nsec >= NSEC_PER_SEC)
   {
      time->Time.tv_nsec -= NSEC_PER_SEC;
      time->Time.tv_sec ++;
   }

   while( time->Time.tv_nsec < 0)
   {
      time->Time.tv_nsec += NSEC_PER_SEC;
      time->Time.tv_sec--;
   }
}

void OsTime_SleepRelative(tOsTime const *time)
{
	i32 res;
	struct timespec remaining = time->Time;
	do
	{
		res = clock_nanosleep(time->ClockId, 0, &remaining, &remaining);
	}
	while( res != 0 && res == EINTR  );
}

void OsTime_SleepAbsolute(tOsTime const *time)
{
	i32 res;
	do
	{
		res = clock_nanosleep(time->ClockId, TIMER_ABSTIME, &time->Time, NULL);
	}
	while( res != 0 && res == EINTR  );
}

tOsTime* OsTime_Init(tOsTime* time)
{
   OsMemory_Set(time, 0, sizeof(*time));
   return time;
}

tOsTime* OsTime_Create(void)
{
	tOsTime* self = calloc(1, sizeof(*self));
	return self;
}

void OsTime_Destroy(tOsTime* self)
{
	free(self);
}

/**
 * Reads the current monotonic time.
 */
i32 OsTime_GetTimeMonotonic(tOsTime *time)
{
   assert(time!=NULL);
   time->ClockId = CLOCK_MONOTONIC;
   return clock_gettime(CLOCK_MONOTONIC, &time->Time);
}

/**
 * Reads the current real time.
 */
i32 OsTime_GetTime(tOsTime *time)
{
   assert(time!=NULL);
   time->ClockId = CLOCK_REALTIME;
   return clock_gettime(CLOCK_REALTIME, &time->Time);
}

/**
 * Gets the second fraction of the tOsTime.
 */
i32 OsTime_GetSeconds(tOsTime const *time)
{
   assert(time!=NULL);
   return time->Time.tv_sec;
}

/**
 * Gets the nanosecond fraction of the tOsTime
 */
i32 OsTime_GetNanoSeconds(tOsTime const *time)
{
   assert(time!=NULL);
   return time->Time.tv_nsec;
}

void OsTime_Set(tOsTime const *in, tOsTime *out)
{
	assert(in != NULL);
	assert(out != NULL);
	__TIME_NSEC(out) = __TIME_NSEC(in);
	__TIME_SEC(out) = __TIME_SEC(in);
	out->ClockId = in->ClockId;
}

/**
 * Sets the second fraction of the tOsTime.
 */
void OsTime_SetSeconds( tOsTime *time, i32 seconds)
{
   assert(time!=NULL);
   time->Time.tv_sec = seconds;
   TimeNormalize(time);

}

void OsTime_Clear(tOsTime *time)
{
	__TIME_SEC(time) = 0;
	__TIME_NSEC(time) = 0;
}

void OsTime_Subtract(tOsTime const *minued, tOsTime const *subtrahend, tOsTime *result)
{
	assert(minued != NULL);
	assert(subtrahend != NULL);
	assert(result != NULL);

	__TIME_NSEC(result) = __TIME_NSEC(minued) - __TIME_NSEC(subtrahend);
	__TIME_SEC(result) = __TIME_SEC(minued) - __TIME_SEC(subtrahend);
	TimeNormalize(result);
}

void OsTime_Add(tOsTime const *add1, tOsTime const *add2, tOsTime *result)
{
	assert(add1 != NULL);
	assert(add2 != NULL);
	assert(result != NULL);

	result->Time.tv_nsec = add1->Time.tv_nsec + add2->Time.tv_nsec;
	result->Time.tv_sec = add1->Time.tv_sec + add2->Time.tv_sec;
	TimeNormalize(result);
}

/**
 * Sets the nano second fraction of the tOsTime.
 */
void OsTime_SetNanoSeconds( tOsTime *time, i32 nanoSeconds)
{
   assert(time!=NULL);
   time->Time.tv_nsec = nanoSeconds;
   TimeNormalize(time);
}

void OsTime_AddSeconds( tOsTime *time, i32 seconds)
{
   assert(time!= NULL);
   time->Time.tv_sec += seconds;
   TimeNormalize(time);
}

void OsTime_AddNanoSeconds( tOsTime *time, i32 nanoSeconds)
{
   assert(time!= NULL);
   time->Time.tv_nsec += nanoSeconds;
   TimeNormalize(time);
}

void OsTime_SubSeconds( tOsTime *time, i32 seconds)
{
   assert(time!= NULL);
   time->Time.tv_sec -= seconds;
   TimeNormalize(time);
}

void OsTime_SubNanoSeconds( tOsTime *time, i32 nanoSeconds)
{
   assert(time!= NULL);
   time->Time.tv_nsec -= nanoSeconds;
   TimeNormalize(time);
}

void const *OsTime_GetOsHandle(tOsTime const *time)
{
   assert(time != NULL);
   return &time->Time;
}

/**
 * Compares t1 and t2 by subtracting t2 from t1.
 * @param t1
 * @param t2
 * @return 0 on equal.
 *         <0 on t1 < t2
 *         >0 on t1 > t2
 */
i32 OsTime_Compare(tOsTime const *t1, tOsTime const *t2)
{
   __time_t diffSec;
   diffSec = t1->Time.tv_sec - t2->Time.tv_sec;

   if ( diffSec != 0)
      return diffSec;

   i32 diffNsec;
   diffNsec = t1->Time.tv_nsec - t2->Time.tv_nsec;

   return diffNsec;
}

static __inline__ bool IsElapsedRealtime( tOsTime const *time)
{
	tOsTime now;
   OsTime_GetTime(&now);
   return now.Time.tv_sec > time->Time.tv_sec
		 || ((now.Time.tv_sec == time->Time.tv_sec) && (now.Time.tv_nsec > time->Time.tv_nsec));
}

static __inline__ bool IsElapsedMonotonic( tOsTime const *time)
{
	tOsTime now;
   OsTime_GetTimeMonotonic(&now);
   return now.Time.tv_sec > time->Time.tv_sec
		 || ((now.Time.tv_sec == time->Time.tv_sec) && (now.Time.tv_nsec > time->Time.tv_nsec));
}

/**
 * \retruns true if time lies in the past.
 */
bool OsTime_IsElapsed( tOsTime const * time)
{
   assert(time);
   switch (time->ClockId)
   {
   case CLOCK_MONOTONIC:
	   return IsElapsedMonotonic(time);
   case CLOCK_REALTIME:
	   return IsElapsedRealtime(time);
   default:
	   return true;
   }

}

void OsTime_ConvertToMonotonic(tOsTime const *realtime, tOsTime *monotonic)
{
	assert(realtime != NULL);
	assert(monotonic != NULL);

	if ( realtime->ClockId == CLOCK_REALTIME)
	{
		monotonic->ClockId = CLOCK_MONOTONIC;
		tOsTime *nowRealtime = alloca(sizeof(tOsTime));
		tOsTime *nowMonotinic = alloca(sizeof(tOsTime));
		tOsTime *diff = alloca(sizeof(tOsTime));

		OsTime_GetTime(nowRealtime);
		OsTime_GetTimeMonotonic(nowMonotinic);

		OsTime_Subtract(nowRealtime, nowMonotinic, diff);
		OsTime_Add(realtime, diff, monotonic);
	}
	else
	{
	   // Clock is already CLOCK_MONOTINIC
	   // just copy to output
		OsTime_Set(realtime, monotonic);
	}
}

void OsTime_ConvertToRealtime(tOsTime const *monotonic, tOsTime *realtime)
{
   tOsTime *nowRealtime = alloca(sizeof(tOsTime));
   tOsTime *nowMonotinic = alloca(sizeof(tOsTime));
   tOsTime *diff = alloca(sizeof(tOsTime));

	OsTime_GetTime(nowRealtime);
	OsTime_GetTimeMonotonic(nowMonotinic);

	OsTime_Subtract(nowRealtime, nowMonotinic, diff);
	OsTime_Add(monotonic, diff, realtime);
}
