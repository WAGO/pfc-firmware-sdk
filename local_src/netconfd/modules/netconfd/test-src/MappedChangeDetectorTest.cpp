// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * MappedChangeDetectorTest.cpp
 *
 *  Created on: 14.10.2019
 *      Author: u014487
 */

#include <gtest/gtest.h>
#include <string>
#include "InterfaceManager/MappedChangeDetector.hpp"

namespace netconf {

using std::string;

TEST(MappedChangeDetectorTest, SomeChanges)
{

  MappedChangeDetector<string, bool> cd;

  EXPECT_TRUE(cd.Change("a", true));
  EXPECT_TRUE(cd.Change("a", false));
  EXPECT_FALSE(cd.Change("a", false));
  EXPECT_TRUE(cd.Change("a", true));
  EXPECT_TRUE(cd.Change("b", false));
  EXPECT_FALSE(cd.Change("b", false));


}



}  // namespace netconf
