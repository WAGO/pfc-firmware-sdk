//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2017-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     ctmain.c
///
///  \brief    Config tool specific main functionality for
///            reserved_area.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/std_includes.h"
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "ctutil/common_main.h"
#include "ctutil/log.h"
#include "ctutil/filesystem.h"
#include "ctutil/json.h"
#include "ctutil/common_functions.h"
#include "ctparts/common_main_defaults.h"
#include "ctparts/pmain.h"
#include "ctparts/ctmain.h"
#include "params.h"
#include "resources.h"

#include <inttypes.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

#define MOUNT_POINT_VALUE_NAME          "MountPoint"
#define FREE_CAPACITY_VALUE_NAME        "FreeCapacity"
#define AVAILABLE_CAPACITY_VALUE_NAME   "AvailableCapacity"


struct printOptions
{
  bool printMountPoint;
  char const * szMountPoint;
  bool printFreeCapacity;
  uint64_t free;
  bool printAvailableCapacity;
  uint64_t available;
};

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

/// Early function for to prepare config tool main of config_linux_user.
exitCode_t ctparts_PrepareMain(int const argc,
                               char * const argv[])
{
  // Prepare functions
  ctparts_commonMainFunctions_t stFunctions;
  ctparts_SetDefaultCommonMainFunctions(&stFunctions);

  // Call common main function to prepare config tool main
  return ctutil_CommonMain(argc, argv, &stFunctions, NULL);
}


static bool IsMounted(char const * const szMountedPath)
{
  return (szMountedPath != NULL);
}


static bool IsReservedAreaAvailable(ctutil_Options_t const * const pstOptions,
                                    ctutil_Resources_t const * const pstResources)
{
  return pstResources->pstSpecificResources->pfIsFileAvailable(&(pstOptions->stCommonOptions),
                                                               pstResources->pstSpecificResources->szDeviceFilePath, "bc");
}


static bool IsEmulationNeeded(ctutil_Options_t const * const pstOptions,
                              ctutil_Resources_t const * const pstResources)
{
  return (!IsReservedAreaAvailable(pstOptions, pstResources)) && pstOptions->pstSpecificOptions->compatibilityMode;
}


static char const * CreateMountPointPath(ctutil_Options_t const * const pstOptions,
                                         ctutil_Resources_t const * const pstResources)
{
  char * szMountedPath = NULL;
  statusCode_t const status = pstResources->pstSpecificResources->pfGetMountPointForDevice(&(pstOptions->stCommonOptions),
                                                                                           pstResources->pstSpecificResources->szDeviceFilePath,
                                                                                           &szMountedPath, 0);
  if(ctutil_IsStatusFailure(status))
  {
    CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "Failed to determine mount point");
  }

  return szMountedPath;
}


static bool IsMountPointPresent(ctutil_Options_t const * const WC_UNUSED_PARAM(pstOptions),
                                ctutil_Resources_t const * const pstResources)
{
  bool result = false;

  result = pstResources->pstSpecificResources->pfIsFolderAccessible(pstResources->pstSpecificResources->szTargetDirectory);

  return result;
}


static statusCode_t CreateMountPoint(ctutil_Options_t const * const pstOptions,
                                     ctutil_Resources_t const * const pstResources)
{
  return pstResources->pstSpecificResources->pfCreateFolder(&(pstOptions->stCommonOptions),
                                                            pstResources->pstSpecificResources->szTargetDirectory,
                                                            true);
}


static statusCode_t MountReservedArea(ctutil_Options_t const * const pstOptions,
                                      ctutil_Resources_t const * const pstResources)
{
  unsigned long mountOptions = 0U;
  if(pstOptions->pstSpecificOptions->readonly)
  {
    mountOptions = MS_MGC_VAL | MS_RDONLY;
  }
  return pstResources->pstSpecificResources->pfMount(&(pstOptions->stCommonOptions),
                                                     pstResources->pstSpecificResources->szDeviceFilePath,
                                                     pstResources->pstSpecificResources->szTargetDirectory,
                                                     pstResources->pstSpecificResources->szFileSystemType,
                                                     mountOptions, NULL);
}


static statusCode_t UnmountReservedArea(ctutil_Options_t const * const pstOptions,
                                        ctutil_Resources_t const * const pstResources)
{
  int const unmountFlags = MNT_FORCE;
  char const * szMountedPath = CreateMountPointPath(pstOptions, pstResources);
  statusCode_t const result = pstResources->pstSpecificResources->pfUmount(&(pstOptions->stCommonOptions),
                                                                           szMountedPath,
                                                                           unmountFlags);
  ctutil_FreeConst(szMountedPath);

  return result;
}


