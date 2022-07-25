//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "rule_file_editor.hpp"
#include "error.hpp"

#include <fstream>
#include <sys/stat.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace wago {
namespace firewall {

namespace {
void write(const ::std::string &file_path, const ::std::vector<::std::string> &lines) {

  ::std::string file_path_tmp = file_path + ".tmp";


  ::std::ofstream stream(file_path_tmp);

  if (stream.good()) {
    auto umask_previous = umask(0022);

    stream << boost::algorithm::join(lines, "\n") << "\n";
    stream.flush();
    stream.close();

    sync();
    ::std::rename(file_path_tmp.c_str(), file_path.c_str());
    sync();

    umask(umask_previous);

  } else {
    throw file_write_error(file_path);
  }
}

void read(const ::std::string &file_path, ::std::vector<::std::string> &lines) {
  ::std::ifstream stream(file_path);
  if (stream.good()) {
    auto data = ::std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    stream.close();

    boost::split(lines, data, boost::is_any_of("\n"), boost::token_compress_off);
    // Add an empty string to account for missing new lines during subsequent line processing.
    lines.push_back("");

  } else {
    throw file_open_error(file_path);
  }

}

void remove_empty_lines(::std::vector<::std::string> &lines) {

  auto end = ::std::remove_if(lines.begin(), lines.end(), [](auto &l) {
    return l.empty();
  });
  lines.erase(end, lines.end());
}

} // anonymous namespace


void RuleFileEditor::remove_duplicate_lines(const ::std::string &rule_file) const {

  ::std::vector<std::string> lines;
  read(rule_file, lines);

  auto size = lines.size();

  remove_empty_lines(lines);

  auto end = lines.end();
  for (auto it = lines.begin(); it != lines.end(); it++) {
    if(it->find("COMMIT") != ::std::string::npos) {
      // Do not remove any COMMIT
      continue;
    }
    end = ::std::remove(it + 1, end, *it);
  }

  lines.erase(end, lines.end());

  if (size != lines.size()) {
    write(rule_file, lines);
  }

}

} // namespace firewall
} // namespace wago
