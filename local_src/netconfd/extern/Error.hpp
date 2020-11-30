// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <exception>
#include <map>
#include <vector>
#include <utility>
#include <cstdint>
#include <cerrno>
#include <cstring>

namespace netconf {

enum class ErrorCode : ::std::uint32_t {
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
  DBUS
};

// @formatter:off
static ::std::map<ErrorCode, ::std::string> error_text = {
    { ErrorCode::OK, ::std::string{} },
    { ErrorCode::FILE_READ,                     "Failed to read file %s" },
    { ErrorCode::FILE_WRITE,                    "Failed to write file %s" },
    { ErrorCode::SYSTEM_CALL,                   "System call error: %s" },
    { ErrorCode::JSON_KEY_MISSING,              "Missing JSON key %s" },
    { ErrorCode::JSON_CONVERT,                  "JSON convert error: %s" },
    { ErrorCode::JSON_INCOMPLETE,               "JSON content is incomplete" },
    { ErrorCode::IPV4_FORMAT,                   "IPv4 format incorrect: %s" },
    { ErrorCode::IP_CONFIG_READONLY,            "IP config of %s is readonly"},
    { ErrorCode::IP_CONFIG_SOURCE,              "IP source of %s is invalid"},
    { ErrorCode::IP_DISTRIBUTED_MULTIPLE_TIMES, "IP address %s is distributed multiple times"},
    { ErrorCode::IP_INVALID,                    "Interface %s ip %s is invalid. %s"},
    { ErrorCode::NETMASK_INVALID,               "Interface %s netmask %s is invalid. %s"},
    { ErrorCode::NETWORK_CONFLICT,              "Network for interface %s and %s is in the same range"},
    { ErrorCode::SYSTEM_EXECUTE,                "System execution failed: %s" },
    { ErrorCode::ENTRY_DUPLICATE,               "Entry is duplicated: %s"},
    { ErrorCode::INTERFACE_NOT_EXISTING,        "Interface does not exist: %s"},
    { ErrorCode::NAME_EMPTY,                    "Name is empty"},
    { ErrorCode::DHCP_CLIENT_START,             "Starting dhcp client for %s failed"},
    { ErrorCode::BOOTP_CLIENT_START,            "Starting bootp client for %s failed"},
    { ErrorCode::SET_IP,                        "Set IP address %s netmask %s for interface %s failed"},
    { ErrorCode::SET_INTERFACE,                 "Set interface configuration for %s failed"},
    { ErrorCode::SET_INTERFACE_STATE,           "Set interface state for %s failed"},
    { ErrorCode::PERSISTENCE_WRITE,             "Failed to write data to persistence file"},
    { ErrorCode::PERSISTENCE_READ,              "Failed to read data from persistence file"},
    { ErrorCode::DIP_NOT_AVAILABLE,             "DIP switch based IP configuration not available"},
    { ErrorCode::BACKUP_CONTENT_MISSING,        "Backup file is missing content for %s"},
    { ErrorCode::BACKUP_CONTENT_INVALID,        "Backup file having invalid content for %s"},
    { ErrorCode::BACKUP_VERSION,                "Unknown backup data version, need %s have %s"},
    { ErrorCode::RESTORE_CONTENT,               "Failed to restore content from %s"},
    { ErrorCode::DBUS,                          "DBUS error occurred: %s: %s"},
    { ErrorCode::GENERIC_ERROR,                 "Error: %s" }

};
// @formatter:on
//

class Error {
 public:
  using Parameters = ::std::vector<::std::string>;
  Error()
      : code_ { ErrorCode::OK } {
  }

#if __cpp_fold_expressions
  template<typename ... Args>
  Error(ErrorCode code, Args &&... args)
      : code_ { code } {
    // @formatter:off
    (parameter_.emplace_back(::std::forward<decltype(args)>(args)), ...);
    // @formatter:on
  }
#else
  Error(ErrorCode code): code_{code}{}
  Error(ErrorCode code, ::std::string&& arg1) : code_{code} {
    parameter_.emplace_back(::std::forward<::std::string>(arg1));
  }
#endif

#if __cpp_fold_expressions
  template<typename ... Args>
  void Set(ErrorCode code, Args &&... args) {
    code_ = code;
    parameter_.clear();
    // @formatter:off
    (parameter_.emplace_back(::std::forward<decltype(args)>(args)), ...);
    // @formatter:on
  }
#endif

  void Set(ErrorCode code, Parameters&& p) {
    code_ = code;
    parameter_ = ::std::forward<Parameters>(p);
  }

  bool IsOk() const noexcept {
    return code_ == ErrorCode::OK;
  }

  bool IsNotOk() const noexcept {
    return !IsOk();
  }

  operator bool()  const noexcept{
    return this->IsOk();
  }

  bool operator==(Error const& other) const noexcept{
      return code_ == other.code_;
  };

  ::std::string ToString() const{
    ::std::string error = error_text.at(code_);

    if (not parameter_.empty()) {
      for (auto &p : parameter_) {
        replaceFirstOccurence(error, "%s", p);
      }
    }

    while(replaceFirstOccurence(error, "%s", ""));

    return error;
  }

  static Error Ok() {
    return Error { ErrorCode::OK };
  }

  ErrorCode GetErrorCode() const {
    return code_;
  }

  const Parameters& GetParameter() const{
    return parameter_;
  }

 private:
  ErrorCode code_;
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

inline Error MakeSystemCallError() {
  return Error { ErrorCode::SYSTEM_CALL, ::std::strerror(errno) };
}

}
