// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/program_options.hpp>
#include "Status.hpp"

namespace network_config
{

  struct FormatOption
  {
      explicit FormatOption(std::string val) :
              value(::std::move(val))
      {
      }
      std::string value;
      friend std::ostream& operator << (std::ostream& s, const FormatOption& option)
      {
        s << option.value;
        return s;
      }
  };

  struct AutonegOption
  {
      explicit AutonegOption(std::string val) :
              value(::std::move(val))
      {
      }
      std::string value;
      friend std::ostream& operator << (std::ostream& s, const AutonegOption& option)
      {
        s << option.value;
        return s;
      }
  };
  struct SpeedOption
  {
      explicit SpeedOption(std::string val) :
              value(::std::move(val))
      {
      }
      std::string value;
      friend std::ostream& operator << (std::ostream& s, const SpeedOption& option)
      {
        s << option.value;
        return s;
      }
  };
  struct DuplexOption
  {
      explicit DuplexOption(std::string val) :
              value(::std::move(val))
      {
      }
      std::string value;
      friend std::ostream& operator << (std::ostream& s, const DuplexOption& option)
      {
        s << option.value;
        return s;
      }
  };
  struct StateOption
  {
      explicit StateOption(std::string val) :
              value(::std::move(val))
      {
      }
      std::string value;

      friend std::ostream& operator << (std::ostream& s, const StateOption& option)
      {
        s << option.value;
        return s;
      }
  };

  class OptionParser
  {
    public:
      OptionParser();
      virtual ~OptionParser() = default;

      OptionParser(const OptionParser&) = delete;
      OptionParser& operator=(const OptionParser&) = delete;
      OptionParser(const OptionParser&&) = delete;
      OptionParser& operator=(const OptionParser&&) = delete;

      void Parse(int argc, const char **argv);
      ::boost::program_options::options_description InitializeOptions() const;
      void OnConfigOption(bool enabled) const;

      const ::boost::program_options::variables_map& GetVariableMap() const { return map_;}

    private:
      ::boost::program_options::basic_parsed_options<char> parsed_options_;
      ::boost::program_options::variables_map map_;
  };

} /* namespace network_config */
