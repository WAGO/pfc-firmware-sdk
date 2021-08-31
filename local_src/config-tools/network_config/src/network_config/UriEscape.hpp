// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>

class UriEscape {
 public:
  explicit UriEscape(::std::string escape_chars = "");
  ~UriEscape() = default;

  ::std::string Unescape(const ::std::string &escaped_uri) const;
  ::std::string Unescape(const char *escaped_uri) const;

  ::std::string Escape(const ::std::string &escaped_uri) const;
  ::std::string Escape(const char *escaped_uri) const;

 private:
  ::std::string escape_chars_;

};
