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
/// \version <Revision>: $Rev: 6632 $
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------


#include "CppUTest/TestHarness.h"
#include <cstring>
#include <cstdio>

#include "config_tool_lib.h"

extern const char * g_proc_cpuinfo;

static char s_path[256];
static char *s_path_end;

extern "C" 
{
  int GetUimageSize(const char *filename, int *size); 
};

TEST_GROUP(get_uimage_size)
{
  void setup()
  {
#ifdef __UNIT_TESTS_HOST__  
    char *slash;
    // quick hack to find out one's own s_path and locate the input files
    // using an absolute s_path
    int size = readlink("/proc/self/exe", s_path, sizeof(s_path));

    s_path[size] = '\0';
    slash = strrchr(s_path, '/');
    *slash = '\0';

    strcat(s_path, "/data_stubs/");
    s_path_end = &s_path[strlen(s_path)];
#endif
  }

  void teardown()
  {
  }
};

TEST(get_uimage_size, get_uimg_sz)
{
#ifdef __UNIT_TESTS_HOST__
  int size = -1;

  int ret;
// No decent hexeditor in stock suse installation
// to create a proper header.
//  strcpy(s_path_end, "emptyUimg");
//
//  ret = GetUimageSize(s_path, &size);
//
//  LONGS_EQUAL(ret, EXIT_SUCCESS);
//  LONGS_EQUAL(size, 0);

/////////////////////////////////////////////////////////

  strcpy(s_path_end, "invalidUimg");
  
  ret = GetUimageSize(s_path, &size);
  
  LONGS_EQUAL(ret, EXIT_FAILURE);

/////////////////////////////////////////////////////////


  strcpy(s_path_end, "bogusFile");

  ret = GetUimageSize(s_path, &size);
  LONGS_EQUAL(ret, EXIT_FAILURE);

/////////////////////////////////////////////////////////

  strcpy(s_path_end, "validUimg");
  ret = GetUimageSize(s_path, &size);
  LONGS_EQUAL(ret, EXIT_SUCCESS);
  LONGS_EQUAL(size, 3372592);
#else
  printf("TODO: need uimage stubs on target to test get_uimage_size.\n");
#endif 

}

