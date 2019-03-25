//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_utils.hpp
///
///  \brief    Test Utils
///
///  \author   WF : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TEST_SRC_TEST_UTILS_HPP_
#define TEST_SRC_TEST_UTILS_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <string>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

namespace wago
{

std::string create_temp_dir(std::string const & prefix);

void create_dir(std::string const & name);

void remove_dir(std::string const & dirname);

bool file_exists(std::string const & name);

void copy_file(std::string const & source, std::string const & target);

}

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* TEST_SRC_TEST_UTILS_HPP_ */
//---- End of source file ------------------------------------------------------

