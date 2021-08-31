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

Status GetIPConfigs(IPConfigs& config) {
  if (activeMock) {
    return activeMock->GetIPConfigs(config);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

Status GetCurrentIPConfigs(IPConfigs& config) {
  if (activeMock) {
    return activeMock->GetCurrentIPConfigs(config);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

Status SetIPConfigs(const IPConfigs &config) {
  if (activeMock) {
    return activeMock->SetIPConfigs(config);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

Status SetTempFixIp() {
  if (activeMock) {
    return activeMock->SetTempFixIp();
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");

}

}  // namespace api
}  // namespace netconf
