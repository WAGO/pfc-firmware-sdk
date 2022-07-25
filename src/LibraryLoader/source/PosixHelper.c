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
/// \brief short description of the file contents
///
/// \author ${user} : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "PosixHelper.h"

#include <assert.h>

#include "LibraryLoader_Logging.h"
#include "cpputest_mem_leak_quirk.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef void *(*fnDlOpen)(__const char *__file, int __mode); // __THROW;

typedef  int (*fnDlClose)(void *__handle); // __THROW __nonnull ((1));

typedef void *(*fnDlSym)(void *__restrict __handle,
          __const char *__restrict __name); // __THROW __nonnull ((2));


typedef int (*fnScanDir)(__const char *__restrict __dir,
      struct dirent ***__restrict __namelist,
      int (*__selector) (__const struct dirent *),
      fnCmp);
 //__nonnull ((1, 2));

typedef char *(*fnDlError)(void);// __THROW;

typedef void (*fnDestroyNamelist)(struct dirent ** namelist);
typedef void (*fnDestroyNamelistEntry)(struct dirent * namelist);


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

static void DestroyNameList(struct dirent ** namelist);
static void DestroyNameListEntry(struct dirent * entry);

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


fnScanDir PosixHelper_ScanDir=scandir;

fnDlOpen PosixHelper_DlOpen = dlopen;
fnDlClose PosixHelper_DlClose = dlclose;
fnDlSym PosixHelper_DlSym = dlsym;
fnDlError PosixHelper_DlError = dlerror;

fnDestroyNamelist PosixHelper_DestroyNamelist = DestroyNameList;
fnDestroyNamelistEntry PosixHelper_DestroyNamelistEntry = DestroyNameListEntry;

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


static void DestroyNameListEntry(struct dirent * entry)
{
   assert(entry != NULL);
   __UNMATCHED_FREE(entry);
}

static void DestroyNameList(struct dirent ** namelist)
{
   assert(namelist != NULL);
   __UNMATCHED_FREE(namelist);
}
