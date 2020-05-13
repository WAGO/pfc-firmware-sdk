#pragma once

#include <gmock/gmock.h>

#include "IIPLinks.hpp"

namespace netconfd {

class MockIIPLinks : public IIPLinks {
 public:

  MOCK_METHOD1(CreateOrGet, ::std::shared_ptr<IPLink>(const IPConfig& ip_config) );
  MOCK_METHOD1(Get, ::std::shared_ptr<IPLink>(const ::std::string &interface) );

};

}
