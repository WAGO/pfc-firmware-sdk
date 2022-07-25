//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     file_access.cpp
///
///  \brief    Provides file accesses functionality.
///
///  \author   WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "error.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <boost/algorithm/string.hpp>
#include <firewall/src/lib/file_accessor.hpp>

namespace wago {
namespace firewall {

namespace {
void update_entries_for_backward_compatibility(const ::std::string &line, ::std::ostream &out) {

  auto line_tmp = line;

  if (line_tmp.find("if=\"br0\"") != ::std::string::npos) {
    ::boost::algorithm::replace_all(line_tmp, "if=\"br0\"", "if=\"X1\"");
  }

  if (line_tmp.find("if=\"br1\"") != ::std::string::npos) {
    ::boost::algorithm::replace_all(line_tmp, "if=\"br1\"", "if=\"X2\"");
  }

  // forwarding rules
  if (line_tmp.find("if1=\"br0\"") != ::std::string::npos) {
    ::boost::algorithm::replace_all(line_tmp, "if1=\"br0\"", "if1=\"X1\"");
  }

  if (line_tmp.find("if2=\"br1\"") != ::std::string::npos) {
    ::boost::algorithm::replace_all(line_tmp, "if2=\"br1\"", "if2=\"X2\"");
  }

  if (line_tmp.find("if1=\"br1\"") != ::std::string::npos) {
    ::boost::algorithm::replace_all(line_tmp, "if1=\"br1\"", "if1=\"X2\"");
  }

  if (line_tmp.find("if2=\"br0\"") != ::std::string::npos) {
    ::boost::algorithm::replace_all(line_tmp, "if2=\"br0\"", "if2=\"X1\"");
  }

  out << line_tmp << '\n';
}
}

FileAccessor::FileAccessor()
    :
    FileAccessor { "" } {
}

FileAccessor::FileAccessor(::std::string base_dir)
    :
    base_dir_ { base_dir } {
}

std::string FileAccessor::get_config_fname(const std::string &name) const {
  if ("ebtables" == name) {
    return base_dir_ + "/etc/firewall/ebtables/ebwlist.xml";
  } else if ("iptables" == name) {
    return base_dir_ + "/etc/firewall/iptables/ipcmn.xml";
  } else if ("params" == name) {
    return base_dir_ + "/etc/firewall/params.xml";
  } else if ("params_gen" == name) {
    return base_dir_ + "/etc/firewall/params_gen.xml";
  } else if ("services" == name) {
    return base_dir_ + "/etc/firewall/services/";
  } else {
    return base_dir_ + "/etc/firewall/services/" + name + ".xml";
  }
}

void FileAccessor::print_file(const ::std::string &fname, ::std::ostream &out) const {
  ::std::ifstream file(fname);
  ::std::string line;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      //out << line << '\n';
      update_entries_for_backward_compatibility(line, out);
    }

    file.close();
  } else {
    throw invalid_param_error("Couldn't open requested file.");
  }
}

void FileAccessor::print_file_ng(const ::std::string &fname, ::std::ostream &out) const {
  ::std::ifstream file(fname);
  ::std::string line;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      out << line << '\n';
    }

    file.close();
  } else {
    throw invalid_param_error("Couldn't open requested file.");
  }
}

xmldoc FileAccessor::read_configuration(const std::string &conf, const bool stdio) const {
  xmldoc doc;

  if (stdio) {
    std::ostringstream oss;
    std::string line;

    while (std::getline(std::cin, line)) {
      oss << line << "\n";
    }

    doc = parse_string(oss.str());
  } else {
    doc = read_file(get_config_fname(conf));
  }

  return doc;
}

void FileAccessor::store_configuration(const std::string &conf, const bool stdio, xmldoc &doc) const {
  remove_text_nodes(doc);

  if (stdio) {
    std::cout << store_string(doc) << std::endl;
  } else {
    store_file(get_config_fname(conf), doc);
  }
}

void FileAccessor::copy_file(const std::string &src_file, const std::string &dst_file) const {
  std::ofstream dst(dst_file, std::ios::binary);
  std::ifstream src(src_file, std::ios::binary);
  if (dst && src) {
    dst << src.rdbuf();
  } else {
    throw invalid_param_error("Couldn't copy file:" + src_file);
  }
}

bool FileAccessor::check_file(const std::string &name) const {
  struct stat buffer;
  int ret = stat(name.c_str(), &buffer);
  return (ret == 0 && buffer.st_size != 0);
}

} // namespace firewall
} // namespace wago
