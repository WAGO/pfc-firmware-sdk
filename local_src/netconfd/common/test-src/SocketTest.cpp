// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <linux/in.h>
#include "Socket.hpp"

using namespace testing;

static int closedfd = 0;

extern "C" {

int socket([[maybe_unused]]int __domain, [[maybe_unused]] int __type, [[maybe_unused]] int __protocol) {
  return 42;
}

int close(int fd) {
  closedfd = fd;
  return 0;
}

}

namespace netconf {

TEST(SocketTest, CreateDestroy) {

  {
    Socket s { AF_INET, SOCK_DGRAM, IPPROTO_IP };
    EXPECT_EQ(static_cast<int>(s), 42);
  }
  EXPECT_EQ(closedfd, 42);

}

TEST(SocketTest, Copy) {

  Socket orig { AF_INET, SOCK_DGRAM, IPPROTO_IP };
  ASSERT_EQ(static_cast<int>(orig), 42);

  Socket copy {orig};

  EXPECT_EQ(static_cast<int>(orig), 42);
  EXPECT_EQ(static_cast<int>(copy), 42);

  Socket assigned;
  EXPECT_EQ(static_cast<int>(assigned), -1);
  assigned = orig;

  EXPECT_EQ(static_cast<int>(assigned), 42);
  EXPECT_EQ(static_cast<int>(copy), 42);

}

TEST(SocketTest, Move) {

  Socket orig { AF_INET, SOCK_DGRAM, IPPROTO_IP };
  ASSERT_EQ(static_cast<int>(orig), 42);

  Socket moved {::std::move(orig)};

  EXPECT_EQ(static_cast<int>(orig), -1);
  EXPECT_EQ(static_cast<int>(moved), 42);

}

}
