//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#include "CppUTest/TestHarness.h"
#include <cstring>
#include <cstdio>

static char g_path[256];

// copied to not have to include config_tools_lib.h
enum eUdevQueryId{
   UDEV_QUERY_BLOCK_PARTITIONS,
   UDEV_QUERY_BLOCK_DEVICES,
   UDEV_QUERY_MTD_PARTITIONS,
   UDEV_QUERY_MTD_DEVICES
}; 

enum eStatusCode
{
  ERROR                           = -1,
  SUCCESS                         = 0,
  MISSING_PARAMETER,
  INVALID_PARAMETER,
  FILE_OPEN_ERROR,
  FILE_READ_ERROR,
  FILE_CLOSE_ERROR,
  NOT_FOUND,
  SYSTEM_CALL_ERROR,
  CONFIG_FILE_INCONSISTENT,
  TIMEOUT
};

extern "C"{
#include "CppUTestExt/MockSupport_c.h"
extern int GetPartitionByIndex(char*, int);

static int RunUdevQuery(char *buf, 
                        size_t bufSize, 
                        enum eUdevQueryId id, 
                        unsigned int reqIndex)
  {

    (void) buf;
    (void) bufSize;
    (void) id;
    (void) reqIndex;

    mock_c()->actualCall("RunUdevQuery")
            ->withStringParameters("buf", buf)
            ->withIntParameters("bufSize", sizeof(buf))
            ->withIntParameters("id", UDEV_QUERY_BLOCK_PARTITIONS)
            ->withIntParameters("reqIndex", 0);

    strcpy(buf, "/dev/XXX");

    return 1;
  }

static void testMock(int test)
{
  printf("Mock: %d", test);
  mock_c()->actualCall("testMock")->withIntParameters("test", 42);
}

};

// local define in get_filesystem_data.c
#define MAX_LENGTH_OUTPUT_STRING 100 

TEST_GROUP(mock_example)
{
  char buf[MAX_LENGTH_OUTPUT_STRING];
  int res;
  
  void setup()
  {  
    char *slash;
    // quick hack to find out one's own g_path and locate the input files
    // using an absolute g_path
    int size = readlink("/proc/self/exe", g_path, sizeof(g_path));

    g_path[size] = '\0';
    slash = strrchr(g_path, '/');
    *slash = '\0';

    strcat(g_path, "/data_stubs/");

  }

  void teardown()
  {
    mock_c()->clear();
  }


};

IGNORE_TEST(mock_example, first)
{
  mock_c()->expectOneCall("testMock")
          ->withIntParameters("test", 42);

  mock_c()->expectOneCall("RunUdevQuery")
          ->withStringParameters("buf", buf)
          ->withIntParameters("bufSize", sizeof(buf))
          ->withIntParameters("id", UDEV_QUERY_BLOCK_PARTITIONS)
          ->withIntParameters("reqIndex", 0);

  mock_c()->expectOneCall("RunUdevQuery")
          ->withStringParameters("buf", buf)
          ->withIntParameters("bufSize", sizeof(buf))
          ->withIntParameters("id", UDEV_QUERY_MTD_PARTITIONS)
          ->withIntParameters("reqIndex", 0);

  res = GetPartitionByIndex(buf, 0);

  mock_c()->checkExpectations();
  LONGS_EQUAL(SUCCESS, res);
}
