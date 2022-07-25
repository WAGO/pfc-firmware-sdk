//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
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
///  \author   WF : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef WAGO_FIREWALL_TEST_UTILS_HPP_
#define WAGO_FIREWALL_TEST_UTILS_HPP_

#include <string>
#include <vector>

namespace wago {
namespace firewall {

class TestUtils {

 public:
  TestUtils() = default;
  ~TestUtils() = default;

  static std::string create_temp_dir(std::string const &prefix);

  static void create_dir(std::string const &name);

  static void remove_dir(std::string const &dirname);

  static bool file_exists(std::string const &name);

  static void copy_file(std::string const &source, std::string const &target);

  static void read_from_file(const ::std::string &file_path, ::std::string &data);

  static void write_to_file(const ::std::string &file_path, const ::std::string &data);

  static size_t get_line_count(const ::std::string &file_path);

  static bool is_content_equal(const ::std::string &file_path1, const ::std::string &file_path2);

  static bool contains(::std::vector<::std::string> vector, ::std::string value);
};

} // firewall namespace
} // wago namespace

#endif // WAGO_FIREWALL_TEST_UTILS_HPP_
