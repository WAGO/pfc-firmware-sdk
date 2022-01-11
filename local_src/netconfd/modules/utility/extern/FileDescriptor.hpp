#pragma once

#include <fcntl.h>
#include <unistd.h>

namespace netconf {

class FileDescriptor final {
public:
  constexpr static int INVALID_FD = -1;
  FileDescriptor() : fd{INVALID_FD}{}
  FileDescriptor(int newfd) : fd{newfd}{}
  FileDescriptor(const char *__file, int __oflag, mode_t mode = S_IRUSR| S_IRGRP) : fd{open(__file, __oflag, mode)}
  {}
  ~FileDescriptor() {if(fd != INVALID_FD){::close(fd);}}

  FileDescriptor(const FileDescriptor&) = delete;
  FileDescriptor& operator=(const FileDescriptor&) = delete;
  FileDescriptor(FileDescriptor&& other) {fd = other.fd; other.fd = INVALID_FD;};
  FileDescriptor& operator=(FileDescriptor&& other) {fd = other.fd; other.fd = INVALID_FD; return *this;};

  operator int() const {
    return fd;
  }

  explicit operator bool() const {
    return fd != INVALID_FD;
  }

private:
  int fd;

};

}  // namespace netconf
