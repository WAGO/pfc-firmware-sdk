
#include "InterfaceConfigurationValidator.hpp"

namespace netconf {

Status InterfaceConfigurationValidator::Validate(const InterfaceConfigs &port_configs, const InterfaceInformations& iis){


    for (auto &port_config : port_configs) {

      auto [exists, interface_info_ptr] = GetFirstOf(iis, byLabel(port_config.device_name_));

      if (not exists) {
        return Status { StatusCode::INTERFACE_NOT_EXISTING, port_config.device_name_ };
      }

      if (port_config.autoneg_ != Autonegotiation::UNKNOWN) {
        auto autoneg_supported = interface_info_ptr->GetAutonegSupported();
        if (port_config.autoneg_ == Autonegotiation::ON) {
          if (autoneg_supported == AutonegotiationSupported::NO) {
            return Status { StatusCode::AUTONEGOTIATION_NOT_SUPPORTED, port_config.device_name_ };
          }
          // if auto negotiation is on we do not have to check the link mode any more
          continue;
        }
      }

      if (port_config.speed_ != InterfaceBase::UNKNOWN_SPEED && port_config.duplex_ != Duplex::UNKNOWN) {
        LinkModes link_modes = interface_info_ptr->GetSupportedlinkModes();
        auto link_mode_unsupported = ::std::find(link_modes.begin(), link_modes.end(),
                    LinkMode { port_config.speed_, port_config.duplex_ }) == link_modes.end();

        if (link_mode_unsupported) {
          ::std::string duplex = (port_config.duplex_ == Duplex::HALF) ? ::std::string("half") : ::std::string("full");
          return Status {
            StatusCode::LINK_MODE_NOT_SUPPORTED,
            port_config.device_name_, ::std::to_string(port_config.speed_), duplex };
        }
      }
    }

    return Status::Ok();
  }

}  // namespace netconf
