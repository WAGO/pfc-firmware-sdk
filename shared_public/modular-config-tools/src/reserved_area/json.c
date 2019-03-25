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
///  \file     json.c
///
///  \brief    JSON format tools.
///            TODO: Should be moved to a library.
///            TODO: Check if there is a library providing this JSON stuff.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "json.h"
#include <string.h>
#include <stdio.h>

#include <inttypes.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

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
static char GetEscapeCharacter(char const character)
{
  char result = '\0';
  char const mapEscapeChars[][2] =
  {
    {'"', '"'},
    {'\b', 'b'},
    {'\f', 'f'},
    {'\n', 'n'},
    {'\r', 'r'},
    {'\t', 't'},
    {'\\', '\\'},
    {'/', '/'}
  };
  for(size_t i = 0U; i < (sizeof(mapEscapeChars) / 2); i++)
  {
    if(character == mapEscapeChars[i][0])
    {
      result = mapEscapeChars[i][1];
      break;
    }
  }

  return result;
}


void EscapeJsonStringValue(char * const szDestination,
                           size_t const freeBufferSize,
                           char const * const szSource,
                           size_t * const pWrittenCharacters)
{
  size_t readCharacters = 0;
  size_t writtenCharacters = 0;
  while((szSource[readCharacters] != '\0') && (writtenCharacters < (freeBufferSize - 1U)))
  {
    char const escapeCharacter = GetEscapeCharacter(szSource[readCharacters]);
    if(escapeCharacter == '\0')
    {
      szDestination[writtenCharacters] = szSource[readCharacters];
      ++writtenCharacters;
    }
    else
    {
      szDestination[writtenCharacters] = '\\';
      ++writtenCharacters;
      szDestination[writtenCharacters] = escapeCharacter;
      ++writtenCharacters;
    }
    ++readCharacters;
  }
  szDestination[writtenCharacters] = '\0';

  // Return output variables
  if(pWrittenCharacters != NULL)
  {
   *pWrittenCharacters = writtenCharacters;
  }

  return;
}


statusCode_t JsonObjectStringOpen(char * const szObjectString,
                                  size_t const freeBufferSize,
                                  size_t * const pAddedCharacters)
{
  statusCode_t status = CTUTIL_SUCCESS;

  if(freeBufferSize < 2)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    szObjectString[0] = '{';
    szObjectString[1] = '\0';
    *pAddedCharacters = 1;
  }

  return status;
}


statusCode_t JsonObjectStringClose(char * const szObjectString,
                                   size_t const freeBufferSize,
                                   size_t * const pAddedCharacters)
{
  statusCode_t status = CTUTIL_SUCCESS;

  if(freeBufferSize < 2)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    szObjectString[0] = '}';
    szObjectString[1] = '\0';
    *pAddedCharacters = 1;
  }

  return status;
}


statusCode_t JsonObjectStringWriteMemberSeparator(char * const szObjectString,
                                                  size_t const freeBufferSize,
                                                  size_t * const pAddedCharacters)
{
  statusCode_t status = CTUTIL_SUCCESS;

  if(freeBufferSize < 2)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    szObjectString[0] = ',';
    szObjectString[1] = '\0';
    *pAddedCharacters = 1;
  }

  return status;
}


statusCode_t JsonObjectStringWriteNameValueSeparator(char * const szObjectString,
                                                     size_t const freeBufferSize,
                                                     size_t * const pAddedCharacters)
{
  statusCode_t status = CTUTIL_SUCCESS;

  if(freeBufferSize < 3)
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    szObjectString[0] = ':';
    szObjectString[1] = ' ';
    szObjectString[2] = '\0';
    *pAddedCharacters = 2;
  }

  return status;
}


