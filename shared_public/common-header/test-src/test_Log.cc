//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project common-header (PTXdist package libcommonheader).
//
// Copyright (c) 2019 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file
///
///  \brief    Test for assertion helper macros.
///
///  \author   MaHe: WAGO Kontakttechnik GmbH & Co. KG
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <string>
#include "wc/log.h"

using std::string;
using std::to_string;

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
static log_level_t current_log_level                           = debug;
static log_level_t used_log_level                              = off;
static char        used_message[wc_get_max_log_message_size()] = { 0 };

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
void wc_log_output(log_level_t const  log_level,
                   char const * const message) 
{
  used_log_level = log_level;
  strncpy(used_message, message, sizeof (used_message));
  used_message[sizeof (used_message) - 1] = '\0';
}

log_level_t wc_get_log_level(void) 
{
  return current_log_level;
}

class Log_Fixture : public ::testing::Test
{
protected:
    Log_Fixture() {};
    virtual ~Log_Fixture() {};
    void SetUp() override
    {
      used_message[0] = '\0';
      used_log_level  = off;
    }
};

TEST_F(Log_Fixture, DebugMessageWhileOff)
{
  current_log_level = off;

  char const message[] = "test 123";
  log_level_t log_level = debug;

  wc_log(log_level, message);

  EXPECT_STREQ("", used_message);
  EXPECT_EQ(off, used_log_level);
}

TEST_F(Log_Fixture, OffMessageWhileOff)
{
  current_log_level = off;

  char const message[] = "test 123";
  log_level_t log_level = off;

  wc_log(log_level, message);

  EXPECT_STREQ("", used_message);
  EXPECT_EQ(off, used_log_level);
}

TEST_F(Log_Fixture, OffMessageWhileDebug)
{
  current_log_level = debug;

  char const message[] = "test 123";
  log_level_t log_level = off;

  wc_log(log_level, message);

  EXPECT_STREQ("", used_message);
  EXPECT_EQ(off, used_log_level);
}

TEST_F(Log_Fixture, DebugMessageWhileDebug)
{
  current_log_level = debug;

  char const message[] = "test 123";
  log_level_t log_level = debug;

  wc_log(log_level, message);

  EXPECT_STREQ(message, used_message);
  EXPECT_EQ(log_level, used_log_level);
}

TEST_F(Log_Fixture, NoDebugMessageWhileInfo)
{
  current_log_level = info;

  char const message[]  = "test 123";
  log_level_t log_level = debug;

  wc_log(log_level, message);

  EXPECT_STREQ("", used_message);
  EXPECT_EQ(off, used_log_level);
}

TEST_F(Log_Fixture, ErrorMessageWhileError)
{
  current_log_level = error;

  char const message[]  = "test 123";
  log_level_t log_level = error;

  wc_log(log_level, message);

  EXPECT_STREQ(message, used_message);
  EXPECT_EQ(log_level, used_log_level);
}

TEST_F(Log_Fixture, NoErrorMessageWhileFatal)
{
  current_log_level = fatal;

  char const message[]  = "test 123";
  log_level_t log_level = error;

  wc_log(log_level, message);

  EXPECT_STREQ("", used_message);
  EXPECT_EQ(off, used_log_level);
}

TEST_F(Log_Fixture, AdvancedErrorMessage)
{
  current_log_level = error;

  char const         message_format[] = "%s|%u";
  char const         message_hello[]  = "Hello";
  unsigned           message_12       = 12;
  char const * const message          = (string(message_hello) + "|" + to_string(message_12)).c_str();

  log_level_t log_level = error;

  wc_log_format_advanced(__FILE__, __func__, __LINE__, log_level, message_format, message_hello, message_12);

  EXPECT_STREQ(message, used_message);
  EXPECT_EQ(log_level, used_log_level);
}

TEST_F(Log_Fixture, AdvancedDebugMessage)
{
  current_log_level = debug;

  char const         message_format[] = "%s|%u";
  char const         message_hello[]  = "Hello";
  unsigned           message_12       = 12;
  char const * const message          = (string(message_hello) + "|" + to_string(message_12)).c_str();

  char const * const file = __FILE__;
  char const * const func = __func__;
  size_t const       line = __LINE__;

  log_level_t log_level = error;

  wc_log_format_advanced(file, func, line, log_level, message_format, message_hello, message_12);

  EXPECT_TRUE(string(used_message).find(message) != std::string::npos);
  EXPECT_TRUE(string(used_message).find(file) != std::string::npos);
  EXPECT_TRUE(string(used_message).find(func) != std::string::npos);
  EXPECT_TRUE(string(used_message).find(to_string(line)) != std::string::npos);
  EXPECT_EQ(log_level, used_log_level);
}

//---- End of source file ------------------------------------------------------
