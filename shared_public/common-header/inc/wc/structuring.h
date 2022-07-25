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
///  \file     structuring.h
///
///  \brief    Helper macros for structuring with member sizes, containers,
///            alignments and so on
///
///  \author   PEn : WAGO GmbH & Co. KG
///  \author   OELH: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_WC_STRUCTURING_H_
#define INC_WC_STRUCTURING_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/compiler.h"
#include "wc/preprocessing.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

/// \def WC_BREAKABLE_SCOPE
/// Defines an breakable scope, maybe exited by a break statement.
#define WC_BREAKABLE_SCOPE for(size_t WC_SUBST_AND_GLUE(scope, __LINE__) = 0U; \
                                      WC_SUBST_AND_GLUE(scope, __LINE__) == 0U;\
                                      WC_SUBST_AND_GLUE(scope, __LINE__)++)

/// \def WC_LOOP_UNITL_BREAK
/// Defines an endless loop, maybe exited by a break statement.
#define WC_LOOP_UNITL_BREAK                                              for(;;)

/// \def WC_SIZEOF_MEMBER(type, member)
/// Returns the size of a structure or public class member.
//lint -estring(961, WC_SIZEOF_MEMBER) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                     function-like macro defined
//lint -estring(960, WC_SIZEOF_MEMBER) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                     disallowed definition for macro
#define WC_SIZEOF_MEMBER(type, member)           (sizeof(((type *)(0))->member))

/// \def WC_SIZEOF_ELEMENT(array)
/// Returns the size of an array member.
//lint -estring(961, WC_SIZEOF_ELEMENT) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                      function-like macro defined
//lint -estring(960, WC_SIZEOF_ELEMENT) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                      disallowed definition for macro
#define WC_SIZEOF_ELEMENT(array)                            (sizeof((array)[0]))

/// \def WC_TYPEOF(variable)
/// Placeholder for the type of variable.
//lint -estring(961, WC_TYPEOF) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                              function-like macro defined
//lint -estring(960, WC_TYPEOF) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                              disallowed definition for macro
#define WC_TYPEOF(variable)
#undef WC_TYPEOF
#ifndef _lint // !Lint
#ifndef __cplusplus // !C++
#if GNUC_PREREQ(2, 0) // GCC >= 2.0
#define WC_TYPEOF(variable)                                     typeof(variable)
#endif
#else // C++
#if (__cplusplus >= 201103L) // C++ >= 201103
#define WC_TYPEOF(variable)                                   decltype(variable)
#else // C++ < 201103
#if GNUC_PREREQ(2, 0) // GCC >= 2.0
#define WC_TYPEOF(variable)                                     typeof(variable)
#endif // GCC >= 2.0
#endif // C++ >= 201103 / C++ < 201103
#endif // !C++ / C++
#if !defined(WC_TYPEOF)
#error "No automatic type declaration supported"
#endif // !defined(WC_TYPEOF)
#else // Lint
#define WC_TYPEOF(variable)                                                 void
#endif // !Lint / Lint

/// \def WC_ARRAY_TO_PTR(array)
/// Returns the pointer of the first array member.
//lint -estring(961, WC_ARRAY_TO_PTR) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                    function-like macro defined
//lint -estring(960, WC_ARRAY_TO_PTR) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                    disallowed definition for macro
#define WC_ARRAY_TO_PTR(array)                                   (&((array)[0]))

