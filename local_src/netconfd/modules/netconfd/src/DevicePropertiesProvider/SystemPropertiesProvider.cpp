// SPDX-License-Identifier: GPL-2.0-or-later

#include "SystemPropertiesProvider.hpp"
#include <sys/utsname.h>

namespace netconf {

::std::string SystemPropertiesProvider::GetHostname() const {

  struct utsname data = { };
  if (0 == uname(&data)) {
    return ::std::string(data.nodename);  //NOLINT: do not implicitly decay an array into a pointer is unavoidable at this point
  }
  return ::std::string { };
}

}
