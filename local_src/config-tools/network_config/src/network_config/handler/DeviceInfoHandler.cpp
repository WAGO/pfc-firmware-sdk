// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceInfoHandler.hpp"

#include <iostream>
#include <string>
#include <exception>
#include <boost/optional/optional.hpp>

#include "InterfaceInformationApi.hpp"
#include "Utils.hpp"
#include "OptionStrings.hpp"
#include "DataFieldAccess.hpp"

namespace po = boost::program_options;

namespace network_config {

using ::netconf::api::InterfaceInformations;
using ::netconf::api::InterfaceInformation;

DeviceInfoHandler::DeviceInfoHandler(const po::variables_map &vm)
    : vm_ { vm } {
}

static void KeepInfosOfName(InterfaceInformations &infos, const ::boost::optional<::std::string> &keep_name) {
  if (keep_name) {
    auto not_mathing_name_or_label = [name=*keep_name](const InterfaceInformation &info) {
      return !((name == info.GetInterfaceName()) || (name == info.GetInterfaceLabel()));
    };

    infos.erase(std::remove_if(infos.begin(), infos.end(), not_mathing_name_or_label), infos.end());
  }
}

void DeviceInfoHandler::Execute() {
  const auto &options = GetOptions();
  const auto &field_opts = GetFields();
  auto format = GetFormat(vm_);
  ::boost::optional<::netconf::DeviceType> type_filter_opt;
  ::boost::optional<::std::string> device_opt;

  if (Contains(vm_, options.filter)) {
    type_filter_opt = netconf::DeviceTypeFromString(Get(vm_, options.filter));
  }
  if (Contains(vm_, field_opts.device)) {
    device_opt = GetDevice(vm_);
  }

  auto infos =
      type_filter_opt ?
          netconf::api::GetInterfaceInformation(*type_filter_opt) : netconf::api::GetInterfaceInformation();

  KeepInfosOfName(infos, device_opt);

  if (format == "text") {
    auto fields = GetValuesOf(vm_, field_opts.field.name);
    if (!fields) {
      ::std::cout << netconf::api::ToString(infos);
    } else {
      for (const auto &info : infos) {
        for (const auto &field : fields.get()) {
          ::std::cout << dfa_.Get(info, field) << ' ';
        }
      }
    }
  } else if (format == "json") {
    ::std::cout << netconf::api::ToJson(infos);
  } else if (format == "pretty-json") {
    ::std::cout << netconf::api::ToPrettyJson(infos);
  }
}

} /* namespace network_config */
