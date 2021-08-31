
#include <gmock/gmock.h>
#include <IPConfig.hpp>

namespace netconf {
namespace api {

struct MockIPConfig {
 public:

  MockIPConfig();
  ~MockIPConfig();

  MOCK_METHOD1(GetIPConfigs,Status(IPConfigs &config) );
  MOCK_METHOD1(GetCurrentIPConfigs,Status(IPConfigs &config) );
  MOCK_METHOD1(SetIPConfigs, Status(const IPConfigs &config) );
  MOCK_METHOD0(SetTempFixIp, Status() );


};



} // namespace api
}  // namespace netconf


