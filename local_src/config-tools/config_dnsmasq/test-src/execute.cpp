//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     execute.cpp
///
///  \brief    Run config_dnsmasq to test argument processing together with memcheck.
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
///  \author   MOe : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>

#include "config_dnsmasq.hpp"

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

TEST(TestExecution, DISABLED_invalid_option) {

//  char* argv[] = {"config_dnsmasq", "-d", "-s", "-p", "br0", "dhcpd-state=disabled"};
  char* argv[] = {"/etc/config-tools/config_dnsmasq_c", "-n", "-s", "dns-state=enabled"};

  execute(4, argv);
}


//---- End of source file ------------------------------------------------------

