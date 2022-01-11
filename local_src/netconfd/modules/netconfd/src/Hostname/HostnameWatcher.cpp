// SPDX-License-Identifier: GPL-2.0-or-later
#include "HostnameWatcher.hpp"

#include <glib-2.0/glib.h>
#include <glib-2.0/glib-unix.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <system_error>
#include <fcntl.h>
#include <boost/algorithm/string/trim.hpp>

namespace netconf {

namespace {

gboolean watch_callback(gint fd, GIOCondition condition, gpointer user_data) {
  if (condition != G_IO_IN) {
    return false;
  }

  std::array<epoll_event, 20> events;
  auto num_events = epoll_wait(fd, events.data(), events.size(), 0);
  if (num_events > 0) {
    HostnameWatcher *self = reinterpret_cast<HostnameWatcher*>(user_data);
    self->UpdateHostname();
  }

  return true;
}

}  // namespace

HostnameWatcher::HostnameWatcher()
    : epoll_fd_ { epoll_create1(0) }, hostname_fd_ {"/proc/sys/kernel/hostname", O_RDONLY} {
  if (not epoll_fd_) {
    throw std::system_error { errno, std::system_category(), "Failed to open epoll fd" };
  }


  if (not hostname_fd_) {
    throw std::system_error { errno, std::system_category(), "Failed to open hostname" };
  }

  struct epoll_event event { };
  event.data.fd = hostname_fd_;
  event.events = EPOLLIN | EPOLLET;

  auto ctl_status = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, hostname_fd_, &event);
  if (ctl_status < 0) {
    throw std::system_error { errno, std::system_category(), "Failed to add epoll event" };
  }

  hostname_ = ReadHostname();

  gsource_ = g_unix_fd_add_full(0, epoll_fd_, G_IO_IN, watch_callback, this, nullptr);
}

HostnameWatcher::HostnameWatcher(UpdateFn update_fn) : HostnameWatcher() {
  update_fn_ = std::move(update_fn);
}

HostnameWatcher::~HostnameWatcher() {
  g_source_remove(gsource_);
}

std::string HostnameWatcher::ReadHostname() {
  std::string newname;
  std::array<char, 200> buffer;
  lseek(hostname_fd_, 0, SEEK_SET);
  auto nbytes = read(hostname_fd_, buffer.data(), buffer.size() - 1);
  if (nbytes > 0) {
    buffer.at(nbytes) = '\0';
    newname.assign(buffer.data(), static_cast<size_t>(nbytes));
  }
  boost::trim(newname);
  return newname;
}

void HostnameWatcher::UpdateHostname() {

  std::string newname = ReadHostname();
  if (hostname_ != newname) {
    hostname_ = newname;
    if (update_fn_)
      update_fn_(newname);
  }

}

void HostnameWatcher::SetUpdateFunction(UpdateFn update_fn) {
  update_fn_ = update_fn;
}


void HostnameWatcher::SetHostname(const std::string &newname) {
  if (hostname_ != newname) {
    hostname_ = newname;

    write(hostname_fd_, newname.data(), newname.size());
  }
}

} /* namespace netconf */
