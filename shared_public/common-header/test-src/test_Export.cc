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
///  \file     test_Export.cc
///
///  \brief    Test for API export header.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "c-modules/test_ExportMod.h"
#include "cpp-modules/test_ExportMod.hpp"

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
TEST(Export, ExportedCFunction)
{
  ASSERT_EQ(42, ExportedFunction(42));
}


TEST(Export, NotExportedCFunction)
{
  ASSERT_EQ(53, NotExportedFunction(53));
}


TEST(Export, InheritedExportedCPPMethod)
{
  ExportedTestClass classInstance;
  ASSERT_EQ(64, classInstance.InheritedExportedMethod(64));
}


TEST(Export, ExportedCPPMethod)
{
  TestClassWithExportedMethod classInstance;
  ASSERT_EQ(75, classInstance.ExportedMethod(75));
}


TEST(Export, NotExportedCPPMethod)
{
  TestClassWithExportedMethod classInstance;
  ASSERT_EQ(86, classInstance.NotExportedMethod(86));
}


//---- End of source file ------------------------------------------------------

