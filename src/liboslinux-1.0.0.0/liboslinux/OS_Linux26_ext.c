/*------------------------------------------------------------------------------*/
/*  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG */
/*  */
/*  PROPRIETARY RIGHTS are involved in the subject matter of this material. All */
/*  manufacturing, reproduction, use, and sales rights pertaining to this */
/*  subject matter are governed by the license agreement. The recipient of this */
/*  software implicitly accepts the terms of the license. */
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*  \file OS_Linux26_ext.c */
/*  */
/*  \version <Revision>: */
/*  */
/*  \brief short description of the file contents */
/*  */
/*  \author WAGODOM\u014042 : WAGO Kontakttechnik GmbH & Co. KG */
/*------------------------------------------------------------------------------*/

#include "OS_Linux26_ext.h"
#include "OS_Dependent.h"
//#include "cifXUser.h"
//#include "cifXToolkit.h"
#include "cifx/cifXErrors.h"
//#include "common.h"
#include "common/UserTrace.h"
#include "common/common.h"

#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

/**
 * TODO: Clean this file up!!!
 **/

static i32 OS_ErrnoToCifxError(i32 _errno)
{
	switch (_errno)
	{
		case E_NVAL:
			return CIFX__NVALID_PARAMETER;
		case ENOSYS:
			return CIFX_FUNCTION_NOT_AVAILABLE;
		case ETIMEDOUT:
			return CIFX_EVENT_TIMEOUT;
		default:
			return _errno;
	}
}

static i32 OS_Conditional_ErrnoToCifxError(bool condition)
{
	if (condition)
		return OS_ErrnoToCifxError(errno);
	return CIFX_NO_ERROR;
}

i32 OS_LockInit(tOS_Lock *lock)
{
   assert(lock != NULL);
   UserTraceDebug(TraceFacility_Osal, "(%s=%p)",_NV(lock));
	return pthread_mutex_init(lock, NULL);
}

tOS_Semaphore* OS_SemaphoreCreate()
{
	sem_t * sem;
	sem = malloc(sizeof(*sem));
	return sem;
}

i32 OS_SemaphoreInit(tOS_Semaphore *sem,
								 i32 value)
{
   assert(sem != NULL);
	UserTraceDebug(TraceFacility_Osal,"(%s=%p, %s=%d)",_NV(sem) ,_NV(value));
	return OS_Conditional_ErrnoToCifxError(sem_init(sem, 0, (u32)value));
}

i32 OS_SemaphoreWait(tOS_Semaphore *sem)
{
   assert(sem!= NULL);
   UserTraceDebug(TraceFacility_Osal,"( %s=%p)",_NV(sem));
	i32 state;
	do
	{
		state = sem_wait(sem);
	}
	while (state == -1 && errno == EINTR);

	return OS_Conditional_ErrnoToCifxError(state);
}

i32 OS_SemaphoreTimedWaitEx(tOS_Semaphore *sem,
										  const tOS_Time *absoluteTimeout)
{
   assert(sem!= NULL);
   UserTraceDebug(TraceFacility_Osal,"( %s=%p, %s=%p)",_NV(sem),_NV(absoluteTimeout));
	i32 s;
	while ((s = sem_timedwait(sem, absoluteTimeout)) == -1 && errno == EINTR)
		continue; /* Restart if interrupted by handler */

	return OS_Conditional_ErrnoToCifxError(s);
}

i32 OS_SemaphoreTimedWait(tOS_Semaphore *sem,
										u32 timeout_ms)
{
   assert(sem!= NULL);
   UserTraceDebug(TraceFacility_Osal,"( %s=%p, %s=%ul)",_NV(sem),_NV(timeout_ms));
	struct timespec ts;
	i32 s;


	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		return CIFX_EVENT_TIMEOUT;
	}
	ts.tv_nsec += ((timeout_ms % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_nsec -= 1000000000;
		ts.tv_sec += 1;
	}
	ts.tv_sec += timeout_ms / 1000;

	while ((s = sem_timedwait(sem, &ts)) == -1 && errno == EINTR)
		continue; /* Restart if interrupted by handler */

	return OS_Conditional_ErrnoToCifxError(s);
}

