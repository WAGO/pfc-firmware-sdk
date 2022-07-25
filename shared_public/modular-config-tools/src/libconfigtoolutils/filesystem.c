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
///  \file     filesystem.c
///
///  \brief    Filesystem tools.
///            TODO: Check if there is a library providing this file system stuff.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/filesystem.h"
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "ctutil/log.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <mntent.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Define for Kernel file interface to obtain mount information from.
#define MOUNT_INFORMATION   "/proc/mounts"

/// Define for default buffer size to build path strings.
#define DEFAULT_PATH_BUFFER_SIZE   256


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

static statusCode_t CopyString(char const * const szSource,
                               size_t const copyLength,
                               char * * const pszTarget,
                               size_t const targetBufferSize,
                               bool allowAllocation)
{
  statusCode_t status = CTUTIL_SUCCESS;

  if(copyLength >= targetBufferSize)
  {
    if(!allowAllocation)
    {
      status = CTUTIL_FAILED;
    }
    else
    {
      *pszTarget = malloc(copyLength + 1);
      if(*pszTarget == NULL)
      {
        status = CTUTIL_FAILED;
      }
    }
  }
  if(ctutil_IsStatusOk(status))
  {
    strncpy(*pszTarget, szSource, copyLength);
    (*pszTarget)[copyLength] = '\0';
  }

  return status;
}


statusCode_t ctutil_GetParentFolderString(char const * const szPath,
                                          size_t const pathLength,
                                          char * * const pszTarget,
                                          size_t const targetBufferSize,
                                          bool allowAllocation)
{
  statusCode_t status = CTUTIL_SUCCESS;

  WC_ASSERT(szPath[0] == '/');
  size_t copyPathLength = pathLength;

  // Remove trailing slashes
  while(szPath[copyPathLength] == '/')
  {
    --copyPathLength;
  }

  // Search next slash from end
  while(szPath[copyPathLength] != '/')
  {
    --copyPathLength;
  }

  // Reached root directory
  if (0 == copyPathLength)
  {
    copyPathLength = 1;
  }

  // Copy substring from original path
  status = CopyString(szPath, copyPathLength, pszTarget, targetBufferSize, allowAllocation);

  return status;
}


bool ctutil_IsFolderAccessible(char const * const szPath)
{
  bool accessible = false;

  // Try to open folder
  DIR * pFolder = opendir(szPath);
  if(pFolder != NULL)
  {
    accessible = true;
    closedir(pFolder);
  }

  return accessible;
}

//------------------------------------------------------------------------------
/// Create a given folder hierarchy.
/// This function works like "mkdir -p".
///
/// \param pstCommonOptions
///   Common option structure pointer.
/// \param szPath
///   Folder path to create with parent folders.
///
/// \return
///   \link CTUTIL_SUCCESS \endlink in case folder hierarchy was created, an error code otherwise.
///
/// \see CTUTIL_SUCCESS
//------------------------------------------------------------------------------
static statusCode_t CreateFolderHierarchy(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                          char const * const szPath)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Remove trailing slashes
  size_t const pathLength = strlen(szPath);
  size_t copyPathLength = pathLength;
  while(szPath[copyPathLength] == '/')
  {
    --copyPathLength;
  }

  char szModifiablePathStackBuffer[DEFAULT_PATH_BUFFER_SIZE];
  char * szModifiablePath = szModifiablePathStackBuffer;

  status = CopyString(szPath, copyPathLength, &szModifiablePath, sizeof(szModifiablePathStackBuffer), true);

  if(ctutil_IsStatusOk(status))
  {
    WC_ASSERT(szModifiablePath[0] == '/');
    WC_ASSERT(szModifiablePath[1] != '\0');
    char * szSubPath = &(szModifiablePath[1]);
    while((szSubPath = strchr(szSubPath, '/')) != NULL)
    {
      // Replace slash by terminator
      szSubPath[0] = '\0';

      // Try to create parent directory if not already present
      if(!ctutil_IsFolderAccessible(szModifiablePath))
      {
        int const createResult = mkdir(szModifiablePath, 0777);
        if(createResult < 0)
        {
          status = CTUTIL_FAILED;
          CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Unable to create parent folder");
          break;
        }
      }
      // Revert replaced slash
      szSubPath[0] = '/';
      szSubPath = &(szSubPath[1]);
    }
    if(!ctutil_IsFolderAccessible(szPath))
    {
      int const createResult = mkdir(szPath, 0777);
      if(createResult < 0)
      {
        status = CTUTIL_FAILED;
        CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Unable to create target folder");
      }
    }

    if(szModifiablePath != szModifiablePathStackBuffer)
    {
      free(szModifiablePath);
    }
  }

  return status;
}


