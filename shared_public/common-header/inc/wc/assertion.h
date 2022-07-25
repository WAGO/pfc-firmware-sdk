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
///  \file     assertion.h
///
///  \brief    Assertion macros for unified assertion usage.
///
///  \author   OELH: WAGO GmbH & Co. KG
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_WC_ASSERTION_H_
#define INC_WC_ASSERTION_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/compiler.h"
#include "wc/preprocessing.h"
#include "wc/structuring.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
/// Fail reason prefix.
///
/// This prefix is used in front of every fail reason triggered by an assertion.
//------------------------------------------------------------------------------
#define WC_ASSERT_PREFIX   "Assertion failed: "

/// \def WC_FAIL_SOURCE_FILE
/// Internal macro to allow disabling of source information.
/// \note Internal macro, don't use externally.
///
/// \def WC_FAIL_SOURCE_FUNC
/// Internal macro to allow disabling of source information.
/// \note For internal use only. Don't use externally.
///
/// \def WC_FAIL_SOURCE_LINE
/// Internal macro to allow disabling of source information.
/// \note For internal use only. Don't use externally.
// This switch disables source information.
#ifdef DISABLE_WC_FAIL_SOURCE
#define WC_FAIL_SOURCE_FILE "[unknown source]"
#define WC_FAIL_SOURCE_FUNC "[unknown function]"
#define WC_FAIL_SOURCE_LINE (0)
#else
#define WC_FAIL_SOURCE_FILE (WC_ARRAY_TO_PTR(__FILE__))
#define WC_FAIL_SOURCE_FUNC (WC_ARRAY_TO_PTR(__func__))
#define WC_FAIL_SOURCE_LINE (__LINE__)
#endif

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  /// Prototype for user specific fail function to handle failures
  /// (e.g. for a failed assertion).
  ///
  /// \note The function implementation has to be provided application specific.
  ///
  /// \param szReason
  ///   Null terminated string with failure reason.
  /// \param szFile
  ///   Null terminated string with source file name of failure source.
  /// \param szFunction
  ///   Null terminated string with function name of failure source.
  /// \param line
  ///   Line number of failure source in source file.
  //------------------------------------------------------------------------------
  void wc_Fail(char const *szReason,
               char const *szFile,
               char const *szFunction,
               int         line);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

/// \def WC_STATIC_ASSERT(e)
/// Helper macro, proves (constant) expression satisfies true.
/// In case expression evaluates to false a compile-time error is issued.
//lint -estring(960, WC_STATIC_ASSERT) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                     disallowed definition for macro
//lint -estring(961, WC_STATIC_ASSERT) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                     function-like macro defined
#ifndef __cplusplus // !C++
#if (GNUC_PREREQ(4, 6)) || (__STDC_VERSION__ >= 201112L) // GCC >= 4.6 or C >= 201112
#define WC_STATIC_ASSERT(e) \
  __extension__ _Static_assert(e, WC_STR(e))
#endif
#else // C++
#ifndef _lint // !Lint
#if (__cplusplus >= 201103L) // C++ >= 201103
#define WC_STATIC_ASSERT(e) \
  static_assert(e, WC_STR(e))
#else // C++ < 201103
#if GNUC_PREREQ(4, 6) // GCC >= 4.6
#define WC_STATIC_ASSERT(e) \
  __extension__ _Static_assert(e, WC_STR(e))
#endif // GCC >= 4.6
#endif // C++ >= 201103 / C++ < 201103
#endif // !Lint
#endif // !C++ / C++
#if !defined(WC_STATIC_ASSERT)
//lint -emacro(1960, WC_STATIC_ASSERT) Boolean expression used with non-permitted operator: '*'
//lint -emacro(961, WC_STATIC_ASSERT) Boolean expression used with non-permitted operator: '*'
//lint -emacro(944, WC_STATIC_ASSERT) Argument for operator '!' always evaluates to false
//lint -emacro(514, WC_STATIC_ASSERT) Unusual use of a boolean expression
//lint -emacro(506, WC_STATIC_ASSERT) Constant value boolean
//lint -emacro(639, WC_STATIC_ASSERT) Strong type mismatch for type 'bool' in binary operation
//lint -emacro(778, WC_STATIC_ASSERT) Constant expression evaluates to 0 in operation '%'
//lint -esym(751, assertion_failed_*_) Local typedef not referenced
//lint -esym(761, assertion_failed_*_) Redundant typedef
#define WC_STATIC_ASSERT(e) \
  typedef char WC_SUBST_AND_GLUE(assertion_failed_, WC_SUBST_AND_GLUE(__LINE__, _))[1 - (2 * !(e))]
