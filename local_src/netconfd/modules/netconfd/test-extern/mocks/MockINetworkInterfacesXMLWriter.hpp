// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "INetworkInterfacesXMLWriter.hpp"

namespace netconf {

class MockINetworkInterfacesXMLWriter : public INetworkInterfacesXMLWriter {
 public:
  MOCK_CONST_METHOD1(UpdateXml, Status(const BridgeConfig& bridge_config) );
  MOCK_CONST_METHOD1(UpdateXml, Status(const IPConfigs& ip_configs) );
};

} // namespace netconf
