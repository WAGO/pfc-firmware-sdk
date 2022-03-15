// SPDX-License-Identifier: GPL-2.0-or-later
#include "HostnameController.hpp"

#include <cstdio>
#include <boost/algorithm/string/trim.hpp>

#include "Logger.hpp"
#include "FileEditor.hpp"
#include "StringUtils.hpp"

namespace netconf {

std::string GetKernelHostname() {
  FileEditor file_editor;
  ::std::string hostname;
  auto result = file_editor.Read("/proc/sys/kernel/hostname", hostname);
  if(result.IsNotOk()){
    LogError("GetKernelHostname: " + result.ToString());
  }
  RemoveCharFromString(hostname, '\n');
  return hostname;
}

void SetKernelHostname(const std::string &name) {
  FileEditor file_editor;

  auto result = file_editor.Write("/proc/sys/kernel/hostname", name);
  if (result.IsNotOk()) {
    LogError("SetKernelHostname: " + result.ToString());
  }

}

} /* namespace netconf */
