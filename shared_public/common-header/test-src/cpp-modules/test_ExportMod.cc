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
///  \file     test_ExportMod.cc
///
///  \brief    Test module for API export header (C++).
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "test_ExportMod.hpp"

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
int ExportedTestClass::InheritedExportedMethod(int const someNumber) const
{
  return someNumber;
}

int TestClassWithExportedMethod::ExportedMethod(int const someNumber) const
{
  return someNumber;
}

int TestClassWithExportedMethod::NotExportedMethod(int const someNumber) const
{
  return someNumber;
}

int TestClassWithExportedMethod::PrivateMethod(int const someNumber) const
{
  return someNumber;
}


//---- End of source file ------------------------------------------------------

