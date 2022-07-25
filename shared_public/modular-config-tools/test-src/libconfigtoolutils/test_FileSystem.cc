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
///  \file     test_FileSystem.cc
///
///  \brief    Tests for file system functions.
///
///  \author   MOe: WAGO GmbH & Co. KG
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctutil/filesystem.h"
#include <gtest/gtest.h>
#include "testutil/file_operations.hpp"

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST(FileSystem, ctutil_CheckForMountPoint)
{
  ctutil_OptionsCommon_t stCommonOptions;
  char const * const szPath = "/tmp";
  statusCode_t status;

  stCommonOptions.quiet = true;

  status = ctutil_CheckForMountPoint(&stCommonOptions, szPath);
  ASSERT_EQ(CTUTIL_SUCCESS, status);
}


TEST(FileSystem, CheckForMountPointEmptyPath)
{
  ctutil_OptionsCommon_t stCommonOptions;
  char const * const szPath = "";

  memset(&stCommonOptions, 0, sizeof(stCommonOptions));
  stCommonOptions.quiet = true;

  statusCode_t status = ctutil_CheckForMountPoint(&stCommonOptions, szPath);
  ASSERT_NE(CTUTIL_SUCCESS, status);
}


TEST(FileSystem, CheckForMountPointAbsolutePath)
{
  ctutil_OptionsCommon_t stCommonOptions;
  char const * const szPath = "tmp";

  memset(&stCommonOptions, 0, sizeof(stCommonOptions));
  stCommonOptions.quiet = true;

  statusCode_t status = ctutil_CheckForMountPoint(&stCommonOptions, szPath);
  ASSERT_NE(CTUTIL_SUCCESS, status);
}


TEST(FileSystem, GetParentFolderStringRoot)
{
  char const * const szPath = "/tmp";

  char szTargetBuffer[256];
  char * szTarget = szTargetBuffer;

  statusCode_t status = ctutil_GetParentFolderString(szPath, strlen(szPath), &szTarget, 256, true);
  ASSERT_EQ(CTUTIL_SUCCESS, status);
  ASSERT_STREQ("/", szTarget);
}


TEST(FileSystem, ctutil_GetParentFolderString)
{
  char const * const szPath = "/tmp/mount";

  char szTargetBuffer[256];
  char * szTarget = szTargetBuffer;

  statusCode_t status = ctutil_GetParentFolderString(szPath, strlen(szPath), &szTarget, 256, true);
  ASSERT_EQ(CTUTIL_SUCCESS, status);
  ASSERT_STREQ("/tmp", szTarget);
}


TEST(FileSystem, IsFileAvailableExistingDirectory)
{
  char const * const szPath = "/tmp";
  ctutil_OptionsCommon_t stCommonOptions;

  memset(&stCommonOptions, 0, sizeof(stCommonOptions));
  stCommonOptions.quiet = true;

  bool fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");

  ASSERT_TRUE(fStatus);
}


TEST(FileSystem, IsFileAvailableMissingDirectory)
{
  char const * const szPath = "/tmp/missing";
  ctutil_OptionsCommon_t stCommonOptions;

  memset(&stCommonOptions, 0, sizeof(stCommonOptions));
  stCommonOptions.quiet = true;

  int const resultSys = system("rm -rf /tmp/missing; sync");
  ASSERT_EQ(0, resultSys);
  bool fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");

  ASSERT_FALSE(fStatus);
}


TEST(FileSystem, ctutil_CreateFolder)
{
  bool fStatus;
  statusCode_t status;
  std::string tempDir = testutil_CreateTempDir("reserved_area_");
  std::string path = tempDir + "/footest";
  char const * const szPath = path.c_str();
  ctutil_OptionsCommon_t stCommonOptions;

  memset(&stCommonOptions, 0, sizeof(stCommonOptions));
  stCommonOptions.quiet = true;

  std::string command = std::string("rm -rf ") + tempDir + "/footest; sync";
  int const resultSys1 = system(command.c_str());
  ASSERT_EQ(0, resultSys1);
  fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");
  ASSERT_FALSE(fStatus);

  status = ctutil_CreateFolder(&stCommonOptions, szPath, false);
  ASSERT_EQ(CTUTIL_SUCCESS, status);

  fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");
  ASSERT_TRUE(fStatus);

  int const resultSys2 = system(command.c_str());
  ASSERT_EQ(0, resultSys2);
  fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");
  ASSERT_FALSE(fStatus);

  ASSERT_TRUE(testutil_RemoveDir(tempDir.c_str()));
}


TEST(FileSystem, ctutil_CreateFolderHierarchy)
{
  bool fStatus;
  statusCode_t status;
  std::string tempDir = testutil_CreateTempDir("reserved_area_");
  std::string path = tempDir + "/foo/bar/baz";
  char const * const szPath = path.c_str();
  ctutil_OptionsCommon_t stCommonOptions;

  memset(&stCommonOptions, 0, sizeof(stCommonOptions));
  stCommonOptions.quiet = true;

  std::string command = std::string("rm -rf ") + tempDir + "/foo; sync";
  int const resultSys1 = system(command.c_str());
  ASSERT_EQ(0, resultSys1);
  fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");
  ASSERT_FALSE(fStatus);

  status = ctutil_CreateFolder(&stCommonOptions, szPath, true);
  ASSERT_EQ(CTUTIL_SUCCESS, status);

  fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");
  ASSERT_TRUE(fStatus);

  int const resultSys2 = system(command.c_str());
  ASSERT_EQ(0, resultSys2);
  fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");
  ASSERT_FALSE(fStatus);

  ASSERT_TRUE(testutil_RemoveDir(tempDir.c_str()));
}


TEST(FileSystem, CreateFolderExists)
{
  bool fStatus;
  statusCode_t status;
  char const * const szPath = "/tmp";
  ctutil_OptionsCommon_t stCommonOptions;

  memset(&stCommonOptions, 0, sizeof(stCommonOptions));
  stCommonOptions.quiet = true;

  fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");
  ASSERT_TRUE(fStatus);

  status = ctutil_CreateFolder(&stCommonOptions, szPath, false);
  ASSERT_EQ(CTUTIL_SUCCESS, status);

  fStatus = ctutil_IsFileAvailable(&stCommonOptions, szPath, "d");
  ASSERT_TRUE(fStatus);
}


//---- End of source file ------------------------------------------------------

