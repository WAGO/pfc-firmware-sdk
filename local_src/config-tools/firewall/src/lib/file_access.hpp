//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     file_access.hpp
///
///  \brief    Provides file accesses functionality.
///
///  \author   WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef FILE_ACCESS_HPP_
#define FILE_ACCESS_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "xmlhlp.hpp"
#include <iostream>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
namespace wago
{

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Returns default path name to configuration file.
///
/// \param name name of type of configuration, e.g. ebtables, iptables, wbm, etc.
/// \return path to cofiguration file
/// \throws std::invalid_argument if a given name isn't recognized
//------------------------------------------------------------------------------
std::string get_config_fname(const std::string& name);

//------------------------------------------------------------------------------
/// Prints content of a text file onto given output stream.
/// \param fname path to file which should be printed
/// \param out output stream to which the file should be dropped
/// \throw std::invalid_argument if file can't be opened
//------------------------------------------------------------------------------
void print_file(const std::string& fname, std::ostream& out = std::cout);

//------------------------------------------------------------------------------
/// Reads and parses xml file.
/// \param conf xml file name
/// \param stdio if false 'conf' file will be read, if true standard input is used
/// \return xml document or empty document if failed to read/parse
//------------------------------------------------------------------------------
xmldoc read_configuration(const std::string& conf, const bool stdio);

//------------------------------------------------------------------------------
/// Stores xml document.
/// \param conf xml file name
/// \param stdio if false 'conf' file will be written to, if true standard output is used
//------------------------------------------------------------------------------
void store_configuration(const std::string& conf, const bool stdio, xmldoc& doc);

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

} // namespace wago

#endif /* FILE_ACCESS_HPP_ */
//---- End of source file ------------------------------------------------------

