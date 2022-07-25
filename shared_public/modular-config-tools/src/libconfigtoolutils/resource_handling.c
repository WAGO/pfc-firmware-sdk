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
///  \file     resource_handling.c
///
///  \brief    Helper functions for config tool resource handling.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "wc/std_includes.h"
#include "ctutil/resource_handling.h"
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

struct ctutil_ChangeableFile
{
  char * szOriginalPath;
  char * szWriteFilePath;
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
void ctutil_InitResources(ctutil_Resources_t * const pstResources)
{
  memset(pstResources, 0, sizeof(*pstResources));
}


statusCode_t ctutil_OpenCommonResources(ctutil_ResourcesCommon_t * const pstResources)
{
  pstResources->szLastErrorFile = "/tmp/last_error.txt";

  return CTUTIL_SUCCESS;
}


void ctutil_ReleaseCommonResources(ctutil_ResourcesCommon_t * const WC_UNUSED_PARAM(pstResources))
{
  // Nothing to do
  return;
}


statusCode_t ctutil_PrepareChangeableFile(char const * const szFilePath,
                                          ctutil_ChangeableFile_t * * const ppFileHandle)
{
  statusCode_t status = CTUTIL_SUCCESS;
  size_t const originalPathLength = strlen(szFilePath);

  // Prepare file handle
  WC_ASSERT(*ppFileHandle == NULL);
  *ppFileHandle = malloc(sizeof(ctutil_ChangeableFile_t));
  ctutil_ChangeableFile_t * const pFile = *ppFileHandle;
  if(pFile == NULL)
  {
    status = CTUTIL_FAILED;
  }

  // Prepare original file path
  if(ctutil_IsStatusOk(status))
  {
    pFile->szOriginalPath = malloc(originalPathLength + 1);
    if(pFile->szOriginalPath == NULL)
    {
      status = CTUTIL_FAILED;
      free(pFile);
      *ppFileHandle = NULL;
    }
    else
    {
      strncpy(pFile->szOriginalPath, szFilePath, originalPathLength);
      pFile->szOriginalPath[originalPathLength] = '\0';
    }
  }

  // Prepare write file path
  if(ctutil_IsStatusOk(status))
  {
    pFile->szWriteFilePath = malloc(originalPathLength + 2);
    if(pFile->szWriteFilePath == NULL)
    {
      status = CTUTIL_FAILED;
      free(pFile->szOriginalPath);
      free(pFile);
      *ppFileHandle = NULL;
    }
    else
    {
      strncpy(pFile->szWriteFilePath, szFilePath, originalPathLength);
      pFile->szWriteFilePath[originalPathLength] = '-';
      pFile->szWriteFilePath[originalPathLength + 1] = '\0';
    }
  }

  if(ctutil_IsStatusOk(status))
  {
    WC_ASSERT(*ppFileHandle != NULL);
  }
  else
  {
    WC_ASSERT(*ppFileHandle == NULL);
  }

  return status;
}


void ctutil_ReleaseFileForChanges(ctutil_ChangeableFile_t * * const ppFileHandle)
{
  ctutil_ChangeableFile_t * const pFile = *ppFileHandle;
  if(pFile != NULL)
  {
    free(pFile->szOriginalPath);
    free(pFile->szWriteFilePath);
    free(pFile);
    *ppFileHandle = NULL;
  }
}


char const * ctutil_GetOriginalFilePath(ctutil_ChangeableFile_t const * const pFileHandle)
{
  return pFileHandle->szOriginalPath;
}


char const * ctutil_GetChangeableFilePath(ctutil_ChangeableFile_t const * const pFileHandle)
{
  return pFileHandle->szWriteFilePath;
}


statusCode_t ctutil_CommitChangeableFile(ctutil_ChangeableFile_t const * const pFileHandle)
{
  statusCode_t status = CTUTIL_SUCCESS;
  char const * const szOriginalPath = ctutil_GetOriginalFilePath(pFileHandle);
  char const * const szModifiedPath = ctutil_GetChangeableFilePath(pFileHandle);

  struct stat stOriginalFileStat;
  int const statOriginalResult = lstat(szOriginalPath, &stOriginalFileStat);
  if(statOriginalResult != 0)
  {
    status = CTUTIL_FILE_OPEN_ERROR;
  }
  if(ctutil_IsStatusOk(status))
  {
    int const modChangeResult = chmod(szModifiedPath, stOriginalFileStat.st_mode);
    if(modChangeResult == 0)
    {
      int const ownChangeResult = chown(szModifiedPath, stOriginalFileStat.st_uid, stOriginalFileStat.st_gid);
      if(ownChangeResult != 0)
      {
        status = CTUTIL_SYSTEM_CALL_ERROR;
      }
    }
    else
    {
      status = CTUTIL_SYSTEM_CALL_ERROR;
    }
  }
  if(ctutil_IsStatusOk(status))
  {
    int const renameResult = rename(szModifiedPath, szOriginalPath);
    if(renameResult != 0)
    {
      status = CTUTIL_SYSTEM_CALL_ERROR;
    }
  }

  return status;
}


//---- End of source file ------------------------------------------------------

