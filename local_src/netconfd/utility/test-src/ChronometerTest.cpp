// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Chronometer.hpp"

using namespace netconf;
using namespace testing;

TEST(Chronometer, PrintTimeInterval) {


  ChronometerStart();

  sleep(1);

  ChronometerPrintTimeMs("After one sec (Expected 1000ms)");

  sleep(1);

  ChronometerPrintTimeMs("After two sec (Expected 2000ms)");


  ChronometerStart();
  sleep(1);

  ChronometerPrintTimeMs("After reset and one sec (Expected 1000ms)");
}
