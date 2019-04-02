//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_store_file.cpp
///
///  \brief    Test xml file persistence.
///
///  \author   WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <string>
#include <cstdio>

#include "xmlhlp.hpp"
#include "file_access.hpp"
#include "error.hpp"
#include "fake_file_access.hpp"
#include "test_utils.hpp"

using namespace wago;
//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

class StoreFileTest: public ::testing::Test
{
  public:
    std::string tempDir_;
    std::string params_;
    std::string ipcmn_;

    StoreFileTest()
    : tempDir_("")
    , params_("../../../test-res/params.xml")
    , ipcmn_("../../../test-res/ipcmn.xml")
    {

    }

    void SetUp() override
    {
      tempDir_ = create_temp_dir("firewall_");
      set_config_dir(tempDir_);

      create_dir(tempDir_ + "/etc/firewall/iptables");
      copy_file(params_, tempDir_ + "/etc/firewall/params.xml");
      copy_file(ipcmn_, tempDir_ + "/etc/firewall/iptables/ipcmn.xml");
    }

    void TearDown() override
    {
      remove_dir(tempDir_);

    }
};

// -----------------------------------------------------------------------------


TEST_F(StoreFileTest, store_ipcmn_xml)
{
    const std::string ipcmn_file = tempDir_ + "/etc/firewall/iptables/ipcmn.xml";

    ASSERT_TRUE(file_exists(ipcmn_file));

    xmldoc doc = read_configuration("iptables", false);
    if (!doc.is_empty())
    {
        store_configuration("iptables", false, doc);
    }

    // Check expected file content.
    ASSERT_EQ(0,system(("diff ../../../test-res/ipcmn.xml " + ipcmn_file + " &> /dev/null").c_str()));
}


TEST_F(StoreFileTest, store_ipcmn_xml_tmp_already_exists)
{
    const std::string ipcmn_file = tempDir_ + "/etc/firewall/iptables/ipcmn.xml";
    const std::string ipcmn_file_tmp = tempDir_ + "/etc/firewall/iptables/ipcmn.xml.tmp";
    system(("echo foo > " + ipcmn_file_tmp).c_str());

    ASSERT_TRUE(file_exists(ipcmn_file));
    ASSERT_TRUE(file_exists(ipcmn_file_tmp));


    xmldoc doc = read_configuration("iptables", false);
    if (!doc.is_empty())
    {
        store_configuration("iptables", false, doc);
    }

    ASSERT_FALSE(file_exists(ipcmn_file_tmp));

    // Check expected file content.
    ASSERT_EQ(0,system(("diff ../../../test-res/ipcmn.xml " + ipcmn_file + " &> /dev/null").c_str()));
}


//---- End of source file ------------------------------------------------------

