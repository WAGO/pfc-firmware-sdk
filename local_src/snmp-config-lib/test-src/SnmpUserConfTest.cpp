//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "SnmpUserConf.hpp"

namespace wago::snmp_config_lib {

struct TestData {
  ::std::string actual_user_conf_;
  ::std::string new_user_conf_;
  ::std::string actual_name_;
  ::std::string new_name_;
  ::std::string actual_desc_;
  ::std::string new_desc_;

  SnmpConfig GetActualConfig() {
    SnmpConfig c;
    c.sys_name_ = actual_name_;
    c.sys_descr_ = actual_desc_;
    return c;
  }
  SnmpConfig GetNewConfig() {
    SnmpConfig c;
    c.sys_name_ = new_name_;
    c.sys_descr_ = new_desc_;
    return c;
  }
};

struct SnmpUserConfTest : public ::testing::Test, public ::testing::WithParamInterface<TestData> {
  TestData param_;
  void SetUp() override {
    param_ = GetParam();
  }
};

TEST_P(SnmpUserConfTest, UpdateSnmpUserConf ) {
  UpdateSnmpUserConfData(param_.actual_user_conf_, param_.GetNewConfig(), param_.GetActualConfig());
  EXPECT_EQ(param_.new_user_conf_, param_.actual_user_conf_);
}

INSTANTIATE_TEST_CASE_P(
    UpdateSnmpUserConf, SnmpUserConfTest,
    ::testing::Values<TestData>(TestData { "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=no\n",
                                    "USE_USER_DEFINED_DEVICE_NAME=yes\nUSE_USER_DEFINED_DESCRIPTION=yes\n", "name1",
                                    "name2", "desc1", "desc2" },
                                TestData { "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=no\n",
                                    "USE_USER_DEFINED_DEVICE_NAME=yes\nUSE_USER_DEFINED_DESCRIPTION=no\n", "name1",
                                    "name2", "desc1", "desc1" },
                                TestData { "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=no\n",
                                    "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=yes\n", "name1",
                                    "name1", "desc1", "desc2" },
                                TestData { "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=no\n",
                                    "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=no\n", "name1",
                                    "name1", "desc1", "desc1" },
                                TestData { "USE_USER_DEFINED_DEVICE_NAME=yes\nUSE_USER_DEFINED_DESCRIPTION=no\n",
                                    "USE_USER_DEFINED_DEVICE_NAME=yes\nUSE_USER_DEFINED_DESCRIPTION=yes\n", "name1",
                                    "name1", "desc1", "desc2" },
                                TestData { "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=yes\n",
                                    "USE_USER_DEFINED_DEVICE_NAME=yes\nUSE_USER_DEFINED_DESCRIPTION=yes\n", "name1",
                                    "name2", "desc1", "desc1" },
                                TestData { "", "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=no\n",
                                    "name1", "name1", "desc1", "desc1" },
                                TestData { "", "USE_USER_DEFINED_DEVICE_NAME=yes\nUSE_USER_DEFINED_DESCRIPTION=no\n",
                                    "name1", "name2", "desc1", "desc1" },
                                TestData { "", "USE_USER_DEFINED_DEVICE_NAME=no\nUSE_USER_DEFINED_DESCRIPTION=yes\n",
                                    "name1", "name1", "desc1", "desc2" },
                                TestData { "", "USE_USER_DEFINED_DEVICE_NAME=yes\nUSE_USER_DEFINED_DESCRIPTION=yes\n",
                                    "name1", "name2", "desc1", "desc2" }));

}
// wago::util
