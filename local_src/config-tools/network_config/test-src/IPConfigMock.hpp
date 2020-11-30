
#include <gmock/gmock.h>
#include <IPConfig.hpp>

namespace netconf {
namespace api {

struct MockIPConfig {
 public:

  MockIPConfig();
  ~MockIPConfig();

  MOCK_METHOD1(GetIPConfigs,Error(IPConfigs &config) );
  MOCK_METHOD1(GetCurrentIPConfigs,Error(IPConfigs &config) );
  MOCK_METHOD1(SetIPConfigs, Error(const IPConfigs &config) );
  MOCK_METHOD0(SetTempFixIp, Error() );


};



} // namespace api
}  // namespace netconf


