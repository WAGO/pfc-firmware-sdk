#ifndef D_PosixMocks_H
#define D_PosixMocks_H

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

#include "PosixHelper.h"

#include "CppUTestExt/MockSupport_c.h"

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

void PosixMock_SetupScandir(MockSupport_c *mock,
                            struct dirent ** resultNamelist,
                            int result);

void PosixMock_SetupDlOpen(MockSupport_c *mock,
                           void * resultHandle);

void PosixMock_SetupDlClose(MockSupport_c *mock,
                            int result);

void PosixMocK_SetupDlSym(MockSupport_c *mock,
                          fnDlSymbol func);

int PosixMock_ScanDir(__const char *__restrict dir,
                      struct dirent ***__restrict namelist,
                      fnSelector sel,
                      fnCmp cmp);

void *PosixMock_DlOpen(__const char *file,
                       int mode) __THROW;

int PosixMock_DlClose(void * __handle)  __THROW __nonnull ((1));

void *PosixMock_DlSym(void *__restrict handle,
                      __const char *__restrict name) __THROW __nonnull ((2));

void PosixMock_DestroyNamelist(struct dirent **list);
void PosixMock_DestroyNamelistEntry( struct dirent *entry);

#endif  // D_PosixMocks_H
