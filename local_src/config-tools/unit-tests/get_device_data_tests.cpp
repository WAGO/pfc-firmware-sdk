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
/// \version <Revision>: $Rev: 33614 $
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
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
  int GetPureDeviceName(char*   pacDeviceName,
                      char*   pacPureDeviceName,
                      size_t  sizeofPureDeviceName);

  int GetEntireSizeByName(char *devPath,
                          char *size);
};

TEST_GROUP(get_device_data)
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

TEST(get_device_data, get_pure_device_name)
{
  char devname[128];
  int res;

  res = GetPureDeviceName(NULL, devname, sizeof(devname));
  LONGS_EQUAL(INVALID_PARAMETER, res);

  res = GetPureDeviceName("test", NULL, sizeof(devname));
  LONGS_EQUAL(INVALID_PARAMETER, res);

  res = GetPureDeviceName("test", devname, 0);
  LONGS_EQUAL(INVALID_PARAMETER, res);

  res = GetPureDeviceName("/dev/bogus_device", devname, sizeof(devname));
  LONGS_EQUAL(INVALID_PARAMETER, res);
  STRCMP_EQUAL("", devname);

  // Again hard-coded to be tested against SuSE 12 image:
  // /dev/sda /dev/sda1 /dev/sda2
  // /dev/sdb
#ifdef __UNIT_TESTS_HOST__
  res = GetPureDeviceName("/dev/sda", devname, sizeof(devname));
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("sda", devname);

  *devname = 0;
  res = GetPureDeviceName("sda", devname, sizeof(devname));
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("sda", devname);

  *devname = 0;
  res = GetPureDeviceName("/dev/sda1", devname, sizeof(devname));
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("sda", devname);

  *devname = 0;
  res = GetPureDeviceName("sda1", devname, sizeof(devname));
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("sda", devname);
#else
  // hard-coded for pac200
  *devname = 0;
  res = GetPureDeviceName("/dev/mmcblk0p2", devname, sizeof(devname));
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("mmcblk0", devname);

  *devname = 0;
  res = GetPureDeviceName("/dev/mmcblk0", devname, sizeof(devname));
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("mmcblk0", devname);

  *devname = 0;
  res = GetPureDeviceName("/dev/ubi0_0", devname, sizeof(devname));
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("mtd2", devname);

  *devname = 0;
  res = GetPureDeviceName("/dev/mtd2", devname, sizeof(devname));
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("mtd2", devname);



#endif
}

TEST(get_device_data, GetSize)
{

#ifdef __UNIT_TESTS_HOST__
  // Hard-coded to be tested against SuSE 12 image:
  char buf[64];

  int res = GetEntireSizeByName("/bogus/device", buf);
  LONGS_EQUAL(ERROR, res);

  res = GetEntireSizeByName("/dev/sda", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("488386584", buf);

  res = GetEntireSizeByName("/dev/sda1", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("102400", buf);

  res = GetEntireSizeByName("/dev/sda2", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("488283160", buf);
#else
  // hard-coded for pac200
  char buf[64];

  int res = GetEntireSizeByName("/bogus/device", buf);
  LONGS_EQUAL(ERROR, res);

  res = GetEntireSizeByName("mmcblk0", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("1931264", buf);

  res = GetEntireSizeByName("mmcblk0p1", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("16384", buf);

  ////////////////////////////////

  res = GetEntireSizeByName("/dev/mmcblk0", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("1931264", buf);

  res = GetEntireSizeByName("/dev/mmcblk0p1", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("16384", buf);

  res = GetEntireSizeByName("/dev/mmcblk0p2", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("122880", buf);

  ////////////////////////////////
  res = GetEntireSizeByName("/dev/ubi0_0", buf); //rootfs0
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("70056", buf); //TODO: size changed!

  res = GetEntireSizeByName("/dev/ubi0_2", buf); //rootfs1
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("70056", buf);  //TODO: size changed!

  res = GetEntireSizeByName("/dev/ubi0_1", buf); //home
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("65772", buf);

  ////////////////////////////////

  res = GetEntireSizeByName("ubi0_1", buf);
  LONGS_EQUAL(SUCCESS, res);
  STRCMP_EQUAL("70056", buf);

#endif
}
