#pragma once

#include "Status.hpp"
#include "BaseTypes.hpp"
#include "FileEditor.hpp"
#include "HostConfFileParser.hpp"

namespace netconf {

class HostConfFile {
 public:
  HostConfFile() = default;
  virtual ~HostConfFile() = default;

  HostConfFile(const HostConfFile &other) = default;
  HostConfFile(HostConfFile &&other) = delete;
  HostConfFile& operator=(const HostConfFile &other) = delete;
  HostConfFile& operator=(HostConfFile &&other) = delete;

  ::std::string GetHostname();
  ::std::string GetDomain();

  static HostConfFile ParseFile();

 private:
  const ::std::string HOST_CONF_FILE_PATH{"/etc/specific/host.conf"};

  HostConfFileParser parser_;
  FileEditor file_editor_;

  void Parse();


};

}
