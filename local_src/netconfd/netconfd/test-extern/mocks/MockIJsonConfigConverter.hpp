#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIJSONCONFIGCONVERTER_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIJSONCONFIGCONVERTER_HPP_

#include <gmock/gmock.h>

#include "IJsonConfigConverter.hpp"

namespace netconfd {

class MockIJsonConfigConverter : public IJsonConfigConverter {
 public:
  MOCK_CONST_METHOD2(JsonToBridgeConfig,
      Status(const ::std::string& json, BridgeConfig& config))
  ;MOCK_CONST_METHOD2(JsonToIPConfigs,
      Status(const ::std::string& json, IPConfigs& config))
  ;MOCK_CONST_METHOD2(BridgeConfigToJson,
      Status(const BridgeConfig& config, ::std::string& json))
  ;MOCK_CONST_METHOD1(IPConfigsToJson,
      ::std::string(const IPConfigs& config))
  ;MOCK_CONST_METHOD1(InterfacesToJson,
      ::std::string(const Interfaces& interfaces))
  ;MOCK_CONST_METHOD1(InterfaceConfigToJson,
      ::std::string(const InterfaceConfigs& port_configs))
  ;MOCK_CONST_METHOD2(InterfaceConfigFromJson,
      Status(const ::std::string& json, InterfaceConfigs& port_configs))
  ;
};

}  // namespace netconfd
#endif 
