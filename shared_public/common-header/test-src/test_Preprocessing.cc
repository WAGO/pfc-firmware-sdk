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
///  \file     test_Preprocessing.cc
///
///  \brief    Test for preprocessing helper macros.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "test_Preprocessing.h"
#include "c-modules/test_PreprocessingMod.h"
#include "cpp-modules/test_PreprocessingMod.hpp"

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
// test implementation
//------------------------------------------------------------------------------
TEST(Preprocessing, StringFromExpressionC)
{
  ASSERT_EQ(8, GetC_3_5());
  ASSERT_STREQ("3+5", GetCStr_3_5());
}


TEST(Preprocessing, StringFromSubstitutionC)
{
  ASSERT_STREQ("3+FIVE", GetCStr_3_FIVE());
  ASSERT_STREQ("3+5", GetCSubstStr_3_FIVE());
}


TEST(Preprocessing, GlueC)
{
  ASSERT_EQ(5, GetCGlue_FI_VE());
}


TEST(Preprocessing, SubstitutionGlueC)
{
  ASSERT_EQ(5, GetCSubstGlue_FIVE_FIRST_SECOND());
}


TEST(Preprocessing, StringFromExpressionCPP)
{
  ASSERT_EQ(8, GetCPP_3_5());
  ASSERT_STREQ("3+5", GetCPPStr_3_5());
}


TEST(Preprocessing, StringFromSubstitutionCPP)
{
  ASSERT_STREQ("3+FIVE", GetCPPStr_3_FIVE());
  ASSERT_STREQ("3+5", GetCPPSubstStr_3_FIVE());
}


TEST(Preprocessing, GlueCPP)
{
  ASSERT_EQ(5, GetCPPGlue_FI_VE());
}


TEST(Preprocessing, SubstitutionGlueCPP)
{
  ASSERT_EQ(5, GetCPPSubstGlue_FIVE_FIRST_SECOND());
}


//---- End of source file ------------------------------------------------------

