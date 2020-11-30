// SPDX-License-Identifier: LGPL-3.0-or-later
#include "InterfaceInformation.hpp"
#include "JsonConverter.hpp"

namespace netconf {

InterfaceInformation::InterfaceInformation(const ::std::string &interface_name, const ::std::string &interface_label,
                                           DeviceType type, bool ip_configuration_readonly)
    :
    interface_name_ { interface_name },
    interface_label_ { interface_label },
    type_ { type },
    ip_configuration_readonly_ { ip_configuration_readonly } {
}

}  // namespace netconf
