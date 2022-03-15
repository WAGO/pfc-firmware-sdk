// SPDX-License-Identifier: GPL-2.0-or-later

#include "DynamicIPClientFactory.hpp"
#include "DHCPClient.hpp"
#include "BOOTPClient.hpp"
#include "LeaseFile.hpp"

namespace netconf {

IDynamicIPClientPtr DynamicIPClientFactory::Create(DynamicIPType type, const ::std::string &itf_name,
                                                   const ::std::string &hostname, const ::std::string &vendorclass) {
  if (type == DynamicIPType::DHCP) {
    return ::std::make_shared<DHCPClient>(itf_name, hostname, vendorclass);
  }
  if (type == DynamicIPType::BOOTP) {
    return ::std::make_shared<BOOTPClient>(itf_name);
  }
  return nullptr;
}

} /* namespace netconf */