i32 OS_SemaphorePost(tOS_Semaphore *sem)
{
   assert(sem!= NULL);
   UserTraceDebug(TraceFacility_Osal,"( %s=%p)",_NV(sem));
	return sem_post(sem);
}

i32 OS_SemaphoreGetValue(tOS_Semaphore *sem,
									  i32 *pValue)
{
   assert(sem!= NULL);
   UserTraceDebug(TraceFacility_Osal,"( %s=%p)",_NV(sem));
	return sem_getvalue(sem, pValue);
}

i32 OS_SemaphoreClose(tOS_Semaphore *sem)
{
   assert(sem!= NULL);
   UserTraceDebug(TraceFacility_Osal,"( %s=%p)",_NV(sem));
	return sem_destroy(sem);
}

i32 OS_SemaphoreDelete(tOS_Semaphore *sem)
{
	assert(sem!= NULL);
   UserTraceDebug(TraceFacility_Osal,"( %s=%p)",_NV(sem));

	i32 result;
	result = sem_destroy(sem);
	free(sem);
	return result;
}

void OS_Sleep_us(u64 sleepTime_us)
{
	OS_Sleep_ns(sleepTime_us * 1000);
}

void OS_Sleep_ns(u64 sleepTime_ns)
{
	struct timespec intervall =
	{ 0 , 0};
	struct timespec remaining =
	{ 0, 0 };
	struct timespec *pIntervall = &intervall;
	struct timespec *pRemaining = &remaining;
	i32 res = 0;
	i64 scalar_remain = 0;

	intervall.tv_sec += NSEC_TO_SEC(sleepTime_ns);
	intervall.tv_nsec += ((sleepTime_ns) % (1000 * 1000 * 1000));

	do
	{
		res = nanosleep(pIntervall, pRemaining);
		scalar_remain = pRemaining->tv_nsec | pRemaining->tv_sec;
		SWAP(pRemaining, pIntervall);
	}
	while (res == -1 && errno == EINTR && scalar_remain != 0);
}

static i32 OS_ThreadAttributes(pthread_attr_t* attribute,
											  i32 priority,
											  i32 schedulerPolicy)
{
   assert(attribute != NULL);
	i32 result = 0;
	struct sched_param schedPriority;
	schedPriority.__sched_priority = priority;

	do
	{


		result = pthread_attr_init(attribute);
		if ( result )
		{
			UserTraceError("%s failed with errno = %d", VAR_NAME(pthread_attr_setschedpolicy), errno);
			break;
		}

		result = pthread_attr_setschedpolicy(attribute, schedulerPolicy);
		if ( result )
		{
			UserTraceError("%s failed with errno = %d", VAR_NAME(pthread_attr_setschedpolicy), errno);
			break;
		}

		result = pthread_attr_setschedparam(attribute, &schedPriority);
		if ( result )
		{
			UserTraceError("%s failed with errno = %d", VAR_NAME(pthread_attr_setschedparam));
			break;
		}
	}
	while(0);

	return result;
}

void OS_ThreadExit(void* result)
{
	pthread_exit(result);
}

i32 OS_ThreadCreateRealtime(tOS_Thread *thread,
                        __start_routine routine,
                        void* routineArgs,
                        i32 priority)
{
   assert(thread != NULL);
   UserTraceDebug(TraceFacility_Osal," (%s = %p, %s = %p, %s = %p, %s = %d, %s = %d)",
        _NV(thread),_NV(routine),
        _NV(routineArgs),_NV(priority));

   i32 result = 0;
   pthread_attr_t attributes;

   do
   {
      const i32 schedulerPolicy =  SCHED_FIFO;

      result =  OS_ThreadAttributes(&attributes, priority, schedulerPolicy);

      if ( result )
      {
         UserTraceError("OS_ThreadAttributes failed with errno = %d", errno);
         break;
      }


      result = pthread_create(thread, &attributes, routine, routineArgs);
      if ( result )
      {
         UserTraceError("pthread_create failed with errno = %d", errno);
         break;
      }

   }
   while(0);

   pthread_attr_destroy(&attributes);

   return result;
}

