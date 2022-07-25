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

#include "OsRwLock.h"

#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>

#include <pthread.h>

#include "OsRessource.h"
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

typedef struct stOsRwLock
{
   tOsRessource Ressource;
   pthread_rwlock_t RwLock;
}_OsRwLock;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------
static u32 s_RwLockIds;

static void CreateDefaultName(tOsRwLock *lock)
{
   OS_RESSOURCE_SET_NAME(lock->Ressource, "RwLock %d", lock->Ressource.Id);
}

static tOsRwLock * OsRwLock_CreateHelper(u32 kind, const char* name)
{
   tOsRwLock *lock = malloc(sizeof(*lock));

   do {
      if ( NULL == lock) {
         OsTraceError("%s", "Out of memory!");
         break;
      }

      lock->Ressource.Id =  __sync_add_and_fetch(&s_RwLockIds, 1);

      if ( NULL == name ) {
         CreateDefaultName(lock);
      } else {
         OsRessource_SetName(&lock->Ressource, name);
      }

      i32 result;
      pthread_rwlockattr_t attr;

      result = pthread_rwlockattr_init(&attr);
      if ( result !=0 ) {
         OsTraceError("pthread_rwlockattr_init faled with 0x%08x: %s", errno, strerror(errno));
         break;
      }

      pthread_rwlockattr_setkind_np(&attr, kind);

      result = pthread_rwlock_init( &lock->RwLock, &attr);
      if (result != 0) {
         OsTraceError("pthread_rwlock_init faled with 0x%08x: %s", errno, strerror(errno));
         break;
      }

      /* don't care if this one fails.. */
      pthread_rwlockattr_destroy(&attr);

   } while(0);

   return lock;
}


int  OsRwLock_WriteLock(tOsRwLock *lock)
{
   assert(lock != NULL);
   return pthread_rwlock_wrlock(&lock->RwLock);
}

int OsRwLock_ReadLock(tOsRwLock *lock)
{
   assert(lock != NULL);
   return pthread_rwlock_rdlock(&lock->RwLock);
}

int OsRwLock_Unlock(tOsRwLock *lock)
{
   assert(lock != NULL);
   return pthread_rwlock_unlock(&lock->RwLock);
}

tOsRwLock * OsRwLock_Create(void)
{
   return OsRwLock_CreateHelper(0, NULL);
}

tOsRwLock * OsRwLock_CreateEx(const char* name)
{
   return OsRwLock_CreateHelper(0, name);
}

void OsRwLock_Destroy(tOsRwLock *lock)
{
   if ( lock != NULL)
   {
      (void)pthread_rwlock_destroy(&lock->RwLock);
      free(lock);
   }
}
