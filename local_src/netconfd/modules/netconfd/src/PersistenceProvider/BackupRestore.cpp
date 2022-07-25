// SPDX-License-Identifier: GPL-2.0-or-later

#include "BackupRestore.hpp"

#include <sstream>
#include <cmath>

#include <vector>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <string>

#include "Logger.hpp"
#include "KeyValueParser.hpp"

namespace netconf {

using namespace std::literals::string_literals;

static Status GetKeyValue(const ::std::string &backup_content, const ::std::string &key, ::std::string &value) {

  Status status;

  value = GetValueByKey(backup_content, key);

  if (value.empty()) {
    status.Set(StatusCode::BACKUP_CONTENT_MISSING, key);
    return status;
  }
  return status;

}

BackupRestore::BackupRestore(IFileEditor &file_editor, uint32_t chars_per_line)
    : file_editor_ { file_editor },
      chars_per_line_ { chars_per_line } {
}
[[gnu::const]]
uint32_t BackupRestore::GetBackupParameterCount() const {
  return 2;
}

Status BackupRestore::AppendTextWithKeyAndSeparateOnNewLine(const ::std::string &file_path, const ::std::string &key,
                                                           const ::std::string &data) const {

  Status status;

  auto lines = static_cast<uint32_t>(::std::floor(data.size() / chars_per_line_));
  for (uint32_t line = 0; line < lines; line++) {
    ::std::string substring = data.substr(line * chars_per_line_, chars_per_line_);
    substring = key + '=' + substring + '\n';
    status = file_editor_.Append(file_path, substring);
    if (status.IsNotOk()) {
      break;
    }
  }

  uint32_t remaining_chars = data.size() % chars_per_line_;
  if (remaining_chars > 0) {
    ::std::string substring = data.substr(lines * chars_per_line_, data.size());
    substring = key + '=' + substring + '\n';
    status = file_editor_.Append(file_path, substring);
  }

  return status;
}

Status BackupRestore::Backup(const ::std::string &file_path, const ::std::string &network_data,
                            const ::std::string &dip_switch_data, uint32_t version) const {

  Status status = AppendTextWithKeyAndSeparateOnNewLine(file_path, KEY_NETCONFD_VERSION, ::std::to_string(version));
  if (status.IsOk()) {
    status = AppendTextWithKeyAndSeparateOnNewLine(file_path, KEY_NETCONFD_NETWORK_DATA, network_data);
  }
  if (status.IsOk() && not dip_switch_data.empty()) {
    status = AppendTextWithKeyAndSeparateOnNewLine(file_path, KEY_NETCONFD_DIPSWITCH_DATA, dip_switch_data);
  }

  return status;

}

Status BackupRestore::Restore(const ::std::string &file_path, ::std::string &backup_network_data,
                             ::std::string &backup_dipswitch_data, uint32_t &version) const {

  ::std::string backup_content;
  version = 0;

  Status status = file_editor_.Read(file_path, backup_content);

  auto stored_version = ::std::string { };
  if (status.IsOk()) {
    status = GetKeyValue(backup_content, KEY_NETCONFD_VERSION, stored_version);

  }

  if (status.IsOk()) {
    if (!boost::conversion::try_lexical_convert(stored_version, version)) {
      version = 0;
      backup_network_data = "";
      status.Set(StatusCode::BACKUP_VERSION, ::std::to_string(version), stored_version);
    }
  }

  if (status.IsOk()) {
    status = GetKeyValue(backup_content, KEY_NETCONFD_NETWORK_DATA, backup_network_data);
  }

  if (status.IsOk()) {
    GetKeyValue(backup_content, KEY_NETCONFD_DIPSWITCH_DATA, backup_dipswitch_data);
  }

  return status;
}

} /* namespace netconf */
