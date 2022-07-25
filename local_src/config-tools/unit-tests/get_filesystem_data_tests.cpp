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
#include <cstdio>
#include <cstdlib>

#include "config_tool_lib.h"

extern const char * g_proc_cmdline;
extern const char * g_proc_partitions;
extern const char * g_pac_eeprom;

#ifdef __UNIT_TESTS_HOST__
static char g_path[256];
static char *g_path_end;
#endif

extern "C" 
{
  extern int GetActivePartition(char *pOutputString, int additionalParam);
  extern int GetActiveDevice(char *pOutputString, int additionalParam);
  extern int GetHomeDevice(char *pOutputString, int additionalParam);
  extern int GetDeviceByIndex(char *pOutputString, int index);
  extern int GetPartitionByIndex(char *pOutputString, int index);
  extern int GetPartitionByIndexExt(char *pOutputString, int index);
  extern int GetSecondActiveDevice(char* pOutputString, int additionalParam);

};

TEST_GROUP(get_filesystem_data)
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

TEST(get_filesystem_data, active_partition)
{
  // CAUTION: currently GetActivePartition always writes 100 chars 
  // to the buffer it gets!
  char active_part[128];
  int unused;
#ifdef __UNIT_TESTS_HOST__
  g_proc_cmdline=g_path;
  g_path_end = &g_path[strlen(g_path)]; 

  strcpy(g_path_end, "cmdline_hda1");
  GetActivePartition(active_part, unused);
  STRCMP_EQUAL("/dev/hda1", active_part);

  strcpy(g_path_end, "cmdline_sda1");
  GetActivePartition(active_part, unused);
  STRCMP_EQUAL("/dev/sda1", active_part);

  strcpy(g_path_end, "cmdline_hdb1");
  GetActivePartition(active_part, unused);
  STRCMP_EQUAL("/dev/hdb1", active_part);

  strcpy(g_path_end, "cmdline_sdb1");
  GetActivePartition(active_part, unused);
  STRCMP_EQUAL("/dev/sdb1", active_part);

  strcpy(g_path_end, "cmdline_sdb10");
  GetActivePartition(active_part, unused);
  STRCMP_EQUAL("/dev/sdb10", active_part);

  strcpy(g_path_end, "cmdline_mmc");
  GetActivePartition(active_part, unused);
  STRCMP_EQUAL("/dev/mmcblk0p2", active_part);

  strcpy(g_path_end, "cmdline_ubifs");
  GetActivePartition(active_part, unused);
  STRCMP_EQUAL("/dev/ubi0_0", active_part);
#else
  // hardcoded for pac200
  GetActivePartition(active_part, unused);

  FILE *cmdline = fopen("/proc/cmdline", "r");
  char cmdline_buf[256];
  char *root_param;
  
  fread(cmdline_buf, 256, 1, cmdline);
  root_param=strstr(cmdline_buf, "root=");

  fclose(cmdline);

  if(strstr(root_param + 5, "/dev/mmc"))
  {
    STRCMP_EQUAL("/dev/mmcblk0p2", active_part);
  }
  else if(strstr(root_param + 5, "ubi0_0") ||
          strstr(root_param +5, "ubi0:ubi_rootfs1"))
  {
    STRCMP_EQUAL("/dev/ubi0_0", active_part);
  }
  else if(strstr(root_param + 5, "ubi0_1") ||
          strstr(root_param +5, "ubi0:ubi_rootfs2"))
  {
    STRCMP_EQUAL("/dev/ubi0_1", active_part);
  }
  else
  {
    FAIL("Unknown root fs.");
  }
#endif
}