#endif // !defined(WC_STATIC_ASSERT)


/// \def WC_ASSERT(e)
/// Helper macro, proves expression satisfies true.
/// In case expression evaluates to false a run-time error is issued (NDEBUG not set).
///
/// \note To use this macro you have to provide an implementation for the C function \link wc_Fail \endlink.
/// \see wc_Fail
//lint -estring(960, WC_ASSERT) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                              disallowed definition for macro
//lint -estring(961, WC_ASSERT) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                              function-like macro defined
#ifndef NDEBUG
#define WC_ASSERT(e) \
  (e) ? (void)0 : WC_FAIL(WC_ASSERT_PREFIX #e) //lint -e 506 Constant value Boolean
#else // NDEBUG
/// Release version of assert to avoid unused variable warnings
#define WC_ASSERT(e) do { ((void)sizeof(e)); } while(false) //lint -e 506 Constant value Boolean
#endif // NDEBUG


/// \def WC_ASSERT_RETURN(e, result)
/// Helper macro, proves expression satisfies true.
/// In case expression evaluates to false a run-time error is issued (NDEBUG not set)
/// and the given result is returned.
/// This macro is intended for pre-execution checks, e. g. to check function/method parameters.
///
/// \note To use this macro you have to provide an implementation for the C function \link wc_Fail \endlink.
/// \see wc_Fail
//lint -estring(960, WC_ASSERT_RETURN) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                     disallowed definition for macro
//lint -estring(961, WC_ASSERT_RETURN) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                     function-like macro defined
#ifndef NDEBUG
#define WC_ASSERT_RETURN(e, result) \
  (e) ? (void)0 : WC_FAIL(WC_ASSERT_PREFIX #e); if(!(e)) return (result) //lint -e 506 Constant value Boolean
#else // NDEBUG
/// Release version of assert only returning error value
#define WC_ASSERT_RETURN(e, result) if(!(e)) return (result) //lint -e 506 Constant value Boolean
#endif // NDEBUG


/// \def WC_ASSERT_RETURN_VOID(e)
/// Helper macro, proves expression satisfies true.
/// In case expression evaluates to false a run-time error is issued (NDEBUG not set)
/// and function returned.
/// This macro is intended for pre-execution checks, e. g. to check function/method parameters.
///
/// \note To use this macro you have to provide an implementation for the C function \link wc_Fail \endlink.
/// \see wc_Fail
//lint -estring(960, WC_ASSERT_RETURN_VOID) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                          disallowed definition for macro
//lint -estring(961, WC_ASSERT_RETURN_VOID) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                          function-like macro defined
#ifndef NDEBUG
#define WC_ASSERT_RETURN_VOID(e) \
  (e) ? (void)0 : WC_FAIL(WC_ASSERT_PREFIX #e); if(!(e)) return //lint -e 506 Constant value Boolean
#else // NDEBUG
/// Release version of assert only returning error value
#define WC_ASSERT_RETURN_VOID(e) if(!(e)) return //lint -e 506 Constant value Boolean
#endif // NDEBUG


/// \def WC_FAIL(reason)
/// Helper macro, fails every time.
/// A run-time error is issued (NDEBUG not set) and reason string may be shown.
///
/// \note To use this macro you have to provide an implementation for the C function \link wc_Fail \endlink.
/// \see wc_Fail
//lint -estring(960, WC_FAIL) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                            disallowed definition for macro
//lint -estring(961, WC_FAIL) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                            function-like macro defined
#ifndef NDEBUG
#define WC_FAIL(reason) \
  wc_Fail(reason, WC_FAIL_SOURCE_FILE, WC_FAIL_SOURCE_FUNC, WC_FAIL_SOURCE_LINE)
#else // NDEBUG
/// Release version of fail to avoid unused variable warnings
#define WC_FAIL(reason) do { ((void)sizeof(reason)); } while(false) //lint -e 506 Constant value Boolean
#endif // NDEBUG


//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* INC_WC_ASSERTION_H_ */
//---- End of source file ------------------------------------------------------

