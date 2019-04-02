//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project mdmd (PTXdist package mdmd).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     preinclude.h
///
///  \brief    Ugle preinclude to specify MDMS config file for test purposes.
///
///  \author   WF : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_SRC_PREINCLUDE_H_
#define TEST_SRC_PREINCLUDE_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

#define MDMD_CONFIG_FILE mdmd_test_GetConfigFileName()

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern char const * mdmd_test_GetConfigFileName(void);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* TEST_SRC_PREINCLUDE_H_ */
//---- End of source file ------------------------------------------------------