statusCode_t JsonObjectStringWriteName(char * const szObjectString,
                                       char const * const szName,
                                       size_t const freeBufferSize,
                                       size_t * const pAddedCharacters)
{
  statusCode_t status = CTUTIL_SUCCESS;

  char szFixSizeBuffer[256];
  char * szEscapedName = szFixSizeBuffer;
  size_t escapeBufferSize = sizeof(szFixSizeBuffer);
  size_t const worstCaseBufferSize = (strlen(szName) * 2) + 1;
  if(worstCaseBufferSize > escapeBufferSize)
  {
    // Fixed size buffer maybe not long enough, allocate memory for worst case
    szEscapedName = malloc(worstCaseBufferSize);
    escapeBufferSize = worstCaseBufferSize;
  }
  size_t escapedNameLength = 0;
  EscapeJsonStringValue(szEscapedName, escapeBufferSize, szName, &escapedNameLength);
  if(freeBufferSize < (escapedNameLength + 3))
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    szObjectString[0] = '"';
    strncpy(&(szObjectString[1]), szEscapedName, escapedNameLength);
    szObjectString[1 + escapedNameLength] = '"';
    szObjectString[2 + escapedNameLength] = '\0';
    *pAddedCharacters = 2 + escapedNameLength;
  }
  if(szEscapedName != szFixSizeBuffer)
  {
    free(szEscapedName);
  }

  return status;
}


statusCode_t JsonObjectStringWriteStringValue(char * const szObjectString,
                                              char const * const szName,
                                              char const * const szValue,
                                              size_t const freeBufferSize,
                                              size_t * const pAddedCharacters)
{
  statusCode_t status = CTUTIL_SUCCESS;

  size_t const nameLength = strlen(szName);
  size_t const valueLength = strlen(szValue);
  // Checks only minimum needed buffer space
  //                   "   name         "   :   _   "   value         "  \0
  if(freeBufferSize < (1 + nameLength + 1 + 1 + 1 + 1 + valueLength + 1 + 1))
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    size_t freeBufferSizeLeft = freeBufferSize;
    char * szObjectStringPos = szObjectString;
    *pAddedCharacters = 0;

    // Write name
    {
      size_t addedCharacters = 0;
      status = JsonObjectStringWriteName(szObjectStringPos, szName, freeBufferSizeLeft, &addedCharacters);
      if(ctutil_IsStatusOk(status))
      {
        freeBufferSizeLeft -= addedCharacters;
        szObjectStringPos  += addedCharacters;
        *pAddedCharacters  += addedCharacters;
      }
    }

    // Write separator
    if(ctutil_IsStatusOk(status))
    {
      size_t addedCharacters = 0;
      status = JsonObjectStringWriteNameValueSeparator(szObjectStringPos, freeBufferSizeLeft, &addedCharacters);
      if(ctutil_IsStatusOk(status))
      {
        freeBufferSizeLeft -= addedCharacters;
        szObjectStringPos  += addedCharacters;
        *pAddedCharacters  += addedCharacters;
      }
    }

    // Write string value
    if(ctutil_IsStatusOk(status))
    {
      // Value string may contain characters invalid as JSON string
      char szFixEscapeBuffer[128];
      char * szEscapedValue = szFixEscapeBuffer;
      size_t escapeBufferSize = sizeof(szFixEscapeBuffer);
      size_t const worstCaseSize = (valueLength * 2) + 1;
      if(escapeBufferSize <= worstCaseSize)
      {
        // Fixed size buffer maybe not long enough, allocate memory for worst case
        szEscapedValue = malloc(worstCaseSize);
        escapeBufferSize = worstCaseSize;
      }
      EscapeJsonStringValue(szEscapedValue, escapeBufferSize, szValue, NULL);
      size_t const escapedValueLength = strlen(szEscapedValue);
      szObjectStringPos[0] = '"';
      strncpy(&(szObjectStringPos[1]), szEscapedValue, escapedValueLength);
      szObjectStringPos[1 + escapedValueLength] = '"';
      size_t const addedCharacters = 1 + escapedValueLength + 1;
      freeBufferSizeLeft -= addedCharacters;
      szObjectStringPos  += addedCharacters;
      *pAddedCharacters  += addedCharacters;
      if(szEscapedValue != szFixEscapeBuffer)
      {
        free(szEscapedValue);
      }
    }

    // Write terminator
    szObjectStringPos[0] = '\0';
  }

  return status;
}