statusCode_t ctutil_CreateFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                 char const * const szPath,
                                 bool createParentsAlso)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Be optimistic and try to create
  int const initialCreateResult = mkdir(szPath, 0777);
  if(initialCreateResult < 0)
  {
    if(errno == EEXIST && ctutil_IsFolderAccessible(szPath))
    {
      CTUTIL_LOG_INFO(pstCommonOptions->quiet, "Folder already exists");
    }
    // Give up or retry possible after creating parent directories?
    else if((errno != ENOENT) || !createParentsAlso)
    {
      status = CTUTIL_FAILED;
      CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Unable to create folder");
    }
    else
    {
      // Try to create also parent folders
      WC_ASSERT(createParentsAlso);
      status = CreateFolderHierarchy(pstCommonOptions, szPath);
      if(ctutil_IsStatusFailure(status))
      {
        CTUTIL_LOG_ERROR(pstCommonOptions->quiet, "Unable to create folder");
      }
    }
  }

  return status;
}


statusCode_t ctutil_ClearFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                char const * const szPath)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Open folder
  size_t const folderPathLength = strlen(szPath);
  DIR * pFolder = opendir(szPath);
  if(pFolder == NULL)
  {
    status = CTUTIL_FAILED;
    CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Unable to open folder");
  }

  if(ctutil_IsStatusOk(status))
  {
    // Remove elements
    char szFilePathStackBuffer[DEFAULT_PATH_BUFFER_SIZE];
    WC_STATIC_ASSERT(sizeof(szFilePathStackBuffer) >= (WC_SIZEOF_MEMBER(struct dirent, d_name) / 2));
    char * szFilePath = szFilePathStackBuffer;
    size_t bufferSize = sizeof(szFilePathStackBuffer);
    struct dirent * pNextEntry;
    while((pNextEntry = readdir(pFolder)) != NULL)
    {
      status = CTUTIL_SUCCESS;

      // Skip . & .. to avoid deletion
      if(    (strcmp(".", pNextEntry->d_name) == 0)
          || (strcmp("..", pNextEntry->d_name) == 0))
      {
        continue;
      }

      // Concat folder path and entry
      size_t const elementNameLength = strlen(pNextEntry->d_name);
      size_t const neededBufferSize = folderPathLength + 1 + elementNameLength + 1;
      if(neededBufferSize > bufferSize)
      {
        // Use a dynamic buffer
        if(szFilePath != szFilePathStackBuffer)
        {
          free(szFilePath);
        }
        szFilePath = malloc(neededBufferSize);
        if(szFilePath == NULL)
        {
          status = CTUTIL_FAILED;
          CTUTIL_LOG_ERROR(pstCommonOptions->quiet, "Failed to allocate memory");
          break;
        }
      }
      strcpy(szFilePath, szPath);
      szFilePath[folderPathLength] = '/';
      strcpy(szFilePath + folderPathLength + 1, pNextEntry->d_name);
      szFilePath[folderPathLength + 1 + elementNameLength] = '\0';

      // Get entry element stats
      struct stat stFilePathStat;
      int const statResult = lstat(szFilePath, &stFilePathStat);
      if(statResult < 0)
      {
        status = CTUTIL_FAILED;
        CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Unable to get folder entry stats");
      }

      // Remove entry element
      if(ctutil_IsStatusOk(status))
      {
        if(S_ISDIR(stFilePathStat.st_mode))
        {
          CTUTIL_LOG_DEBUG1(pstCommonOptions->quiet, "Remove folder %s", szFilePath);
          status = ctutil_RemoveFolder(pstCommonOptions, szFilePath);
        }
        else
        {
          CTUTIL_LOG_DEBUG1(pstCommonOptions->quiet, "Unlink file %s", szFilePath);
          int const unlinkResult = unlink(szFilePath);
          if(unlinkResult < 0)
          {
            status = CTUTIL_FAILED;
            CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Unable to remove file");
          }
        }
      }
    }

    // Free dynamically allocated buffer if necessary
    if(szFilePath != szFilePathStackBuffer)
    {
      free(szFilePath);
    }

    // Close folder
    closedir(pFolder);
  }

  return status;
}


