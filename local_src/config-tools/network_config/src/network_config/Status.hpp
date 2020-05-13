// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>

namespace network_config {

enum class StatusCode {
  OK,
  ERROR,
  OPTION_PARSE_ERROR
};

class Status {
 public:
  Status() {
    error_code_ = StatusCode::OK;
    message_ = "";
  }
  explicit Status(StatusCode error) {
    error_code_ = error;
    message_ = "";
  }
  Status(StatusCode error, ::std::string message)
  : error_code_{error}, message_{::std::move(message)}{
  }

  virtual ~Status() = default;

  Status(const Status &) = default;
  Status &operator=(const Status &) = default;
  Status(Status &&) = default;
  Status &operator=(Status &&) = default;

  explicit operator StatusCode() const { return error_code_; }

  bool Ok() const {
    return error_code_ == StatusCode::OK;
  }

  bool NotOk() const {
    return !Ok();
  }

  StatusCode Get() const {
    return error_code_;
  }

  ::std::string GetMessage() const {
    return message_;
  }

  Status Prepend(const ::std::string& message) {
    if (error_code_ == StatusCode::OK) {
      error_code_ = StatusCode::ERROR;
    }
    message_.insert(0, message);
    return *this;
  }

  Status Prepend(StatusCode code, const ::std::string& message) {
    error_code_ = code;
    message_.insert(0, message);
    return *this;
  }

  Status Append(const ::std::string& message) {
    if (error_code_ == StatusCode::OK) {
      error_code_ = StatusCode::ERROR;
    }
    message_.append(message);
    return *this;
  }

  Status Append(StatusCode code, const ::std::string& message) {
    error_code_ = code;
    message_.append(message);
    return *this;
  }

  Status& Merge(const Status& other) {
    error_code_ = other.error_code_;
    message_.append(other.message_);
    return *this;
  }

  explicit operator bool() { return this->Ok(); }

 private:
  StatusCode error_code_;
  ::std::string message_;
};

}  // namespace netconfd
