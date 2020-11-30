#include "IPConfigMock.hpp"

namespace netconf {
namespace api {

using InstancePtr = ::std::unique_ptr<MockIPConfig, void(*)(MockIPConfig*)>;

static InstancePtr activeMock = { nullptr, [](MockIPConfig*) {
} };

MockIPConfig::MockIPConfig() {
  activeMock.reset(this);
}
MockIPConfig::~MockIPConfig() {
  activeMock.reset();
}

Error GetIPConfigs(IPConfigs& config) {
  if (activeMock) {
    return activeMock->GetIPConfigs(config);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

Error GetCurrentIPConfigs(IPConfigs& config) {
  if (activeMock) {
    return activeMock->GetCurrentIPConfigs(config);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

Error SetIPConfigs(const IPConfigs &config) {
  if (activeMock) {
    return activeMock->SetIPConfigs(config);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

Error SetTempFixIp() {
  if (activeMock) {
    return activeMock->SetTempFixIp();
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");

}

}  // namespace api
}  // namespace netconf
