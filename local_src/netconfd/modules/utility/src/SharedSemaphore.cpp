#include "SharedSemaphore.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <system_error>

#include <cerrno>
#include <iostream>
#include <fstream>

#include <grp.h>
#include <pwd.h>

namespace netconf {

static constexpr mode_t semaphore_file_permission = 0660;
static constexpr auto semaphore_file_owner_user = "root";
static constexpr auto semaphore_file_owner_group = "netconf";

namespace {

  void set_ownership(const ::std::string& user_name, const ::std::string& group_name, const ::std::string& sem_name) {

    auto semaphore_file_path = "/dev/shm/sem." + sem_name;

    struct stat file_info;
    int result = stat(semaphore_file_path.c_str(), &file_info);
    if(result == 0) {
      struct passwd *file_user = getpwuid(file_info.st_uid);
      struct group  *file_group = getgrgid(file_info.st_gid);

      if(user_name != file_user->pw_name || group_name != file_group->gr_name) {
        passwd* user_info = getpwnam(user_name.c_str());
        if(user_info == nullptr) {
          throw ::std::system_error(errno, ::std::generic_category(), "SharedSemaphore: getpwnam()");
        }

        group* group_info = getgrnam(group_name.c_str());
        if(group_info == nullptr) {
          throw ::std::system_error(errno, ::std::generic_category(), "SharedSemaphore: getgrnam()");
        }

        result = chown(semaphore_file_path.c_str(), user_info->pw_uid, group_info->gr_gid);
        if(result != 0) {
          throw ::std::system_error(errno, ::std::generic_category(), "SharedSemaphore: chown()");
        }
      }
    }
  }

} // Anonymous namespace

SharedSemaphore::SharedSemaphore(const ::std::string& path, uint32_t init_value) {

  uid_t uid = geteuid ();

  // sem_open checks against the process' umask => alter it to allow any mode
  // and restore its previous value after opening the semaphore
  auto mask  = umask(0);
  semaphore_ = sem_open(("/" + path).c_str(), O_CREAT, semaphore_file_permission, init_value);
  umask(mask);
  if (semaphore_ == nullptr) {
    throw ::std::system_error(errno, ::std::generic_category(), "SharedSemaphore: sem_open");
  }

  // Only run as root
  if(uid == 0) {
    set_ownership(semaphore_file_owner_user, semaphore_file_owner_group, path);
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