i32 OS_ThreadCreate(tOS_Thread *thread,
								__start_routine routine,
								void* routineArgs,
								i32 priority,
								int isRealtimeThread)
{
	assert(thread != NULL);
   UserTraceDebug(TraceFacility_Osal," (%s = %p, %s = %p, %s = %p, %s = %d, %s = %d)",
		_NV(thread),_NV(routine),
		_NV(routineArgs),_NV(priority),
		_NV(isRealtimeThread));

	i32 result = 0;
	pthread_attr_t attributes;

	do
	{
		const i32 schedulerPolicy = isRealtimeThread
													? SCHED_RR
													: SCHED_OTHER;

		result =  OS_ThreadAttributes(&attributes, priority, schedulerPolicy);

		if ( result )
		{
			UserTraceError("OS_ThreadAttributes failed with errno = %d", errno);
			break;
		}


		result = pthread_create(thread, &attributes, routine, routineArgs);
		if ( result )
		{
			UserTraceError("pthread_create failed with errno = %d", errno);
			break;
		}

	}
	while(0);

	pthread_attr_destroy(&attributes);

	return result;
}

/* copied from dal osal */
//void Os_Thread_Init(tOs_Thread *thread, tOs_ThreadRoutine routine, tOs_ThreadArgs threadArgs)
//{
//	assert(thread != NULL);
//	assert(routine != NULL);
//
//	pthread_attr_t attr;
//	pthread_attr_init(&attr);
//	pthread_create(thread, &attr, routine, threadArgs);
//	pthread_attr_destroy(&attr);
//}
//
//tOs_Thread *Os_Thread_Create(tOs_ThreadRoutine routine, tOs_ThreadArgs threadArgs)
//{
//	assert(routine != NULL);
//	tOs_Thread *thread = malloc( sizeof(*thread));
//	if ( thread != NULL)
//		Os_Thread_Init(thread, routine, threadArgs);
//	return thread;
//}
//
//
//void Os_Thread_Destroy(tOs_Thread *thread, tOs_ThreadResult *result)
//{
//	assert(thread != NULL);
//	pthread_cancel(*thread);
//	pthread_join(*thread, result);
//	free(thread);
//}

