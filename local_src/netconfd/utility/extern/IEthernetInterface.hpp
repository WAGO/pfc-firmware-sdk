// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IEthernetInterface.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include "NetworkInterfaceConstants.hpp"
#include <gsl.hpp>

namespace netconfd {

using MacAddress = ::gsl::span<const ::std::uint8_t>;

class IEthernetInterface {
 public:
  virtual ~IEthernetInterface() = default;

  virtual void UpdateConfig() = 0;
  virtual eth::EthernetMauType GetMauType() const = 0;
  virtual const MacAddress GetMac() const = 0;
  virtual bool GetAutonegSupport() const = 0;
  virtual bool GetAutonegEnabled() const = 0;
  virtual ::std::uint32_t GetAutonegCapabilities() const = 0;
  virtual eth::MediaType GetMediaType() const = 0;
  virtual eth::DeviceState GetState() const = 0;
  virtual eth::InterfaceLinkState GetLinkState() const = 0;
  virtual ::std::uint32_t GetInterfaceIndex() const = 0;
  virtual ::std::uint32_t GetAutonegCapabilitiesXdot3() const = 0;
  virtual ::std::uint32_t GetMTU() const = 0;
  virtual void Commit() = 0;
  virtual void SetAutoneg(eth::Autoneg autoneg) = 0;
  virtual void SetState(eth::DeviceState state) = 0;
  virtual void SetSpeed(::std::int32_t speed) = 0;
  virtual void SetDuplex(eth::Duplex duplex) = 0;

};


}  // namespace netconfd


//---- End of header file ------------------------------------------------------