statusCode_t JsonObjectStringWriteSizeValue(char * const szObjectString,
                                            char const * const szName,
                                            size_t const value,
                                            size_t const freeBufferSize,
                                            size_t * const pAddedCharacters)
{
  statusCode_t status = CTUTIL_SUCCESS;

  size_t const nameLength = strlen(szName);
  // Checks only minimum needed buffer space
  //                   "   name         "   :   _   value  \0
  if(freeBufferSize < (1 + nameLength + 1 + 1 + 1 + 1     + 1))
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    size_t freeBufferSizeLeft = freeBufferSize;
    char * szObjectStringPos = szObjectString;
    *pAddedCharacters = 0;

    // Write name
    {
      size_t addedCharacters = 0;
      status = JsonObjectStringWriteName(szObjectStringPos, szName, freeBufferSizeLeft, &addedCharacters);
      if(ctutil_IsStatusOk(status))
      {
        freeBufferSizeLeft -= addedCharacters;
        szObjectStringPos  += addedCharacters;
        *pAddedCharacters  += addedCharacters;
      }
    }

    // Write separator
    if(ctutil_IsStatusOk(status))
    {
      size_t addedCharacters = 0;
      status = JsonObjectStringWriteNameValueSeparator(szObjectStringPos, freeBufferSizeLeft, &addedCharacters);
      if(ctutil_IsStatusOk(status))
      {
        freeBufferSizeLeft -= addedCharacters;
        szObjectStringPos  += addedCharacters;
        *pAddedCharacters  += addedCharacters;
      }
    }

    // Write value
    if(ctutil_IsStatusOk(status))
    {
      int const printResult = snprintf(szObjectStringPos, freeBufferSizeLeft, "%u", value);
      if(    (printResult < 0)
          || (((size_t)printResult) >= freeBufferSizeLeft))
      {
        status = CTUTIL_FAILED;
        *pAddedCharacters = 0;
        szObjectString[0] = '\0';
      }
      else
      {
        WC_STATIC_ASSERT(SIZE_MAX >= INT_MAX);
        size_t const addedCharacters = (size_t)printResult;
        freeBufferSizeLeft -= addedCharacters;
        szObjectStringPos  += addedCharacters;
        *pAddedCharacters  += addedCharacters;
      }
    }

    // Write terminator
    szObjectStringPos[0] = '\0';
  }

  return status;
}


statusCode_t JsonObjectStringWriteUInt64Value(char * const szObjectString,
                                              char const * const szName,
                                              uint64_t const value,
                                              size_t const freeBufferSize,
                                              size_t * const pAddedCharacters)
{
  statusCode_t status = CTUTIL_SUCCESS;

  size_t const nameLength = strlen(szName);
  // Checks only minimum needed buffer space
  //                   "   name         "   :   _   value  \0
  if(freeBufferSize < (1 + nameLength + 1 + 1 + 1 + 1     + 1))
  {
    status = CTUTIL_FAILED;
  }
  else
  {
    size_t freeBufferSizeLeft = freeBufferSize;
    char * szObjectStringPos = szObjectString;
    *pAddedCharacters = 0;

    // Write name
    {
      size_t addedCharacters = 0;
      status = JsonObjectStringWriteName(szObjectStringPos, szName, freeBufferSizeLeft, &addedCharacters);
      if(ctutil_IsStatusOk(status))
      {
        freeBufferSizeLeft -= addedCharacters;
        szObjectStringPos  += addedCharacters;
        *pAddedCharacters  += addedCharacters;
      }
    }

    // Write separator
    if(ctutil_IsStatusOk(status))
    {
      size_t addedCharacters = 0;
      status = JsonObjectStringWriteNameValueSeparator(szObjectStringPos, freeBufferSizeLeft, &addedCharacters);
      if(ctutil_IsStatusOk(status))
      {
        freeBufferSizeLeft -= addedCharacters;
        szObjectStringPos  += addedCharacters;
        *pAddedCharacters  += addedCharacters;
      }
    }

    // Write value
    if(ctutil_IsStatusOk(status))
    {
      int const printResult = snprintf(szObjectStringPos, freeBufferSizeLeft, "%" PRIu64, value);
      if(    (printResult < 0)
          || (((size_t)printResult) >= freeBufferSizeLeft))
      {
        status = CTUTIL_FAILED;
        *pAddedCharacters = 0;
        szObjectString[0] = '\0';
      }
      else
      {
        WC_STATIC_ASSERT(SIZE_MAX >= INT_MAX);
        size_t const addedCharacters = (size_t)printResult;
        freeBufferSizeLeft -= addedCharacters;
        szObjectStringPos  += addedCharacters;
        *pAddedCharacters  += addedCharacters;
      }
    }

    // Write terminator
    szObjectStringPos[0] = '\0';
  }

  return status;
}

//---- End of source file ------------------------------------------------------

