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

#include "OsSemaphore.h"

#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>
#include <semaphore.h>

#include <pthread.h>

#include "OsRessource.h"
#include "OsTrace.h"
#include "OsTypedefs.h"



//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

#define HANDLE_ERROR(x) return  ( x != OS_S_OK ) ? OsSemConvertErrno(errno) : OS_S_OK;

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct OsSemaphore
{
   sem_t Semaphore;
   tOsRessource Ressource;
   u32 Id;
}_OsSemaphore;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

static u32 s_SemaphoreIds;

static i32 OsSemConvertErrno(i32 error)
{
   switch (error)
   {
      case EINVAL:
         return OS_SEM_E_INVALID_PARAMETER;
      case EAGAIN:
         return OS_SEM_E_TRY_AGAIN;
      case ETIMEDOUT:
         return OS_SEM_E_TIMED_OUT;
      case EOVERFLOW:
         return OS_SEM_E_COUNTER_OVERFLOW;
      case EINTR:
         return OS_SEM_E_INTERRUPTED;
      default:
         OsTraceError("Error not handled in switch statement: %" PRId32, error);
         return OS_E_UNKNOWN;
   }
}


tOsSemaphore* OsSemaphore_Create(void)
{
   return OsSemaphore_CreateEx(0, NULL);
}


tOsSemaphore* OsSemaphore_CreateEx(i32 value, const char* name)
{

   tOsSemaphore * sem;
   sem = malloc(sizeof(*sem));

   if ( sem != NULL)
   {
      sem->Id = __sync_add_and_fetch(&s_SemaphoreIds, 1);
      OsSemaphore_Init(sem, value);

      if ( name == NULL) {
         OS_RESSOURCE_SET_NAME(sem->Ressource, "Semaphore %" PRIu32, sem->Id );
      } else {
         OsRessource_SetName(&sem->Ressource, name);
      }
   }

   return sem;

}

i32 OsSemaphore_Init(tOsSemaphore *sem,
                         i32 value)
{
   assert(sem != NULL);
   OsTraceDebug(TraceFacility_OsalSemaphore,"(%s=%p, %s=%d)",_NV(sem) ,_NV(value));

   HANDLE_ERROR( sem_init(&sem->Semaphore, 0, (u32)value));
}

i32 OsSemaphore_Wait(tOsSemaphore *sem)
{
   assert(sem!= NULL);
   OsTraceDebug(TraceFacility_OsalSemaphore,"( %s=%p)",_NV(sem));
   i32 state;
   do
   {
      state = sem_wait(&sem->Semaphore);
   }
   while (state == -1 && errno == EINTR);

   HANDLE_ERROR(state);
}

i32 OsSemaphore_TimedWaitEx(tOsSemaphore *sem,
                                const tOsTime *absoluteTimeout)
{
   assert(sem!= NULL);
   OsTraceDebug(TraceFacility_OsalSemaphore,"( %s=%p, %s=%p)",_NV(sem),_NV(absoluteTimeout));

   i32 result;
   while ((result = sem_timedwait(&sem->Semaphore, OsTime_GetOsHandle(absoluteTimeout))) == -1 && errno == EINTR)
      continue; /* Restart if interrupted by handler */

   HANDLE_ERROR(result);
}

i32 OsSemaphore_TimedWait(tOsSemaphore *sem,
                              u32 timeout_ms)
{
   assert(sem!= NULL);
   OsTraceDebug(TraceFacility_OsalSemaphore,"( %s=%p, %s=%ul)",_NV(sem),_NV(timeout_ms));

   struct timespec ts;
   i32 res;

   if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
   {
      return OS_SEM_E_TIMED_OUT;
   }
   ts.tv_nsec += ((timeout_ms % 1000) * 1000000);
   if (ts.tv_nsec >= 1000000000)
   {
      ts.tv_nsec -= 1000000000;
      ts.tv_sec += 1;
   }
   ts.tv_sec += timeout_ms / 1000;

   while ((res = sem_timedwait(&sem->Semaphore, &ts)) == -1 && errno == EINTR)
      continue; /* Restart if interrupted by handler */

   HANDLE_ERROR(res);
}

i32 OsSemaphore_Post(tOsSemaphore *sem)
{
   assert(sem!= NULL);
   OsTraceDebug(TraceFacility_OsalSemaphore,"( %s=%p)",_NV(sem));

   HANDLE_ERROR( sem_post(&sem->Semaphore));
}

i32 OsSemaphore_GetValue(tOsSemaphore *sem,
                             i32 *pValue)
{
   assert(sem!= NULL);
   OsTraceDebug(TraceFacility_OsalSemaphore,"( %s=%p)",_NV(sem));

   HANDLE_ERROR( sem_getvalue(&sem->Semaphore, pValue));
}

i32 OsSemaphore_Deinit(tOsSemaphore *sem)
{
   assert(sem!= NULL);
   OsTraceDebug(TraceFacility_OsalSemaphore,"( %s=%p)",_NV(sem));

   HANDLE_ERROR(sem_destroy(&sem->Semaphore));
}

i32 OsSemaphore_Destroy(tOsSemaphore *sem)
{
   assert(sem!= NULL);
   OsTraceDebug(TraceFacility_OsalSemaphore,"( %s=%p)",_NV(sem));

   i32 result;
   result = sem_destroy(&sem->Semaphore);
   free(sem);
   HANDLE_ERROR( result);
}