TEST(get_filesystem_data, active_device)
{
  // CAUTION: currently GetActiveDevice always writes 100 chars 
  // to the buffer it gets!
  // This should be fixed
  char active_part[128];
  int unused;

  int res;
#ifdef __UNIT_TESTS_HOST__
  g_proc_cmdline=g_path;
  g_path_end = &g_path[strlen(g_path)]; 

  strcpy(g_path_end, "cmdline_hda1");
  res = GetActiveDevice(active_part, unused);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/hda", active_part);

  strcpy(g_path_end, "cmdline_sda1");
  res = GetActiveDevice(active_part, unused);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/sda", active_part);

  strcpy(g_path_end, "cmdline_hdb1");
  res = GetActiveDevice(active_part, unused);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/hdb", active_part);

  strcpy(g_path_end, "cmdline_sdb1");
  res = GetActiveDevice(active_part, unused);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/sdb", active_part);

  strcpy(g_path_end, "cmdline_sdb10");
  res = GetActiveDevice(active_part, unused);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/sdb", active_part);

  strcpy(g_path_end, "cmdline_mmc");
  res = GetActiveDevice(active_part, unused);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/mmcblk0", active_part);

  // ubi case tries to get mtd device over udev
  // It cannot be tested on host. 
  //
//  strcpy(g_path_end, "cmdline_ubifs");
//  res = GetActiveDevice(active_part, unused);
//  LONGS_EQUAL(SUCCESS, res);
//  STRCMP_EQUAL("/dev/mtd2", active_part);
#else
  // hard-coded for pac200
  FILE *cmdline = fopen("/proc/cmdline", "r");
  char cmdline_buf[256];
  char *root_param;
  
  fread(cmdline_buf, 256, 1, cmdline);
  root_param=strstr(cmdline_buf, "root=");

  fclose(cmdline);

  res = GetActiveDevice(active_part, unused);
  LONGS_EQUAL(SUCCESS, res);
  if(strstr(root_param + 5, "/dev/mmc"))
  {
    STRCMP_EQUAL("/dev/mmcblk0", active_part);
  }
  else if(strstr(root_param + 5, "ubi"))
  {
    STRCMP_EQUAL("/dev/mtd2", active_part);
  }
  else
  {
    FAIL("Unknown root fs.");
  }
#endif
}

TEST(get_filesystem_data, get_partition_by_index)
{

   char device_name[128];

  int res;

  res = GetPartitionByIndex(device_name, 0);
  LONGS_EQUAL(NOT_FOUND, res);

  // WARNING: this test is hard-coded for Wago's SuSE 12.1 image
#ifdef __UNIT_TESTS_HOST__
  res = GetPartitionByIndex(device_name, 1);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/sda1", device_name);

  res = GetPartitionByIndex(device_name, 2);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/sda2", device_name);

  //   res = GetPartitionByIndex(device_name, 3);
  //   LONGS_EQUAL(SUCCESS, res);
  //   STRCMP_EQUAL("/dev/sda", device_name);
  //   
  //   res = GetPartitionByIndex(device_name, 4);
  //   LONGS_EQUAL(SUCCESS, res);
  //   STRCMP_EQUAL("/dev/sdb", device_name);
  //
  //   res = GetPartitionByIndex(device_name, 5);
  //   LONGS_EQUAL(SUCCESS, res);
  //   STRCMP_EQUAL("/dev/mmcblk0", device_name);
  //
  //   res = GetPartitionByIndex(device_name, 6);
  //   LONGS_EQUAL(SUCCESS, res);
  //   STRCMP_EQUAL("/dev/mtd", device_name);
#else
  // hard-coded for pac200
  res = GetPartitionByIndex(device_name, 1);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/mmcblk0p1", device_name);

  res = GetPartitionByIndex(device_name, 2);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/mmcblk0p2", device_name);

  res = GetPartitionByIndex(device_name, 3);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/ubi0_0", device_name);

  res = GetPartitionByIndex(device_name, 4);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/ubi0_1", device_name);

  res = GetPartitionByIndex(device_name, 5);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/ubi0_2", device_name);

#endif
  res = GetPartitionByIndex(device_name, 23);
  LONGS_EQUAL(NOT_FOUND, res);

}

