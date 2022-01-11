
#include "FixIP.hpp"
#include "TypesHelper.hpp"

namespace netconf {

void SetFixedIps(IPConfigs &ip_configs) {
  Address baseip = { "192.168.1.17" };
  for (auto &ip_config : ip_configs) {
    auto bridgeIndexOpt = ExtractInterfaceIndex(ip_config.interface_);
    if (bridgeIndexOpt) {
      ip_config.address_ = IpAddressV4Increment(baseip, 0x100 * bridgeIndexOpt.value());
      ip_config.netmask_ = "255.255.255.0";
      ip_config.source_ = IPSource::FIXIP;
    }
  }
}

}  // namespace netconfd
