#ifndef D_Posix_helper_H
#define D_Posix_helper_H

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
/// \version <Revision>:
///
/// \brief This module provides function pointers to some posix functions.
///        This way we can mock calls to posix functions during unit testing.
///
/// \author ${user} : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdint.h>
#include <dirent.h>
#include <dlfcn.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef int (fnSelector)(__const struct dirent * dirent);

#if 1
typedef int (*fnCmp)(__const struct dirent **__e1,
                     __const struct dirent **__e2);

#else

typedef int (*fnCmp)(__const void *__e1,
                     __const void *__e2);

#endif

typedef void (*fnDlSymbol)(void);

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

extern int32_t (*PosixHelper_ScanDir)(__const char *__restrict __dir,
          struct dirent ***__restrict __namelist,
          fnSelector selector,
          fnCmp cmp);
     //__nonnull ((1, 2));


extern void *(*PosixHelper_DlOpen)(__const char *__file, int __mode); // __THROW;

extern int (*PosixHelper_DlClose)(void *__handle); // __THROW __nonnull ((1));

extern void *(*PosixHelper_DlSym)(void *__restrict __handle,
          __const char *__restrict __name); // __THROW __nonnull ((2));

extern char *(*PosixHelper_DlError)(void); // __THROW;

extern void (*PosixHelper_DestroyNamelist)(struct dirent ** namelist);
extern void (*PosixHelper_DestroyNamelistEntry)(struct dirent * namelist);

#endif  // D_Posix_helper_H
