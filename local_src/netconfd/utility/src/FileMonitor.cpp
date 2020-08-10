// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     FileMonitor.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "FileMonitor.hpp"

#include <string>
#include <boost/filesystem.hpp>

#include <system_error>
#include <iostream>

#include <libgen.h>
#include <sys/inotify.h>
#include <gio/gio.h>
#include <glib-2.0/glib-unix.h>
#include <unistd.h>
#include <fcntl.h>

#include "Logger.hpp"

namespace netconf {

template<typename GObjectType>
using GObjectPtr = std::unique_ptr<GObjectType, decltype(&g_object_unref)>;

template<typename T, typename U>
constexpr bool BitIsSet(T value, U flag) {
  return (value & static_cast<T>(flag)) == static_cast<T>(flag);
}

void Change(FileMonitor& m) {
  m.HandleNotification();
}

static gboolean GSourceCallback(gint /*fd*/, GIOCondition /*condition*/, gpointer data) {
  auto this_ = reinterpret_cast<FileMonitor*>(data);  //NOLINT: need reinterpret cast here
  Change(*this_);
  return static_cast<gboolean>(true);
}

FileMonitor::FileMonitor(std::string filename, std::function<void(FileMonitor&)> changed)
    : changed_(std::move(changed)),
      file_path_(std::move(filename)),
      notify_fd_ { inotify_init1(IN_NONBLOCK) },
      watch_descriptor_fd_ { -1 } {
  if (notify_fd_ < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "inotify_init() failed");
  }

  auto file_path = boost::filesystem::path { file_path_ };
  parent_path_ = file_path.parent_path().string();
  file_name_ = file_path.filename().string();

  watch_descriptor_parent_fd_ = inotify_add_watch(notify_fd_, parent_path_.c_str(), IN_CREATE | IN_MOVE);
  if (watch_descriptor_parent_fd_ < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "inotify_add_watch() for " + parent_path_);
  }

  watch_descriptor_fd_ = inotify_add_watch(notify_fd_, file_path_.c_str(),
  IN_DELETE_SELF | IN_MOVE_SELF | IN_MODIFY | IN_MOVED_FROM);

  auto source = g_unix_fd_source_new(notify_fd_, G_IO_IN);
  g_source_set_callback(source, reinterpret_cast<GSourceFunc>(GSourceCallback), static_cast<gpointer>(this), nullptr);  //NOLINT: need reinterpret cast here
  source_id_ = g_source_attach(source, nullptr);
}

FileMonitor::~FileMonitor() {
  inotify_rm_watch(notify_fd_, watch_descriptor_fd_);
  inotify_rm_watch(notify_fd_, watch_descriptor_parent_fd_);

  g_source_remove(source_id_);

  close(notify_fd_);

}

std::string FileMonitor::GetFileName() const {
  return file_path_;
}

void FileMonitor::RecreateFileWatchDescriptor(bool notify) {
  if (watch_descriptor_fd_ > 0) {
    inotify_rm_watch(notify_fd_, watch_descriptor_fd_);
  }
  watch_descriptor_fd_ = inotify_add_watch(notify_fd_, file_path_.c_str(),
  IN_DELETE_SELF | IN_MOVE_SELF | IN_MODIFY | IN_MOVED_FROM);
  if (notify && (watch_descriptor_fd_ > 0)) {
    changed_(*this);
  }
}

void FileMonitor::HandleNotificationEvent(struct inotify_event * event) {
  if (event->wd == watch_descriptor_fd_) {
    if (BitIsSet(event->mask, IN_MODIFY)) {
      changed_(*this);
    } else if (BitIsSet(event->mask, IN_IGNORED)) {
      RecreateFileWatchDescriptor(true);
    } else if (BitIsSet(event->mask, IN_MOVE_SELF)) {
      changed_(*this);
    }
  } else if ((watch_descriptor_fd_ == -1) && (event->wd == watch_descriptor_parent_fd_)) {
    if (BitIsSet(event->mask, IN_CREATE)) {
      if (file_name_ == static_cast<char*>(event->name)) {
        RecreateFileWatchDescriptor(false);
      }
    } else if (BitIsSet(event->mask, IN_MOVED_TO)) {
      if (file_name_ == static_cast<char*>(event->name)) {
        RecreateFileWatchDescriptor(true);
      }
    }
  }
}

void FileMonitor::HandleNotification() {
  constexpr uint32_t EVENT_SIZE = sizeof(struct inotify_event) + NAME_MAX + 1;
  std::uint8_t buffer[EVENT_SIZE];

  auto bytes_read = read(notify_fd_, static_cast<uint8_t*>(buffer), sizeof(buffer));
  int i = 0;
  while (i < bytes_read) {
    auto event = reinterpret_cast<struct inotify_event *>(&buffer[i]); //NOLINT: need reinterpret cast for this action
    printf("fd: %d event mask : 0x%08X\n", event->wd, event->mask);
    HandleNotificationEvent(event);
    i += EVENT_SIZE + event->len;
  }

}

}
