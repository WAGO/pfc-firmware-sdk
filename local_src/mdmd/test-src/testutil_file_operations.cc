//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project mdmd (PTXdist package mdmd).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     testutil_file_operations.cc
///
///  \brief    File operation util function implementations for google test.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "testutil/file_operations.hpp"
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

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
void testutil_CreateFile(char const * const szFilePath)
{
  ::std::ofstream outfile;
  outfile.open(szFilePath, std::ofstream::out);
  outfile.flush();
  outfile.close();

  return;
}

std::string testutil_CreateTempDir(char const * szPrefix)
{
  std::string dirTemplate = std::string("/tmp/") + szPrefix + "XXXXXX";
  char * szTemplate = strdup(dirTemplate.c_str());
  std::string result = mkdtemp(szTemplate);
  free(szTemplate);

  return result;
}

bool testutil_RemoveFile(char const * const szFilePath)
{
  int const unlinkResult = unlink(szFilePath);

  return (unlinkResult == 0);
  return true;
}

bool testutil_RemoveDir(char const * const szPath)
{
  int result = rmdir(szPath);
  return (0 == result);
}

void testutil_AppendToFile(char const * const szFilePath,
                           uint8_t const * const arByteArray,
                           size_t const bytesToWrite)
{
  ::std::ofstream outfile;
  outfile.open(szFilePath, std::ofstream::out | std::ofstream::app);
  for(size_t i = 0; i < bytesToWrite; ++i)
  {
    outfile << arByteArray[i];
  }
  outfile.flush();
  outfile.close();

  return;
}


char const * testutil_ReadFromFile(char const * const szFilePath,
                                   ::std::string &fileBuffer)
{
  ::std::ifstream infile(szFilePath);
  ::std::ostringstream fileContent;
  copy(::std::istreambuf_iterator<char>(infile),
       ::std::istreambuf_iterator<char>(),
       ::std::ostreambuf_iterator<char>(fileContent));
  fileBuffer = fileContent.str();

  return fileBuffer.c_str();
}


char const * testutil_GetFirstMatchingLineFromFile(char const * const szFilePath,
                                                   char const * const szPattern,
                                                   ::std::string &lineBuffer)
{
  char const * szResult = NULL;

  ::std::ifstream infile(szFilePath);
  ::std::string line;
  while(::std::getline(infile, lineBuffer))
  {
    if(lineBuffer.find(szPattern) != ::std::string::npos)
    {
      szResult = lineBuffer.c_str();
      break;
    }
  }

  return szResult;
}


//---- End of source file ------------------------------------------------------

