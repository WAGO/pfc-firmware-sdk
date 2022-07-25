// SPDX-License-Identifier: GPL-2.0-or-later

#include "BOOTPClient.hpp"
#include "Logger.hpp"
#include "CollectionUtils.hpp"

#include <glib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

#include <csignal>
#include <cstdlib>
#include <thread>

namespace netconf {

namespace {

void RemoveFile(::std::string path) {
  const boost::filesystem::path file = path;
  if (boost::filesystem::exists(file)) {
    boost::filesystem::remove(file);
  }
}

}

BOOTPClient::BOOTPClient(::std::string itf_name)
    : itf_name_ { ::std::move(itf_name) },
      pid_{}{

  pid_file_path_ = "/var/run/udhcpc_" + itf_name_ + ".pid";

  Start();
}

BOOTPClient::~BOOTPClient() {
  Stop();
}

DynamicIPType BOOTPClient::GetType() {
  return DynamicIPType::BOOTP;
}

void BOOTPClient::UpdateContentFromLease() {
  lease_file_.Parse(LeaseFile::GetLeaseFilePath(itf_name_));
}

Address BOOTPClient::GetAddressFromLease() {
  return lease_file_.GetAddress();
}
Netmask BOOTPClient::GetNetmaskFromLease() {
  return lease_file_.GetNetmask();
}
::std::string BOOTPClient::GetHostnameFromLease() {
  return lease_file_.GetDHCPHostname();
}
::std::string BOOTPClient::GetDomainFromLease() {
  return lease_file_.GetDHCPDomain();
}

void BOOTPClient::Start() {

  LOG_DEBUG("Run Bootp client for interface " + itf_name_);

  auto argv_array = make_array(BOOTP_SCRIPT_PATH.c_str(), "start", itf_name_.c_str(), nullptr);

  GError *g_error = nullptr;
  auto spawned = g_spawn_async(nullptr, const_cast<gchar**>(argv_array.data()), nullptr, G_SPAWN_DEFAULT, nullptr,  // NOLINT(cppcoreguidelines-pro-type-const-cast)
                               nullptr, &pid_, &g_error);

  if (spawned != TRUE) {
    LogError("Start Bootp Client for interface " + itf_name_ + " failed!");
  }
}

void BOOTPClient::Stop() {

  if (0 == kill(pid_, SIGKILL)) {
    LOG_DEBUG("Stopped DHCP Client for interface " + itf_name_);
  }

  RemoveFile(LeaseFile::GetLeaseFilePath(itf_name_));
  RemoveFile(pid_file_path_);

  pid_ = 0;
}

void BOOTPClient::Release() {
  Stop();
}

void BOOTPClient::Renew() {
  Start();
}

void BOOTPClient::Restart(::std::string hostname){
  (void)hostname;
}

} /* namespace netconf */
