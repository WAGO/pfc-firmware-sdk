
#pragma once

#include "IEthernetInterfaceFactory.hpp"

namespace netconfd {

class EthernetInterfaceFactory : public IEthernetInterfaceFactory {
 public:
  EthernetInterfaceFactory() = default;
  virtual ~EthernetInterfaceFactory() = default;

  ::std::unique_ptr<IEthernetInterface> getEthernetInterface(std::string name) override;

};

} /* namespace netconfd */

