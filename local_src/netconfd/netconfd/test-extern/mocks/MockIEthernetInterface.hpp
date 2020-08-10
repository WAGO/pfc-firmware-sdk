// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     MockIEthernetInterface.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

namespace netconf {

#include <gmock/gmock.h>

#include "IEthernetInterface.hpp"
#include "Types.hpp"

class MockIEthernetInterface : public IEthernetInterface {
 public:
  MOCK_METHOD0(UpdateConfig,
      void());
  MOCK_CONST_METHOD0(GetName,
      const ::std::string&());
  MOCK_CONST_METHOD0(GetMauType,
      eth::EthernetMauType());
  MOCK_CONST_METHOD0(GetMac,
      MacAddress());
  MOCK_CONST_METHOD0(GetAutonegSupport,
      bool());
  MOCK_CONST_METHOD0(GetAutonegEnabled,
      bool());
  MOCK_CONST_METHOD0(GetAutonegCapabilities,
      ::std::uint32_t());
  MOCK_CONST_METHOD0(GetMediaType,
      eth::MediaType());
  MOCK_CONST_METHOD0(GetState,
      eth::DeviceState());
  MOCK_CONST_METHOD0(GetLinkState,
      eth::InterfaceLinkState());
  MOCK_CONST_METHOD0(GetInterfaceIndex,
      ::std::uint32_t());
  MOCK_CONST_METHOD0(GetAutonegCapabilitiesXdot3,
      ::std::uint32_t());
  MOCK_CONST_METHOD0(GetMTU,
      ::std::uint32_t());
  MOCK_METHOD0(Commit,
      void());
  MOCK_METHOD1(SetAutoneg,
      void(eth::Autoneg autoneg));
  MOCK_METHOD1(SetState,
      void(eth::DeviceState state));
  MOCK_METHOD1(SetSpeed,
      void(::std::int32_t speed));
  MOCK_METHOD1(SetDuplex,
      void(eth::Duplex duplex));
};

}  // namespace netconf
//---- End of header file ------------------------------------------------------

