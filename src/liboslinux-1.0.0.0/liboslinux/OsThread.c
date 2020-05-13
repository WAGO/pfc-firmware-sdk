//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
/// SPDX-License-Identifier: MPL-2.0
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \author WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "OsThread.h"

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <memory.h>
#include <pthread.h>
#include <bits/local_lim.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syscall.h>

#include "OsCommon.h"
#include "OsMemory.h"
#include "OsRessource.h"
#include "OsTrace.h"
#include "OsTypedefs.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct stOsThread
{
   tOsRessource Ressource;
   pthread_t Thread;
   i32 Priority;
   i32 Policy;
   ThreadRoutine Routine;
   void * Args;
   u32 Id;
   u32  Sid;
   size_t StackSize;
} _OsThread;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

static void OsThread_Exit(void* result) __attribute__ ((__noreturn__));

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


static u32 sThreadCount = 0;

static void OsThread_TouchAllStack(tOsThread *self)
{
   UNUSED(self);
//   // -64 to leave some room for the function calls!
//   // todo: get the exact stack usage.
//   u8 stack[self->StackSize-64-PTHREAD_STACK_MIN];
//
//   for( unsigned int i = 0; i < ELEMENT_COUNT(stack); i+= sysconf(_SC_PAGESIZE))
//   {
//      stack[i]= 0;
//   }
}

static void * DefaultThreadRoutine(void *thread)
{
   tOsThread * self = thread;
   OsThread_TouchAllStack(self);

   assert( self != NULL);
   assert( self->Routine != NULL);


   self->Sid = syscall(SYS_gettid);
   OsTraceInfo(TraceFacility_OsalThread, "Starting thread %s with pid=%d", self->Ressource.Name, self->Sid);

   void * threadResult = self->Routine(self->Args);

   OsThread_Exit(threadResult);
}

static void CreateDefaultName(tOsThread *thread)
{
   OS_RESSOURCE_SET_NAME(thread->Ressource, "Thread_%02" PRIx8 "_S%" PRIi32 "_P%" PRIi32, (u8)thread->Id, thread->Policy, thread->Priority );
}

static void SetName(tOsThread *thread,
                    const char* name)
{
   if (name == NULL )
   {
      CreateDefaultName(thread);
   }
   else
   {
      OsRessource_SetName(&thread->Ressource, name);
   }
}

static void OsThread_Exit(void* result)
{
   pthread_exit(result);
}


static i32 OsThread_Attributes(pthread_attr_t* attribute,
                                i32 priority,
                                i32 schedulerPolicy,
                                size_t stackSize)
{
   assert(attribute != NULL);
   i32 result = 0;
   struct sched_param schedPriority;
   schedPriority.__sched_priority = priority;

   do
   {
      result = pthread_attr_init(attribute);
      if (result)
      {
         OsTraceError("%s failed: %s",
               VAR_NAME(pthread_attr_init), strerror(result));
         break;
      }

      result = pthread_attr_setschedpolicy(attribute, schedulerPolicy);
      if (result)
      {
         OsTraceError("%s failed: %s",
               VAR_NAME(pthread_attr_setschedpolicy), strerror(result));
         break;
      }

      result = pthread_attr_setschedparam(attribute, &schedPriority);
      if (result)
      {
         OsTraceError("%s failed: %s",
               __STRING(pthread_attr_setschedparam), strerror(result));
         break;
      }

      result = pthread_attr_setinheritsched(attribute, PTHREAD_EXPLICIT_SCHED);
      if (result)
      {
         OsTraceError("%s failed: %s",__STRING(pthread_attr_setinheritsched()), strerror(result));
         break;
      }

      if (stackSize <= PTHREAD_STACK_MIN)
      {
         stackSize = 2*PTHREAD_STACK_MIN;
      }

      result = pthread_attr_setstacksize(attribute, stackSize);
      if ( result)
      {
         OsTraceError("%s failed: %d", VAR_NAME(pthread_attr_setstacksize), result);
      }
   } while (0);

   return result;
}