statusCode_t ctutil_RemoveFolder(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                 char const * const szPath)
{
  statusCode_t status = CTUTIL_SUCCESS;

  status = ctutil_ClearFolder(pstCommonOptions, szPath);
  if(ctutil_IsStatusOk(status))
  {
    int const removeResult = rmdir(szPath);
    if(removeResult < 0)
    {
      status = CTUTIL_FAILED;
      CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Unable to remove folder");
    }
  }

  return status;
}


statusCode_t ctutil_GetMountPointForDevice(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                           char const * const szDeviceFilePath,
                                           char * * const pszMountPoint,
                                           size_t const mountPointBufferSize)
{
  statusCode_t status = CTUTIL_SUCCESS;

  FILE * pFile;
  pFile = setmntent(MOUNT_INFORMATION, "r");
  if(pFile == NULL)
  {
    status = CTUTIL_FAILED;
    CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Failed to get mount information from " MOUNT_INFORMATION);
  }
  else
  {
    struct mntent * pMountEntry;
    while((pMountEntry = getmntent(pFile)) != NULL)
    {
      if(strcmp(szDeviceFilePath, pMountEntry->mnt_fsname) == 0)
      {
        status = CopyString(pMountEntry->mnt_dir, strlen(pMountEntry->mnt_dir), pszMountPoint, mountPointBufferSize, true);
        break;
      }
    }
    if((ctutil_IsStatusFailure(status)) || (pMountEntry == NULL))
    {
      // Not mounted
      *pszMountPoint = NULL;
    }

    endmntent(pFile);
  }

  return status;
}


statusCode_t ctutil_CheckForMountPoint(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                       char const * const szPath)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Check for root
  if((szPath[0] == '/') && (szPath[1] == '\0'))
  {
    status = CTUTIL_FAILED;
    CTUTIL_LOG_ERROR(pstCommonOptions->quiet, "Ask for \"/\" as mount point");
  }

  // Get file (folder) statistics
  struct stat stStats;
  if(ctutil_IsStatusOk(status))
  {
    int const statResult = stat(szPath, &stStats);
    if(statResult < 0)
    {
      status = CTUTIL_FAILED;
      CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Failed to get folder statistics");
    }
  }

  // Build upper folder path string
  char szUpperPathStackBuffer[DEFAULT_PATH_BUFFER_SIZE];
  char * szUpperPath = szUpperPathStackBuffer;
  if(ctutil_IsStatusOk(status))
  {
    size_t const pathLength = strlen(szPath);
    status = ctutil_GetParentFolderString(szPath, pathLength, &szUpperPath, sizeof(szUpperPathStackBuffer), true);
  }

  // Compare device ID with device ID from upper folder
  if(ctutil_IsStatusOk(status))
  {
    struct stat stUpperStats;
    int const statResult = stat(szUpperPath, &stUpperStats);
    if(statResult < 0)
    {
      status = CTUTIL_FAILED;
      CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Failed to get upper folder statistics");
    }
    else
    {
      if(stUpperStats.st_dev != stStats.st_dev)
      {
        // Folder is mount point
        status = CTUTIL_FAILED;
      }
    }
    if(szUpperPath != szUpperPathStackBuffer)
    {
      free(szUpperPath);
    }
  }

  return status;
}


