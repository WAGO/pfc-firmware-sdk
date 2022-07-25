//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#ifndef WAGO_FILESYS_HPP
#define WAGO_FILESYS_HPP


#include <iostream>
#include <string>


namespace wago {


std::string get_real_path(const std::string& path);

//------------------------------------------------------------------------------
/// Prints content of a text file onto given output stream.
/// \param fname path to file which should be printed
/// \param out output stream to which the file should be dropped
/// \throw std::invalid_argument if file can't be opened
//------------------------------------------------------------------------------
void print_file(const std::string& fname, std::ostream& out = std::cout);


bool file_exists(const std::string& fname);
bool symlink_exists(const std::string& fname);

void remove_file(const std::string& fname, const bool throw_on_fail = true);



} // namespace wago


#endif // WAGO_FILESYS_HPP
