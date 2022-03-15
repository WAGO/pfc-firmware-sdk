// SPDX-License-xpIdentifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include "HostsFile.hpp"

namespace netconf {

TEST(HostsFileTest, UpdateFileContent) {

  ::std::string old_hosts = "user data befor marker line1\n"
      "user data befor marker line2\n"
      "user data befor marker line3\n"
      "#SYSTEM HOST ENTRY -- DO NOT REMOVE -- WILL BE CREATED BY NETCONF\n"
      "127.0.0.1\tlocalhost\n"
      "127.0.1.1\tfred.mydomain.org\tfred\n"
      "192.168.10.10\tfred.mydomain.org\tfred\n"
      "192.168.20.10\tfred.mydomain.org\tfred\n"
      "#END SYSTEM HOST -- DO NOT REMOVE\n"
      "user data after marker line1\n"
      "user data after marker line2\n"
      "user data after marker line3\n";

  ::std::string expected_new_hosts = "user data befor marker line1\n"
      "user data befor marker line2\n"
      "user data befor marker line3\n"
      "#SYSTEM HOST ENTRY -- DO NOT REMOVE -- WILL BE CREATED BY NETCONF\n"
      "127.0.0.1\tlocalhost\n"
      "127.0.1.1\tfred2.mydomain2.org\tfred2\n"
      "192.168.20.10\tfred2.mydomain2.org\tfred2\n"
      "192.168.30.10\tfred2.mydomain2.org\tfred2\n"
      "192.168.40.10\tfred2.mydomain2.org\tfred2\n"
      "#END SYSTEM HOST -- DO NOT REMOVE\n"
      "user data after marker line1\n"
      "user data after marker line2\n"
      "user data after marker line3\n";

  ::std::vector<::std::string> ips { "192.168.20.10", "192.168.30.10", "192.168.40.10" };
  ::std::string hostname = "fred2";
  ::std::string domain = "mydomain2.org";
  auto new_hosts = UpdateFileContent(old_hosts, ips, hostname, domain);

  EXPECT_EQ(expected_new_hosts, new_hosts);
}

TEST(HostsFileTest, AppendEntiresIfMarkersAreMissing) {

  ::std::string old_hosts = "user data befor marker line1\n"
      "user data befor marker line2\n"
      "user data befor marker line3\n"
      "\n"
      "\n"
      "user data after marker line4\n";

  ::std::string expected_new_hosts = "user data befor marker line1\n"
      "user data befor marker line2\n"
      "user data befor marker line3\n"
      "\n"
      "\n"
      "user data after marker line4\n"
      "\n#SYSTEM HOST ENTRY -- DO NOT REMOVE -- WILL BE CREATED BY NETCONF\n"
      "127.0.0.1\tlocalhost\n"
      "127.0.1.1\tfred.mydomain.org\tfred\n"
      "192.168.20.10\tfred.mydomain.org\tfred\n"
      "#END SYSTEM HOST -- DO NOT REMOVE\n";

  ::std::vector<::std::string> ips { "192.168.20.10"};
  ::std::string hostname = "fred";
  ::std::string domain = "mydomain.org";

  auto new_hosts = UpdateFileContent(old_hosts, ips, hostname, domain);

  EXPECT_EQ(expected_new_hosts, new_hosts);
}

TEST(HostsFileTest, LeaveExistingLocalHostEntryUntouched) {

  ::std::string old_hosts = "127.0.0.1\tlocalhost";

  ::std::string expected_new_hosts = "127.0.0.1\tlocalhost\n"
      "#SYSTEM HOST ENTRY -- DO NOT REMOVE -- WILL BE CREATED BY NETCONF\n"
      "127.0.1.1\tfred.mydomain.org\tfred\n"
      "192.168.20.10\tfred.mydomain.org\tfred\n"
      "#END SYSTEM HOST -- DO NOT REMOVE\n";

  ::std::vector<::std::string> ips { "192.168.20.10"};
  ::std::string hostname = "fred";
  ::std::string domain = "mydomain.org";

  auto new_hosts = UpdateFileContent(old_hosts, ips, hostname, domain);

  EXPECT_EQ(expected_new_hosts, new_hosts);
}


}  // namespace netconf
