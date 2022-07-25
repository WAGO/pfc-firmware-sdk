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


extern "C"
{
#include "config_tool_lib.h"
};

TEST_GROUP(config_tool_lib)
{
  void setup()
  {
  }

  void teardown()
  {
  }
};

TEST(config_tool_lib, run_udev_query)
{
  char buf[128];
  int res;
  buf[0] = '\0';
#ifdef __UNIT_TESTS_TARGET__
  // following tests are hard-coded for the pac200 platform:
  // TODO: dynamically inject expected values from outside

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS, 0, NULL);
  STRCMP_EQUAL("/dev/mmcblk0p1", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS, 1, NULL);
  STRCMP_EQUAL("/dev/mmcblk0p2", buf);
  LONGS_EQUAL(2, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS, 2, NULL);
  STRCMP_EQUAL("", buf);
  LONGS_EQUAL(2, res);

  // Have to test this before ubi: otherwise it is masked by ubi test failing
  // This query is a fallback for the case that the system has no ubi
  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_INTERNAL_FLASH_NAND, 0, NULL);
  STRCMP_EQUAL("/dev/mtd2", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_UBI_PARTITIONS, 0, NULL);
  STRCMP_EQUAL("/dev/ubi0_0", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_UBI_DEVICES, 0, NULL);
  STRCMP_EQUAL("/dev/ubi0", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_DEVICES, 0, NULL);
  STRCMP_EQUAL("/dev/mmcblk0", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_UBI_PARTITION_BY_NAME, 0, (void*) "ubi_rootfs1");
  STRCMP_EQUAL("/dev/ubi0_0", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_UBI_PARTITION_BY_NAME, 0, (void*) "ubi_rootfs2");
  STRCMP_EQUAL("/dev/ubi0_3", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_UBI_PARTITION_BY_NAME, 0, (void*) "ubi_home");
  STRCMP_EQUAL("/dev/ubi0_1", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_UBI_PARTITION_BY_NAME, 0, NULL);
  LONGS_EQUAL(0, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_UBI_PARTITION_BY_NAME, 0, (void*) "fantasy_label");
  LONGS_EQUAL(0, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 0, (void*) "");
  LONGS_EQUAL(0, res);
  STRCMP_EQUAL("", buf);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 0, (void*) NULL);
  LONGS_EQUAL(0, res);
  STRCMP_EQUAL("", buf);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 0, (void*) "ext3");
  LONGS_EQUAL(1, res);
  STRCMP_EQUAL("/dev/mmcblk0p2", buf);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 23, (void*) "ext3");
  LONGS_EQUAL(1, res);
  STRCMP_EQUAL("", buf);

#else
#ifdef __UNIT_TESTS_HOST__
  // following tests are hard-coded for SuSE12.1 image@WAGO:
  // they rely on 2 available block partitions /dev/sda1 and /dev/sda2
  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS, 0, NULL);
  STRCMP_EQUAL("/dev/sda1", buf);
  LONGS_EQUAL(1, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS, 1, NULL);
  STRCMP_EQUAL("/dev/sda2", buf);
  LONGS_EQUAL(2, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS, 2, NULL);
  STRCMP_EQUAL("", buf);
  LONGS_EQUAL(2, res);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 0, (void*) "");
  LONGS_EQUAL(0, res);
  STRCMP_EQUAL("", buf);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 0, (void*) NULL);
  LONGS_EQUAL(0, res);
  STRCMP_EQUAL("", buf);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 0, (void*) "ext4");
  LONGS_EQUAL(1, res);
  STRCMP_EQUAL("/dev/sda1", buf);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 1, (void*) "ext4");
  LONGS_EQUAL(2, res);
  STRCMP_EQUAL("/dev/dm-1", buf);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 2, (void*) "ext4");
  LONGS_EQUAL(3, res);
  STRCMP_EQUAL("/dev/dm-2", buf);

  res = RunUdevQuery(buf, sizeof(buf), UDEV_QUERY_BLOCK_PARTITIONS_EXT, 42, (void*) "ext4");
  LONGS_EQUAL(4, res);
  STRCMP_EQUAL("", buf);

#else
#error "Neither Target nor host mode chosen!"
#endif
#endif
}

