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
///  \file     test_Structuring.cc
///
///  \brief    Test for structuring helper macros.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "c-modules/test_StructuringMod.h"
#include "cpp-modules/test_StructuringMod.hpp"

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
TEST(Structuring, BreakableScopeC)
{
  ASSERT_EQ(1U, BreakScopePlainC());
}


TEST(Structuring, InfiniteLoopC)
{
  size_t const value = 42U;
  ASSERT_EQ(value, BreakLoopOnValueMatchPlainC(value));
}


TEST(Structuring, MemberSizeC)
{
  ASSERT_EQ(GetSizeOfMemberPlainC(), GetSizeOfMemberMacroC());
}


TEST(Structuring, ElementSizeC)
{
  ASSERT_EQ(GetSizeOfElementPlainC(), GetSizeOfElementMacroC());
}


TEST(Structuring, ArrayToPointerC)
{
  int test_array[8];
  ASSERT_EQ(GetArrayPointerPlainC(test_array), GetArrayPointerMacroC(test_array));
}


TEST(Structuring, ContainerFromMemberC)
{
  ASSERT_EQ(GetContainerAddressPlainC(), GetContainerAddressMacroC());
}


TEST(Structuring, ConstContainerFromMemberC)
{
  ASSERT_EQ(GetContainerAddressReadonlyPlainC(), GetContainerAddressReadonlyMacroC());
}


TEST(Structuring, BreakableScopeCPP)
{
  ASSERT_EQ(1U, BreakScopePlainCPP());
}


TEST(Structuring, InfiniteLoopCPP)
{
  size_t const value = 53U;
  ASSERT_EQ(value, BreakLoopOnValueMatchPlainCPP(value));
}


TEST(Structuring, MemberSizeCPP)
{
  ASSERT_EQ(GetSizeOfMemberPlainCPP(), GetSizeOfMemberMacroCPP());
}


TEST(Structuring, ElementSizeCPP)
{
  ASSERT_EQ(GetSizeOfElementPlainCPP(), GetSizeOfElementMacroCPP());
}


TEST(Structuring, ArrayToPointerCPP)
{
  int test_array[8];
  ASSERT_EQ(GetArrayPointerPlainCPP(test_array), GetArrayPointerMacroCPP(test_array));
}


TEST(Structuring, ContainerFromMemberCPP)
{
  ASSERT_EQ(GetContainerAddressPlainCPP(), GetContainerAddressMacroCPP());
}


TEST(Structuring, ConstContainerFromMemberCPP)
{
  ASSERT_EQ(GetContainerAddressReadonlyPlainCPP(), GetContainerAddressReadonlyMacroCPP());
}


//---- End of source file ------------------------------------------------------

