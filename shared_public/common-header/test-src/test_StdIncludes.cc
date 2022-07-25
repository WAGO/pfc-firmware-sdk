//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project common-header (PTXdist package libcommonheader).
//
// Copyright (c) 2017-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_StdIncludes.cc
///
///  \brief    Test for standard includes.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "c-modules/test_StdIncludesMod.h"
#include "cpp-modules/test_StdIncludesMod.hpp"

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
TEST(StdIncludes, stdintC)
{
  ASSERT_EQ(UINT8_MAX, GetUint8MaxC());
  ASSERT_EQ(INT_FAST64_MAX, GetFastInt64MaxC());
}


TEST(StdIncludes, stddefC)
{
  ASSERT_EQ(sizeof(ptrdiff_t), GetPtrDiffSizeC());
}


TEST(StdIncludes, stdboolC)
{
  ASSERT_TRUE(GetTrueC());
  ASSERT_FALSE(GetFalseC());
}


TEST(StdIncludes, stdintCPP)
{
  ASSERT_EQ(UINT8_MAX, GetUint8MaxCPP());
  ASSERT_EQ(INT_FAST64_MAX, GetFastInt64MaxCPP());
}


TEST(StdIncludes, stddefCPP)
{
  ASSERT_EQ(sizeof(ptrdiff_t), GetPtrDiffSizeCPP());
}


TEST(StdIncludes, stdboolCPP)
{
  ASSERT_TRUE(GetTrueCPP());
  ASSERT_FALSE(GetFalseCPP());
}


//---- End of source file ------------------------------------------------------

