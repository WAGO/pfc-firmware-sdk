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
///  \file     std_includes.h
///
///  \brief    Standard includes normally used in a C **and** C++ compatible way.
///
///  \author   OELH: WAGO GmbH & Co. KG
///  \author   PEn : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_WC_STD_INCLUDES_H_
#define INC_WC_STD_INCLUDES_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#ifdef __cplusplus // C++
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#if __cplusplus >= 201103L // C++ >= 201103
#include <cstdint>
#else // C++ < 201103
#include <stdint.h>
#endif // C++ >= 201103 / C++ < 201103
#include <cstddef>
#include <climits>
using std::size_t;
using std::ptrdiff_t;
#else // !C++
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>
#endif // C++ / !C++

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


#endif /* INC_WC_STD_INCLUDES_H_ */
//---- End of source file ------------------------------------------------------

