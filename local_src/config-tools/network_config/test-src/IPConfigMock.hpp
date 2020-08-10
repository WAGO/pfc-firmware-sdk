
#include <gmock/gmock.h>
#include "IPConfig.hpp"

namespace netconf {
namespace api {

struct MockIPConfig {
 public:

  MockIPConfig();
  ~MockIPConfig();

  MOCK_METHOD0(GetIPConfigs,IPConfigs() );
  MOCK_METHOD0(GetCurrentIPConfigs,IPConfigs() );
  MOCK_METHOD1(SetIPConfigs, Status(const IPConfigs &config) );
  MOCK_METHOD0(SetTempFixIp, void() );


};



} // namespace api
}  // namespace netconf


