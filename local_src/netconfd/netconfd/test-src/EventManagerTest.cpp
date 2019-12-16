// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     EventManagerTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "EventManager.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

namespace netconfd {

class AnEventManager : public Test {
 public:
  AnEventManager() {

  }

  void SetUp() override {
  }

  void TearDown() override {

  }

};

TEST_F(AnEventManager, TestName) {

}

} /* namespace netconfd */
