// SPDX-License-Identifier: LGPL-3.0-or-later
#include "FileOperations.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace netconf {


void TouchFile(::std::string& path){
  int fd = open(path.c_str(), O_CREAT | O_WRONLY,
  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  if (fd >= 0) {
    futimens(fd, nullptr);
    close(fd);
  }
}


}  // namespace netconf
