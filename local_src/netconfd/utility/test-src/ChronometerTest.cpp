// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Chronometer.hpp"

using namespace netconfd;
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


//---- End of source file ------------------------------------------------------

