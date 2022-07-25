// SPDX-License-Identifier: GPL-2.0-or-later

#include "HostsFile.hpp"

#include "FileEditor.hpp"
#include "Logger.hpp"
#include "Status.hpp"

namespace netconf {

namespace {

/*
 Static const string storage may throw an exception that cannot be caught.
 Furthermore, HostFile does not have its own class that could initialize a static constant.
 It remains possible to define constants via functions.
 Thus, clan-tidy warnings can be avoided.
 */

const auto LOCAL_HOST_ENTRY = "127.0.0.1\tlocalhost";

::std::string HOSTS_FILE_PATH() {
  return "/etc/hosts";
}

::std::string SYSTEM_HOST_ENTRY_MARKER_BEGIN() {
  return "\n#SYSTEM HOST ENTRY -- DO NOT REMOVE -- WILL BE CREATED BY NETCONF\n";
}

::std::string SYSTEM_HOST_ENTRY_MARKER_END() {
  return "#END SYSTEM HOST -- DO NOT REMOVE\n";
}

::std::string CreateEntriesSection(::std::vector<::std::string> ips, bool needs_localhost_entry,
                                   const ::std::string &hostname, const ::std::string &domain) {
  ::std::string section;

  section.append(SYSTEM_HOST_ENTRY_MARKER_BEGIN());
  if (needs_localhost_entry) {
    section.append("127.0.0.1\tlocalhost\n");
  }
  auto entry = domain.empty() ? hostname : hostname + "." + domain + "\t" + hostname;
  section.append("127.0.1.1\t" + entry + "\n");
  for (auto &ip : ips) {
    ::std::string line = ip + "\t" + entry + "\n";
    section.append(line);
  }
  section.append(SYSTEM_HOST_ENTRY_MARKER_END());

  return section;
}

::std::string FindAndReplaceOldMarkerSection(::std::string &old_hosts, ::std::string &new_section) {
  auto section_begin_pos = old_hosts.find(SYSTEM_HOST_ENTRY_MARKER_BEGIN());
  auto end_marker_pos    = old_hosts.find(SYSTEM_HOST_ENTRY_MARKER_END());

  ::std::string new_hosts;
  if (section_begin_pos != ::std::string::npos && end_marker_pos != ::std::string::npos) {
    auto section_end_pos = end_marker_pos + SYSTEM_HOST_ENTRY_MARKER_END().size();

    auto len = section_end_pos - section_begin_pos;

    new_hosts = old_hosts.replace(section_begin_pos, len, new_section, 0);
  } else {
    new_hosts = old_hosts.append(new_section);
  }

  return new_hosts;
}

}  // namespace

void UpdateHostsFile(::std::vector<::std::string> &ips, const ::std::string &hostname, const ::std::string &domain) {
  FileEditor file_editor;
  ::std::string old_hosts;
  ::std::string new_hosts;

  Status status = file_editor.Read(HOSTS_FILE_PATH(), old_hosts);
  if (status.IsOk()) {
    new_hosts = UpdateFileContent(old_hosts, ips, hostname, domain);
  } else {
    new_hosts = CreateEntriesSection(ips, true, hostname, domain);
  }

  status = file_editor.WriteAndReplace(HOSTS_FILE_PATH(), new_hosts);
  if (status.IsNotOk()) {
    LogError("Failed to update /etc/hosts file. " + status.ToString());
  }
}

::std::string UpdateFileContent(::std::string old_hosts, ::std::vector<::std::string> &ips,
                                const ::std::string &hostname, const ::std::string &domain) {
  auto local_host_pos        = old_hosts.find(LOCAL_HOST_ENTRY);
  auto section_begin_pos     = old_hosts.find(SYSTEM_HOST_ENTRY_MARKER_BEGIN());
  bool needs_localhost_entry = local_host_pos == ::std::string::npos || (section_begin_pos != ::std::string::npos &&
                               local_host_pos > section_begin_pos);

  auto section = CreateEntriesSection(ips, needs_localhost_entry, hostname, domain);
  return FindAndReplaceOldMarkerSection(old_hosts, section);
}

} /* namespace netconf */
