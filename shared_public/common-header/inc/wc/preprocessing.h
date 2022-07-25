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
///  \file     preprocessing.h
///
///  \brief    Preprocessor helper macros.
///
///  \author   OELH: WAGO GmbH & Co. KG
///  \author   PEn : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_WC_PREPROCESSING_H_
#define INC_WC_PREPROCESSING_H_

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

/// \def WC_STR(x)
/// Helper macro makes a string of an expression.
//lint -estring(961, WC_STR) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                           '#/##' operator used in macro
//lint -estring(960, WC_STR) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                           disallowed definition for macro
#define WC_STR(x) #x

/// \def WC_SUBST_STR(x)
/// Helper macro makes a string of an expression, after value substitution.
//lint -estring(961, WC_SUBST_STR) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                 function-like macro defined
//lint -estring(960, WC_SUBST_STR) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                 disallowed definition for macro
#define WC_SUBST_STR(x) WC_STR(x)

/// \def WC_GLUE(a, b)
/// Helper macro glues two expressions to one.
//lint -estring(961, WC_GLUE) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                            '#/##' operator used in macro
//lint -estring(960, WC_GLUE) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                            disallowed definition for macro
#define WC_GLUE(a, b) a##b

/// \def WC_SUBST_AND_GLUE(a, b)
/// Helper macro glues two expressions to one, after value substitution.
//lint -estring(961, WC_SUBST_AND_GLUE) to disable Rule 19.7 it is necessary to disable all 961 messages,
//                                      function-like macro defined
//lint -estring(960, WC_SUBST_AND_GLUE) to disable Rule 19.4 it is necessary to disable all 960 messages,
//                                      disallowed definition for macro
#define WC_SUBST_AND_GLUE(a, b) WC_GLUE(a, b)


//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* INC_WC_PREPROCESSING_H_ */
//---- End of source file ------------------------------------------------------