static statusCode_t ClearReservedArea(ctutil_Options_t const * const pstOptions,
                                      ctutil_Resources_t const * const pstResources,
                                      char const * const szMountedPath)
{
  statusCode_t status = CTUTIL_SUCCESS;

  status = pstResources->pstSpecificResources->pfClearFolder(&(pstOptions->stCommonOptions), szMountedPath);
  if(ctutil_IsStatusFailure(status))
  {
    CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "Failed to clear reserved area");
  }

  return status;
}


static statusCode_t PrintJsonOutput(ctutil_Options_t const * const WC_UNUSED_PARAM(pstOptions),
                                    ctutil_Resources_t const * const WC_UNUSED_PARAM(pstResources),
                                    struct printOptions * pstPrintOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

  bool separatorNeeded = false;
  char szObjectBuffer[512];
  char * szJsonObject = szObjectBuffer;
  size_t freeBufferSize = sizeof(szObjectBuffer);
  size_t addedChars = 0U;
  status = ctutil_JsonObjectStringOpen(szJsonObject, freeBufferSize, &addedChars);
  if(pstPrintOptions->printMountPoint)
  {
    if(ctutil_IsStatusOk(status))
    {
      WC_ASSERT(addedChars < freeBufferSize);
      szJsonObject += addedChars;
      freeBufferSize -= addedChars;
      separatorNeeded = true;
      status = ctutil_JsonObjectStringWriteStringValue(szJsonObject, MOUNT_POINT_VALUE_NAME, pstPrintOptions->szMountPoint,
                                                       freeBufferSize, &addedChars);
    }
  }
  if(pstPrintOptions->printFreeCapacity)
  {
    if(ctutil_IsStatusOk(status) && separatorNeeded)
    {
      WC_ASSERT(addedChars < freeBufferSize);
      szJsonObject += addedChars;
      freeBufferSize -= addedChars;
      separatorNeeded = false;
      status = ctutil_JsonObjectStringWriteMemberSeparator(szJsonObject, freeBufferSize, &addedChars);
    }
    if(ctutil_IsStatusOk(status))
    {
      WC_ASSERT(addedChars < freeBufferSize);
      szJsonObject += addedChars;
      freeBufferSize -= addedChars;
      separatorNeeded = true;
      status = ctutil_JsonObjectStringWriteUInt64Value(szJsonObject, FREE_CAPACITY_VALUE_NAME, pstPrintOptions->free,
                                                       freeBufferSize, &addedChars);
    }
  }
  if(pstPrintOptions->printAvailableCapacity)
  {
    if(ctutil_IsStatusOk(status) && separatorNeeded)
    {
      WC_ASSERT(addedChars < freeBufferSize);
      szJsonObject += addedChars;
      freeBufferSize -= addedChars;
      separatorNeeded = false;
      status = ctutil_JsonObjectStringWriteMemberSeparator(szJsonObject, freeBufferSize, &addedChars);
    }
    if(ctutil_IsStatusOk(status))
    {
      WC_ASSERT(addedChars < freeBufferSize);
      szJsonObject += addedChars;
      freeBufferSize -= addedChars;
      separatorNeeded = true;
      status = ctutil_JsonObjectStringWriteUInt64Value(szJsonObject, AVAILABLE_CAPACITY_VALUE_NAME, pstPrintOptions->available,
                                                       freeBufferSize, &addedChars);
    }
  }
  if(ctutil_IsStatusOk(status))
  {
    WC_ASSERT(addedChars < freeBufferSize);
    szJsonObject += addedChars;
    freeBufferSize -= addedChars;
    status = ctutil_JsonObjectStringClose(szJsonObject, freeBufferSize, &addedChars);
  }
  if(ctutil_IsStatusOk(status))
  {
    int const printResult = printf("%s", szObjectBuffer);
    if(printResult < 0)
    {
      status = CTUTIL_FAILED;
    }
  }

  return status;
}


static statusCode_t PrintTextOutput(ctutil_Options_t const * const WC_UNUSED_PARAM(pstOptions),
                                    ctutil_Resources_t const * const WC_UNUSED_PARAM(pstResources),
                                    struct printOptions * pstPrintOptions)
{
  statusCode_t status = CTUTIL_SUCCESS;

  if(pstPrintOptions->printMountPoint)
  {
    int const printResult = printf("%s=%s\n", MOUNT_POINT_VALUE_NAME, pstPrintOptions->szMountPoint);
    if(printResult < 0)
    {
      status = CTUTIL_FAILED;
    }
  }
  if(ctutil_IsStatusOk(status) && pstPrintOptions->printFreeCapacity)
  {
    int const printResult = printf("%s=%" PRIu64 "\n", FREE_CAPACITY_VALUE_NAME, pstPrintOptions->free);
    if(printResult < 0)
    {
      status = CTUTIL_FAILED;
    }
  }
  if(ctutil_IsStatusOk(status) && pstPrintOptions->printAvailableCapacity)
  {
    int const printResult = printf("%s=%" PRIu64 "\n", AVAILABLE_CAPACITY_VALUE_NAME, pstPrintOptions->available);
    if(printResult < 0)
    {
      status = CTUTIL_FAILED;
    }
  }