static tOsThread * OsThread_CreateHelper( ThreadRoutine routine,
                                    void* routineArgs,
                                    i32 priority,
                                    i32 schedulerPolicy,
                                    size_t  stackSize,
                                    const char* name)
{
   OsTraceDebug(TraceFacility_OsalThread,
         " (%s = %p, %s = %p, %s = %d, %s = %d)",
       _NV(routine),_NV(routineArgs),_NV(priority),_NV(schedulerPolicy),_NV(stackSize));

   i32 result = 0;
   pthread_attr_t attributes;
   tOsThread * thread;

   do
   {
      thread = calloc(1, sizeof(*thread));
      if ( thread == NULL)
      {
         result = OS_E_NO_MEMORY;
         break;
      }

      thread->Args = routineArgs;
      thread->Policy = schedulerPolicy;
      thread->Priority = priority;
      thread->Routine = routine;
      thread->Id = __sync_add_and_fetch( &sThreadCount, 1);

      SetName(thread, name);

      result = OsThread_Attributes(&attributes, priority, schedulerPolicy, stackSize + PTHREAD_STACK_MIN);

      if (result)
      {
         free(thread);
         thread = NULL;
         OsTraceError("OsThread_Attributes failed:", errno);
         break;
      }

      pthread_attr_getstacksize(&attributes, &thread->StackSize);

      result = pthread_create(&thread->Thread, &attributes, DefaultThreadRoutine, thread);
      if (result)
      {
         free(thread);
         thread = NULL;
         OsTraceError("pthread_create failed: %s", strerror(result));
         break;
      }

      char pthreadName[16];
      strncpy(pthreadName, thread->Ressource.Name, sizeof(pthreadName));
      pthreadName[sizeof(pthreadName) - 1]= '\0';

      result = pthread_setname_np(thread->Thread, pthreadName);
      if (result)
      {
         OsTraceError("pthread_setname_np failed  with %d", result);
      }

   } while (0);

   pthread_attr_destroy(&attributes);

   return thread;
}



i32 OsThread_Cancel(tOsThread *thread)
{
   assert(thread != NULL);
   OsTraceDebug(TraceFacility_OsalThread, "Cancelling thread %s", thread->Ressource.Name);

   return pthread_cancel(thread->Thread);
}

i32 OsThread_Join(tOsThread *thread, void** threadResult)
{
   assert(thread != NULL);
   OsTraceDebug(TraceFacility_OsalThread, "Joining thread %s..", thread->Ressource.Name);

   return pthread_join(thread->Thread, threadResult);
}

tOsThread * OsThread_CreateRealtime(ThreadRoutine routine,
                                void* routineArgs,
                                i32 priority)
{
   OsTraceDebug(TraceFacility_OsalThread,
         " (%s = %p, %s = %p, %s = %d)",
        _NV(routine),_NV(routineArgs),_NV(priority));

   return OsThread_CreateHelper( routine, routineArgs, priority, DEFAULT_RT_SCHEDULER, DEFAULT_STACK_SIZE, NULL);
}

tOsThread * OsThread_CreateRealtimeEx( ThreadRoutine routine,
                                void* routineArgs,
                                i32 priority,
                                size_t  stackSize,
                                const char* name)
{
   OsTraceDebug(TraceFacility_OsalThread,
         " (%s = %p, %s = %d, %s = %d, %s = %s)",
        _NV(routine),_NV(routineArgs),_NV(priority), _NV(name));

   return OsThread_CreateHelper( routine, routineArgs, priority, DEFAULT_RT_SCHEDULER, stackSize, name);
}

tOsThread * OsThread_Create(ThreadRoutine routine,
                        void* routineArgs,
                        i32 priority)
{
   return OsThread_CreateHelper(routine, routineArgs, priority,
         SCHED_OTHER, DEFAULT_STACK_SIZE, NULL);
}

tOsThread * OsThread_CreateEx(ThreadRoutine routine,
                           void* routineArgs,
                           i32 priority,
                           size_t stackSize,
                           const char* name)
{
   return OsThread_CreateHelper(routine, routineArgs, priority, SCHED_OTHER, stackSize, name);
}


i32 OsThread_CancelEnable(void)
{
   return pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

i32 OsThread_CancelDisable(void)
{
   return pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
}

i32 OsThread_CancelTypeDeferred(void)
{
   return pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
}

i32 OsThread_CancelTypeAsynchronous(void)
{
   return pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
}

i32 OsThread_Destroy(tOsThread *thread,
                         void** threadResult)
{
   /*TODO(JSo): Add error handling */
   i32 res;
   res = OsThread_Cancel(thread);
   res = OsThread_Join(thread, threadResult);

   OsTraceDebug(TraceFacility_OsalThread, "Destroying thread %s", thread->Ressource.Name);
   free(thread);
   return res;
}
