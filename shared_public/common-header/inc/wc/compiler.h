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
///  \file     compiler.h
///
///  \brief    Compiler specific helper macros.
///
///  \author   OELH: WAGO Kontakttechnik GmbH & Co. KG
///  \author   PEn : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_WC_COMPILER_H_
#define INC_WC_COMPILER_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

/// \def GNUC_PREREQ(ma, mi)
/// Macro to test if we're using a specific version of gcc or later.
//lint -estring(961, GNUC_PREREQ) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                function-like macro defined
#if defined(__GNUC__) && (!(defined(__INTEL_COMPILER)))
#define GNUC_PREREQ(ma, mi) \
  (   (__GNUC__ > (ma)) \
   || (   (__GNUC__ == (ma)) \
       && (   (__GNUC_MINOR__ > (mi)) \
           || ((__GNUC_MINOR__ == (mi)) && (__GNUC_PATCHLEVEL__ > 0)))))
#else
// We don't use false while expression is used within preprocessor condition-statements
#define GNUC_PREREQ(ma, mi) (0 == 1)
#endif


//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* INC_WC_COMPILER_H_ */
//---- End of source file ------------------------------------------------------

