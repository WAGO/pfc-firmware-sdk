//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     shadow_functions.h
///
///  \brief    Shadow modify and cryption functionality for config_linux_user.
///
///  \author   HFS: WAGO Kontakttechnik GmbH & Co. KG
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "wc/assertion.h"
#include "shadow_functions.h"
#include "resources.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <shadow.h>
#include <crypt.h>
#include <paths.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define SALT_PLACE_HOLDER                                     "................"
#define SALT_LENGTH                             (sizeof(SALT_PLACE_HOLDER) - 1U)
#define HASH_ID_STRING                                                       "6"
#define SALT_CHARS                            "./0123456789ABCDEFGHIJKLMNOPQRST" \
                                              "UVWXYZabcdefghijklmnopqrstuvwxyz"

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
static bool IsValidUser(const char* userName)
{
  // TODO: Switch to a configurable white list solution
  return    (strcmp(userName, "admin") == 0)
         || (strcmp(userName, "visualisation") == 0);
}

void GetRandomBytes(ctutil_Resources_t const * const pstResources,
                    unsigned char * const arBuffer,
                    size_t const bufferSize)
{
  int fd;
  int rbytes=0;
  fd = open(pstResources->pstSpecificResources->szRandomSource1, O_NONBLOCK | O_RDONLY);
  rbytes = read(fd, arBuffer, bufferSize);
  close(fd);
  // If first random source does not get enough bytes use second source
  WC_ASSERT(bufferSize <= INT_MAX);
  int const requested = (int)bufferSize;
  if(rbytes < requested)
  {
    size_t const received = (rbytes > 0) ? (size_t)rbytes : 0U;
    fd = open(pstResources->pstSpecificResources->szRandomSource2, O_RDONLY);
    read(fd, arBuffer+received, bufferSize-received); // FIXME: Return value not honored, no error handling
    close(fd);
  }
}


char * CryptPwd(ctutil_Resources_t const * const pstResources,
                char const * const szPassword)
{
  // Create random bytes for making random salt
  unsigned char arNaCl[SALT_LENGTH];
  GetRandomBytes(pstResources, arNaCl, sizeof(arNaCl));

  // Make salt
  const char szNaClChars[] = SALT_CHARS;
  char szCryptSalt[] = "$" HASH_ID_STRING "$" SALT_PLACE_HOLDER;
  WC_STATIC_ASSERT(sizeof(szCryptSalt) >= (3 + SALT_LENGTH + 1));
  for(size_t i = 0; i < SALT_LENGTH; i++)
  {
    szCryptSalt[3+i] = szNaClChars[arNaCl[i] % (sizeof(szNaClChars)-1)];
  }

  // Hash password
  char * const szHash = crypt(szPassword, szCryptSalt);

  return szHash;
}


statusCode_t FillUserData(struct spwd * const stUserData,
                          ctutil_Resources_t const * const pstResources,
                          char const * const WC_UNUSED_PARAM(szUsername),
                          char const * const szPassword1,
                          char const * const szPassword2)
{
  if(!IsValidUser(stUserData->sp_namp))
  {
    return CTUTIL_INVALID_PARAMETER;
  }
  if((szPassword1 == NULL || szPassword2 == NULL) || strcmp(szPassword1,szPassword2))
  {
    return CTUTIL_INVALID_PARAMETER;
  }

  stUserData->sp_pwdp = CryptPwd(pstResources, szPassword1);
  stUserData->sp_lstchg = time(NULL) / (86400);

  return CTUTIL_SUCCESS;
}


statusCode_t FillUserDataFromHash(struct spwd * const pstUserData,
                                  ctutil_Resources_t const * const WC_UNUSED_PARAM(pstResources),
                                  char const * const szUsername,
                                  char const * const szHash)
{
  statusCode_t status = CTUTIL_FAILED;

  char *userEntry = NULL;
  struct spwd *_newUserData = NULL;

  if(!IsValidUser(szUsername))
  {
    return CTUTIL_INVALID_PARAMETER;
  }

  status = CTUTIL_SYSTEM_CALL_ERROR;
  userEntry = malloc(strlen(szUsername) + strlen(szHash) + 2);

  if(NULL != userEntry)
  {
    strcpy(userEntry, szUsername);
    strcat(userEntry, ":");
    strcat(userEntry, szHash);

    if(NULL != (_newUserData = sgetspent(userEntry)))
    {
      memcpy(pstUserData, _newUserData, sizeof(struct spwd));
      status = CTUTIL_SUCCESS;
    }

    free(userEntry);
  }

  return status;
}


