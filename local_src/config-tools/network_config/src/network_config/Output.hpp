// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <fstream>
#include <ostream>
#include <iostream>

namespace network_config {

class Output {
 public:
  virtual ~Output() = default;
  virtual ::std::ostream& GetStream() = 0;
};

class StderrOutput : public Output {
 public:
  ::std::ostream& GetStream() override {
    return ::std::cerr;
  }
};

class FileOutput : public Output {
 public:
  FileOutput(::std::string filename)
      : ostream_ { filename } {
    if (!ostream_.is_open()) {
      throw ::std::ios::failure { "Failed to open: " + filename };
    }

  }
  ~FileOutput() {
    ostream_.close();
  }

  ::std::ostream& GetStream() override {
    return ostream_;
  }

 private:
  ::std::ofstream ostream_;

};

class NullOutput : public Output , private std::ostream  {
 public:

  NullOutput() = default;
  NullOutput(const NullOutput& other) = default;
  NullOutput(NullOutput&& other) = default;

  ::std::ostream& GetStream() override {
    return *this;
  }

 private:

};

} /* namespace network_config */

