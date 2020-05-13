//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_utils.cpp
///
///  \brief    Implementation of test utils
///
///  \author   WF : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "test_utils.hpp"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace wago {

std::string TestUtils::create_temp_dir(std::string const &prefix){
  std::string tempDirTemplate = std::string("/tmp/") + prefix + "XXXXXX";
  char *szTemplate = strdup(tempDirTemplate.c_str());

  std::string result = mkdtemp(szTemplate);
  free(szTemplate);
  return result;
}

void TestUtils::create_dir(std::string const &name){

  fs::path dir = name;
  fs::create_directories(dir);
}

void TestUtils::remove_dir(std::string const &name){
  fs::path file_path = name;
  if (fs::is_directory(file_path)) {
    fs::remove_all(file_path);
  } else {
    fs::remove(file_path);
  }
}

bool TestUtils::file_exists(std::string const &name){

  fs::path file_path = name;
  return fs::exists(file_path);
}

void TestUtils::copy_file(std::string const &source, std::string const &target){
  fs::path from = source;
  fs::path to = target;
  fs::copy_file(from, to, fs::copy_option::overwrite_if_exists);
}

void TestUtils::read_from_file(const ::std::string &file_path, ::std::string &data){
  ::std::ifstream stream(file_path);

  if (stream.good()) {
    data = ::std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    stream.close();

  } else {
    data = nullptr;
  }
}

void TestUtils::write_to_file(const ::std::string &file_path, const ::std::string &data){

  ::std::ofstream stream(file_path);

  if (stream.good()) {

    stream << data;
    stream.flush();
    stream.close();

    sync();
  }
}

size_t TestUtils::get_line_count(const ::std::string &file_path){
  size_t number_of_lines = 0;
  std::string line;
  std::ifstream file(file_path);

  while (std::getline(file, line)) {
    number_of_lines++;
  }

  return number_of_lines;
}

bool TestUtils::content_is_equal(const ::std::string &file_path1, const ::std::string &file_path2){
  fs::path p1(file_path1);
  fs::path p2(file_path2);

  if (!fs::is_regular_file(p1) || !fs::is_regular_file(p2)) {
    return false;
  }

  ::std::string content1;
  ::std::string content2;

  read_from_file(file_path1, content1);
  read_from_file(file_path2, content2);

  return content1 == content2;
}

bool TestUtils::contains(::std::vector<::std::string> vector, ::std::string value) {
  auto it = ::std::find(vector.begin(), vector.end(), value);
  return it != vector.end();
}

}
