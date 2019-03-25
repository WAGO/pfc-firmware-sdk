//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project common-header (PTXdist package libcommonheader).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_StdIncludesMod.cc
///
///  \brief    Test module for standard includes (C++).
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "test_StdIncludesMod.hpp"

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
uint8_t GetUint8MaxCPP(void)
{
  return UINT8_MAX;
};


int_fast64_t GetFastInt64MaxCPP(void)
{
  return INT_FAST64_MAX;
};


size_t GetPtrDiffSizeCPP(void)
{
  return sizeof(ptrdiff_t);
};


bool GetTrueCPP(void)
{
  return true;
};


bool GetFalseCPP(void)
{
  return false;
};


//---- End of source file ------------------------------------------------------

