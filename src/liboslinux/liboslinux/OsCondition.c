//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev$
///
/// \brief short description of the file contents
///
/// \author ${user} $Author$ : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "OsCondition.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>

#include "OsMutex.h"
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

typedef struct _OsCondition
{
   pthread_cond_t Condition;
   tOsMutex *Mutex;
} _OsCondition;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

i32 OsCondition_Broadcast(tOsCondition *self)
{
   assert(self != NULL);

   //OsCondition_Lock(self);
   pthread_cond_broadcast(&self->Condition);
   //OsCondition_Unlock(self);

   return OS_S_OK;
}


i32 OsCondition_Signal(tOsCondition *self)
{
   assert(self != NULL);

   //OsCondition_Lock(self);
   pthread_cond_signal(&self->Condition);
   //OsCondition_Unlock(self);

   return OS_S_OK;
}

i32 OsCondition_Wait(tOsCondition *self)
{
   assert(self != NULL);

   pthread_cond_wait(&self->Condition, OsMutex_GetOsHandle(self->Mutex));

   return OS_S_OK;
}


i32 OsCondition_Lock(tOsCondition *self)
{
	assert(self != NULL);

	OsMutex_Lock(self->Mutex);

	return OS_S_OK;
}

i32 OsCondition_Unlock(tOsCondition *self)
{
	assert(self != NULL);

	OsMutex_Unlock(self->Mutex);

	return OS_S_OK;
}


tOsCondition* OsCondition_Create(void)
{
   tOsCondition *self = calloc(1, sizeof(_OsCondition));

   /* TODO(JSo): Implement Error Handling and use attributes ..*/
   pthread_cond_init(&self->Condition, NULL );
   self->Mutex = OsMutex_Create();

   return self;
}

void OsCondition_Destroy(tOsCondition *self)
{
   if (self != NULL )
   {
      OsMutex_Destroy(self->Mutex);
      pthread_cond_destroy(&self->Condition);
   }
   free(self);
}

