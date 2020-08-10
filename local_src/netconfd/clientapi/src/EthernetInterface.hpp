// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <sys/socket.h>
#include <linux/if.h>

#include "InterfaceConfig.hpp"
#include "NetworkInterfaceConstants.hpp"
#include "Socket.h"

namespace netconf {
namespace api {

class EthernetInterface {
 public:
  explicit EthernetInterface(::std::string name);
  explicit EthernetInterface(::std::uint32_t ifindex);
  EthernetInterface(EthernetInterface &&other) noexcept;

  const MacAddress GetMac() const;
  static ::std::string IndexToName(::std::uint32_t ifindex);

 private:
  void InitializeData();

  MacAddress mac_;

  ::std::string name_;
  ::std::uint32_t if_index_;

  ::ifreq ifreq_;
  Socket socket_;
};

}  // namespace api
}  // namespace pfcspecific

