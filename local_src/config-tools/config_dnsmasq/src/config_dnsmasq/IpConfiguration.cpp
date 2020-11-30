//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Class representation of a port configuration
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "IpConfiguration.hpp"

#include <boost/format.hpp>

#include "error_handling.hpp"

IpConfiguration::IpConfiguration()
    :
    host_name { "" },
    port_name_list { },
    port_data_list { } {

}

bool IpConfiguration::portIsAvailable(const std::string &port) {
  auto p = std::find(port_name_list.begin(), port_name_list.end(), port);
  return p != port_name_list.end();
}

PortData IpConfiguration::getPortData(const std::string &port) {
  auto data = std::find_if(port_data_list.begin(), port_data_list.end(), [&port](auto d) {
    return d.port_name_ == port;
  });

  erh_assert(data != port_data_list.end(), INVALID_PARAMETER,
             boost::format("There is no ip configuration for port %s") % port);

  return *data;
}