// fileType: b = block, c = character, d = directory/folder, f = fifo, l = link, s = socket, - = regular file
bool ctutil_IsFileTypeMatch(struct stat * pstStats,
                            char const fileType)
{
  bool result =  false;

  switch(fileType)
  {
    case 'b':
      result = S_ISBLK(pstStats->st_mode);
      break;

    case 'c':
      result = S_ISCHR(pstStats->st_mode);
      break;

    case 'd':
      result = S_ISDIR(pstStats->st_mode);
      break;

    case 'f':
      result = S_ISFIFO(pstStats->st_mode);
      break;

    case 'l':
      result = S_ISLNK(pstStats->st_mode);
      break;

    case 's':
      result = S_ISSOCK(pstStats->st_mode);
      break;

    case '-':
      result = S_ISREG(pstStats->st_mode);
      break;

    default:
      // Unknown file type
      result =  false;
  }

  return result;
}


// szFileTypes: b = block, c = character, d = directory/folder, f = fifo, l = link, s = socket, - = regular file
// szFileTypes may be NULL
bool ctutil_IsFileAvailable(ctutil_OptionsCommon_t const * const WC_UNUSED_PARAM(pstCommonOptions),
                            char const * const szFilePath,
                            char const * const szFileTypes)
{
  bool result = false;

  // Get file statistics
  struct stat stStats;
  int const statResult = stat(szFilePath, &stStats);
  if(statResult == 0)
  {
    if(szFileTypes == NULL)
    {
      result = true;
    }
    else
    {
      // Check for requested file types
      char const * pFileType = szFileTypes;
      while((!result) && (*pFileType) != '\0')
      {
        result = ctutil_IsFileTypeMatch(&stStats, *pFileType);
        ++pFileType;
      }
    }
  }

  return result;
}


statusCode_t ctutil_Mount(ctutil_OptionsCommon_t const * const pstCommonOptions,
                          char const * const szSpecialFile,
                          char const * const szDir,
                          char const * const szFileSystemType,
                          unsigned long int const flags,
                          void const * const pData)
{
  statusCode_t status = CTUTIL_SUCCESS;

  status = ctutil_CheckForMountPoint(pstCommonOptions, szDir);
  if(ctutil_IsStatusOk(status))
  {
    int const mountResult = mount(szSpecialFile, szDir, szFileSystemType, flags, pData);
    if(mountResult < 0)
    {
      status = CTUTIL_FAILED;
      CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Failed to mount");
    }
  }

  return status;
}


statusCode_t ctutil_Umount(ctutil_OptionsCommon_t const * const pstCommonOptions,
                           char const * const szSpecialFile,
                           int const flags)
{
  statusCode_t status = CTUTIL_SUCCESS;

  int const umountResult = umount2(szSpecialFile, flags);
  if(umountResult < 0)
  {
    status = CTUTIL_FAILED;
    CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Failed to unmount");
  }

  return status;
}


statusCode_t ctutil_GetFileSystemStatistics(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                            char const * const szPath,
                                            struct statvfs * const pstFileSystemStats)
{
  statusCode_t status = CTUTIL_SUCCESS;

  int const statResult = statvfs(szPath, pstFileSystemStats);
  if(statResult < 0)
  {
    status = CTUTIL_FAILED;
    CTUTIL_LOG_ERRNO_ERROR(pstCommonOptions->quiet, errno, "Failed to get file system statistics");
  }

  return status;
}


statusCode_t ctutil_GetFreeBytes(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                 char const * const szFolderPath,
                                 uint64_t * const freeBytes)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Get file system statistics
  struct statvfs stFileSystemStats;
  status = ctutil_GetFileSystemStatistics(pstCommonOptions, szFolderPath, &stFileSystemStats);
  if(ctutil_IsStatusOk(status))
  {
    *freeBytes = (uint64_t)stFileSystemStats.f_bsize * stFileSystemStats.f_bfree;
  }

  return status;
}


statusCode_t ctutil_GetAvailableBytes(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                      char const * const szFolderPath,
                                      uint64_t * const availableBytes)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // Get file system statistics
  struct statvfs stFileSystemStats;
  status = ctutil_GetFileSystemStatistics(pstCommonOptions, szFolderPath, &stFileSystemStats);
  if(ctutil_IsStatusOk(status))
  {
    *availableBytes = (uint64_t)stFileSystemStats.f_bsize * stFileSystemStats.f_bavail;
  }

  return status;
}


//---- End of source file ------------------------------------------------------