  return status;
}


static statusCode_t GiveInformation(ctutil_Options_t const * const pstOptions,
                                    ctutil_Resources_t const * const pstResources,
                                    char const * const szMountedPath,
                                    bool fIsTempTargetDirectory)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Prepare output options
  struct printOptions stPrintOptions =
  {
    .printMountPoint = pstOptions->pstSpecificOptions->printMountPoint,
    .szMountPoint = NULL,
    .printFreeCapacity = pstOptions->pstSpecificOptions->printFreeCapacity && IsMounted(szMountedPath),
    .free = 0U,
    .printAvailableCapacity = pstOptions->pstSpecificOptions->printAvailableCapacity && IsMounted(szMountedPath),
    .available = 0U
  };

  // Collect information
  if(stPrintOptions.printMountPoint)
  {
    bool const emulateReservedArea = IsEmulationNeeded(pstOptions, pstResources);
    stPrintOptions.szMountPoint = (emulateReservedArea || fIsTempTargetDirectory || (szMountedPath == NULL)) ?
                                  MARKER_NOT_MOUNTED : szMountedPath;
  }
  if(stPrintOptions.printFreeCapacity)
  {
    statusCode_t const getStatus = pstResources->pstSpecificResources->pfGetFreeBytes(&(pstOptions->stCommonOptions),
                                                                                      szMountedPath,
                                                                                      &(stPrintOptions.free));
    if(ctutil_IsStatusFailure(getStatus))
    {
      status = getStatus;
      stPrintOptions.printFreeCapacity = false;
    }
  }
  if(stPrintOptions.printAvailableCapacity)
  {
    statusCode_t const getStatus = pstResources->pstSpecificResources->pfGetAvailableBytes(&(pstOptions->stCommonOptions),
                                                                                           szMountedPath,
                                                                                           &(stPrintOptions.available));
    if(ctutil_IsStatusFailure(getStatus))
    {
      status = getStatus;
      stPrintOptions.printAvailableCapacity = false;
    }
  }

  // Print information
  bool const print =    stPrintOptions.printMountPoint
                     || stPrintOptions.printFreeCapacity
                     || stPrintOptions.printAvailableCapacity;
  if(ctutil_IsStatusOk(status) && print && pstOptions->stCommonOptions.jsonOutput)
  {
    status = PrintJsonOutput(pstOptions, pstResources, &stPrintOptions);
  }
  if(ctutil_IsStatusOk(status) && print && pstOptions->stCommonOptions.textOutput)
  {
    status = PrintTextOutput(pstOptions, pstResources, &stPrintOptions);
  }

  return status;
}


static statusCode_t SetTargetDirectory(ctutil_Options_t const * const pstOptions,
                                       ctutil_Resources_t const * const pstResources,
                                       char * const szTargetDirectoryTemplate,
                                       bool * const fIsTmpTargtDirectory)
{
  statusCode_t status = CTUTIL_SUCCESS;
  *fIsTmpTargtDirectory = false;

  if(pstOptions->pstSpecificOptions->szTargetDirectory != NULL)
  {
    pstResources->pstSpecificResources->szTargetDirectory = pstOptions->pstSpecificOptions->szTargetDirectory;
  }
  else
  {
    // Generate temporary directory.
    pstResources->pstSpecificResources->szTargetDirectory = mkdtemp(szTargetDirectoryTemplate);
    if(pstResources->pstSpecificResources->szTargetDirectory != NULL)
    {
      *fIsTmpTargtDirectory = true;
    }
    else
    {
      CTUTIL_LOG_ERRNO_ERROR(pstOptions->stCommonOptions.quiet, errno, "Failed to create temporary directory");
      status = CTUTIL_FAILED;
    }
  }

  return status;
}


