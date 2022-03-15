// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * UriEscapeTest.cpp
 *
 *  Created on: 29.10.2019
 *      Author: u014487
 */

#include <gtest/gtest.h>

#include "UriEscape.hpp"

TEST(UriEscapeTest, EncodeUri) {
  ::std::string expectation = R"({
  "bridge-config": {
    "br0": [
      "X1"
    ],
    "br1": [
      "X2"
    ]
  },
  "ip-config": {
    "br0": {
      "bcast": "192.168.1.255",
      "ipaddr": "192.168.1.17",
      "netmask": "255.255.255.0",
      "source": "dhcp"
    },
    "br1": {
      "bcast": "",
      "ipaddr": "192.168.2.100",
      "netmask": "255.255.255.0",
      "source": "static"
    }
  }
})";

  const auto encoded_request =
      "%7B%0A%20%20%22bridge-config%22%3A%20%7B%0A%20%20%20%20%22br0%22%3A%20%5B%0A%20%20%20%20%20%20%22X1%22%0A%20%20%"
      "20%20%5D%2C%0A%20%20%20%20%22br1%22%3A%20%5B%0A%20%20%20%20%20%20%22X2%22%0A%20%20%20%20%5D%0A%20%20%7D%2C%0A%"
      "20%20%22ip-config%22%3A%20%7B%0A%20%20%20%20%22br0%22%3A%20%7B%0A%20%20%20%20%20%20%22bcast%22%3A%20%22192.168."
      "1.255%22%2C%0A%20%20%20%20%20%20%22ipaddr%22%3A%20%22192.168.1.17%22%2C%0A%20%20%20%20%20%20%22netmask%22%3A%20%"
      "22255.255.255.0%22%2C%0A%20%20%20%20%20%20%22source%22%3A%20%22dhcp%22%0A%20%20%20%20%7D%2C%0A%20%20%20%20%"
      "22br1%22%3A%20%7B%0A%20%20%20%20%20%20%22bcast%22%3A%20%22%22%2C%0A%20%20%20%20%20%20%22ipaddr%22%3A%20%22192."
      "168.2.100%22%2C%0A%20%20%20%20%20%20%22netmask%22%3A%20%22255.255.255.0%22%2C%0A%20%20%20%20%20%20%22source%22%"
      "3A%20%22static%22%0A%20%20%20%20%7D%0A%20%20%7D%0A%7D";
  UriEscape uri_escape;

  auto unescaped_request      = uri_escape.Unescape(encoded_request);
  auto escaped_request_result = uri_escape.Escape(expectation);
  EXPECT_EQ(expectation, unescaped_request);
  EXPECT_EQ(encoded_request, escaped_request_result);
}
