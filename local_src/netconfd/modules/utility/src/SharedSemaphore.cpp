#include "SharedSemaphore.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <system_error>

namespace netconf {

static constexpr mode_t semaphore_file_permission = 0666;

SharedSemaphore::SharedSemaphore(const ::std::string& path, uint32_t init_value) {
  // sem_open checks against the process' umask => alter it to allow any mode
  // and restore its previous value after opening the semaphore
  auto mask  = umask(0);
  semaphore_ = sem_open(path.c_str(), O_CREAT, semaphore_file_permission, init_value);
  umask(mask);
  if (semaphore_ == nullptr) {
    throw ::std::system_error(errno, ::std::generic_category(), "SharedSemaphore: sem_open");
  }
}

SharedSemaphore::~SharedSemaphore() {
  if (semaphore_ != SEM_FAILED) {
    sem_close(semaphore_);
  }
}

void SharedSemaphore::lock() {
  auto res = sem_wait(semaphore_);
  if (res != 0) {
    throw ::std::system_error(errno, ::std::generic_category(), "SharedSemaphore: sem_wait");
  }
}

void SharedSemaphore::unlock() {
  auto res = sem_post(semaphore_);
  if (res != 0) {
    throw ::std::system_error(errno, ::std::generic_category(), "SharedSemaphore: sem_post");
  }
}

} /* namespace netconf */