i32 OS_ThreadCancelEnable(void)
{
   return pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

i32 OS_ThreadCancelDisable(void)
{
   return pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
}


i32 OS_ThreadDestroy(tOS_Thread thread,
								 void** threadResult)
{
	pthread_cancel(thread);
	return pthread_join(thread, threadResult);
}

i32 OS_PiMutexInit(tOS_Mutex *mutex)
{
	assert(mutex != NULL);
	UserTraceDebug(TraceFacility_Osal," (%s = %p)",_NV(mutex));
	i32 result;
	pthread_mutexattr_t mutexAttribs =
	{ {0} };

	do
	{

		result = pthread_mutexattr_init(&mutexAttribs);
		if (result)
		{
			UserTraceError("pthread_mutexattr_init failed with errno = 0x%08x", errno);
			break;
		}

		result = pthread_mutexattr_setprotocol(&mutexAttribs, PTHREAD_PRIO_INHERIT);
		if (result)
		{
			UserTraceError("pthread_mutexattr_setprotocol failed with errno = 0x%08x", errno);
			break;
		}

		result = pthread_mutex_init(mutex, &mutexAttribs);
		if (result)
		{
			UserTraceError("pthread_mutex_init failed with errno = 0x%08x", errno);
			break;
		}

		result = pthread_mutexattr_destroy(&mutexAttribs);
		if (result)
		{
			UserTraceError("pthread_mutexattr_destroy failed with errno = 0x%08x", errno);
			break;
		}
	}
	while (0);

	return result;
}

int OS_Open(const char *file, tOpenFlags oflag, ...)
{
	int fd;
	va_list ap;
	va_start(ap, oflag);
	fd = open(file, oflag, ap);
	va_end(ap);
	return fd;
}


int OS_Close(int fd)
{
	UNUSED(fd);
	return 0;
}

void *OS_Mmap (void *addr, size_t len, tMemoryProtection prot, int flags, int fd, __off_t offset)
{
	return mmap( addr, len, prot, flags, fd, offset);
}

int  OS_Munmap (void *addr, size_t len)
{

	return munmap(addr, len);
}


int OS_GetPageSize(void)
{
	return getpagesize();
}


int OS_Read(int fd, void* buffer, size_t size)
{
	return read(fd, buffer, size);
}

void Os_RwLock_Init(tOs_RwLock *lock)
{
	assert(lock != NULL);
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	pthread_rwlock_init( lock, &attr);
	pthread_rwlockattr_destroy(&attr);
}


int  Os_RwLock_WriteLock(tOs_RwLock *lock)
{
	assert(lock != NULL);
	return pthread_rwlock_wrlock(lock);
}

int Os_RwLock_ReadLock(tOs_RwLock *lock)
{
	assert(lock != NULL);
	return pthread_rwlock_rdlock(lock);
}

int Os_RwLock_Unlock(tOs_RwLock *lock)
{
	assert(lock != NULL);
	return pthread_rwlock_unlock(lock);
}

tOs_RwLock * Os_RwLock_Create(void)
{
	tOs_RwLock *lock = malloc(sizeof(*lock));
	Os_RwLock_Init(lock);
	return lock;
}

void Os_RwLock_Destroy(tOs_RwLock *lock)
{
	if ( lock != NULL)
	{
		pthread_rwlock_destroy(lock);
		free(lock);
	}
}

i32 OS_MutexInitRecursivePriorityInversion(tOS_Mutex *mutex)
{
   assert(mutex != NULL);
   UserTraceDebug(TraceFacility_Osal," (%s = %p)",_NV(mutex));
   i32 result;
   pthread_mutexattr_t mutexAttribs =
   { {0} };

   do
   {

      result = pthread_mutexattr_init(&mutexAttribs);
      if (result)
      {
         UserTraceError("pthread_mutexattr_init failed with errno = 0x%08x: %s", errno, strerror(errno));
         break;
      }

      result = pthread_mutexattr_setprotocol(&mutexAttribs, PTHREAD_PRIO_INHERIT);
      if (result)
      {
         UserTraceError("pthread_mutexattr_setprotocol failed with errno = 0x%08x: %s", errno, strerror(errno));
         break;
      }

      result = pthread_mutexattr_settype(&mutexAttribs, PTHREAD_MUTEX_RECURSIVE_NP);
      if (result)
      {
         UserTraceError("pthread_mutexattr_setprotocol failed with errno = 0x%08x: %s", errno, strerror(errno));
         break;
      }

      result = pthread_mutex_init(mutex, &mutexAttribs);
      if (result)
      {
         UserTraceError("pthread_mutex_init failed with errno = 0x%08x: %s", errno, strerror(errno));
         break;
      }

      result = pthread_mutexattr_destroy(&mutexAttribs);
      if (result)
      {
         UserTraceError("pthread_mutexattr_destroy failed with errno = 0x%08x: %s", errno, strerror(errno));
         break;
      }
   }
   while (0);

   return result;
}

tOS_Mutex *OS_MutexCreate(void)
{
	tOS_Mutex *mutex;
	mutex =  malloc(sizeof(tOS_Mutex));
	if ( OS_MutexInit(mutex))
	{
		OS_MutexDestroy(mutex);
		mutex = NULL;
	}
	return mutex;
}

i32	OS_MutexInit(tOS_Mutex *mutex)
{
   assert(mutex != NULL);
   UserTraceDebug(TraceFacility_Osal,"%s=%p",_NV(mutex));
	i32 result = -1;
	if ( mutex != NULL)
	{
		result = pthread_mutex_init(mutex, NULL);
		if ( result )
		   UserTraceError("pthread_mutex_init failed with %s", strerror(result));
	}
	return result;
}

i32 OS_MutexDeinit(tOS_Mutex *mutex)
{
   assert(mutex != NULL);
   UserTraceDebug(TraceFacility_Osal,"%s=%p",_NV(mutex));
	i32 result = -1;
	if( mutex  != NULL)
	{
		result = pthread_mutex_destroy(mutex);
      if (result)
         UserTraceError("pthread_mutex_destroy failed with %s", strerror(result));
	}
	return result;
}

i32 OS_MutexDestroy(tOS_Mutex *mutex)
{
   assert(mutex != NULL);
   UserTraceDebug(TraceFacility_Osal,"%s=%p",_NV(mutex));
	i32 result = -1;
	if ( mutex != NULL)
	{
		result = pthread_mutex_destroy(mutex);
      if (result)
         UserTraceError("pthread_mutex_destroy failed with %s", strerror(result));
		free(mutex);
	}
	return result;
}

i32 OS_MutexLock(tOS_Mutex *mutex)
{
   assert(mutex != NULL);
   UserTraceDebug(TraceFacility_Osal,"%s=%p",_NV(mutex));
	i32 result = -1;
	if ( mutex != NULL)
	{
		result = pthread_mutex_lock(mutex);
      if (result)
         UserTraceError("pthread_mutex_lock failed with %s", strerror(result));

	}
	return result;
}

i32 OS_MutexUnlock(tOS_Mutex *mutex)
{
   assert(mutex != NULL);
   UserTraceDebug(TraceFacility_Osal,"%s=%p",_NV(mutex));
	i32 result = -1;
	if( mutex != NULL)
	{
		result = pthread_mutex_unlock(mutex);
      if (result)
         UserTraceError("pthread_mutex_unlock failed with %s", strerror(result));

	}
	return result;
}

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000
#endif


static void Os_TimeNormalize(tOS_Time *time)
{
   while( time->tv_nsec >= NSEC_PER_SEC)
   {
      time->tv_nsec -= NSEC_PER_SEC;
      time->tv_sec ++;
   }

   while( time->tv_nsec < 0)
   {
      time->tv_nsec += NSEC_PER_SEC;
      time->tv_sec--;
   }
}

/**
 * Reads the current monotonic time.
 */
i32 OS_TimeGetTimeMonotonic(tOS_Time *time)
{
   assert(time!=NULL);
   return clock_gettime(CLOCK_MONOTONIC, time);
}

/**
 * Reads the current real time.
 */
i32 OS_TimeGetTime(tOS_Time *time)
{
	assert(time!=NULL);
	return clock_gettime(CLOCK_REALTIME, time);
}

/**
 * Gets the second fraction of the tOS_Time.
 */
i32 OS_TimeGetSeconds(tOS_Time *time)
{
	assert(time!=NULL);
	return time->tv_sec;
}

/**
 * Gets the nanosecond fraction of the tOS_Time
 */
i32 OS_TimeGetNanoSeconds(tOS_Time *time)
{
	assert(time!=NULL);
	return time->tv_nsec;
}

/**
 * Sets the second fraction of the tOS_Time.
 */
void OS_TimeSetSeconds( tOS_Time *time, i64 seconds)
{
	assert(time!=NULL);
	time->tv_sec = seconds;
}

/**
 * Sets the nano second fraction of the tOS_Time.
 */
void OS_TimeSetNanoSeconds( tOS_Time *time, i64 nanoSeconds)
{
	assert(time!=NULL);
	time->tv_nsec = nanoSeconds;
}

void OS_TimeAddSeconds( tOS_Time *time, i32 seconds)
{
   assert(time!= NULL);
   time->tv_sec += seconds;
   Os_TimeNormalize(time);
}

void OS_TimeAddNanoSeconds( tOS_Time *time, i32 nanoSeconds)
{
   assert(time!= NULL);
   time->tv_nsec += nanoSeconds;
   Os_TimeNormalize(time);
}



/**
 * Compares t1 and t2 by subtracting t2 from t1.
 * @param t1
 * @param t2
 * @return 0 on equal.
 *         <0 on t1 < t2
 *         >0 on t1 > t2
 */
i32 OS_TimeCompare(tOS_Time *t1, tOS_Time *t2)
{
   __time_t diffSec;
   diffSec = t1->tv_sec - t2->tv_sec;

   if ( diffSec != 0)
      return diffSec;

   i32 diffNsec;
   diffNsec = t1->tv_nsec - t2->tv_nsec;

   return diffNsec;
}

/**
 * \retruns true if time lies in the past.
 */
i64 OS_TimeIsElapsed( tOS_Time * time)
{
	assert(time);
	tOS_Time now;
	OS_TimeGetTime(&now);
	return now.tv_sec > time->tv_sec
			|| ((now.tv_sec == time->tv_sec) && (now.tv_nsec > time->tv_nsec));
}

/**
 * \retruns true if time lies in the past.
 */
i64 OS_TimeIsElapsedMonotonic( tOS_Time * time)
{
   assert(time);
   tOS_Time now;
   OS_TimeGetTimeMonotonic(&now);
   return now.tv_sec > time->tv_sec
         || ((now.tv_sec == time->tv_sec) && (now.tv_nsec > time->tv_nsec));
}
