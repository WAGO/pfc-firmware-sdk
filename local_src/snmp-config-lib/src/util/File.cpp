//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "File.hpp"

#include <fstream>
#include <filesystem>

namespace wago::util {

void WriteFile(const ::std::string &path, const ::std::string &data) {
  std::ofstream os(path + ".tmp");
  os << data << ::std::flush;
  os.close();
  std::filesystem::rename(path + ".tmp", path);

}

::std::string ReadFile(const ::std::string &path) {
  std::ifstream t(path);
  return ::std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

}
