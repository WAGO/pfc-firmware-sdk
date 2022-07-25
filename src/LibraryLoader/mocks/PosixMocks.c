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

#include "PosixMocks.h"

#include <stdlib.h>
#include <memory.h>

#include "common_helper.h"

#include "CppUTest/TestHarness_c.h"

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

static struct stScandirFakeData
{
   MockSupport_c *Mock;
   struct dirent ** ResultNamelist;
   int Result;
} s_ScandirFakeData;

static struct stDlOpenFakeData
{
   MockSupport_c * Mock;
   void * ResultHandle;
} s_DlOpenFakeData;

static struct stDlCloseFakeData
{
   MockSupport_c * Mock;
   int Result;
} s_DlCloseFakeData;

static struct stDlSymFakeData
{
   MockSupport_c * Mock;
   fnDlSymbol Func;
} s_DlSymFakeData;

void PosixMock_SetupScandir(MockSupport_c *mock,
                            struct dirent ** resultNamelist,
                            int result)
{
   s_ScandirFakeData.Mock = mock;
   s_ScandirFakeData.ResultNamelist = resultNamelist;
   s_ScandirFakeData.Result = result;
}

void PosixMock_SetupDlOpen(MockSupport_c *mock,
                           void * resultHandle)
{
   s_DlOpenFakeData.Mock = mock;
   s_DlOpenFakeData.ResultHandle = resultHandle;
}

void PosixMock_SetupDlClose(MockSupport_c *mock,
                            int result)
{
   s_DlCloseFakeData.Mock = mock;
   s_DlCloseFakeData.Result = result;
}

void PosixMocK_SetupDlSym(MockSupport_c *mock,
                          fnDlSymbol func)
{
   s_DlSymFakeData.Mock = mock;
   s_DlSymFakeData.Func = func;
}

int PosixMock_ScanDir(__const char *__restrict dir,
                      struct dirent ***__restrict namelist,
                      fnSelector sel,
                      fnCmp cmp)
{
   UNUSED_PARAMETER(sel);
   UNUSED_PARAMETER(cmp);

   CHECK_C(dir != NULL);
   CHECK_C(namelist != NULL);
//   CHECK_C(sel != NULL);
   CHECK_C(cmp != NULL);

   *namelist = s_ScandirFakeData.ResultNamelist;
   if (s_ScandirFakeData.Mock != NULL )
   {
      return
            s_ScandirFakeData.Mock->actualCall("scandir")
            ->withStringParameters("dir", dir)
            //->withPointerParameters("namelist", namelist)
            ->withPointerParameters("selector", sel)
            ->withPointerParameters("cmp", cmp)
            ->returnValue().value.intValue;

   }

   return s_ScandirFakeData.Result;
}

void *PosixMock_DlOpen(__const char *file,
                       int mode)
{
   CHECK_C(file != NULL);

   if (s_DlOpenFakeData.Mock)
   {
      return s_DlOpenFakeData.Mock->actualCall("dlopen")
            ->withStringParameters("file",file)
            ->withIntParameters("mode", mode)
            ->returnValue().value.pointerValue;
   }
   return s_DlOpenFakeData.ResultHandle;
}

int PosixMock_DlClose(void * handle)
{
   if ( s_DlCloseFakeData.Mock != NULL)
   {
      return s_DlCloseFakeData.Mock->actualCall("dlclose")
            ->withPointerParameters("handle", handle)
            ->returnValue().value.intValue;
   }

   return s_DlCloseFakeData.Result;
}

void *PosixMock_DlSym(void *__restrict handle,
                      __const char *__restrict name)
{
   CHECK_C(handle != NULL);

   if ( s_DlSymFakeData.Mock != NULL)
   {
      return s_DlSymFakeData.Mock->actualCall("dlsym")
            ->withPointerParameters("handle", handle)
            ->withStringParameters("name", name)
            ->returnValue().value.pointerValue;
   }
   return s_DlSymFakeData.Func;
}

void PosixMock_DestroyNamelist(struct dirent **list)
{
   CHECK_C(list != NULL);

}

void PosixMock_DestroyNamelistEntry( struct dirent *entry)
{
   CHECK_C(entry != NULL);
}

