// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "UriEscape.hpp"

#include <string>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/optional/optional.hpp>
#include <initializer_list>
#include <array>
#include "OptionParser.hpp"
#include "OptionStrings.hpp"

namespace network_config {

static inline bool Contains(const boost::program_options::variables_map &map, const ::std::string &key) {
  return map.count(key) > 0;
}

static inline bool Contains(const boost::program_options::variables_map &map, const Option &option) {
  return map.count(option.name) > 0;
}

static inline ::std::string GetValueOf(const boost::program_options::variables_map &map, const ::std::string &key) {
  auto value = map[key].as<::std::string>();
  UriEscape uri_escape;
  value = uri_escape.Unescape(value);
  boost::algorithm::trim_if(value, boost::algorithm::is_any_of("'"));
  return value;
}

static inline ::std::string GetValueOf(const boost::program_options::variables_map &map, const Option &option) {
  return GetValueOf(map, option.name);
}


static inline boost::optional<::std::vector<::std::string>> GetValuesOf(const boost::program_options::variables_map &map, const ::std::string &key) {
  if(!Contains(map, key)){
    return boost::optional<::std::vector<::std::string>>{};
  }
  auto values = map[key].as<::std::vector<::std::string>>();
  UriEscape uri_escape;
  std::transform(values.begin(), values.end(), values.begin(), [&](auto& value){
    boost::algorithm::trim_if(value, boost::algorithm::is_any_of("'"));
    return uri_escape.Unescape(value);});

  return boost::optional<::std::vector<::std::string>>{values};
}

static inline ::std::string GetValueOfGet(const boost::program_options::variables_map &map) {
  return GetValueOf(map, "get");
}

static inline ::std::string GetValueOfSet(const boost::program_options::variables_map &map) {
  return GetValueOf(map, "set");
}

static inline ::std::string GetFormat(const boost::program_options::variables_map &map) {
  auto value = map["format"].as<FormatOption>().value;
  boost::algorithm::trim_if(value, boost::algorithm::is_any_of("'"));
  return value;
}

static inline ::std::string GetDevice(const boost::program_options::variables_map &map) {
  return GetValueOf(map, "device");
}

static inline ::std::string Get(const boost::program_options::variables_map &map, const Option &option) {
  return GetValueOf(map, option.name);
}


template<typename ... Options>
::std::string JoinOptionNames(std::string separator, Options&& ... options){
  ::std::array<::std::string, sizeof...(options)> options_a{{(options.name)...}};
  return boost::algorithm::join(options_a, separator);
}

template<typename ... Args>
static void OptionalAndMutuallyExclusive(const boost::program_options::variables_map &map, Args&& ... args) {
  int option_count = (map.count(args.name) + ...);

  if (option_count > 1) {
    throw boost::program_options::error(
        ::std::string("Optional arguments are mutually exclusive: " + JoinOptionNames(",",args...)));
  }
}


template<typename ... Args>
static void MutuallyExclusiveAndOnlyOnce(const boost::program_options::variables_map &map, Args&& ... args) {
  int option_count = (map.count(args.name) + ... );

  if (option_count != 1) {
    throw boost::program_options::error(
        ::std::string("Either options are set that are mutually exclusive, or an option is set multiple times."));
  }
}

template<typename ... Args>
static void ExpectOptionPair(const boost::program_options::variables_map &map, Option option, Args&& ... suboptions) {
  if (Contains(map, option)) {
    MutuallyExclusiveAndOnlyOnce(map, suboptions...);
  }
}

}
