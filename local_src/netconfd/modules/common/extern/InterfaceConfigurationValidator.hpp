
#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

struct InterfaceConfigurationValidator {

  static Status Validate(const InterfaceConfigs &port_configs, const InterfaceInformations& iis);

};

}  // namespace netconf