static bool IsShadowFileOnline(ctutil_Resources_t const * const pstResources)
{
  char const * const szShadowFilePath = ctutil_GetOriginalFilePath(pstResources->pstSpecificResources->pShadowFile);
  return (strcmp(_PATH_SHADOW, szShadowFilePath) == 0);
}


static statusCode_t LockPasswordFileIfOnline(ctutil_Resources_t const * const pstResources,
                                             bool * const pHasLock)
{
  bool const online = IsShadowFileOnline(pstResources);
  if(online)
  {
    int const lockResult = lckpwdf();
    *pHasLock = (lockResult == 0);
    WC_ASSERT(*pHasLock);
  }

  return ((!online) || (*pHasLock)) ? CTUTIL_SUCCESS : CTUTIL_FAILED;
}


static void UnlockPasswordFile(void)
{
  int const unlockResult = ulckpwdf();
  WC_ASSERT(unlockResult == 0);
}


statusCode_t SearchShadowEntry(char const * const szUsername)
{
  statusCode_t status = CTUTIL_SUCCESS;

  // TODO: Need abstract check for entry existence instead using current active shadow file
  if((getspnam(szUsername)) == NULL)
  {
    status = CTUTIL_NOT_FOUND;
  }

  return status;
}


statusCode_t WriteShadow(ctutil_Resources_t const * const pstResources,
                         struct spwd const * const pstNewUserData)
{
  statusCode_t status = CTUTIL_SUCCESS;
  char const * const szShadowPath = ctutil_GetOriginalFilePath(pstResources->pstSpecificResources->pShadowFile);
  char const * const szNewShadowPath = ctutil_GetChangeableFilePath(pstResources->pstSpecificResources->pShadowFile);

  // Lock an online shadow file to be safe for parallel access
  bool hasLock = false;
  status = LockPasswordFileIfOnline(pstResources, &hasLock);
  if(ctutil_IsStatusFailure(status))
  {
    status = CTUTIL_FILE_OPEN_ERROR;
  }

  FILE * fpShadowOld = NULL;
  FILE * fpShadowNew = NULL;
  if(ctutil_IsStatusOk(status))
  {
    fpShadowOld = fopen(szShadowPath, "r");
    fpShadowNew = fopen(szNewShadowPath, "w");
    if(fpShadowOld == NULL)
    {
      status = CTUTIL_FILE_OPEN_ERROR;
    }
    if(fpShadowNew == NULL)
    {
      status = CTUTIL_FILE_OPEN_ERROR;
    }
  }

  struct spwd * pstPasswordFile = NULL;
  bool shadowFileChanged = false;
  if(ctutil_IsStatusOk(status))
  {
    // Buildup new shadow file
    while((pstPasswordFile = fgetspent(fpShadowOld)) != NULL)
    {
      if(!strcmp(pstNewUserData->sp_namp, pstPasswordFile->sp_namp))
      {
        pstPasswordFile->sp_pwdp = pstNewUserData->sp_pwdp;
        pstPasswordFile->sp_lstchg = pstNewUserData->sp_lstchg;
        shadowFileChanged = true;
      }
      int const putResult = putspent(pstPasswordFile, fpShadowNew);
      if(putResult != 0)
      {
        status = CTUTIL_EXIT_FILE_WRITE_ERROR;
        break;
      }
    }
    fclose(fpShadowOld);
    fclose(fpShadowNew);

    // Restore file mode and replace old shadow file by new one if necessary
    if(shadowFileChanged)
    {
      status = ctutil_CommitChangeableFile(pstResources->pstSpecificResources->pShadowFile);
    }
    else
    {
      status = CTUTIL_NOT_FOUND;
    }
  }

  if(ctutil_IsStatusFailure(status))
  {
    unlink(szNewShadowPath);
  }

  if(ctutil_IsStatusOk(status))
  {
    sync();
  }

  if(hasLock)
  {
    UnlockPasswordFile();
  }

  return status;
}


//---- End of source file ------------------------------------------------------

