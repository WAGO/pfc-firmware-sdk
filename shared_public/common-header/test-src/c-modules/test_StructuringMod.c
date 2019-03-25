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
///  \file     test_StructuringMod.c
///
///  \brief    Test module for structuring helper macros (C).
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "test_StructuringMod.h"

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
static struct TestStructC stTestContainer = { 7.88L };
static struct TestStructC const stTestContainerR = { 3.22L };
static double const arDoubles[2] = { 1.23L, 4.56L }; //lint -e 551 to avoid error for not accessed

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
size_t BreakScopePlainC()
{
  size_t counter = 0U;
  WC_BREAKABLE_SCOPE
  {
    if(counter >= 2)
    {
      break;
    }
    counter++;
  }

  return counter;
}


size_t BreakLoopOnValueMatchPlainC(size_t const value)
{
  size_t counter = 0U;
  WC_LOOP_UNITL_BREAK
  {
    if(counter == value)
    {
      break;
    }
    counter++;
  }

  return counter;
}


size_t GetSizeOfMemberPlainC()
{
  return sizeof(stTestContainer.doubleMember);
}


size_t GetSizeOfMemberMacroC()
{
  return WC_SIZEOF_MEMBER(struct TestStructC, doubleMember);
}


size_t GetSizeOfElementPlainC()
{
  return sizeof(double);
}


size_t GetSizeOfElementMacroC()
{
  return WC_SIZEOF_ELEMENT(arDoubles);
}


void * GetContainerAddressPlainC()
{
  return &stTestContainer;
}


void * GetContainerAddressMacroC()
{
  return WC_CONTAINER_OF(struct TestStructC, doubleMember, &(stTestContainer.doubleMember));
}


void const * GetContainerAddressReadonlyPlainC()
{
  return &stTestContainerR;
}


void const * GetContainerAddressReadonlyMacroC()
{
  return WC_CONTAINER_OF_R(struct TestStructC, doubleMember, &(stTestContainerR.doubleMember));
}


//---- End of source file ------------------------------------------------------

