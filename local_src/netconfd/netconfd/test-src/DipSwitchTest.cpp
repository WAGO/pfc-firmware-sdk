/*
 * DipSwitchTest.cpp
 *
 *  Created on: 09.08.2019
 *      Author: u014487
 */



#include <gtest/gtest.h>
#include "DipSwitch.hpp"

#include <boost/filesystem.hpp>

namespace netconf {

using namespace boost::filesystem;
using namespace std::string_literals;

static void PrepareDipFile(std::string const& file, std::string const& value)
{
  auto dip_sw = path{file};
  ofstream file_stream{dip_sw, std::ios_base::openmode::_S_out};

  file_stream << value;
  file_stream.flush();
  file_stream.close();
}

TEST(TheDipSwitch, ValueIsZeroWhenNoDipSwitchIsPresent)
{
  auto sut = DipSwitch("/tmp/not/existing/dip/file");

  EXPECT_EQ(0, sut.GetValue());

}

TEST(TheDipSwitch, ValueIsZeroWhenFileContainsCrap)
{
  PrepareDipFile("/tmp/dip-switch","Not a valid file content" );

  auto sut = DipSwitch("/tmp/dip-switch");

  EXPECT_EQ(0, sut.GetValue());

}


TEST(TheDipSwitch, ValueIsCorrectWhenFileExists)
{
  PrepareDipFile("/tmp/dip-switch","0x25" );

  auto sut = DipSwitch("/tmp/dip-switch");

  EXPECT_EQ(0x25, sut.GetValue());

}

TEST(TheDipSwitch, ValueIsZeroOnEmptyFile)
{

  PrepareDipFile("/tmp/dip-switch", "");

  auto sut = DipSwitch("/tmp/dip-switch");

  EXPECT_EQ(0, sut.GetValue());

}

TEST(TheDipSwitch, WorksWithOverflows)
{

  PrepareDipFile("/tmp/dip-switch", "0x401");

  auto sut = DipSwitch("/tmp/dip-switch");

  EXPECT_EQ(static_cast<uint8_t>(1), sut.GetValue());

}

TEST(TheDipSwitch, WorksWithHexValues)
{

  PrepareDipFile("/tmp/dip-switch", "0x12");

  auto sut = DipSwitch("/tmp/dip-switch");

  EXPECT_EQ(static_cast<uint8_t>(18), sut.GetValue());

}

TEST(TheDipSwitch, ReachesItsMaximum)
{

  PrepareDipFile("/tmp/dip-switch", "0xFF");

  auto sut = DipSwitch("/tmp/dip-switch");

  EXPECT_TRUE(sut.HasReachedMaxValue());

}




}  // namespace netconf
