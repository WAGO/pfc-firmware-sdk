// SPDX-License-Identifier: GPL-2.0-or-later
#include <gtest/gtest.h>

#include <thread>

#include "NetconfdDbusClient.hpp"

namespace netconf {

class NetconfdDbusClientTest_Target : public testing::Test {
 public:
  ::std::chrono::seconds service_timeout    = ::std::chrono::seconds(5);
  #define BACKUP_FILE "/tmp/backup.1234"
  ::std::string backup_file = BACKUP_FILE;

  void Stop() {
    system("/etc/init.d/netconfd stop");
  }

  void Start() {
    system("/etc/init.d/netconfd start");
  }

  void SetUp() override {
    Start();
  }

  void TearDown() override {
    Start();
  }
};

TEST_F(NetconfdDbusClientTest_Target, CheckServiceAvailability) {
  NetconfdDbusClient client;

  auto is_available = client.CheckServiceAvailability(service_timeout);
  EXPECT_TRUE(is_available);

  Stop();

  is_available = client.CheckServiceAvailability(service_timeout);
  EXPECT_FALSE(is_available);
}


TEST_F(NetconfdDbusClientTest_Target, BackupRestore) {
    NetconfdDbusClient client;
    system("touch " BACKUP_FILE);

    auto backup_success = client.Backup(backup_file, "");
    EXPECT_TRUE(backup_success);

    auto param_count = client.GetBackupParameterCount();
    EXPECT_FALSE(param_count.empty());

    auto restore_success = client.Restore(backup_file);
    EXPECT_TRUE(restore_success);

    system("rm -f "  BACKUP_FILE);
}

}  // namespace netconf