TEST(get_filesystem_data, get_partition_by_index_ext)
{

   char device_name[128];

  int res;

  res = GetPartitionByIndexExt(device_name, 0);
  LONGS_EQUAL(NOT_FOUND, res);

  // WARNING: this test is hard-coded for Wago's SuSE 12.1 image
#ifdef __UNIT_TESTS_HOST__
  // returns ext2/ext3 partitions => none configured on development PC
  res = GetPartitionByIndexExt(device_name, 1);
  LONGS_EQUAL(NOT_FOUND, res);
  STRCMP_EQUAL("", device_name);

  //   res = GetPartitionByIndexExt(device_name, 3);
  //   LONGS_EQUAL(SUCCESS, res);
  //   STRCMP_EQUAL("/dev/sda", device_name);
  //   
  //   res = GetPartitionByIndexExt(device_name, 4);
  //   LONGS_EQUAL(SUCCESS, res);
  //   STRCMP_EQUAL("/dev/sdb", device_name);
  //
  //   res = GetPartitionByIndexExt(device_name, 5);
  //   LONGS_EQUAL(SUCCESS, res);
  //   STRCMP_EQUAL("/dev/mmcblk0", device_name);
  //
  //   res = GetPartitionByIndexExt(device_name, 6);
  //   LONGS_EQUAL(SUCCESS, res);
  //   STRCMP_EQUAL("/dev/mtd", device_name);
#else
  // hard-coded for pac200

  res = GetPartitionByIndexExt(device_name, 1);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/mmcblk0p2", device_name);

#endif
  res = GetPartitionByIndexExt(device_name, 23);
  LONGS_EQUAL(NOT_FOUND, res);

}

TEST(get_filesystem_data, get_device_by_index)
{

  char device_name[128];

  int res;

  res = GetDeviceByIndex(device_name, 0);
  LONGS_EQUAL(NOT_FOUND, res);

  // WARNING: this test is hard-coded for Wago's SuSE 12.1 image
#ifdef __UNIT_TESTS_HOST__
  res = GetDeviceByIndex(device_name, 1);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/sda", device_name);

  res = GetDeviceByIndex(device_name, 2);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/sdb", device_name);

  /*
     res = GetDeviceByIndex(device_name, 3);
     LONGS_EQUAL(SUCCESS, res);
     STRCMP_EQUAL("/dev/sda", device_name);

     res = GetDeviceByIndex(device_name, 4);
     LONGS_EQUAL(SUCCESS, res);
     STRCMP_EQUAL("/dev/sdb", device_name);

     res = GetDeviceByIndex(device_name, 5);
     LONGS_EQUAL(SUCCESS, res);
     STRCMP_EQUAL("/dev/mmcblk0", device_name);

     res = GetDeviceByIndex(device_name, 6);
     LONGS_EQUAL(SUCCESS, res);
     STRCMP_EQUAL("/dev/mtd", device_name);
     */
#else
  res = GetDeviceByIndex(device_name, 1);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/mmcblk0", device_name);

  // simulate non-existing ubi system"
  system("ubidetach /dev/ubi_ctrl -m 2");
  res = GetDeviceByIndex(device_name, 2);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/mtd2", device_name);

  // test normal configuration
  system("ubiattach /dev/ubi_ctrl -m 2");
  res = GetDeviceByIndex(device_name, 2);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("/dev/mtd2", device_name);


#endif
  res = GetDeviceByIndex(device_name, 23);
  LONGS_EQUAL(NOT_FOUND, res);

}

TEST(get_filesystem_data, get_2nd_active_device)
{

  char device_name[128];

  int res;
  int dummy;

  // WARNING: this test is hard-coded for Wago's SuSE 12.1 image
#ifdef __UNIT_TESTS_HOST__

  g_pac_eeprom=g_path;
  g_path_end = &g_path[strlen(g_path)]; 

  strcpy(g_path_end, "pac_boot_mode_1");

  res = GetSecondActiveDevice(device_name, dummy);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("1", device_name);

  strcpy(g_path_end, "pac_boot_mode_2");
  res = GetSecondActiveDevice(device_name, dummy);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("2", device_name);

#elif __UNIT_TESTS_TARGET__
  res = GetSecondActiveDevice(device_name, dummy);
  LONGS_EQUAL(SUCCESS, res);

  printf("This test assumes the default NAND boot partition is 1!\n"); 
  STRCMP_EQUAL("1", device_name);
#else
#error "Neither host nor target chosen as test environment!"
#endif

}

