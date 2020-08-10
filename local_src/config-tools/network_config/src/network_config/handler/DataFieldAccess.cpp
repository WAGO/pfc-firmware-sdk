#include "DataFieldAccess.hpp"
#include "DataField.hpp"
#include "IPConfig.hpp"

namespace network_config {

auto interface_information_label_get = [](const auto &info) -> ::std::string {
  return info.GetInterfaceLabel();
};
auto interface_information_name_get = [](const auto &info) -> ::std::string {
  return info.GetInterfaceName();
};
auto interface_information_type_get = [](const auto &info) -> ::std::string {
  return ::netconf::ToString(info.GetType());
};

auto interface_information_ipro_get = [](const auto &info) -> ::std::string {
  return info.IsIpConfigurationReadonly() ? "true" : "false";
};

template<>
DataFieldAccess<::netconf::InterfaceInformation>::DataFieldAccess()
    :
    field_map_ {
      { "label", DataField<::netconf::InterfaceInformation>{interface_information_label_get} },
      { "name", DataField<::netconf::InterfaceInformation>{interface_information_name_get} },
      { "type", DataField<::netconf::InterfaceInformation>{interface_information_type_get} },
      { "ip-ro", DataField<::netconf::InterfaceInformation>{interface_information_ipro_get} },
} {

}


} /* namespace network_config */
