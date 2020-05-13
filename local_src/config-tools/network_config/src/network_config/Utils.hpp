// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "UriEscape.hpp"

#include <string>
#include <boost/algorithm/string/trim.hpp>
#include <initializer_list>

#include "OptionParser.hpp"
#include "OptionStrings.hpp"

namespace network_config
{

  template<typename T>
  static inline ::std::string GetValueOf(const boost::program_options::variables_map &map, const ::std::string& key) {
    auto value = map[key].as<T>();
    UriEscape uri_escape;
    value = uri_escape.Unescape(value);
    boost::algorithm::trim_if(value, boost::algorithm::is_any_of("'"));
    return value;
  }

  static inline ::std::string GetValueOfGet(const boost::program_options::variables_map &map)
  {
    return GetValueOf<::std::string>(map, "get");
  }

  static inline ::std::string GetValueOfSet(const boost::program_options::variables_map &map)
  {
    return GetValueOf<::std::string>(map, "set");
  }

  static inline ::std::string GetFormat(const boost::program_options::variables_map &map)
  {
    auto value = map["format"].as<FormatOption>().value;
    boost::algorithm::trim_if(value, boost::algorithm::is_any_of("'"));
    return value;
  }

  static inline ::std::string GetDevice(const boost::program_options::variables_map &map)
  {
    return GetValueOf<::std::string>(map, "device");
  }

  static inline ::std::string Get(const boost::program_options::variables_map &map, const Option& option)
  {
    return GetValueOf<::std::string>(map, option.name);
  }

  static inline bool Contains(const boost::program_options::variables_map &map, const ::std::string& key)
  {
    return map.count(key) > 0;
  }

  static inline bool Contains(const boost::program_options::variables_map &map, const Option& option){
    return map.count(option.name) > 0;
  }

  template<typename ... Args>
  static void MutuallyExclusiveAndOnlyOnce(const boost::program_options::variables_map &map, Args ... args) {
    int option_count = 0;
    (void) std::initializer_list<int> { (option_count += map.count(args),0)... };

    if (option_count != 1) {
      throw boost::program_options::error(::std::string("Mutually exclusive options are set or one option multiple times"));
    }
  }

  template<typename ... Args>
  static void ExpectedOptionPair(const boost::program_options::variables_map &map, const char *option, Args ... suboptions) {
    if (map.count(option) > 0) {
      MutuallyExclusiveAndOnlyOnce(map, suboptions...);
    }
  }

}
