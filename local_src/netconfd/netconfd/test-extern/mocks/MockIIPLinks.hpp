#pragma once

#include <gmock/gmock.h>

#include "IIPLinks.hpp"

namespace netconf {

class MockIIPLinks : public IIPLinks {
 public:

  MOCK_METHOD1(CreateOrGet, ::std::shared_ptr<IPLink>(const ::std::string& ip_config) );
  MOCK_METHOD1(Get, ::std::shared_ptr<IPLink>(const ::std::string &interface) );

};

}
