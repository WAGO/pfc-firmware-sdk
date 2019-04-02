//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_process_params.cpp
///
///  \brief    Test function processing params configuration file.
///
///  \author   WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <cstdio>


#include "process_params.hpp"
#include  "error.hpp"
#include "fake_file_access.hpp"
#include "test_utils.hpp"

using namespace wago;

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

class ProcessParametersTest: public ::testing::Test
{
  public:
    std::string tempDir_;
    std::string params_;
    std::string ipcmn_;

    ProcessParametersTest()
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

// -----------------------------------------------------------------------------

TEST_F(ProcessParametersTest, update_xml_params)
{
    wago::xmldoc params_gen;
    wago::xmldoc ipcmn;
    
    const std::string params_gen_file = tempDir_ + "/etc/firewall/params_gen.xml";

    ASSERT_FALSE(file_exists(params_gen_file));

    wago::update_network_interface_name_mapping();

    ASSERT_TRUE(file_exists(params_gen_file));

    // Check expected file content.
    ASSERT_EQ(0,system(("diff ../../../test-res/params_gen.xml " + params_gen_file + " &> /dev/null").c_str()));
}


TEST_F(ProcessParametersTest, params_xml_empty)
{
    wago::xmldoc params_gen;
    wago::xmldoc ipcmn;
    
    // Set empty params.xml file
    system(("echo '' > " + tempDir_ + "/etc/firewall/params.xml").c_str());

    const std::string params_gen_file = tempDir_ + "/etc/firewall/params_gen.xml";

    ASSERT_FALSE(file_exists(params_gen_file));

    ASSERT_THROW(wago::update_network_interface_name_mapping(), std::runtime_error);

    ASSERT_FALSE(file_exists(params_gen_file));
}


TEST_F(ProcessParametersTest, ipcmn_xml_empty)
{
    wago::xmldoc params_gen;
    wago::xmldoc ipcmn;

    // Set empty params.xml file
    system(("echo '' > " + tempDir_ + "/etc/firewall/iptables/ipcmn.xml").c_str());

    const std::string params_gen_file = tempDir_ + "/etc/firewall/params_gen.xml";

    ASSERT_FALSE(file_exists(params_gen_file));

    wago::update_network_interface_name_mapping();

    ASSERT_TRUE(file_exists(params_gen_file));

    // Check expected file content.
    ASSERT_EQ(0,system(("diff ../../../test-res/params.xml " + params_gen_file + " &> /dev/null").c_str()));
}


TEST_F(ProcessParametersTest, ipcmn_xml_no_nat_entries)
{
     wago::xmldoc params_gen;
     wago::xmldoc ipcmn;

     params_ = "../../../test-res/params.xml";
     ipcmn_ = "../../../test-res/ipcmn_no_nat.xml";
     copy_file(params_, tempDir_ + "/etc/firewall/params.xml");
     copy_file(ipcmn_, tempDir_ + "/etc/firewall/iptables/ipcmn.xml");

     const std::string params_gen_file = tempDir_ + "/etc/firewall/params_gen.xml";

     ASSERT_FALSE(file_exists(params_gen_file));

     wago::update_network_interface_name_mapping();

     ASSERT_TRUE(file_exists(params_gen_file));

     // Check expected file content.
     ASSERT_EQ(0,system(("diff ../../../test-res/params.xml " + params_gen_file + " &> /dev/null").c_str()));
}


//---- End of source file ------------------------------------------------------

