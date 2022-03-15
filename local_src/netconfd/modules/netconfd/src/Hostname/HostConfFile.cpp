// SPDX-License-Identifier: GPL-2.0-or-later

#include "HostConfFile.hpp"

#include "Logger.hpp"

namespace netconf {

HostConfFile HostConfFile::ParseFile() {
  HostConfFile host_conf_file;
  host_conf_file.Parse();
  return host_conf_file;
}

void HostConfFile::Parse() {
  ::std::string content;
  Status status = file_editor_.Read(HOST_CONF_FILE_PATH, content);
  if (status.IsNotOk()) {
    LogError("Failed to read /etc/specific/host.conf. " + status.ToString());
  } else {
    parser_.Parse(content);
  }
}

::std::string HostConfFile::GetHostname() {
  return parser_.GetHostname();
}

::std::string HostConfFile::GetDomain() {
  return parser_.GetDomain();
}

} /* namespace netconf */
