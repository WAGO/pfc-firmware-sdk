//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_main.cpp
///
///  \brief    ToDo:
///
///  \author   NiB : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <glib.h>
#include "config_tool_lib.h"

class Excuteable {
  private:
    std::string _command;
    gchar * _standard_output = nullptr;
    gchar * _standard_error = nullptr;
    gint _exit_status = 0;
    GError * _err = nullptr;
  public:
    ~Excuteable() {
      freeValues();
    };
    void setCommand(const std::string & command) {
      _command=command;
    };
    gboolean call() {
      return g_spawn_command_line_sync(_command.c_str(),
                                       &_standard_output,
                                       &_standard_error,
                                       &_exit_status,
                                       &_err);
    };
    gboolean checkExitStatus() {
      return g_spawn_check_exit_status(_exit_status,
                                       &_err);
    };
    gint getExitStatus() {
      checkExitStatus();
      return _exit_status;
    }
    std::string getStdout() {
      std::string ret;
      if(_standard_output != nullptr){
        ret = _standard_output;
      }
      return ret;
    }
    void freeValues() {
      if(_standard_output != nullptr) {
        g_free(_standard_output);
        _standard_output = nullptr;
      }
      if(_standard_output != nullptr) {
        g_free(_standard_output);
        _standard_output = nullptr;
      }
      if(_err != nullptr) {
        g_clear_error(&_err);
        _err = nullptr;
      }
      _exit_status = 0;
    };
};

//------------------------------------------------------------------------------
class main_Target: public ::testing::Test {
  protected:
    Excuteable excMain;
    std::string stdoutput;
    void SetUp() override {
    }

    void TearDown() override {
      excMain.freeValues();
    }
};

TEST_F(main_Target, InvailidOptionArg_Nothing) {
  excMain.setCommand("./print_log.elf");
  ASSERT_TRUE(excMain.call());
  stdoutput = excMain.getStdout();
  ASSERT_EQ(0, stdoutput.length()) << stdoutput;;
}

TEST_F(main_Target, OptionArg_h) {
  excMain.setCommand("./print_log.elf -h");
  ASSERT_TRUE(excMain.call());
  stdoutput = excMain.getStdout();
  ASSERT_NE(0, stdoutput.length()) << stdoutput;;
}

TEST_F(main_Target, OptionArg_s) {
  excMain.setCommand("./print_log.elf -s");
  ASSERT_TRUE(excMain.call());
  stdoutput = excMain.getStdout();
  ASSERT_NE(0, stdoutput.length()) << stdoutput;;
}

TEST_F(main_Target, OptionArg_s_j) {
  excMain.setCommand("./print_log.elf -s -j");
  ASSERT_TRUE(excMain.call());
  stdoutput = excMain.getStdout();
  ASSERT_NE(0, stdoutput.length()) << stdoutput;;
}

TEST_F(main_Target, OptionArg_r) {
  excMain.setCommand("./print_log.elf -r");
  ASSERT_TRUE(excMain.call());
  stdoutput = excMain.getStdout();
  ASSERT_EQ(0, stdoutput.length()) << stdoutput;
}

