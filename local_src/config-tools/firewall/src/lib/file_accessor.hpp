//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
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
///  \author   WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef WAGO_FIREWALL_FILE_ACCESSOR_HPP_
#define WAGO_FIREWALL_FILE_ACCESSOR_HPP_

#include "xmlhlp.hpp"
#include <iostream>

namespace wago {
namespace firewall {

class FileAccessor {

 public:
  FileAccessor();

  explicit FileAccessor(::std::string base_dir);
  ~FileAccessor() = default;
  FileAccessor(const FileAccessor& other) = delete;
  FileAccessor& operator=(const FileAccessor& other) = delete;
  FileAccessor(const FileAccessor&&) = delete;
  FileAccessor& operator=(const FileAccessor&&) = delete;

  //------------------------------------------------------------------------------
  /// Returns default path name to configuration file.
  ///
  /// \param name name of type of configuration, e.g. ebtables, iptables, wbm, etc.
  /// \return path to cofiguration file
  /// \throws std::invalid_argument if a given name isn't recognized
  //------------------------------------------------------------------------------
  std::string get_config_fname(const std::string& name ) const;

  //------------------------------------------------------------------------------
  /// Prints content of a text file onto given output stream.
  /// Output contains interfaces e.g. X1. but not bridges e.g. br0. This function
  /// produces output expected from legacy wbm.
  /// \param fname path to file which should be printed
  /// \param out output stream to which the file should be dropped
  /// \throw std::invalid_argument if file can't be opened
  //------------------------------------------------------------------------------
  void print_file(const std::string& fname, std::ostream& out = std::cout) const;

  //------------------------------------------------------------------------------
  /// Prints content of a text file onto given output stream.
  /// Output contains bridges e.g. br0 but not interfaces e.g. X1.
  /// \param fname path to file which should be printed
  /// \param out output stream to which the file should be dropped
  /// \throw std::invalid_argument if file can't be opened
  //------------------------------------------------------------------------------
  void print_file_ng(const std::string& fname, std::ostream& out = std::cout) const;

  //------------------------------------------------------------------------------
  /// Reads and parses xml file.
  /// \param conf xml file name
  /// \param stdio if false 'conf' file will be read, if true standard input is used
  /// \return xml document or empty document if failed to read/parse
  //------------------------------------------------------------------------------
  xmldoc read_configuration(const std::string& conf, const bool stdio) const;

  //------------------------------------------------------------------------------
  /// Stores xml document.
  /// \param conf xml file name
  /// \param stdio if false 'conf' file will be written to, if true standard output is used
  //------------------------------------------------------------------------------
  void store_configuration(const std::string& conf, const bool stdio, xmldoc& doc) const;

  //------------------------------------------------------------------------------
  /// Copy file.
  /// \param src_file source file name
  /// \param dst_file destination file name
  //------------------------------------------------------------------------------
  void copy_file(const std::string& src_file, const std::string& dst_file) const;

  //------------------------------------------------------------------------------
  /// Check if file exists and not empty.
  /// \param name  name of file to check
  /// \return true if file exists and not empty, otherwise false
  //------------------------------------------------------------------------------
  bool check_file(const std::string& name) const;

 private:
  ::std::string base_dir_;

};

} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_FILE_ACCESSOR_HPP_
