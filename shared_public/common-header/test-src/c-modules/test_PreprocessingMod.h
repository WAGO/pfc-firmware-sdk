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
///  \file     test_PreprocessingMod.h
///
///  \brief    Test module for preprocessing helper macros (C).
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_SRC_TEST_PREPROCESSINGMOD_H_
#define TEST_SRC_TEST_PREPROCESSINGMOD_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/preprocessing.h"

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

  int          GetC_3_5(void);
  char const * GetCStr_3_5(void);
  char const * GetCStr_3_FIVE(void);
  char const * GetCSubstStr_3_FIVE(void);
  int          GetCGlue_FI_VE(void);
  int          GetCSubstGlue_FIVE_FIRST_SECOND(void);

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


#endif /* TEST_SRC_TEST_PREPROCESSINGMOD_H_ */
//---- End of source file ------------------------------------------------------

