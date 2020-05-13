// SPDX-License-Identifier: GPL-2.0-or-later

#include "Chronometer.hpp"

#include <chrono>
#include <iostream>

namespace netconfd {

namespace Color {
enum Code {
  FG_RED = 31,
  FG_GREEN = 32,
  FG_BLUE = 34,
  FG_DEFAULT = 39,
  BG_RED = 41,
  BG_GREEN = 42,
  BG_BLUE = 44,
  BG_DEFAULT = 49
};

class Modifier {
  Code code;
 public:
  explicit Modifier(Code pCode)
      : code(pCode) {
  }
  friend std::ostream&
  operator<<(std::ostream &os, const Modifier &mod) {
    return os << "\033[" << static_cast<unsigned int>(mod.code) << "m";
  }
};
}

class Chronometer {
 public:
  Chronometer()
      : modifier_red_ { Color::BG_GREEN },
        modifier_default_ { Color::BG_DEFAULT } {
    Reset();
  }

  void Reset() {
    begin_ = std::chrono::steady_clock::now();
    last_ = std::chrono::steady_clock::now();

    PrintMs("On Reset");
  }

  void PrintTimeMs(const ::std::string &discripton) {
    PrintMs(discripton);
    last_ = std::chrono::steady_clock::now();
  }

 private:

  void PrintMs(const ::std::string &discripton) {
    std::chrono::steady_clock::time_point actual = std::chrono::steady_clock::now();

    ::std::cout << modifier_red_ << "TIME MESUREMENT:" << modifier_default_ << " " << ": abs:"
        << std::chrono::duration_cast < std::chrono::milliseconds
        > (actual - begin_).count() << "[ms]" << ": rel:" << std::chrono::duration_cast < std::chrono::milliseconds
        > (actual - last_).count() << "[ms]" << " " << discripton << std::endl;
  }

  std::chrono::steady_clock::time_point begin_;
  std::chrono::steady_clock::time_point last_;

  Color::Modifier modifier_red_;
  Color::Modifier modifier_default_;

};

static Chronometer& GetChronometer() {
  static Chronometer chronometer;
  return chronometer;
}

void ChronometerStart() {
  GetChronometer().Reset();
}

void ChronometerPrintTimeMs(const ::std::string &discripton) {
  GetChronometer().PrintTimeMs(discripton);
}

}