/// \def WC_CONTAINER_OF(container, member, pMember)
/// Returns the container pointer for a pointer of its member.
//lint -estring(961, WC_CONTAINER_OF) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                    function-like macro defined
//lint -estring(960, WC_CONTAINER_OF) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                    disallowed definition for macro
#if 0
#define WC_CONTAINER_OF(container, member, pMember) \
  ({ \
    WC_TYPEOF(((container *)0)->member) * const pMemberCopy##__LINE__ = (pMember); \
    (container *)( (char *)pMemberCopy##__LINE__ - offsetof(container, member) ); \
  }) //lint -e 160 to avoid error for sequence "({" (GNU statement expression)
#else
#define WC_CONTAINER_OF(container, member, pMember) \
    (container *)( ((char *)(pMember)) - offsetof(container, member) ) //lint -e 826 to avoid info for pointer area to small
#endif

/// \def WC_CONTAINER_OF_R(container, member, pMember)
/// Returns the readonly container pointer for a pointer of its member.
//lint -estring(961, WC_CONTAINER_OF_R) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                      function-like macro defined
//lint -estring(960, WC_CONTAINER_OF_R) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                      disallowed definition for macro
#if 0
#define WC_CONTAINER_OF_R(container, member, pMember) \
  ({ \
    WC_TYPEOF(((container *)0)->member) const * const pMemberCopy##__LINE__ = (pMember); \
    (container const *)( (char const *)pMemberCopy##__LINE__ - offsetof(container, member) ); \
  }) //lint -e 160 to avoid error for sequence "({" (GNU statement expression)
#else
#define WC_CONTAINER_OF_R(container, member, pMember) \
    (container const *)( ((char const *)(pMember)) - offsetof(container, member) ) //lint -e 826 to avoid info for pointer area to small
#endif

/// \def WC_UNUSED_PARAM(x)
/// Helper macro to explicitly define a function parameter as unused.
#ifdef __cplusplus // C++
// CPP version
#define WC_UNUSED_PARAM(x)
#else // !C++
#ifndef _lint // !Lint
//generic version
#define WC_UNUSED_PARAM(x) \
  WC_SUBST_AND_GLUE(_unused_, x) __attribute__((unused))
#else // Lint
//lint -emacro({715}, WC_UNUSED_PARAM) Not referenced
//lint -emacro({818}, WC_UNUSED_PARAM) Could be declared as pointing to const
//lint -estring(961, WC_UNUSED_PARAM) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                    function-like macro defined
#define WC_UNUSED_PARAM(x) (x)
#endif // !Lint / Lint
#endif // C++ / !C++

/// \def WC_UNUSED_DATA(x)
/// Helper macro to explicitly define a variable as unused.
//lint -estring(961, WC_UNUSED_DATA) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                   function-like macro defined
#define WC_UNUSED_DATA(x) ((void)(x))

/// \def WC_DEPRECATED(func)
/// Helper macro to indicate a deprecated function.
#ifdef __GNUC__
#define WC_DEPRECATED(func) func __attribute__ ((deprecated)) //lint !e960 !e961 No parentheses possible!
#elif defined(_MSC_VER)
#define WC_DEPRECATED(func) __declspec(deprecated) func //lint !e960 !e961 No parentheses possible!
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define WC_DEPRECATED(func) func //lint !e960 !e961 No parentheses possible!
#endif

/// \def WC_INTERFACE_CLASS(x)
/// Helper macro to explicitly create copy, assign, move, move assign, constructor and virtual destructor for a class.
//lint -estring(961, WC_INTERFACE_CLASS) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                       function-like macro defined
#ifdef __cplusplus // C++
#if (__cplusplus >= 201103L) // C++ >= 201103
#define WC_INTERFACE_CLASS(x) \
  protected: \
    x() = default; \
    x(x const &) = default; \
    x(x&&) = default; \
    x& operator=(const x&) = default; \
    x& operator=(x&&) = default; \
  public: \
    virtual ~x() = default; \
  private:
#else // C++ < 201103
#define WC_INTERFACE_CLASS(x) \
  protected: \
    x() {}; \
    x(x const &) {}; \
    x& operator=(const x&) {}; \
  public: \
    virtual ~x() {}; \
  private:
#endif // C++ >= 201103 / C++ < 201103
#else // !C++
#define WC_INTERFACE_CLASS(x)
#endif // C++ / !C++

/// \def WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE_ASSIGN(x)
/// Helper macro to disable copy, assign, move and move assign for a class.
//lint -estring(961, WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE_ASSIGN) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                                                 function-like macro defined
#ifdef __cplusplus // C++
#if (__cplusplus >= 201103L) // C++ >= 201103
#define WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE_ASSIGN(x) \
  x(x const &) = delete; \
  x(x&&) = delete; \
  x& operator=(const x&) = delete; \
  x& operator=(x&&) = delete;
#else // C++ < 201103
#define WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE_ASSIGN(x) \
  private: \
    x(x const &); \
    x& operator=(const x&);
#endif // C++ >= 201103 / C++ < 201103
#else // !C++
#define WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE_ASSIGN(x)
#endif // C++ / !C++

/// \def WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE(x)
/// Helper macro to disable copy, assign and move for a class.
//lint -estring(961, WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                                          function-like macro defined
#ifdef __cplusplus // C++
#if (__cplusplus >= 201103L) // C++ >= 201103
#define WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE(x) \
  x(x const &) = delete; \
  x(x&&) = delete; \
  x& operator=(const x&) = delete;
#else // C++ < 201103
#define WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE(x) \
  private: \
    x(x const &); \
    x& operator=(const x&);
#endif // C++ >= 201103 / C++ < 201103
#else // !C++
#define WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE(x)
#endif // C++ / !C++

/// \def WC_DISBALE_CLASS_COPY_AND_ASSIGN(x)
/// Helper macro to disable copy and assign for a class.
//lint -estring(961, WC_DISBALE_CLASS_COPY_AND_ASSIGN) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                                     function-like macro defined
#ifdef __cplusplus // C++
#if (__cplusplus >= 201103L) // C++ >= 201103
#define WC_DISBALE_CLASS_COPY_AND_ASSIGN(x) \
  x(x const &) = delete; \
  x& operator=(const x&) = delete;
#else // C++ < 201103
#define WC_DISBALE_CLASS_COPY_AND_ASSIGN(x) \
  private: \
    x(x const &); \
    x& operator=(const x&);
#endif // C++ >= 201103 / C++ < 201103
#else // !C++
#define WC_DISBALE_CLASS_COPY_AND_ASSIGN(x)
#endif // C++ / !C++

/// \def WC_DISBALE_CLASS_MOVE_ASSIGN(x)
/// Helper macro to disable move and move assign for a class.
//lint -estring(961, WC_DISBALE_CLASS_MOVE_ASSIGN) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                                 function-like macro defined
#ifdef __cplusplus // C++
#if (__cplusplus >= 201103L) // C++ >= 201103
#define WC_DISBALE_CLASS_MOVE_ASSIGN(x) \
  x(x&&) = delete; \
  x& operator=(x&&) = delete;
#else // C++ < 201103
#define WC_DISBALE_CLASS_MOVE_ASSIGN(x)
#endif // C++ >= 201103 / C++ < 201103
#else // !C++
#define WC_DISBALE_CLASS_MOVE_ASSIGN(x)
#endif // C++ / !C++

/// \def WC_DISBALE_CLASS_MOVE(x)
/// Helper macro to disable move for a class.
//lint -estring(961, WC_DISBALE_CLASS_MOVE) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                          function-like macro defined
#ifdef __cplusplus // C++
#if (__cplusplus >= 201103L) // C++ >= 201103
#define WC_DISBALE_CLASS_MOVE(x) \
  x(x&&) = delete;
#else // C++ < 201103
#define WC_DISBALE_CLASS_MOVE(x)
#endif // C++ >= 201103 / C++ < 201103
#else // !C++
#define WC_DISBALE_CLASS_MOVE(x)
#endif // C++ / !C++


//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* INC_WC_STRUCTURING_H_ */
//---- End of source file ------------------------------------------------------

