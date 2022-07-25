//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2019-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_MainFunctionality.cc
///
///  \brief    Tests for config tool reserved_area main functionality.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "ctparts/common_main_defaults.h"
#include "ctparts/ctmain.h"
#include "params.h"
#include "resources.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <cstring>

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
static bool IsFileAvailableDummy(ctutil_OptionsCommon_t const *,
                                 char const *,
                                 char const *)
{
  return false;
}

class MainFunctionalityTest_reserved_area : public ::testing::Test
{
  private:

  protected:
    ctutil_Options_t stOptions;
    ctutil_Resources_t stResources;

  protected:
    MainFunctionalityTest_reserved_area() {};
    virtual ~MainFunctionalityTest_reserved_area() {};
    virtual void SetUp()
    {
      statusCode_t status = CTUTIL_SUCCESS;

      // Prepare options
      memset(&(this->stOptions), 0, sizeof(this->stOptions));
      status = ctparts_SetOptionDefaults(NULL, &(this->stOptions));
      ASSERT_EQ(CTUTIL_SUCCESS, status);

      // Prepare resources
      memset(&(this->stResources), 0, sizeof(this->stResources));
      this->stResources.pstSpecificResources = (ctutil_ResourcesSpecific_t *)malloc(sizeof(ctutil_ResourcesSpecific_t));
      memset(this->stResources.pstSpecificResources, 0, sizeof(*(this->stResources.pstSpecificResources)));
      this->stResources.pstSpecificResources->pfIsFileAvailable = &IsFileAvailableDummy;
    }
    virtual void TearDown()
    {
      // Clean resources and options
      free(this->stResources.pstSpecificResources);
      ctparts_ReleaseSpecificOptions(NULL, &(this->stOptions));
    }
};

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST_F(MainFunctionalityTest_reserved_area, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_F(MainFunctionalityTest_reserved_area, NoOptionQuiet)
{
  char szArg00[] = "test";
  char szArg01[] = "plainOptionShouldBeIgnored";
  char * const arszArgs[]
  {
    szArg00,
    szArg01,
  };
  int const argCount = sizeof(arszArgs) / sizeof(char *);
  this->stOptions.stCommonOptions.quiet = true;

  testing::internal::CaptureStdout();
  exitCode_t const result = ctparts_ctMain(argCount, arszArgs, &(this->stOptions), &(this->stResources));
  EXPECT_STREQ("", testing::internal::GetCapturedStdout().c_str()); // Expected no ASSERT before GetCapturedStdout!
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, result);
}


//---- End of source file ------------------------------------------------------

