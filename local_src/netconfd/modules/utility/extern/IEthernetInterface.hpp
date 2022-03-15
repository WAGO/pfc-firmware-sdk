// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "NetworkInterfaceConstants.hpp"
#include "MacAddress.hpp"
#include <gsl/gsl>

namespace netconf {

class IEthernetInterface {
 public:
  virtual ~IEthernetInterface() = default;

  virtual void UpdateConfig() = 0;
  virtual MacAddress GetMac() const = 0;
  virtual bool GetAutonegSupport() const = 0;
  virtual bool GetAutonegEnabled() const = 0;
  virtual eth::MediaType GetMediaType() const = 0;
  virtual ::std::uint32_t GetSpeed() const = 0;
  virtual eth::Duplex GetDuplex() const = 0;
  virtual eth::DeviceState GetState() const = 0;
  virtual eth::InterfaceLinkState GetLinkState() const = 0;
  virtual ::std::uint32_t GetInterfaceIndex() const = 0;
  virtual ::std::uint32_t GetMTU() const = 0;
  virtual gsl::span<const uint32_t> GetSupportedLinkModes() const = 0;
  virtual void Commit() = 0;
  virtual void SetAutoneg(eth::Autoneg autoneg) = 0;
  virtual void SetState(eth::DeviceState state) = 0;
  virtual void SetSpeed(::std::int32_t speed) = 0;
  virtual void SetDuplex(eth::Duplex duplex) = 0;

};


}  // namespace netconf


//---- End of header file ------------------------------------------------------

