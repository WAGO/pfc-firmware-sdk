#pragma once

#include <string>

namespace netconf {

static void RemoveCharFromString(::std::string &str, char c) {
  str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

static void StringToUpper(::std::string &str) {
  ::std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
    return std::toupper(c);
  }
  );
}

}
