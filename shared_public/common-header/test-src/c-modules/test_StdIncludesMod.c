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
///  \file     test_StdIncludesMod.c
///
///  \brief    Test module for standard includes (C).
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "test_StdIncludesMod.h"

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
uint8_t GetUint8MaxC(void)
{
  return UINT8_MAX;
}


int_fast64_t GetFastInt64MaxC(void)
{
  return INT_FAST64_MAX;
}


size_t GetPtrDiffSizeC(void)
{
  return sizeof(ptrdiff_t);
}


bool GetTrueC(void)
{
  return true;
}


bool GetFalseC(void)
{
  return false;
}


//---- End of source file ------------------------------------------------------

