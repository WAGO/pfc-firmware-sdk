//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test.cpp
///
///  \brief    ToDo:
///
///  \author   NiB : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
//#include <stdlib.h>
#include <util_log.hpp>

//using namespace testing;

//------------------------------------------------------------------------------
class FilesForTesting {
  private:
    const unsigned int lineEnd = 5;
    unsigned int i = 0;
    unsigned int k = 0;
  public:
    bool create(const boost::filesystem::path & folderPath) {
      if(boost::filesystem::is_directory(folderPath)) {
        k = 1;
        for(std::string filename : filenames) {
          boost::filesystem::path filePath = folderPath / filename;
          std::ofstream file(filePath.c_str());
          for(i = lineBegin; i <= (lineEnd * k); i++) {
            file << i << testStr << std::endl;
          }
          if(file.is_open()) {
            paths.push_back(filePath);
            lines.push_back(i);
            file.close();
          }
          k++;
        }
      }
      else
      {
        return false;
      }
      return true;
    };
    bool check() {
      for(const boost::filesystem::path & path : paths) {
        if(!boost::filesystem::exists(path)) {
          return false;
        }
      }
      return true;
    };
    void remove() {
      for(const boost::filesystem::path & path : paths) {
        boost::filesystem::remove(path);
      }
    };
    const unsigned int lineBegin = 0;
    std::string testStr = ". Line: ---Test---";
    std::vector<std::string> filenames = {"afile.log",
                                          "afile.log.1",
                                          "afile.log.2",
                                          "bfile.log",
                                          "cfile.log",
                                          "dfile.log"};
    unsigned int arrayCount = filenames.size();
    std::vector<boost::filesystem::path> paths;
    std::vector<unsigned int> lines;
};
//------------------------------------------------------------------------------

static void ASSERT_EQ_VECTOR_STR(std::vector<std::string> soll,
                                 std::vector<std::string> ist) {
  ASSERT_EQ(soll.size(), ist.size());
  for(unsigned int i = 0; i < soll.size(); i++) {
    ASSERT_STREQ(soll[i].c_str(), ist[i].c_str());
    ASSERT_EQ(soll[i].size(), ist[i].size());
  }
}

static void ASSERT_EQ_VECTOR_PATH(std::vector<boost::filesystem::path> soll,
                                  std::vector<boost::filesystem::path> ist) {
  ASSERT_EQ(soll.size(), ist.size());
  for(unsigned int i = 0; i < soll.size(); i++) {
    ASSERT_STREQ(soll[i].c_str(), ist[i].c_str());
    ASSERT_EQ(soll[i].size(), ist[i].size());
  }
}

static unsigned int Count_String_Existence(const std::string & msgStr,
                                           const std::string & lineStr){
  unsigned int count = 0;
  auto pos = msgStr.find(lineStr);
  while(pos != std::string::npos) {
    count++;
    pos = msgStr.find(lineStr, pos+1);
  }
  return count;
}

static std::string uInt_To_String(const unsigned int linenumber) {
  std::stringstream buffer;
  buffer << linenumber;
  return buffer.str();
}

static unsigned int Get_String_Linenumber(const std::string & msgStr,
                                          const std::string & lineStr,
                                          const unsigned int linenumber) {
  unsigned int count = 0;
  std::string soll = uInt_To_String(linenumber);
  soll.append(lineStr);
  auto pos = msgStr.find(soll);
  if(pos != std::string::npos) {
    if(pos != 0)
    {
      auto sizeMsg = msgStr.size();
      auto sizeSoll = soll.size();
      count = sizeMsg / sizeSoll;
    }
    count = pos;
  }
  return count;
}