/// Main function for config tool core functionality (for example implementation see HelloWorld example config tool).
exitCode_t ctparts_ctMain(int const WC_UNUSED_PARAM(argc),
                          char * const WC_UNUSED_PARAM(argv[]),
                          ctutil_Options_t const * const pstOptions,
                          ctutil_Resources_t const * const pstResources)
{
  exitCode_t result = CTUTIL_EXIT_SUCCESS;
  statusCode_t status = CTUTIL_SUCCESS;
  bool fIsTmpTargetDir = false;
  bool const emulateReservedArea = IsEmulationNeeded(pstOptions, pstResources);
  // TODO: Move to upper function
  char szTmpDirTemplate[]="/tmp/reserved_area.XXXXXX";

  // Check state
  char const * szMountedPath = NULL;
  if(emulateReservedArea)
  {
    if(    (pstOptions->pstSpecificOptions->szTargetDirectory != NULL)
        && ctutil_IsFolderAccessible(pstOptions->pstSpecificOptions->szTargetDirectory))
    {
      // Folder was already created.
      szMountedPath = pstOptions->pstSpecificOptions->szTargetDirectory;
      pstResources->pstSpecificResources->szTargetDirectory = pstOptions->pstSpecificOptions->szTargetDirectory;
    }
  }
  else
  {
    szMountedPath = CreateMountPointPath(pstOptions, pstResources);
  }

  // Mount if required
  bool unmountBeforeExit = pstOptions->pstSpecificOptions->umount;
  if(!IsMounted(szMountedPath))
  {
    if(   (pstOptions->pstSpecificOptions->mount)
       || (pstOptions->pstSpecificOptions->clear)
       || (pstOptions->pstSpecificOptions->printFreeCapacity)
       || (pstOptions->pstSpecificOptions->printAvailableCapacity))
    {
      unmountBeforeExit = unmountBeforeExit || (!(pstOptions->pstSpecificOptions->mount));

      // Set resource target directory.
      status = SetTargetDirectory(pstOptions, pstResources, szTmpDirTemplate, &fIsTmpTargetDir);
      if(ctutil_IsStatusOk(status))
      {
        // Check if mount point exists (file exists and is a folder)
        bool mountPointPresent = IsMountPointPresent(pstOptions, pstResources);

        if(!mountPointPresent && pstOptions->pstSpecificOptions->createMountPoint)
        {
          // Try to create mount point
          status = CreateMountPoint(pstOptions, pstResources);
          if(!ctutil_IsStatusOk(status))
          {
            CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "Creating mount point failed");
            result=CTUTIL_EXIT_GENERAL_ERROR;
          }
        }
      }
      else
      {
        CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "Setup target directory failed");
        result=CTUTIL_EXIT_GENERAL_ERROR;
      }

      if(ctutil_IsStatusOk(status))
      {
        if(emulateReservedArea)
        {
          // Empty folder (mount point) is enough to emulate a mounted reserved area
          status = CTUTIL_SUCCESS;
        }
        else
        {
          status = MountReservedArea(pstOptions, pstResources);
          if(!ctutil_IsStatusOk(status))
          {
            CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "Mounting reserved area failed");
            result=CTUTIL_EXIT_MOUNT_ERROR;
          }
        }
      }
      if(ctutil_IsStatusOk(status))
      {
        szMountedPath = pstResources->pstSpecificResources->szTargetDirectory;
      }
    }
  }

  // Clear area if requested
  if(IsMounted(szMountedPath) && pstOptions->pstSpecificOptions->clear)
  {
    // Try to clear reserved area
    statusCode_t const clearStatus = ClearReservedArea(pstOptions, pstResources, szMountedPath);
    // Don't poison overall status on failure, maybe we can go on without having cleaned everything
    (void)clearStatus;
  }

  // Collect and print information
  if(ctutil_IsStatusOk(status))
  {
    if(    (pstOptions->pstSpecificOptions->printMountPoint)
        || ((pstOptions->pstSpecificOptions->printFreeCapacity) && IsMounted(szMountedPath))
        || ((pstOptions->pstSpecificOptions->printAvailableCapacity) && IsMounted(szMountedPath)))
    {
      status = GiveInformation(pstOptions, pstResources, szMountedPath, fIsTmpTargetDir);
      if(!ctutil_IsStatusOk(status))
      {
        CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "Returning information failed");
      }
    }
  }

  // Unmount if requested or mount was only temporarily necessary
  if((!emulateReservedArea) && IsMounted(szMountedPath) && unmountBeforeExit)
  {
    status = UnmountReservedArea(pstOptions, pstResources);
    if(!ctutil_IsStatusOk(status))
    {
      CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "Unmounting reserved area failed");
      result=CTUTIL_EXIT_MOUNT_ERROR;
    }
  }

  // Remove directory if mount was only requested temporarily
  if(fIsTmpTargetDir)
  {
    status = ctutil_RemoveFolder(&pstOptions->stCommonOptions, pstResources->pstSpecificResources->szTargetDirectory);
    if(!ctutil_IsStatusOk(status))
    {
      CTUTIL_LOG_ERROR(pstOptions->stCommonOptions.quiet, "Removing folder failed");
    }
  }

  if(szMountedPath != pstResources->pstSpecificResources->szTargetDirectory)
  {
    ctutil_FreeConst(szMountedPath);
  }

  return result;
}


//---- End of source file ------------------------------------------------------

