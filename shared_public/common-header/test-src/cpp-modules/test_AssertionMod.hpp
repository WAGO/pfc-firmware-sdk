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
///  \file     test_AssertionMod.hpp
///
///  \brief    Test module for assertion helper macros (C++).
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_SRC_CPP_MODULES_TEST_ASSERTIONMOD_HPP_
#define TEST_SRC_CPP_MODULES_TEST_ASSERTIONMOD_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  void TriggerSuccessCPP();
  void TriggerFailCPP();
  int  TriggerSuccessReturnCPP(int value);
  int  TriggerFailReturnCPP(int value);
  void TriggerSuccessReturnVoidCPP();
  void TriggerFailReturnVoidCPP();

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* TEST_SRC_CPP_MODULES_TEST_ASSERTIONMOD_HPP_ */
//---- End of source file ------------------------------------------------------

