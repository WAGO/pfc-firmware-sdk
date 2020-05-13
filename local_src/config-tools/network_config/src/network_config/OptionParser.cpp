// SPDX-License-Identifier: GPL-2.0-or-later

#include "OptionParser.hpp"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#include "OptionStrings.hpp"
#include "Utils.hpp"

using std::string;

namespace po = boost::program_options;
namespace network_config
{

  OptionParser::OptionParser() :
          parsed_options_{nullptr}
  {
  }

  template<typename T>
  static void ValidateOption(boost::any &v,
                             ::std::vector<std::string> const &values,
                             const ::std::function<bool(::std::string)> &predicate)
  {
    po::validators::check_first_occurrence(v);
    const std::string &s = po::validators::get_single_string(values);

    if(predicate(s))
    {
      v = boost::any(T(s));
    }
    else
    {
      throw po::validation_error(po::validation_error::invalid_option_value); //NOLINT(cert-err60-cpp) see: https://www.boost.org/doc/libs/1_65_1/doc/html/program_options/howto.html
    }
  }

  static void validate(boost::any &v,
                       std::vector<std::string> const &values,
                       FormatOption *format,
                       int i)
  {
    (void)format;
    (void)i;
    ValidateOption<FormatOption>(v, values, [](const ::std::string &s)
    {
      return s == "text" || s == "json";
    });
  }

  static void validate(boost::any &v,
                       std::vector<std::string> const &values,
                       StateOption *state,
                       int i)
  {
    (void)state;
    (void)i;
    ValidateOption<StateOption>(v, values, [](const ::std::string &s)
    {
      return s == "up" || s == "down";
    });
  }

  static void validate(boost::any &v,
                       std::vector<std::string> const &values,
                       AutonegOption *autoneg,
                       int i)
  {
    (void)autoneg;
    (void)i;
    ValidateOption<AutonegOption>(v, values, [](const ::std::string &s)
    {
      return s == "on" || s == "off";
    });
  }

  static void validate(boost::any &v,
                       std::vector<std::string> const &values,
                       SpeedOption *speed,
                       int i)
  {
    (void)speed;
    (void)i;
    ValidateOption<SpeedOption>(v, values, [](const ::std::string &s)
    {
      return s == "10" || s == "100";
    });
  }

  static void validate(boost::any &v,
                       std::vector<std::string> const &values,
                       DuplexOption *duplex,
                       int i)
  {
    (void)duplex;
    (void)i;
    ValidateOption<DuplexOption>(v, values, [](const ::std::string &s)
    {
      return s == "full" || s == "half";
    });
  }

  static void ExpectedValueOrOption(const po::variables_map &map,
                                    const ::std::string &option,
                                    const ::std::string &alternative_option)
  {
    if(map.count(option) == 0)
    {
      return;
    }
    auto value = map[option].as<::std::string>();
    if(value.empty() and 0 == map.count(alternative_option))
    {
      throw po::error("Neither " + option + " has a value nor " + alternative_option + " is set");
    }
  }

  static void ExpectValueForGetOption(const po::variables_map &map,
                                      const ::std::string &option)
  {
    if(map.count(option) == 0)
    {
      return;
    }
    if(map.count("get") == 0)
    {
      throw po::error("Missing \"get\" for option " + option + ".");
    }
    auto value = GetValueOfGet(map);
    if(value.empty())
    {
      throw po::error(::std::string("No interface is specified as get value for mac-address option."));
    }
  }

