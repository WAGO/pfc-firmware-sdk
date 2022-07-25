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
///  \file     compiler.h
///
///  \brief    Compiler specific helper macros.
///
///  \author   OELH: WAGO GmbH & Co. KG
///  \author   PEn : WAGO GmbH & Co. KG
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

/// \def GNUC_ATTRIBUTE(attribute)
/// Macro to add an attribute only for gnu gcc.
//lint -estring(961, GNUC_ATTRIBUTE) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                   function-like macro defined
#if defined(__GNUC__) && (!(defined(__INTEL_COMPILER)))
#define GNUC_ATTRIBUTE(attribute) \
  __attribute__ ((attribute))
#else
#define GNUC_ATTRIBUTE(attribute)
#endif

/// \def GNUC_PRAGMA(pragmatext)
/// Macro to add an pragma only for gnu gcc.
//lint -estring(961, GNUC_PRAGMA) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                function-like macro defined
#if defined(__GNUC__) && (!(defined(__INTEL_COMPILER)))
#define GNUC_PRAGMA(pragmatext) \
  _Pragma (#pragmatext)
#else
#define GNUC_PRAGMA(pragmatext)
#endif

/// \def GNUC_DIAGNOSTIC_ERROR(attribute)
/// Macro to raise an error for a diagnostic attribute only on gnu gcc.
//lint -estring(961, GNUC_DIAGNOSTIC_ERROR) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                          function-like macro defined
#if ( GNUC_PREREQ(4, 7))
#define GNUC_DIAGNOSTIC_ERROR(attribute) \
  GNUC_PRAGMA (GCC diagnostic error attribute)
#else
#define GNUC_DIAGNOSTIC_ERROR(attribute)
#endif

/// \def GNUC_DIAGNOSTIC_WARNING(attribute)
/// Macro to raise a warning for a diagnostic attribute only on gnu gcc.
//lint -estring(961, GNUC_DIAGNOSTIC_WARNING) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                            function-like macro defined
#if ( GNUC_PREREQ(4, 7))
#define GNUC_DIAGNOSTIC_WARNING(attribute) \
  GNUC_PRAGMA (GCC diagnostic warning attribute)
#else
#define GNUC_DIAGNOSTIC_WARNING(attribute)
#endif

/// \def GNUC_DIAGNOSTIC_IGNORE(attribute)
/// Macro to ignore a diagnostic attribute only for gnu gcc.
//lint -estring(961, GNUC_DIAGNOSTIC_IGNORE) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                           function-like macro defined
#if ( GNUC_PREREQ(4, 7))
#define GNUC_DIAGNOSTIC_IGNORE(attribute) \
  GNUC_PRAGMA (GCC diagnostic ignored attribute)
#else
#define GNUC_DIAGNOSTIC_IGNORE(attribute)
#endif

/// \def GNUC_DIAGNOSTIC_PUSH(attribute)
/// Macro to push diagnostic settings only for gnu gcc.
#if ( GNUC_PREREQ(4, 7))
#define GNUC_DIAGNOSTIC_PUSH \
  GNUC_PRAGMA (GCC diagnostic push)
#else
#define GNUC_DIAGNOSTIC_PUSH
#endif

/// \def GNUC_DIAGNOSTIC_POP(attribute)
/// Macro to pop diagnostic settings only for gnu gcc.
#if ( GNUC_PREREQ(4, 7))
#define GNUC_DIAGNOSTIC_POP \
  GNUC_PRAGMA (GCC diagnostic pop)
#else
#define GNUC_DIAGNOSTIC_POP
#endif


//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* INC_WC_COMPILER_H_ */
//---- End of source file ------------------------------------------------------

