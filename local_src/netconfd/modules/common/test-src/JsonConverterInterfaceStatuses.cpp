//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     JsonConverterInterfaceStatuses.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "JsonConverter.hpp"

namespace netconf {

using namespace testing;

class JsonConverterInterfaceStatusesTest : public Test {
 public:
  ::std::shared_ptr<JsonConverter> parser_;

  void SetUp() override {
    parser_ = ::std::make_shared<JsonConverter>();
  }

  /* ::std::string device_name, InterfaceState state = InterfaceState::UNKNOWN, Autonegotiation autoneg =
   Autonegotiation::UNKNOWN,
   int speed = UNKNOWN_SPEED, Duplex duplex = Duplex::UNKNOWN, LinkState link_state = LinkState::UNKNOWN,
   MacAddress mac = MacAddress{} */

  InterfaceStatuses statuses_ {
    { "X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, LinkState::UP, MacAddress::FromString("00:30:DE:01:02:03") },
    { "X2", InterfaceState::UP, Autonegotiation::OFF, 100, Duplex::HALF, LinkState::UP, MacAddress::FromString("00:30:DE:01:02:04") },
    { "X3", InterfaceState::DOWN, Autonegotiation::OFF, 10, Duplex::HALF, LinkState::DOWN, MacAddress::FromString("00:30:DE:01:02:05") }
  };


};


TEST_F(JsonConverterInterfaceStatusesTest, ConvertBackAndForth){
  auto jsonstring = parser_->ToJsonString(statuses_);

  InterfaceStatuses converted;
  parser_->FromJsonString(jsonstring, converted);

  EXPECT_EQ(converted, statuses_);
}


}  // namespace netconf

//---- End of source file ------------------------------------------------------

