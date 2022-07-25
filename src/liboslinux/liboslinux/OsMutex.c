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

#include "OsMutex.h"

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
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


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct stOsMutex
{
   pthread_mutex_t Mutex;
   tOsRessource    Ressource;
   u32        Id;
} _OsMutex;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

static u32 s_MutexIds;


void * OsMutex_GetOsHandle(tOsMutex *self)
{
   assert(self);
   return &self->Mutex;
}

static i32 OsMutex_InitHelper(tOsMutex *self,
                                  bool isRobust,
                                  bool isRecursive,
                                  bool hasPriorityInheritance,
                                  const char* name)
{

   assert(self != NULL);
   OsTraceDebug(TraceFacility_OsalMutex, "%s=%p", _NV(self));

   i32 result = -1;
   if (self != NULL )
   {
      do
      {
         pthread_mutexattr_t attr;

         result = pthread_mutexattr_init(&attr);
         if (result)
         {
            OsTraceError(
                  "pthread_mutexattr_init failed with errno = 0x%08x: %s",
                  errno, strerror(errno));
            break;
         }

         if (hasPriorityInheritance)
         {
            result = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
            if (result)
            {
               OsTraceError(
                     "pthread_mutexattr_setprotocol failed with errno = 0x%08x: %s",
                     errno, strerror(errno));
               break;
            }
         }

         if (isRecursive)
         {
            result = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            if (result)
            {
               OsTraceError(
                     "pthread_mutexattr_settype failed with errno = 0x%08x: %s",
                     errno, strerror(errno));
               break;
            }
         }

         if (isRobust)
         {
            result = pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
            if (result)
            {
               OsTraceError(
                     "pthread_mutexattr_setrobust failed with errno = 0x%08x: %s",
                     errno, strerror(errno));
               break;
            }
         }

#ifndef NDEBUG
         /* always create robust mutex in non release build */
         result = pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
         if (result)
         {
            OsTraceError(
                  "pthread_mutexattr_setrobust failed with errno = 0x%08x: %s",
                  errno, strerror(errno));
            break;
         }
#endif

         result = pthread_mutex_init(&self->Mutex, &attr);
         if (result)
         {
            OsTraceError("pthread_mutex_init failed with errno = 0x%08x: %s",
                         errno, strerror(errno));
            break;
         }

         result = pthread_mutexattr_destroy(&attr);
         if (result)
         {
            OsTraceError(
                  "pthread_mutexattr_destroy failed with errno = 0x%08x: %s",
                  errno, strerror(errno));
            break;
         }
      } while (0);
   }

   if (result == OS_S_OK)
   {
      self->Id = __sync_add_and_fetch(&s_MutexIds, 1);

      if (name == NULL )
      {
         OS_RESSOURCE_SET_NAME(self->Ressource, "Mutex %d", self->Id);
      }
      else
      {
         OsRessource_SetName(&self->Ressource, name);
      }

   }
   return result;
}

tOsMutex *OsMutex_CreateHelper(bool isRobust,
                               bool isRecursive,
                               bool hasPriorityInheritance,
                               const char* name)
{
   tOsMutex *self;
   self = malloc(sizeof(*self));

   if (OsMutex_InitHelper(self, isRobust, isRecursive, hasPriorityInheritance, name ))
   {
      OsMutex_Destroy(self);
      self = NULL;
   }
   return self;
}

tOsMutex *OsMutex_Create(void)
{
   return OsMutex_CreateEx( NULL);

}

tOsMutex *OsMutex_CreateEx(const char* name)
{
   return OsMutex_CreateHelper(false, false, true, name);
}

tOsMutex *OsMutex_CreateRobust(void)
{
   return OsMutex_CreateRobustEx(NULL);
}

tOsMutex *OsMutex_CreateRobustEx(const char* name)
{
   return OsMutex_CreateHelper(true, false, true, name);
}

