//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     loadavg.c
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "../checkcpu/loadavg.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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

//------------------------------------------------------------------------------
uint32_t loadavg_get(double * load_avg)
{
  uint32_t retVal      = 0;
  int32_t  num_samples = (int32_t)getloadavg(load_avg, 3);

  if (num_samples < 0)
  {
//    fprintf(stderr, "Load averages unobtainable.\n");
    retVal = 2;
  }

  return retVal;
}



//---- End of source file ------------------------------------------------------
