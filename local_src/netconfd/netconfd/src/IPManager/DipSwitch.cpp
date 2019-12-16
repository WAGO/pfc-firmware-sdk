// SPDX-License-Identifier: GPL-2.0-or-later

#include "DipSwitch.hpp"

#include <boost/filesystem.hpp>
#include <boost/convert.hpp>
#include <boost/convert/stream.hpp>

namespace netconfd {

namespace fs = boost::filesystem;

using boost::convert;

DipSwitch::DipSwitch(const std::string& dip_switch_device_value_file)
    : value_ { 0 } {
      boost::cnv::cstream ccnv;
  try {
    fs::path dip_sw_path { dip_switch_device_value_file };
    if (fs::exists(dip_sw_path)) {
      std::string dip_value;
      fs::ifstream stream { dip_sw_path };
      stream >> dip_value;
      value_ = static_cast<std::uint8_t>(convert<int>( dip_value, ccnv(std::hex)).value_or(0));
    }
  } catch (...) {
    /* nothing to do here */
  }

}

} /* namespace netconfd */
