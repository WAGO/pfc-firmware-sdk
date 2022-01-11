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
///  \file     test_StructuringMod.h
///
///  \brief    Test module for structuring helper macros (C).
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_SRC_C_MODULES_TEST_STRUCTURINGMOD_H_
#define TEST_SRC_C_MODULES_TEST_STRUCTURINGMOD_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"
#include "wc/std_includes.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
struct TestStructC
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

  size_t BreakScopePlainC(void);
  size_t BreakLoopOnValueMatchPlainC(size_t const value);
  size_t GetSizeOfMemberPlainC(void);
  size_t GetSizeOfMemberMacroC(void);
  size_t GetSizeOfElementPlainC(void);
  size_t GetSizeOfElementMacroC(void);
  int  * GetArrayPointerPlainC(int array[]);
  int  * GetArrayPointerMacroC(int array[]);
  void * GetContainerAddressPlainC(void);
  void * GetContainerAddressMacroC(void);
  void const * GetContainerAddressReadonlyPlainC(void);
  void const * GetContainerAddressReadonlyMacroC(void);

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


#endif /* TEST_SRC_C_MODULES_TEST_STRUCTURINGMOD_H_ */
//---- End of source file ------------------------------------------------------

