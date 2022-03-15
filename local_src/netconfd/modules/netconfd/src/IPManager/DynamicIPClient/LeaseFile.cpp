// SPDX-License-Identifier: GPL-2.0-or-later

#include "LeaseFile.hpp"
#include "Logger.hpp"

namespace netconf {


void LeaseFile::Parse(const ::std::string &lease_file_path) {
  lease_file_exists_ = false;
  ::std::string content;

  FileEditor file_editor;
  Status status = file_editor.Read(lease_file_path, content);
  if (status.IsOk()) {
    lease_file_exists_ = true;
    parser_.Parse(content);
  }
}

Address LeaseFile::GetAddress() {
  return parser_.GetAddress();
}
Netmask LeaseFile::GetNetmask() {
  return parser_.GetNetmask();
}
::std::string LeaseFile::GetDHCPHostname() {
  return parser_.GetDHCPHostname();
}
::std::string LeaseFile::GetDHCPDomain() {
  return parser_.GetDHCPDomain();
}

bool LeaseFile::Exists() {
  return lease_file_exists_;
}

} /* namespace netconf */
