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
///  \file     cpputest_mem_leak_quirk.h
///
///  \version  $Revision: 3737 $Rev$ $
///
///  \brief    Defines internal structures and functions of the DAL.
///
///  \author   $Author$ : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef _CPPUTEST_MEM_LEAK_QUIRK_H_
#define _CPPUTEST_MEM_LEAK_QUIRK_H_

#include <stdlib.h>
// WORKAROUND FOR CPPUNIT's MEMORY LEAK DETECTION 
// Cpputest reports a memory leak if it finds a free call without a matching
// malloc.  However, POSIX's scandir() call allocates memory inside an external
// lib and expects the user to free it. Another example is POSIX' strdup()
// function. That's why we use Cpputest's built in wrapper PlatformSpecificFree
// in asymmetric allocation cases when running unit tests.

#ifdef __CPPUTEST

#include "CppUTest/PlatformSpecificFunctions_c.h"

# define __UNMATCHED_FREE(x) PlatformSpecificFree(x)

#else

# define __UNMATCHED_FREE(x) free(x)

#endif

#ifndef __STATIC
#  ifdef _CPPUTEST
#    define __STATIC
#  else
#    define __STATIC static
#  endif
#endif

#endif

