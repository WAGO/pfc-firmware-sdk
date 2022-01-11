// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IEthernetInterfaceFactory.hpp"

namespace netconf {

class EthernetInterfaceFactory : public IEthernetInterfaceFactory {
 public:
  EthernetInterfaceFactory() = default;
  virtual ~EthernetInterfaceFactory() = default;

  ::std::unique_ptr<IEthernetInterface> getEthernetInterface(std::string name) override;

};

} /* namespace netconf */

