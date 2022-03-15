//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     blackbox.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "@name@_API.h"
#include "first/afolder/source1.h"

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
TEST(basic,true)
{
  ASSERT_TRUE(true);
}


TEST(Blackbox,APIVersion)
{
  ASSERT_EQ(1, @name_c@_GetVersion());
}


TEST(Blackbox,first_Self)
{
  ASSERT_EQ(1, @name_c@_first_Self());
}

#if false
// Compiling this test will result in a linker error, as the referenced function is not part of the shared object's API.
TEST(Whitebox,first_Self)
{
  ASSERT_EQ(1, first_Self());
}
#endif

TEST(Blackbox,second_Self)
{
  ASSERT_EQ(3, @name_c@_second_Self());
}


//---- End of source file ------------------------------------------------------

