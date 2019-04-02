//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev: 4371 $
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------


#include "CppUTest/TestHarness.h"
#include <cstring>
#include <cstdio>
#include <sys/stat.h>

// Existing config tools have some problematic code lines of the following kind:
// char buf[MAX_SIZE];
// strncpy(buf, string, MAX_SIZE);
//
// (the problem is when strlen(string) >= MAX_SIZE, no trailing zero is copied into buf)
//
// Moreover, there are hard-coded boundaries and functions relying on the caller using the a specific constant to initialize the return buffer:
//
// void foo(char *ret_buf)
// {
//    char source_buf[MAX_SIZE];
//    strncpy(ret_buf, source_buf, MAX_SIZE);
// }
// 
// ...
//
// void bar(void)
// {
//    char ret_buf[MAX_SIZE];
//    foo(ret_buf);
// }
//
// These code lines seem to have caused no problems in the field as of today (2012/09/11).
// Changing them may lead to side effects - "if it ain't broke, don't fix it" :)
// But when such a tool is extended or rewritten, one should pay attention and fix them.
// The correct behaviour is to be verified using the test cases in this file.

extern "C" 
{
   char* DeleteMultibleSpaces(char* pInputString,
                           int   maxLength);
   
   int SearchLineInDeviceMediumFile(char* pSearchText,
                                    char* pFoundLine);

};

TEST_GROUP(string_copy_tests)
{

  void setup()
  {  
  }

  void teardown()
  {
  }
};

char result_page[512];
char *res_buf = result_page + 16;

void reset_page(void)
{
  memset(result_page, '^', sizeof(result_page));
}

TEST(string_copy_tests, DeleteMultipleSpaces)
{
  reset_page();

  char *str = "01234567890ABCDE"; // 15 characters + trailing zero

  strcpy(res_buf, str);  

  DeleteMultibleSpaces(res_buf, strlen(str)+1);
 
  CHECK_EQUAL('^', *(res_buf-1));
  CHECK_EQUAL('\0', *(res_buf+16)); 
  CHECK_EQUAL('^', *(res_buf+17)); 
  STRCMP_EQUAL(str, res_buf); 
}

TEST(string_copy_tests, SearchLineInDeviceMediumFile)
{
  reset_page();

  char *str = "mtd2";
#ifdef __UNIT_TESTS_TARGET__
  char *line_in_file= "mtd2=internal-flash-nand"; // from real /etc/DEVICE_MEDIA on target
#else
  char *line_in_file= "mtd2=internal-flash-nandxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // 199 characters -> hard-coded maximal allowed line length (MAX_LINE_LENGTH in ..._GetLineByNr; from dummy ./data_stubs/DEVICE_MEDIA
#endif

  int ret = SearchLineInDeviceMediumFile("mtd2", res_buf);
  LONGS_EQUAL(0, ret);

  STRCMP_EQUAL(line_in_file, res_buf); 
  
  CHECK_EQUAL('^', *(res_buf-1));
  CHECK_EQUAL('\0', *(res_buf+strlen(line_in_file) + 1)); 
  CHECK_EQUAL('^',  *(res_buf+strlen(line_in_file) + (300 - strlen(line_in_file) + 1) )); // SearchLineInDeviceMediumFile strncopies up to 300 characters => \0 padding
}
