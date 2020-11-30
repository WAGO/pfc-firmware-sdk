// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gsl/gsl>
#include <memory>

#include "MessagePrinter.hpp"
#include "Error.hpp"
#include "StreamTee.hpp"

using namespace testing;

using namespace netconf;


namespace network_config {

class MessagePrinterTest : public ::testing::Test {
 public:

  ::std::string output_file = "./output.txt";
  MessagePrinter sut_;


 void SetUp() override{
  sut_.SetDestination(::std::make_unique<FileOutput>(output_file));
 }

 void TearDown() override{
   ::std::remove(output_file.c_str());
 }

 ::std::string GetLogFileText(){
   std::string str;
   std::ifstream t(output_file);

   t.seekg(0, std::ios::end);
   str.reserve(::gsl::narrow_cast<::std::string::size_type>(t.tellg()));
   t.seekg(0, std::ios::beg);

   str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
   return str;
 }

};


TEST_F(MessagePrinterTest, PrintDefaultToStdError)
{
  StreamTee t{::std::cerr, output_file};

  MessagePrinter sut;
  sut.Print("Hello MessagePrinter");
  EXPECT_EQ("Hello MessagePrinter", GetLogFileText());
}

TEST_F(MessagePrinterTest, PrintToOtherOutput)
{
  sut_.Print("Hello MessagePrinter");
  EXPECT_EQ("Hello MessagePrinter", GetLogFileText());
}

TEST_F(MessagePrinterTest, PrintErrorObject)
{
  sut_.Print(Error{ErrorCode::GENERIC_ERROR, "Something went wrong"});
  EXPECT_EQ("Error: Something went wrong", GetLogFileText());
}

TEST_F(MessagePrinterTest, ErrorOkIsNotPrinted)
{
  sut_.Print(Error{ErrorCode::OK});
  EXPECT_EQ("", GetLogFileText());
}


}  // namespace netconf
