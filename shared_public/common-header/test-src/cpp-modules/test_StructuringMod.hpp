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
///  \file     test_StructuringMod.hpp
///
///  \brief    Test module for structuring helper macros (C++).
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_SRC_CPP_MODULES_TEST_STRUCTURINGMOD_HPP_
#define TEST_SRC_CPP_MODULES_TEST_STRUCTURINGMOD_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "wc/std_includes.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
struct TestStructCPP
{
  double doubleMember;
};

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  size_t BreakScopePlainCPP(void);
  size_t BreakLoopOnValueMatchPlainCPP(size_t const value);
  size_t GetSizeOfMemberPlainCPP(void);
  size_t GetSizeOfMemberMacroCPP(void);
  size_t GetSizeOfElementPlainCPP(void);
  size_t GetSizeOfElementMacroCPP(void);
  void * GetContainerAddressPlainCPP(void);
  void * GetContainerAddressMacroCPP(void);
  void const * GetContainerAddressReadonlyPlainCPP(void);
  void const * GetContainerAddressReadonlyMacroCPP(void);

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


#endif /* TEST_SRC_CPP_MODULES_TEST_STRUCTURINGMOD_HPP_ */
//---- End of source file ------------------------------------------------------

