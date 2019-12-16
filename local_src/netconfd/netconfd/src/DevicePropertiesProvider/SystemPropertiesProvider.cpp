// SPDX-License-Identifier: GPL-2.0-or-later

#include "SystemPropertiesProvider.hpp"
#include <sys/utsname.h>

namespace netconfd {

Status SystemPropertiesProvider::GetHostname(::std::string& hostname) const {
  Status status(StatusCode::OK);

  struct utsname data = {};
  if(0 == uname(&data)) {
    hostname = ::std::string(data.nodename); //NOLINT: do not implicitly decay an array into a pointer is unavoidable at this point

  }
  else{
    status.Append(StatusCode::ERROR, "Failed to get system name via uname");
  }
  return status;
}

} /* namespace netconfd */
