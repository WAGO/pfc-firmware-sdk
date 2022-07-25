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
#include <glib.h>
#include <filesystem>

#include "ControlSnmp.hpp"
#include "Constants.hpp"


namespace wago::snmp_config_lib {

namespace fs = std::filesystem;

static bool SymlinkExists(const ::std::string &link_path) {
  return fs::exists(link_path);
}

static bool IsRunning(const ::std::string &process) {

  ::std::string command_line = "pidof " + process;

  gint exit_status = 0;
  gchar *p_stdout = nullptr;
  gchar *p_stderr = nullptr;
  GError *p_error = nullptr;

  g_spawn_command_line_sync(command_line.c_str(), &p_stdout, &p_stderr, &exit_status, &p_error);

  char *end;
  int pid = ::std::strtol(p_stdout, &end, 0);

  return (pid > 0);
}

static void AssertSnmpIsEnabled(bool enabled) {
  auto snmp_paths = Constants::GetSnmpFilePaths();
  auto snmp_trap_paths = Constants::GetSnmpTrapFilePaths();
  
  if (enabled) {
    EXPECT_TRUE(SymlinkExists(snmp_paths.rc_d));
    EXPECT_TRUE(SymlinkExists(snmp_trap_paths.rc_d));
    EXPECT_TRUE(IsRunning(snmp_paths.daemon));
    EXPECT_TRUE(IsRunning(snmp_trap_paths.daemon));
  } else {
    EXPECT_FALSE(SymlinkExists(snmp_paths.rc_d));
    EXPECT_FALSE(SymlinkExists(snmp_trap_paths.rc_d));
    EXPECT_FALSE(IsRunning(snmp_paths.daemon));
    EXPECT_FALSE(IsRunning(snmp_trap_paths.daemon));
  }
}

TEST(ControlSnmpTest_Target, EnableDisableSnmp) {

  StartDaemons();
  EXPECT_TRUE(IsSnmpRunning());
  AssertSnmpIsEnabled(true);

  StopDaemons();
  EXPECT_FALSE(IsSnmpRunning());
  AssertSnmpIsEnabled(false);

  StartDaemons();
  EXPECT_TRUE(IsSnmpRunning());
  AssertSnmpIsEnabled(true);

  StartDaemons();
  EXPECT_TRUE(IsSnmpRunning());
  AssertSnmpIsEnabled(true);

  StopDaemons();
  EXPECT_FALSE(IsSnmpRunning());
  AssertSnmpIsEnabled(false);

  StopDaemons();
  EXPECT_FALSE(IsSnmpRunning());
  AssertSnmpIsEnabled(false);

}

}  // wago::util
