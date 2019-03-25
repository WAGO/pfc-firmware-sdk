#ifndef D_OsMemory_H
#define D_OsMemory_H

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

#include <stddef.h>
#include <stdint.h>

#include <OsCommon.h>

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

void *OsMemory_Alloc(size_t size);
void *OsMemory_Calloc(size_t numberOfElements, size_t elementSize);

void *OsMemory_Realloc(void *ptr, size_t size);
void OsMemory_Free(void * ptr);

void OsMemory_Copy(void *dest, const void* src, size_t size);
i32 OsMemory_Compare( const void* lhs, const void *rhs, size_t count);

void OsMemory_Set(void *ptr, u8 value, size_t size);

#endif  // D_OsMemory_H
