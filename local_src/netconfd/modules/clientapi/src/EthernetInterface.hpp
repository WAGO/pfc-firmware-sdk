// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <sys/socket.h>
#include <linux/if.h>

#include "InterfaceConfig.hpp"
#include "NetworkInterfaceConstants.hpp"
#include "Socket.hpp"

namespace netconf {
namespace api {

class EthernetInterface {
 public:
  explicit EthernetInterface(::std::string name);
  explicit EthernetInterface(::std::uint32_t ifindex);
  EthernetInterface(EthernetInterface &&other) noexcept;

  const MacAddress GetMac() const;
  static ::std::string IndexToName(::std::uint32_t ifindex);
  void SetState(InterfaceState s);
  InterfaceState GetState();

 private:
  void InitializeData();

  MacAddress mac_{};

  ::std::string name_{};
  ::std::uint32_t if_index_{};
  uint16_t if_flags_{};

  ::ifreq ifreq_{};
  Socket socket_{};
};

}  // namespace api
}  // namespace pfcspecific