  po::options_description OptionParser::InitializeOptions() const
  {
    const auto &optstr = GetOptions();
    // @formatter:off
    po::options_description options("Primary options");
    options.add_options()
      (optstr.brigde_config.name, optstr.brigde_config.description)
      (optstr.interface_config.name, optstr.interface_config.description)
      (optstr.ip_config.name, optstr.ip_config.description)
      (optstr.mac_address.name, optstr.mac_address.description)
      (optstr.device_info.name, optstr.device_info.description)
      (
          optstr.backup.name,
          po::value<::std::string>()->value_name("file path"),
          optstr.backup.description
      )
      (
          optstr.restore.name,
          po::value<::std::string>()->value_name("file path"),
          optstr.restore.description
      )
      (optstr.get_backup_parameter_count.name, optstr.get_backup_parameter_count.description)
      (optstr.dsa_mode.name, optstr.dsa_mode.description)
      (optstr.fix_ip.name, optstr.fix_ip.description)
      (optstr.dip_switch_config.name, optstr.dip_switch_config.description);

    po::options_description operations("Operations");
    operations.add_options()
      (
          "get",
          po::value<::std::string>()
            ->value_name("<value>")
            ->implicit_value(""),
          "Get configuration; Optionally specify a value to get only part of the configuration. The \"value\" can only be used together with the --mac-address option."
      )
      ("get-current",
          po::value<::std::string>()
            ->value_name("<value>")
            ->implicit_value(""),
          "Get the current configuration")
      (
          "set",
          po::value<::std::string>()
            ->value_name("<value>")
            ->implicit_value(""),
          "Set configuration value"
      );

    const auto &fieldstr = GetFields();
    po::options_description fields("Fields");
    fields.add_options()
      ( fieldstr.device.name,
        po::value<::std::string>()->value_name(fieldstr.device.parameter),
        fieldstr.device.description)
      ( fieldstr.source.name,
        po::value<::std::string>()->value_name(fieldstr.source.parameter)->implicit_value(""),
        fieldstr.source.description)
      ( fieldstr.address.name,
        po::value<::std::string>()->value_name(fieldstr.address.parameter)->implicit_value(""),
        fieldstr.address.description)
      ( fieldstr.netmask.name,
        po::value<::std::string>()->value_name(fieldstr.netmask.parameter)->implicit_value(""),
        fieldstr.netmask.description)
      ( fieldstr.prefix.name,
        po::value<::std::string>()->value_name(fieldstr.prefix.parameter)->implicit_value(""),
        fieldstr.prefix.description)
      ( fieldstr.state.name,
        po::value<StateOption>()->value_name(fieldstr.state.parameter)->implicit_value(StateOption{""}),
        fieldstr.state.description)
      ( fieldstr.autoneg.name,
        po::value<AutonegOption>()->value_name(fieldstr.autoneg.parameter)->implicit_value(AutonegOption{""}),
        fieldstr.autoneg.description)
      ( fieldstr.speed.name,
        po::value<SpeedOption>()->value_name(fieldstr.speed.parameter)->implicit_value(SpeedOption{""}),
        fieldstr.speed.description)
      ( fieldstr.duplex.name,
        po::value<DuplexOption>()->value_name(fieldstr.duplex.parameter)->implicit_value(DuplexOption{""}),
        fieldstr.duplex.description)
      ( fieldstr.backup_version.name,
        po::value<::std::string>()->value_name(fieldstr.backup_version.parameter),
        fieldstr.backup_version.description
      );

    po::options_description formats("Format");
    formats.add_options()
      (
          "format,f",
          po::value<FormatOption>()
            ->value_name("<format>")
            ->default_value(FormatOption("text"),"text"),
          "Use json format."
      );

    po::options_description misc("Misc");
    misc.add_options()
      ("help,h", "Print help messages")
      ("verbose,v", "Log to stdout.");
    // @formatter:on
    options.add(operations).add(fields).add(formats).add(misc);

    return options;
  }

  void OptionParser::Parse(int argc,
                           const char **argv)
  {
    auto options = InitializeOptions();

    parsed_options_ = po::parse_command_line(argc, argv, options);
    po::store(parsed_options_, map_);

    if(map_.count("help") > 0)
    {
      std::cout << "\nNetwork configuration command line tool\n" << std::endl << options << std::endl;
    }

    if(map_.count("verbose") > 0)
    {
    }

    po::notify(map_);

    MutuallyExclusiveAndOnlyOnce(map_, "bridge-config", "interface-config", "device-info", "ip-config", "backup",
                                 "restore", "get-backup-parameter-count", "dsa-mode", "fix-ip", "dip-switch-config",
                                 "mac-address", "help");

    ExpectedOptionPair(map_, "bridge-config", "get", "set");
    ExpectedOptionPair(map_, "ip-config", "get", "get-current", "set");
    ExpectedOptionPair(map_, "interface-config", "get", "set");
    ExpectedOptionPair(map_, "dsa-mode", "get", "set");
    ExpectedOptionPair(map_, "dip-switch-config", "get", "set");
    ExpectedOptionPair(map_, "mac-address", "get");

    if(map_.count("netmask") > 0 and map_.count("prefix") > 0)
    {
      throw po::error(::std::string("Both options netmask and prefix are set although they are mutually exclusive"));
    }

    ExpectedValueOrOption(map_, "set", "device");

    ExpectedValueOrOption(map_, "mac-address", "device");
//    ExpectValueForGetOption(map_, "mac-address");
  }

} /* namespace network_config */
