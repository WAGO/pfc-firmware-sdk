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
///  \file     test_PreprocessingMod.c
///
///  \brief    Test module for preprocessing helper macros (C).
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "../test_Preprocessing.h"
#include "test_PreprocessingMod.h"

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
int GetC_3_5(void)
{
  return (3+5);
}


char const * GetCStr_3_5(void)
{
  return WC_STR(3+5);
}


char const * GetCStr_3_FIVE(void)
{
  return WC_STR(3+FIVE);
}


char const * GetCSubstStr_3_FIVE(void)
{
  return WC_SUBST_STR(3+FIVE);
}


int GetCGlue_FI_VE(void)
{
  return WC_GLUE(FI, VE);
}


int GetCSubstGlue_FIVE_FIRST_SECOND(void)
{
  return WC_SUBST_AND_GLUE(FIVE_FIRST, FIVE_SECOND);
}


//---- End of source file ------------------------------------------------------

