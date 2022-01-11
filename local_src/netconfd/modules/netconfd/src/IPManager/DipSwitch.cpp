// SPDX-License-Identifier: GPL-2.0-or-later

#include "DipSwitch.hpp"

#include <boost/filesystem.hpp>
#include <boost/convert.hpp>
#include <boost/convert/stream.hpp>

namespace netconf {

namespace fs = boost::filesystem;

using boost::convert;

DipSwitch::DipSwitch(const std::string& dip_switch_device_value_file)
    : value_ { 0 }, hw_available_{false} {
      boost::cnv::cstream ccnv;
  try {
    fs::path dip_sw_path { dip_switch_device_value_file };
    if (fs::exists(dip_sw_path)) {
      std::string dip_value;
      fs::ifstream stream { dip_sw_path };
      stream >> dip_value;
      value_ = static_cast<std::uint8_t>(convert<int>( dip_value, ccnv(std::hex)).value_or(0));
      hw_available_ = true;
    }
  } catch (...) {
    /* nothing to do here */
  }
}

[[gnu::pure]]
DipSwitchMode DipSwitch::GetMode() const {
  if(!hw_available_) {
    return DipSwitchMode::HW_NOT_AVAILABLE;
  }

  switch (value_)
  {
    case 0:
      return DipSwitchMode::OFF;
    case 255:
      return DipSwitchMode::DHCP;
    default:
      return DipSwitchMode::STATIC;
  }
}

DipSwitchState DipSwitch::GetState() const {
  return {GetMode(), value_};
}

} /* namespace netconf */
