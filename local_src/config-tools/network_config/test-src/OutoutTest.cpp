

#include <gtest/gtest.h>
#include <gsl/gsl>
#include <fstream>
#include "Output.hpp"
#include "StreamTee.hpp"

namespace network_config {



class OutputTest : public ::testing::Test {
 public:

  ::std::string output_file = "./output.txt";


 void SetUp() override{

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

 void PutToLogFile(::std::string text){
   std::ofstream t(output_file);
   t << text;
   t.flush();
   t.close();
   ASSERT_EQ(text, GetLogFileText());
 }

};


TEST_F(OutputTest, StderrorPrint){
  StreamTee t{::std::cerr, output_file};

  StderrOutput sut;
  sut.GetStream() << "Hello StderrorPrint" << ::std::endl;

  EXPECT_EQ("Hello StderrorPrint\n", GetLogFileText());
}

TEST_F(OutputTest, FilePrint){
  FileOutput sut{output_file};
  sut.GetStream() << "Hello FilePrint" << ::std::endl;

  EXPECT_EQ("Hello FilePrint\n", GetLogFileText());
}

TEST_F(OutputTest, FilePrintOverridesFormerContent){
  PutToLogFile("SomeOtherStuffinFile");

  FileOutput sut{output_file};
  sut.GetStream() << "Hello FilePrint" << ::std::endl;

  EXPECT_EQ("Hello FilePrint\n", GetLogFileText());
}

TEST_F(OutputTest, NullPrint){
  StreamTee t{::std::cout, output_file};
  StreamTee t2{::std::cerr, output_file};

  NullOutput sut{};
  sut.GetStream() << "Hello FilePrint" << ::std::endl;
  sync();
  EXPECT_EQ("", GetLogFileText());
}

}  // namespace network_config



