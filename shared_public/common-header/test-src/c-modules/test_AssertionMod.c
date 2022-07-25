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
///  \file     test_AssertionMod.c
///
///  \brief    Test module for assertion helper macros (C).
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "../test_Assertion.h"
#include "test_AssertionMod.h"

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
void TriggerSuccessC(void)
{
  WC_ASSERT(ONE==ONE); //lint -e 506 Constant value Boolean
}


void TriggerFailC(void)
{
  WC_ASSERT(ONE==ZERO); //lint -e 506 Constant value Boolean
}


int TriggerSuccessReturnC(int const value)
{
  WC_ASSERT_RETURN(ONE==ONE, value); //lint -e 506 Constant value Boolean
  return -1;
}


int TriggerFailReturnC(int const value)
{
  WC_ASSERT_RETURN(ONE==ZERO, value); //lint -e 506 Constant value Boolean
  return -1;
}


void TriggerSuccessReturnVoidC(void)
{
  WC_ASSERT_RETURN_VOID(ONE==ONE); //lint -e 506 Constant value Boolean
}


void TriggerFailReturnVoidC(void)
{
  WC_ASSERT_RETURN_VOID(ONE==ZERO); //lint -e 506 Constant value Boolean
}


//---- End of source file ------------------------------------------------------

