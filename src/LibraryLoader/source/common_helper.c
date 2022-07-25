//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     CommonHelpers.c
///
///  \version  $Revision: 3546 $
///
///  \brief    Internal helper function used throughout the DAL code.
///            Not part of the DAL User API!
///            Source file. 
///
///  \author   <AGa> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "common_helper.h"

#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "LibraryLoader_Logging.h"



//-- Function: safe_calloc -----------------------------------------------------
///
///   calloc wrapper that aborts the execution on error.
///   Allocates count * elsize bytes and fills the result with zeros.
///
/// \param count Number of elements.
/// \param elSize Size of one element.
///
/// \return Pointer to allocated memory.
//------------------------------------------------------------------------------
void *safe_calloc(size_t count,
                  size_t elsize)
{
   void *p = calloc(count, elsize);

   if (NULL == p)
   {
      ERROR("%s", "Fatal allocation error!");
      exit(EXIT_FAILURE);
   }

   return p;
}

