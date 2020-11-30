// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <functional>

struct inotify_event;

namespace netconf
{

class FileMonitor
{
 public:
  explicit FileMonitor(std::string filename, std::function<void(FileMonitor&)> changed);
  ~FileMonitor();

  std::string GetFileName() const;

 private:
  void HandleNotification();
  void HandleNotificationEvent(struct inotify_event*);
  void RecreateFileWatchDescriptor(bool notify);

  friend void Change(FileMonitor& m);

  ::std::function<void(FileMonitor&)> changed_;
  ::std::string file_path_;
  ::std::string parent_path_;
  ::std::string file_name_;
  ::std::int32_t notify_fd_;
  ::std::int32_t watch_descriptor_fd_;
  ::std::int32_t watch_descriptor_parent_fd_;
  ::std::uint32_t source_id_;
};

}
//---- End of header file ------------------------------------------------------

