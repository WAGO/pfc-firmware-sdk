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
///  \file     test_ErrorCodeHelper.cc
///
///  \brief    Test for error code helper functions.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "ctutil/error_codes.h"
#include <gtest/gtest.h>

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
WC_STATIC_ASSERT(CTUTIL_SUCCESS > CTUTIL_FAILED);
WC_STATIC_ASSERT(CTUTIL_SUCCESS <= (INT_MAX - 100));
WC_STATIC_ASSERT(CTUTIL_FAILED >= (INT_MIN + 100));


TEST(ErrorCodeHelper, StatusOk)
{
  // Check OK codes
  for(statusCode_t statusCode = CTUTIL_SUCCESS; statusCode < (CTUTIL_SUCCESS + 100); ++statusCode)
  {
    EXPECT_TRUE(ctutil_IsStatusOk(statusCode));
  }
  EXPECT_TRUE(ctutil_IsStatusOk(INT_MAX));

  // Check FAILURE codes
  for(statusCode_t statusCode = CTUTIL_FAILED; statusCode > (CTUTIL_FAILED - 100); --statusCode)
  {
    EXPECT_FALSE(ctutil_IsStatusOk(statusCode));
  }
  EXPECT_FALSE(ctutil_IsStatusOk(INT_MIN));

  // Check undefined codes (if any)
  for(statusCode_t statusCode = (CTUTIL_FAILED + 1); statusCode < CTUTIL_SUCCESS; ++statusCode)
  {
    EXPECT_FALSE(ctutil_IsStatusOk(statusCode));
  }
}


TEST(ErrorCodeHelper, StatusFailure)
{

  // Check FAILURE codes
  for(statusCode_t statusCode = CTUTIL_FAILED; statusCode > (CTUTIL_FAILED - 100); --statusCode)
  {
    EXPECT_TRUE(ctutil_IsStatusFailure(statusCode));
  }
  EXPECT_TRUE(ctutil_IsStatusFailure(INT_MIN));

  // Check OK codes
  for(statusCode_t statusCode = CTUTIL_SUCCESS; statusCode < (CTUTIL_SUCCESS + 100); ++statusCode)
  {
    EXPECT_FALSE(ctutil_IsStatusFailure(statusCode));
  }
  EXPECT_FALSE(ctutil_IsStatusFailure(INT_MAX));

  // Check undefined codes (if any)
  for(statusCode_t statusCode = (CTUTIL_FAILED + 1); statusCode < CTUTIL_SUCCESS; ++statusCode)
  {
    EXPECT_FALSE(ctutil_IsStatusFailure(statusCode));
  }
}


TEST(ErrorCodeHelper, StatusCodeMapping)
{
  // Check negative status codes
  for(statusCode_t statusCode = CTUTIL_FAILED; statusCode > ((CTUTIL_EXIT_MAX) * (-1)); --statusCode)
  {
    EXPECT_NE(CTUTIL_EXIT_SUCCESS, ctutil_GetExitCodeFromStatus(statusCode));
  }

  // Check positive status codes
  for(statusCode_t statusCode = CTUTIL_SUCCESS; statusCode < CTUTIL_EXIT_MAX; ++statusCode)
  {
    EXPECT_EQ(CTUTIL_EXIT_SUCCESS, ctutil_GetExitCodeFromStatus(statusCode));
  }

  // Check well known status codes
  EXPECT_EQ(CTUTIL_EXIT_SUCCESS, ctutil_GetExitCodeFromStatus(CTUTIL_SUCCESS));
  EXPECT_EQ(CTUTIL_EXIT_GENERAL_ERROR, ctutil_GetExitCodeFromStatus(CTUTIL_FAILED));
}


//---- End of source file ------------------------------------------------------

