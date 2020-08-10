#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKINETWORKINTERFACESXMLWRITER_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKINETWORKINTERFACESXMLWRITER_HPP_

#include <gmock/gmock.h>
#include "INetworkInterfacesXMLWriter.hpp"

namespace netconf {

class MockINetworkInterfacesXMLWriter : public INetworkInterfacesXMLWriter {
 public:
  MOCK_CONST_METHOD1(UpdateXml, Status(const BridgeConfig& bridge_config) );
  MOCK_CONST_METHOD1(UpdateXml, Status(const IPConfigs& ip_configs) );
};

}
  // namespace netconf
#endif 
