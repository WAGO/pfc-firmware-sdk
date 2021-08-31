// SPDX-License-Identifier: GPL-2.0-or-later
#include <cstdlib>
#include <glib.h>
#include <UriEscape.hpp>
#include <memory>

/*
 * int main() {
 char *uri = "http://www.example.com?hello world";
 char *encoded_uri = NULL;
 //as per wiki (https://en.wikipedia.org/wiki/Percent-encoding)
 char *escape_char_str = "!*'();:@&=+$,/?#[]";
 encoded_uri = g_uri_escape_string(uri, escape_char_str, TRUE);
 printf("[%s]\n", encoded_uri);
 free(encoded_uri);

 return 0;
 }
 */

using std::string;

UriEscape::UriEscape(::std::string escape_chars)
    : escape_chars_ { std::move(escape_chars) } {
}

::std::string UriEscape::Unescape(const ::std::string &escaped_uri) const {
  auto result_cstr = std::unique_ptr<char, decltype(std::free)*>(
      g_uri_unescape_string(escaped_uri.c_str(), escape_chars_.c_str()), std::free);
  if (result_cstr != nullptr) {
    return std::string { result_cstr.get() };
  }
  return std::string { };
}

::std::string UriEscape::Unescape(const char *escaped_uri) const {
  return Unescape(string { escaped_uri });
}

::std::string UriEscape::Escape(const ::std::string &unescaped_uri) const {
  auto result_cstr = std::unique_ptr<char, decltype(std::free)*>(
      g_uri_escape_string(unescaped_uri.c_str(), escape_chars_.c_str(), TRUE), std::free);
  if (result_cstr != nullptr) {
    return std::string { result_cstr.get() };
  }
  return std::string { };
}

::std::string UriEscape::Escape(const char *escaped_uri) const {
  return Escape(string { escaped_uri });
}
