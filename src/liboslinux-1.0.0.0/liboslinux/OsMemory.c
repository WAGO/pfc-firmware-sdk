//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
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
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "OsMemory.h"

#include <assert.h>
#include <stdlib.h>
#include <memory.h>

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

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


void *OsMemory_Alloc(size_t size)
{
   assert(size != 0);

   void *res = malloc(size);
   if (res == NULL)
   {
      OsTraceError("%s", "Out of memory, shutting down program");
      abort();
   }
   return res;
}

void *OsMemory_Calloc(size_t numberOfElements, size_t elementSize)
{
   assert(numberOfElements != 0);
   assert(elementSize != 0);

   void *res = calloc(numberOfElements, elementSize);
   if (res == NULL)
   {
      OsTraceError("%s", "Out of memory, shutting down program");
      abort();
   }
   return res;
}

void *OsMemory_Realloc(void *ptr, size_t size)
{
   assert(ptr != NULL);
   assert(size != 0);

   void *res = realloc(ptr, size);
   if (res == NULL)
   {
      OsTraceError("%s", "Out of memory, shutting down program");
      abort();
   }
   return res;
}

void OsMemory_Copy(void *dest, const void* src, size_t size)
{
   memcpy(dest, src, size);
}

void OsMemory_Set( void* ptr, u8 value, size_t size)
{
   memset(ptr, value, size);
}

i32 OsMemory_Compare( const void* lhs, const void *rhs, size_t count)
{
   return memcmp(lhs, rhs, count);
}


void OsMemory_Free(void * ptr)
{
   free(ptr);
}


