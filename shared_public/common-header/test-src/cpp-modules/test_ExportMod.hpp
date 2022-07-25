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
///  \file     test_ExportMod.hpp
///
///  \brief    Test module for API export header (C++).
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_SRC_TEST_EXPORTMOD_HPP_
#define TEST_SRC_TEST_EXPORTMOD_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/api_export.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
class WC_API_EXPORT ExportedTestClass
{
  public:
    int InheritedExportedMethod(int const someNumber) const;
};


class TestClassWithExportedMethod
{
  public:
    WC_API_EXPORT int ExportedMethod(int const someNumber) const;
    int NotExportedMethod(int const someNumber) const;

  private:
    int PrivateMethod(int const someNumber) const;
};


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* TEST_SRC_TEST_EXPORTMOD_HPP_ */
//---- End of source file ------------------------------------------------------

