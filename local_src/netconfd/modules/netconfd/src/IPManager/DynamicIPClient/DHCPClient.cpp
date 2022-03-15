// SPDX-License-Identifier: GPL-2.0-or-later

#include "DHCPClient.hpp"
#include "CollectionUtils.hpp"

#include <glib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <thread>
#include <boost/filesystem.hpp>

#include "Logger.hpp"

namespace netconf {

namespace {

void RemoveFile(::std::string path) {
  const boost::filesystem::path file = path;
  if (boost::filesystem::exists(file)) {
    boost::filesystem::remove(file);
  }
}

}

DHCPClient::DHCPClient(const ::std::string &itf_name, const ::std::string &hostname, const ::std::string &vendorclass)
    : pid_ { },
      hostname_ { hostname },
      vendorclass_ { vendorclass } {

  itf_name_ = itf_name;
  pid_file_path_ = "/var/run/udhcpc_" + itf_name_ + ".pid";

  Start(hostname, vendorclass);
}

DHCPClient::~DHCPClient() {
  Stop();
}

void DHCPClient::Release() {
  LogDebug(
      "DHCPClient::Release | Send SIGUSR2 to dhcp client for interface " + itf_name_ + " to release its ip address");
  kill(pid_, SIGUSR2);
}

void DHCPClient::Renew() {
  LogDebug("DHCPClient::Renew | Send SIGUSR1 to dhcp client for interface " + itf_name_ + " to renew its ip address");
  kill(pid_, SIGUSR1);
}

DynamicIPType DHCPClient::GetType() {
  return DynamicIPType::DHCP;
}

void DHCPClient::UpdateContentFromLease() {
  lease_file_.Parse(LeaseFile::GetLeaseFilePath(itf_name_));
}

Address DHCPClient::GetAddressFromLease() {
  return lease_file_.GetAddress();
}

Netmask DHCPClient::GetNetmaskFromLease() {
  return lease_file_.GetNetmask();
}

::std::string DHCPClient::GetHostnameFromLease() {
  return lease_file_.GetDHCPHostname();
}

::std::string DHCPClient::GetDomainFromLease() {
  return lease_file_.GetDHCPDomain();
}

static void OnProcessStop(GPid pid, gint status, gpointer user_data) {
  (void) user_data;
  GError *err = nullptr;
  if (g_spawn_check_exit_status(status, &err) == FALSE) {
    LogDebug("Stopped DHCP client with pid " + ::std::to_string(pid) + " and exit status abnormally");
  }

  if (err != nullptr) {
    LogDebug("GError message: " + ::std::string(err->message));
    g_error_free(err);
  }

  // Free any resources associated with the child here, such as I/O channels
  // on its stdout and stderr FDs. If you have no code to put in the
  // child_watch_cb() callback, you can remove it and the g_child_watch_add()
  // call, but you must also remove the G_SPAWN_DO_NOT_REAP_CHILD flag,
  // otherwise the child process will stay around as a zombie until this
  // process exits.
  g_spawn_close_pid(pid);

}

void DHCPClient::Restart(::std::string hostname) {
  hostname_ = hostname;

  Stop();
  Start(hostname_, vendorclass_);
}

void DHCPClient::Start(const ::std::string &hostname, const ::std::string &vendorclass) {

  ::std::string hostname_option = "hostname:" + hostname;

  auto options = make_array(DHCP_CLIENT_PATH.c_str(), "--foreground", "--interface", itf_name_.c_str(), "--vendorclass",
                            vendorclass.c_str(), "--pidfile", pid_file_path_.c_str(), "-x", hostname_option.c_str(),
                            "--syslog", "-R", nullptr);

  GError *g_error = nullptr;
  auto spawned = g_spawn_async(nullptr, const_cast<gchar**>(options.data()), nullptr, G_SPAWN_DO_NOT_REAP_CHILD,  // NOLINT(cppcoreguidelines-pro-type-const-cast)
                               nullptr, nullptr, &pid_, &g_error);

  LogDebug("Started DHCP Client for interface " + itf_name_ + ", pid: " + ::std::to_string(pid_));
  if (g_error != nullptr) {
    LogDebug("g_error: code: " + ::std::to_string(g_error->code) + ", message: " + g_error->message);
  }
  /*
   * TODO: Es wurde beobachtet, dass der Spawn des DHCP-Client-Prozesses sporadisch nicht funktioniert.
   * Das heißt, g_spawn_async gibt TRUE zurück und eine PID existiert, aber der udhcpc client wurde nicht gestartet.
   * Wir sehen, dass in Busybox die main und main_udhcp nicht aufgerufen werden.
   * Tests zeigen, dass ein sleep das Problem zumindest bei manuellen Neustarts des netconfd umgeht.
   */
  sleep(1);

  int process_state = kill(pid_, 0);
  if (spawned == TRUE && g_error == nullptr && 0 == process_state) {
    g_child_watch_add(pid_, OnProcessStop, nullptr);
  } else {
    LogError("Failed to spawn DHCP client for interface " + itf_name_);
  }

}

void DHCPClient::Stop() {
  /*
   * TODO: Besser wäre hier ein SIGTERM. Damit der udhcp client das deconfig aufruft, selbstständig sein Lease File löscht
   * und sich ordnungsgemäß beim DHCP-Server abmeldet. Dies führt aber dazu, dass kurzzeitig zwei Clients aktiv sind, wenn man
   * eine schnelle umkonfiguration vornimmt (dhcp -> static -> dhcp). Wenn sich dann der erste Client beendet und am Ende
   * das deconfig aufruft, löscht er das Lease des neu gestarteten Clients.
   */
  if (0 == kill(pid_, SIGKILL)) {
    LogDebug("Stopped DHCP Client for interface " + itf_name_ + ", pid: " + ::std::to_string(pid_));
  }

  RemoveFile(LeaseFile::GetLeaseFilePath(itf_name_));
  RemoveFile(pid_file_path_);

  pid_ = 0;
}

} /* namespace netconf */