tOsMutex *OsMutex_CreateRecursive(void)
{
   return OsMutex_CreateRecursiveEx(NULL);
}

tOsMutex *OsMutex_CreateRecursiveEx(const char* name)
{
   return OsMutex_CreateHelper(true, false, true, name);
}

tOsMutex *OsMutex_CreateRobustRecursive(void)
{
   return OsMutex_CreateRobustRecursiveEx( NULL);
}

tOsMutex *OsMutex_CreateRobustRecursiveEx(const char* name)
{
   return OsMutex_CreateHelper(true, true, true, name);
}

i32  OsMutex_Init(tOsMutex *self)
{
   assert(self != NULL);
   OsTraceDebug(TraceFacility_OsalMutex,"%s=%p",_NV(self));

   i32 result = -1;
   if ( self != NULL)
   {
      do
      {
         pthread_mutexattr_t attr;

         result = pthread_mutexattr_init(&attr);
         if (result)
         {
            OsTraceError("pthread_mutexattr_init failed with errno = 0x%08x: %s",
                  errno, strerror(errno));
            break;
         }

         result = pthread_mutexattr_setprotocol(&attr,
               PTHREAD_PRIO_INHERIT);
         if (result)
         {
            OsTraceError(
                  "pthread_mutexattr_setprotocol failed with errno = 0x%08x: %s",
                  errno, strerror(errno));
            break;
         }

#ifndef NDEBUG
         result = pthread_mutexattr_setrobust(&attr,
               PTHREAD_MUTEX_ROBUST);
         if (result)
         {
            OsTraceError(
                  "pthread_mutexattr_setrobust failed with errno = 0x%08x: %s",
                  errno, strerror(errno));
            break;
         }
#endif

         result = pthread_mutex_init(&self->Mutex, &attr);
         if (result)
         {
            OsTraceError("pthread_mutex_init failed with errno = 0x%08x: %s",
                  errno, strerror(errno));
            break;
         }

         result = pthread_mutexattr_destroy(&attr);
         if (result)
         {
            OsTraceError(
                  "pthread_mutexattr_destroy failed with errno = 0x%08x: %s",
                  errno, strerror(errno));
            break;
         }
      } while (0);
   }
   return result;
}

i32 OsMutex_Deinit(tOsMutex *self)
{
   assert(self != NULL);
   OsTraceDebug(TraceFacility_OsalMutex,"%s=%p",_NV(self));
   i32 result = -1;
   if( self  != NULL)
   {
      result = pthread_mutex_destroy(&self->Mutex);
      if (result)
      {
         OsTraceError("pthread_mutex_destroy failed with %s", strerror(result));
      }
   }
   return result;
}

i32 OsMutex_Destroy(tOsMutex *self)
{
   assert(self != NULL);
   OsTraceDebug(TraceFacility_OsalMutex,"%s=%p",_NV(self));

   i32 result = OsMutex_Deinit(self);
   free(self);
   return result;
}

i32 OsMutex_Lock(tOsMutex *self)
{
   assert(self != NULL);
   OsTraceDebug(TraceFacility_OsalMutex,"%s=%p",_NV(self));

   i32 result = -1;
   if (self != NULL)
   {
      result = pthread_mutex_lock(&self->Mutex);

      if (result == EOWNERDEAD)
      {
         result = pthread_mutex_consistent(&self->Mutex);
      }

      if (result)
      {
         OsTraceError("pthread_mutex_lock failed with %s", strerror(result));
      }

   }
   return result;
}

i32 OsMutex_Unlock(tOsMutex *self)
{
   assert(self != NULL);
   OsTraceDebug(TraceFacility_OsalMutex,"%s=%p",_NV(self));

   i32 result = -1;
   if( self != NULL)
   {
      result = pthread_mutex_unlock(&self->Mutex);
      if (result)
      {
         OsTraceError("pthread_mutex_unlock failed with %s", strerror(result));
      }

   }
   return result;
}
