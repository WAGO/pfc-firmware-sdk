// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <exception>
#include <map>
#include <vector>
#include <utility>
#include <cstdint>
#include <cerrno>
#include <cstring>
#include <algorithm>

namespace netconf {

enum class StatusCode : ::std::uint32_t {
  OK = 0,
  GENERIC_ERROR,
  SYSTEM_CALL,
  FILE_READ,
  FILE_WRITE,
  JSON_KEY_MISSING,
  JSON_CONVERT,
  JSON_INCOMPLETE,
  IPV4_FORMAT,
  IP_CONFIG_READONLY,
  IP_CONFIG_SOURCE,
  IP_INVALID,
  IP_DISTRIBUTED_MULTIPLE_TIMES,
  NETMASK_INVALID,
  NETWORK_CONFLICT,
  SYSTEM_EXECUTE,
  ENTRY_DUPLICATE,
  NAME_EMPTY,
  INTERFACE_NOT_EXISTING,
  BRIDGE_NAME_INVALID,
  DHCP_CLIENT_START,
  BOOTP_CLIENT_START,
  PERSISTENCE_WRITE,
  PERSISTENCE_READ,
  RESTORE_CONTENT,
  BACKUP_CONTENT_MISSING,
  BACKUP_CONTENT_INVALID,
  BACKUP_VERSION,
  DIP_NOT_AVAILABLE,
  SET_IP,
  SET_INTERFACE,
  SET_INTERFACE_STATE,
  DBUS,
  LINK_MODE_NOT_SUPPORTED,
  AUTONEGOTIATION_NOT_SUPPORTED
};

inline std::string GetTextForCode(StatusCode c){
// @formatter:off
static const ::std::map<StatusCode, ::std::string> status_text = {
    { StatusCode::OK, ::std::string{} },
    { StatusCode::FILE_READ,                     "Failed to read file %s" },
    { StatusCode::FILE_WRITE,                    "Failed to write file %s" },
    { StatusCode::SYSTEM_CALL,                   "System call error: %s" },
    { StatusCode::JSON_KEY_MISSING,              "Missing JSON key %s" },
    { StatusCode::JSON_CONVERT,                  "JSON convert error: %s" },
    { StatusCode::JSON_INCOMPLETE,               "JSON content is incomplete" },
    { StatusCode::IPV4_FORMAT,                   "IPv4 format incorrect: %s" },
    { StatusCode::IP_CONFIG_READONLY,            "IP config of %s is readonly"},
    { StatusCode::IP_CONFIG_SOURCE,              "IP source of %s is invalid"},
    { StatusCode::IP_DISTRIBUTED_MULTIPLE_TIMES, "IP address %s is distributed multiple times"},
    { StatusCode::IP_INVALID,                    "Interface %s ip %s is invalid. %s"},
    { StatusCode::NETMASK_INVALID,               "Interface %s netmask %s is invalid. %s"},
    { StatusCode::NETWORK_CONFLICT,              "Network for interface %s and %s is in the same range"},
    { StatusCode::SYSTEM_EXECUTE,                "System execution failed: %s" },
    { StatusCode::ENTRY_DUPLICATE,               "Entry is duplicated: %s"},
    { StatusCode::INTERFACE_NOT_EXISTING,        "Interface does not exist: %s"},
    { StatusCode::NAME_EMPTY,                    "Name is empty"},
    { StatusCode::DHCP_CLIENT_START,             "Starting dhcp client for %s failed"},
    { StatusCode::BOOTP_CLIENT_START,            "Starting bootp client for %s failed"},
    { StatusCode::SET_IP,                        "Set IP address %s netmask %s for interface %s failed"},
    { StatusCode::SET_INTERFACE,                 "Set interface configuration for %s failed"},
    { StatusCode::SET_INTERFACE_STATE,           "Set interface state for %s failed"},
    { StatusCode::PERSISTENCE_WRITE,             "Failed to write data to persistence file"},
    { StatusCode::PERSISTENCE_READ,              "Failed to read data from persistence file"},
    { StatusCode::DIP_NOT_AVAILABLE,             "DIP switch based IP configuration not available"},
    { StatusCode::BACKUP_CONTENT_MISSING,        "Backup file is missing content for %s"},
    { StatusCode::BACKUP_CONTENT_INVALID,        "Backup file having invalid content for %s"},
    { StatusCode::BACKUP_VERSION,                "Unknown backup data version, need %s have %s"},
    { StatusCode::RESTORE_CONTENT,               "Failed to restore content from %s"},
    { StatusCode::DBUS,                          "DBUS error occurred: %s: %s"},
    { StatusCode::GENERIC_ERROR,                 "Error: %s" },
    { StatusCode::LINK_MODE_NOT_SUPPORTED,      "Interface %s does not support (speed %s/duplex %s)" },
    { StatusCode::AUTONEGOTIATION_NOT_SUPPORTED,"Interface %s does not support Autonegotiation" }

};
// @formatter:on
//
return status_text.at(c);
}

class Status {
 public:
  using Parameters = ::std::vector<::std::string>;
  Status()
      : code_ { StatusCode::OK } {
  }

#if __cpp_fold_expressions
  template<typename ... Args>
  Status(StatusCode code, Args &&... args)
      : code_ { code } {
    // @formatter:off
    (parameter_.emplace_back(::std::forward<decltype(args)>(args)), ...);
    // @formatter:on
  }
#else
  Status(StatusCode code): code_{code}{}
  Status(StatusCode code, ::std::string&& arg1) : code_{code} {
    parameter_.emplace_back(::std::forward<::std::string>(arg1));
  }
#endif

#if __cpp_fold_expressions
  template<typename ... Args>
  void Set(StatusCode code, Args &&... args) {
    code_ = code;
    parameter_.clear();
    // @formatter:off
    (parameter_.emplace_back(::std::forward<decltype(args)>(args)), ...);
    // @formatter:on
  }
#endif

  void Set(StatusCode code, Parameters&& p) {
    code_ = code;
    parameter_ = ::std::forward<Parameters>(p);
  }

  Status& Append(const std::string& parameter) {
    parameter_.push_back(parameter);
    return *this;
  }

  bool IsOk() const noexcept {
    return code_ == StatusCode::OK;
  }

  bool IsNotOk() const noexcept {
    return !IsOk();
  }

  operator bool()  const noexcept{
    return this->IsOk();
  }

  bool operator==(Status const& other) const noexcept{
      return code_ == other.code_;
  };

  ::std::string ToString() const{
    ::std::string error = GetTextForCode(code_);
    Parameters append_at_end;

    if (not parameter_.empty()) {
      for (auto &p : parameter_) {
        if(not replaceFirstOccurence(error, "%s", p)){
          append_at_end.push_back(p);
        }
      }
    }

    if(append_at_end.size() > 0){
      std::for_each(append_at_end.begin(), append_at_end.end(), [&error](auto& s){error.append(": ").append(s);});
    }

    while(replaceFirstOccurence(error, "%s", ""));

    return error;
  }

  static Status Ok() {
    return Status { StatusCode::OK };
  }

  StatusCode GetStatusCode() const {
    return code_;
  }

  const Parameters& GetParameter() const{
    return parameter_;
  }

 private:
  StatusCode code_;
  Parameters parameter_;

  bool replaceFirstOccurence(std::string &str, const std::string &from, const std::string &to) const {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos) {
      return false;
    }
    str.replace(start_pos, from.length(), to);
    return true;
  }

};

inline Status MakeSystemCallError() {
  return Status { StatusCode::SYSTEM_CALL, ::std::strerror(errno) };
}

}
