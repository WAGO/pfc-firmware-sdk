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
///  \file     test_AssertionMod.cc
///
///  \brief    Test module for assertion helper macros (C++).
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "../test_Assertion.h"
#include "test_AssertionMod.hpp"

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
void TriggerSuccessCPP(void)
{
  WC_ASSERT(ONE==ONE);
}


void TriggerFailCPP(void)
{
  WC_ASSERT(ONE==ZERO);
}


int TriggerSuccessReturnCPP(int const value)
{
  WC_ASSERT_RETURN(ONE==ONE, value);
  return -1;
}


int TriggerFailReturnCPP(int const value)
{
  WC_ASSERT_RETURN(ONE==ZERO, value);
  return -1;
}


void TriggerSuccessReturnVoidCPP()
{
  WC_ASSERT_RETURN_VOID(ONE==ONE);
}


void TriggerFailReturnVoidCPP()
{
  WC_ASSERT_RETURN_VOID(ONE==ZERO);
}


//---- End of source file ------------------------------------------------------

