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
///  \file     api_export.h
///
///  \brief    API export define.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_WC_API_EXPORT_H_
#define INC_WC_API_EXPORT_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/compiler.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// Define to set public function/method/class visibilty (export symbols).
///
/// This macro is **not** intended to be used directly in front of your function/method or class.
/// Instead of using it directly declare your project specific define (e. g. *MYLIB_API*) as empty define
/// when not yet defined:
///
/// > \#ifndef MYLIB\_API\n
/// > \#define MYLIB\_API\n
/// > \#endif
///
/// > MYLIB\_API int MyFunc(int param);
///
/// To publish the marked API declare your project specific define to be equal to *WC_API_EXPORT*
/// in front of your build process like the following:
///
/// > gcc mylib.c -DMYLIB\_API=WC\_API\_EXPORT
#if defined(__linux__) // Linux
#if GNUC_PREREQ(4, 0) // GCC >= 4.0
#define WC_API_EXPORT __attribute__((visibility("default")))
#else // GCC < 4.0
#error "Compiler not supported yet (to set public visibility)"
#define WC_API_EXPORT
#endif
#else // !Linux
#error "OS not supported yet (to set public visibility)"
#define WC_API_EXPORT
#endif


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


#endif /* INC_WC_API_EXPORT_H_ */
//---- End of source file ------------------------------------------------------

