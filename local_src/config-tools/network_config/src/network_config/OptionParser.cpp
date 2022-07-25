// SPDX-License-Identifier: GPL-2.0-or-later

#include "OptionParser.hpp"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Utils.hpp"

using std::string;

namespace po = boost::program_options;
namespace network_config {

OptionParser::OptionParser()
    : options_ { GetOptions() },
      fields_ { GetFields() },
      descriptions_ { CreateDescriptions() },
      parsed_options_ { nullptr } {
}

template<typename T>
static void ValidateOption(boost::any &v, ::std::vector<std::string> const &values,
                           const ::std::function<bool(::std::string)> &predicate) {
  po::validators::check_first_occurrence(v);
  const std::string &s = po::validators::get_single_string(values);

  if (predicate(s)) {
    v = boost::any(T(s));
  } else {
    throw po::validation_error(po::validation_error::invalid_option_value);  //NOLINT(cert-err60-cpp) see: https://www.boost.org/doc/libs/1_65_1/doc/html/program_options/howto.html
  }
}

static void validate(boost::any &v, std::vector<std::string> const &values, FormatOption *format, int i) {
  (void) format;
  (void) i;
  ValidateOption<FormatOption>(v, values, [](const ::std::string &s) {
    return s == "text" || s == "json" || s == "pretty-json";
  });
}

static void validate(boost::any &v, std::vector<std::string> const &values, StateOption *state, int i) {
  (void) state;
  (void) i;
  ValidateOption<StateOption>(v, values, [](const ::std::string &s) {
    return s == "up" || s == "down";
  });
}

static void validate(boost::any &v, std::vector<std::string> const &values, AutonegOption *autoneg, int i) {
  (void) autoneg;
  (void) i;
  ValidateOption<AutonegOption>(v, values, [](const ::std::string &s) {
    return s == "on" || s == "off";
  });
}

static void validate(boost::any &v, std::vector<std::string> const &values, SpeedOption *speed, int i) {
  (void) speed;
  (void) i;
  ValidateOption<SpeedOption>(v, values, [](const ::std::string &s) {
    return s == "10" || s == "100";
  });
}

static void validate(boost::any &v, std::vector<std::string> const &values, DuplexOption *duplex, int i) {
  (void) duplex;
  (void) i;
  ValidateOption<DuplexOption>(v, values, [](const ::std::string &s) {
    return s == "full" || s == "half";
  });
}

static void ExpectValueOrField(const po::variables_map &map, const ::std::string &option, const ::std::string &field) {
  if (map.count(option) == 0) {
    return;
  }
  auto value = map[option].as<::std::string>();
  if (value.empty() and 0 == map.count(field)) {
    throw po::error("Neither the option '" + option + "' has a value nor the field '" + field + "' is set");
  }
}

po::options_description OptionParser::CreateDescriptions() const {
  // @formatter:off
  po::options_description options("Primary options");
  options.add_options()
    (options_.bridge_config.name, options_.bridge_config.description)
    (options_.interface_config.name, options_.interface_config.description)
    (options_.ip_config.name, options_.ip_config.description)
    (options_.dip_switch_config.name, options_.dip_switch_config.description)
    (options_.mac_address.name, options_.mac_address.description)
    (options_.device_info.name, options_.device_info.description)
    (options_.interface_status.name, options_.interface_status.description)
    (
        options_.backup.name,
        po::value<::std::string>()->value_name("file path"),
        options_.backup.description
    )
    (
        options_.restore.name,
        po::value<::std::string>()->value_name("file path"),
        options_.restore.description
    )
    (options_.get_backup_parameter_count.name, options_.get_backup_parameter_count.description)
    (options_.dsa_mode.name, options_.dsa_mode.description)
    (options_.fix_ip.name, options_.fix_ip.description)
    (options_.dynamic_ip_event.name, options_.dynamic_ip_event.description)
    (options_.reload_host_conf.name, options_.reload_host_conf.description);

  po::options_description operations("Operations");
  operations.add_options()
    (
        options_.get.name,
        po::value<::std::string>()
          ->value_name("<value>")
          ->implicit_value(""),
        options_.get.description
    )
    (
        options_.get_current.name,
        po::value<::std::string>()
          ->value_name("<value>")
          ->implicit_value(""),
          options_.get_current.description)
    (
        options_.set.name,
        po::value<::std::string>()
          ->value_name("<value>")
          ->implicit_value(""),
        options_.set.description
    )
    (
        options_.filter.name,
        po::value<::std::string>()
          ->value_name("<value>")
          ->implicit_value(""),
        options_.filter.description
    );

  po::options_description fields("Fields");
  fields.add_options()

    ( fields_.field.name,
      po::value<std::vector<::std::string>>()->value_name(fields_.device.parameter)->multitoken(),
      fields_.field.description)
    ( fields_.device.name,
      po::value<::std::string>()->value_name(fields_.device.parameter),
      fields_.device.description)
    ( fields_.source.name,
      po::value<::std::string>()->value_name(fields_.source.parameter)->implicit_value(""),
      fields_.source.description)
    ( fields_.address.name,
      po::value<::std::string>()->value_name(fields_.address.parameter)->implicit_value(""),
      fields_.address.description)
    ( fields_.netmask.name,
      po::value<::std::string>()->value_name(fields_.netmask.parameter)->implicit_value(""),
      fields_.netmask.description)
    ( fields_.prefix.name,
      po::value<::std::string>()->value_name(fields_.prefix.parameter)->implicit_value(""),
      fields_.prefix.description)
    ( fields_.state.name,
      po::value<StateOption>()->value_name(fields_.state.parameter)->implicit_value(StateOption{""}),
      fields_.state.description)
    ( fields_.autoneg.name,
      po::value<AutonegOption>()->value_name(fields_.autoneg.parameter)->implicit_value(AutonegOption{""}),
      fields_.autoneg.description)
    ( fields_.speed.name,
      po::value<SpeedOption>()->value_name(fields_.speed.parameter)->implicit_value(SpeedOption{""}),
      fields_.speed.description)
    ( fields_.duplex.name,
      po::value<DuplexOption>()->value_name(fields_.duplex.parameter)->implicit_value(DuplexOption{""}),
      fields_.duplex.description)
    ( fields_.backup_version.name,
      po::value<::std::string>()->value_name(fields_.backup_version.parameter),
      fields_.backup_version.description)
    ( fields_.action.name,
      po::value<::std::string>()->value_name(fields_.action.parameter),
      fields_.action.description
  );

  po::options_description formats("Format");
  formats.add_options()
    (
        options_.format.name,
        po::value<FormatOption>()
          ->value_name("<format>")
          ->default_value(FormatOption("text"), "text"),
        options_.format.description
    );

  po::options_description misc("Miscellaneous options");
  misc.add_options()
    (options_.help.name, options_.help.description)
    (options_.quiet.name, options_.quiet.description)
    (options_.error_msg_dst.name,
     po::value<::std::string>()->value_name(options_.error_msg_dst.parameter),
     options_.error_msg_dst.description)
    (options_.dryrun.name, options_.dryrun.description);
  // @formatter:on
  options.add(operations).add(fields).add(formats).add(misc);

  return options;
}

void OptionParser::Parse(int argc, const char **argv) {
  parsed_options_ = po::parse_command_line(argc, argv, descriptions_);
  map_.clear();
  po::store(parsed_options_, map_);
  po::notify(map_);

  MutuallyExclusiveAndOnlyOnce(map_, options_.bridge_config, options_.interface_config, options_.ip_config,
                               options_.dip_switch_config, options_.mac_address, options_.device_info,
                               options_.interface_status, options_.backup, options_.restore,
                               options_.get_backup_parameter_count, options_.dsa_mode, options_.fix_ip,
                               options_.dynamic_ip_event, options_.reload_host_conf, options_.help);

  OptionalAndMutuallyExclusive(map_, options_.error_msg_dst, options_.quiet);

  ExpectOptionPair(map_, options_.bridge_config, options_.get, options_.set);
  ExpectOptionPair(map_, options_.ip_config, options_.get, options_.get_current, options_.set);
  ExpectOptionPair(map_, options_.interface_config, options_.get, options_.set);
  ExpectOptionPair(map_, options_.dsa_mode, options_.get, options_.set);
  ExpectOptionPair(map_, options_.dip_switch_config, options_.get, options_.set);
  ExpectOptionPair(map_, options_.mac_address, options_.get);

  if (IsSet(fields_.netmask.name) and IsSet(fields_.prefix.name)) {
    throw po::error(::std::string("Both options netmask and prefix are set although they are mutually exclusive"));
  }

  ExpectValueOrField(map_, options_.set.name, fields_.device.name);
  ExpectValueOrField(map_, options_.mac_address.name, fields_.device.name);
}

bool OptionParser::IsSet(const ::std::string &option) const {
  return Contains(map_, option);
}

void OptionParser::PrintHelp() const {
  std::cout << "\nNetwork configuration command line tool\n" << std::endl << descriptions_ << std::endl;
}

} /* namespace network_config */
