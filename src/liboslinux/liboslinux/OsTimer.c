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

#include "OsTimer.h"

#include <errno.h>
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include <signal.h>

#define __need_timespec
#include <time.h>

#include "OsTime.h"
#include "OsTrace.h"
//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef struct stOsTimer
{
	fnTimerCallback Callback;
	void * UserData;
	timer_t Timer;
	volatile bool	IsStarted;
	bool 			IsInterval;
} _stOsTimer;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

static void Callback(sigval_t sigval)
{
	assert(sigval.sival_ptr != NULL);
	tOsTimer * timer = sigval.sival_ptr;
	timer->Callback(timer, timer->UserData );

	if (! timer->IsInterval)
	{
		timer->IsStarted = false;
	}
}

i32 OsTimer_StartInterval(tOsTimer * self, tOsTime *first, tOsTime* interval)
{
	assert(self != NULL);
	assert(interval != NULL);

	struct itimerspec timerspec;

	self->IsInterval = true;
	self->IsStarted = true;

	timerspec.it_interval = *(struct timespec*) OsTime_GetOsHandle(interval);

	i32 res = 0;
	if (first != NULL)
	{
		tOsTime *startTime = OsTime_CreateStack();
		OsTime_ConvertToMonotonic(first, startTime);

		timerspec.it_value = *(struct timespec*) OsTime_GetOsHandle(startTime);

		res = timer_settime(self->Timer, TIMER_ABSTIME,
				&timerspec,
				NULL);

	}
	else
	{
		res = timer_settime(self->Timer, 0,
				&timerspec, NULL);
	}

	return res;
}

i32 OsTimer_Start(tOsTimer *self, tOsTime *value)
{
	assert(self != NULL);
	assert(value != NULL);

	struct itimerspec timerspec;

	self->IsInterval = false;
	self->IsStarted = true;

	timerspec.it_interval.tv_nsec = 0;
	timerspec.it_interval.tv_sec = 0;

	tOsTime *time = OsTime_CreateStack();

	OsTime_ConvertToMonotonic(value, time);

	timerspec.it_value = *(struct timespec*) OsTime_GetOsHandle(value);

	return timer_settime(self->Timer, TIMER_ABSTIME, &timerspec, NULL);
}


i32 OsTimer_Stop(tOsTimer *self)
{
   assert(self !=  NULL);
   struct itimerspec timerspec;

   memset(&timerspec, 0, sizeof(timerspec));
   return timer_settime(self->Timer, TIMER_ABSTIME, &timerspec, NULL);
}


tOsTimer* OsTimer_Create(fnTimerCallback callback,
                         void const * userData)
{
   assert(callback != NULL);
   tOsTimer *self = calloc(1, sizeof(*self));

   sigevent_t sigevt;
   sigevt.sigev_notify = SIGEV_THREAD;
   sigevt.sigev_notify_function = Callback;
   sigevt.sigev_value.sival_ptr = self;

   self->Callback = callback;

   /* we don't alter the userData, but the user is
    * allowed to alter it in the callback, thus
    * it is ok to cast away the const qualifier.
    */
   self->UserData = (void*) userData;

   if (timer_create(CLOCK_MONOTONIC, &sigevt, &self->Timer))
   {
      OsTraceError("timer_create failed with errno = %d: %s", errno, strerror(errno));
      OsTimer_Destroy(self);
      self = NULL;
   }

   return self;
}


void OsTimer_Destroy(tOsTimer * self)
{
   if ( self != NULL)
   {
      OsTimer_Stop(self);
      timer_delete(self->Timer);
      free(self);
   }
}