//------------------------------------------------------------------------------
class libutil_StringList2JsonArray: public ::testing::Test {
  protected:
    std::string retStr;
    std::string::iterator it;
    std::vector<std::string> emptyList;
    std::vector<std::string> testList = {"name", "test", "123", "!$&?#"};
    std::string testString = R"(["name","test","123","!$&?#"])";
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(libutil_StringList2JsonArray, emptyList) {
  retStr = StringList2JsonArray(emptyList);
  ASSERT_TRUE(retStr.empty());
}

TEST_F(libutil_StringList2JsonArray, testList) {
  retStr = StringList2JsonArray(testList);
  ASSERT_FALSE(retStr.empty());
  ASSERT_STREQ(testString.c_str(), retStr.c_str());
}

//------------------------------------------------------------------------------
class libutil: public ::testing::Test {
  protected:
    std::vector<std::string> retStrList;
    std::vector<boost::filesystem::path> retPathList;
    boost::filesystem::path emptyPath;
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(libutil, GetFilePaths_emptyFolderPath) {
  auto ret = GetFilePaths(emptyPath);
  ASSERT_TRUE(ret.empty());
}

TEST_F(libutil, GetFilenames_emptyFolderPath) {
  auto ret = GetFilenames(emptyPath);
  ASSERT_TRUE(ret.empty());
}

TEST_F(libutil, GetNumberOfLines_emptyFolderPath) {
  auto ret = GetNumberOfLines(emptyPath);
  ASSERT_EQ(0, ret);
}

TEST_F(libutil, ReadFileLineByLine_emptyFolderPath) {
  auto ret = ReadFileLineByLine(emptyPath, 0, std::cout);
  ASSERT_EQ(0, ret);
}

//------------------------------------------------------------------------------
class libutil_Target: public ::testing::Test {
  protected:
    boost::filesystem::path tempPath = "/tmp/gtest_log";
    FilesForTesting files;
    unsigned int limit = 0;
    unsigned int soll = 0;
    std::string output;
    void SetUp() override {
      if(boost::filesystem::exists(tempPath)) {
        boost::filesystem::remove_all(tempPath);
      }
      ASSERT_TRUE(boost::filesystem::create_directory(tempPath));
      ASSERT_TRUE(files.create(tempPath));
      ASSERT_TRUE(files.check());
    }

    void TearDown() override {
      files.remove();
      boost::filesystem::remove_all(tempPath);
    }
};

TEST_F(libutil_Target, GetFilePaths_tmpFolderPath) {
  auto ret = GetFilePaths(tempPath);
  ASSERT_EQ_VECTOR_PATH(files.paths, ret);
}

TEST_F(libutil_Target, GetFilenames_tmpFolderPath) {
  auto ret = GetFilenames(tempPath);
  ASSERT_EQ_VECTOR_STR(files.filenames, ret);
}

TEST_F(libutil_Target, GetNumberOfLines_tmpFolderPath_) {
  for(unsigned int i = 0; i < files.arrayCount; i++){
    auto ret = GetNumberOfLines(files.paths[i]);
    ASSERT_EQ(files.lines[i], ret);
  }
}

TEST_F(libutil_Target, ReadFileLineByLine_tmpFolderPath_unlimit) {
  limit = 0;
  for(unsigned int i = 0; i < files.arrayCount; i++){
      testing::internal::CaptureStdout();
      auto ret = ReadFileLineByLine(files.paths[i], limit, std::cout);
      output = testing::internal::GetCapturedStdout();
      // check return value
      ASSERT_EQ(files.lines[i], ret);
      // check count of cout
      ASSERT_EQ(ret, Count_String_Existence(output, files.testStr));
      // check begin
      ASSERT_EQ(0, Get_String_Linenumber(output, files.testStr, files.lineBegin));
  }
}

TEST_F(libutil_Target, ReadFileLineByLine_tmpFolderPath_limit_eq_fileline) {
  for(unsigned int i = 0; i < files.arrayCount; i++){
      limit = files.lines[i];
      testing::internal::CaptureStdout();
      auto ret = ReadFileLineByLine(files.paths[i], limit, std::cout);
      output = testing::internal::GetCapturedStdout();
      // check return value
      ASSERT_EQ(files.lines[i], ret);
      // check count of cout
      ASSERT_EQ(ret, Count_String_Existence(output, files.testStr));
      // check begin
      ASSERT_EQ(0, Get_String_Linenumber(output, files.testStr, files.lineBegin));
  }
}

TEST_F(libutil_Target, ReadFileLineByLine_tmpFolderPath_limit_gt_fileline) {
  for(unsigned int i = 0; i < files.arrayCount; i++){
      limit = files.lines[i] + 1;
      soll = files.lines[i];
      testing::internal::CaptureStdout();
      auto ret = ReadFileLineByLine(files.paths[i], limit, std::cout);
      output = testing::internal::GetCapturedStdout();
      // check return value
      ASSERT_EQ(soll, ret);
      // check count of cout
      ASSERT_EQ(ret, Count_String_Existence(output, files.testStr));
      // check begin
      ASSERT_EQ(0, Get_String_Linenumber(output, files.testStr, files.lineBegin));
  }
}

TEST_F(libutil_Target, ReadFileLineByLine_tmpFolderPath_limit_lt_fileline) {
  unsigned int diff = 1;
  for(unsigned int i = 0; i < files.arrayCount; i++){
      limit = files.lines[i] - diff;
      soll = limit;
      testing::internal::CaptureStdout();
      auto ret = ReadFileLineByLine(files.paths[i], limit, std::cout);
      output = testing::internal::GetCapturedStdout();
      // check return value
      ASSERT_EQ(soll, ret);
      // check count of cout
      ASSERT_EQ(ret, Count_String_Existence(output, files.testStr));
      // check begin
      ASSERT_EQ(0, Get_String_Linenumber(output, files.testStr, files.lineBegin + diff));
  }
}


