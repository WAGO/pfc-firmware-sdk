//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev: 65689 $
///
/// \author ${user} $Author$ : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------


#include "CppUTest/TestHarness.h"
#include <cstring>

#include "config_tool_lib.h"

extern const char * g_proc_cpuinfo;

static char g_path[256];
static char *g_path_end;

extern "C" 
{
int GetProcessorType(char* pProcessorTypeString);
};

#ifdef __UNIT_TESTS_HOST__
// Stub dummies from libtypelabel


extern "C"
{

#include <typelabel_API.h>

  tTypeLabel typelabel_OBJECT_New(void)
  {
    return NULL;
  }

  tTypelabelReturn typelabel_OBJECT_Open(tTypeLabel t)
  {
    UNUSED_PARAMETER(t);
    return (tTypelabelReturn) 0;
  }

  char *typelabel_OBJECT_GetValue(tTypeLabel t, const char *c)
  {
    UNUSED_PARAMETER(t);
    UNUSED_PARAMETER(c);
    return NULL;
  }


  void typelabel_OBJECT_Destroy(tTypeLabel t)
  {
    UNUSED_PARAMETER(t);
  }

  tTypelabelReturn  typelabel_OBJECT_Sync(tTypeLabel t)
  {
    UNUSED_PARAMETER(t);
    return (tTypelabelReturn) 0;
  }

}
#endif

TEST_GROUP(get_coupler_details)
{
  void setup()
  {
#ifdef __UNIT_TESTS_HOST__  
    char *slash;
    // quick hack to find out one's own g_path and locate the input files
    // using an absolute g_path
    int size = readlink("/proc/self/exe", g_path, sizeof(g_path));

    g_path[size] = '\0';
    slash = strrchr(g_path, '/');
    *slash = '\0';

    strcat(g_path, "/data_stubs/");
#endif
  }

  void teardown()
  {
  }
};

TEST(get_coupler_details, get_processor_type)
{
  char cpuinfo[128];

#ifdef __UNIT_TESTS_HOST__
  g_proc_cpuinfo=g_path;
  g_path_end = &g_path[strlen(g_path)];
#endif 
/*
  strcpy(g_path_end, "cmdline_geode");
  GetActivePartition(active_part, unused);
  STRCMP_EQUAL("/dev/hda1", active_part);
*/
#ifdef __UNIT_TESTS_HOST__
  strcpy(g_path_end, "cpuinfo_pentium");
  GetProcessorType(cpuinfo);
  STRCMP_EQUAL("Intel(R) Pentium(R) M processor 1.40GHz", cpuinfo);
#endif

#ifdef __UNIT_TESTS_HOST__
  strcpy(g_path_end, "cpuinfo_pac200");
#endif
  GetProcessorType(cpuinfo);
  STRCMP_EQUAL("ARMv7 Processor rev 7 (v7l)", cpuinfo);

}

// TODO: unit test for RS232-owner (iscodesys/islinux)
