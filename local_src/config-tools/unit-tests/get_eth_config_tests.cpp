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
#include <iostream>

using namespace std;

#include "config_tool_lib.h"

extern const char * g_proc_cpuinfo;
extern const char * g_settings_conf_file;

static char g_path[256];
static char *g_path_end;

extern "C" 
{
  int GetDeviceId(char* pPortString, 
                  char* pSearchString,
                  int   searchLineNr,
                  char* pOutputString);

  int GetState(char* pPortString, 
               char* pSearchString,
               int   searchLineNr,
               char* pOutputString);

  int GetInterfacesParameter(char* pPortString, 
                             char* pSearchString,
                             int   searchLineNr,
                             char* pOutputString);
};

TEST_GROUP(get_eth_config)
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

// FIXME: Doesn't work on host because of a hard-coded call to get_coupler_details inside GetDeviceId
#ifndef __UNIT_TESTS_HOST__
TEST(get_eth_config, get_device_id)
{
  char port_id[128];
  char *unused1;
  int   unused2;

//#ifdef __UNIT_TESTS_HOST__
//  g_proc_cpuinfo=g_path;
//  g_path_end = &g_path[strlen(g_path)];
//#endif 
//
//#ifdef __UNIT_TESTS_HOST__
//  strcpy(g_path_end, "cpuinfo_geode");
//  
//  GetDeviceId("eth0", unused1, unused2, port_id);
//  STRCMP_EQUAL("X9", port_id);
//
//  GetDeviceId("eth1", unused1, unused2, port_id);
//  STRCMP_EQUAL("X8", port_id);
//
//  strcpy(g_path_end, "cpuinfo_pentium");
//
//  GetDeviceId("eth0", unused1, unused2, port_id);
//  STRCMP_EQUAL("X8", port_id);
//
//  GetDeviceId("eth1", unused1, unused2, port_id);
//  STRCMP_EQUAL("X9", port_id);
//
//#endif
//
//#ifdef __UNIT_TESTS_HOST__
//  strcpy(g_path_end, "cpuinfo_pac200");
//#endif
  cout << "get_device_id test may not work for sth. other than PFC200." << endl;
  
  GetDeviceId("eth0", unused1, unused2, port_id);
  STRCMP_EQUAL("X2", port_id);

  GetDeviceId("eth1", unused1, unused2, port_id);
  STRCMP_EQUAL("X1", port_id);
}
#endif
// This one is rather ugly as it needs a manually changed
// eth_settings.conf when run on target.
// To make things worse, it only supports PFC200 at the moment.
TEST(get_eth_config, get_eth_settings)
{
  char param[128];

#ifdef __UNIT_TESTS_HOST__
  g_settings_conf_file=g_path;
  g_path_end = &g_path[strlen(g_path)];
  strcpy(g_path_end, "eth_settings.conf");
#else
  cout << "get_eth_settings test may not work for sth. other than PFC200." << endl;
#endif 
  int res = GetState("X1", "state", 0, param);
//  STRCMP_EQUAL("on", param);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("enabled", param);

  res = GetState("X2", "state", 0, param);
  LONGS_EQUAL(SUCCESS, res);
//  STRCMP_EQUAL("off", param);
  STRCMP_EQUAL("disabled", param);

  res = GetInterfacesParameter("X1", "autoneg ", 5, param);
  LONGS_EQUAL(SUCCESS, res);
//  STRCMP_EQUAL("on", param);
  STRCMP_EQUAL("enabled", param);

  res = GetInterfacesParameter("X2", "autoneg ", 5, param);
  LONGS_EQUAL(SUCCESS, res);
//  STRCMP_EQUAL("off", param);
  STRCMP_EQUAL("enabled", param);

  res = GetInterfacesParameter("X1", "speed ", 5, param);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("100M", param);

  res = GetInterfacesParameter("X2", "speed ", 5, param);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("100M", param);

  res = GetInterfacesParameter("X1", "duplex ", 5, param);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("full", param);

  res = GetInterfacesParameter("X2", "duplex ", 5, param);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("full", param);

  // When called with "eth0" on PFC200, get_eth_config
  // shall always return "enabled" (no way to disable it via UI)
  res = GetState("eth0", "", 0, param);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("enabled", param);
}

#ifndef __UNIT_TESTS_HOST__
TEST(get_eth_config, get_ip_address)
{
  cout << "get_ip_address test may not work for sth. other than PFC200." << endl;

  char param[128];
  char searchString[128] = "eth";

  int res = GetInterfacesParameter("eth0", searchString, 1, param);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("192.168.2.17", param);
 
}
#endif
