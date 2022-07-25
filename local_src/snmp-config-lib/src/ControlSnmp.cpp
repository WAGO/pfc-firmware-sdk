#include "ControlSnmp.hpp"

#include <glib.h>
#include <filesystem>
#include <string>

#include "Constants.hpp"
#include "Program.hpp"

namespace wago::snmp_config_lib {

namespace fs = std::filesystem;

namespace {

int GetPidOf(const ::std::string &daemon_path) {
  ::std::string command_line = "/usr/bin/pidof " + daemon_path;
  auto program = wago::util::Program::Execute(command_line);

  if (program.GetResult() == 0) {
    char *end;
    return ::std::strtol(program.GetStdout().c_str(), &end, 0);
  }

  return 0;
}

bool IsRunning(const ::std::string &daemon_path) {
  int pid = GetPidOf(daemon_path);
  return pid > 0;
}

void AddLink(const LinuxDaemonFilePaths paths) {
  if (not fs::exists(paths.rc_d)) {
    fs::create_symlink(paths.init_d, paths.rc_d);
  }
}

void RemoveLink(const LinuxDaemonFilePaths paths) {
  if (fs::exists(paths.rc_d)) {
    fs::remove_all(paths.rc_d);
  }
}

DaemonState Start(const LinuxDaemonFilePaths paths) {
  if (not IsRunning(paths.daemon)) {
    ::std::string command_line = paths.init_d + " start";
    wago::util::Program::Execute(command_line);
    return NewlyStarted;
  }
  return AlreadyRunning;
}

void Stop(const LinuxDaemonFilePaths paths) {
  if (IsRunning(paths.daemon)) {
    ::std::string command_line = paths.init_d + " stop";
    wago::util::Program::Execute(command_line);
  }
}

}

DaemonState StartDaemons() {
  auto snmp_paths = Constants::GetSnmpFilePaths();
  auto snmp_trap_paths = Constants::GetSnmpTrapFilePaths();

  AddLink(snmp_paths);
  AddLink(snmp_trap_paths);
  auto state = Start(snmp_paths); // we only need to know if this daemon was restarted
  Start(snmp_trap_paths);

  return state;
}

void StopDaemons() {
  auto snmp_paths = Constants::GetSnmpFilePaths();
  auto snmp_trap_paths = Constants::GetSnmpTrapFilePaths();

  RemoveLink(snmp_paths);
  RemoveLink(snmp_trap_paths);
  Stop(snmp_paths);
  Stop(snmp_trap_paths);
}

bool IsSnmpRunning() {
  auto snmp_paths = Constants::GetSnmpFilePaths();
  auto snmp_trap_paths = Constants::GetSnmpTrapFilePaths();
  return (IsRunning(snmp_paths.daemon) && IsRunning(snmp_trap_paths.daemon));
}

void InformDaemonAboutChangedConfig() {
  auto paths = Constants::GetSnmpFilePaths();
  auto pid = GetPidOf(paths.daemon);
  wago::util::Program::Execute("/usr/bin/kill -HUP " + ::std::to_string(pid));
}

}  // namespace wago::snmp_config_lib
