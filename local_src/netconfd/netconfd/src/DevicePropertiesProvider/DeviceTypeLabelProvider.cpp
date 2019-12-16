// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceTypeLabelProvider.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>
#include <EthernetInterface.hpp>
#include <iomanip>

#include "Logger.hpp"
#include "CommandExecutor.hpp"

namespace netconfd {


using boost::property_tree::ptree;
using namespace std::literals;

DeviceTypeLabelProvider::DeviceTypeLabelProvider(CommandExecutor& executor)
    : executor_(executor) {

  ::std::string type_label_ini;
  Status status = executor_.Execute("/etc/config-tools/get_typelabel_value -a", type_label_ini);

  if (status.NotOk()) {
    LogError("Failed to extract typelabel values! Taking fallback values."s);
    GetMacFallback();
    GetOrderFallback();
  }

  try {
    ptree type_label_values;
    ::std::istringstream type_label_stringstream(type_label_ini);

    boost::property_tree::read_ini(type_label_stringstream, type_label_values);

    order_number_ = type_label_values.get<::std::string>("ORDER");
    mac_ = type_label_values.get<::std::string>("MAC");

    if (type_label_values.count("MAC_ID_INC") == 0) {
      mac_increment_ = 1;
    } else {
      mac_increment_ = type_label_values.get<uint32_t>("MAC_ID_INC");
    }

  } catch (std::exception& e) {
    LogError("Failed to extract typelabel values! Taking fallback values. ("s.append(e.what()).append(")") );
    if (order_number_.empty()) {
      GetOrderFallback();
    }
    if (mac_.empty()) {
      GetMacFallback();
    }
  }

}

::std::string DeviceTypeLabelProvider::GetOrderNumber() const {
  return order_number_;
}

::std::string DeviceTypeLabelProvider::GetMac() const {
  return mac_;
}

static std::string IntToMac(uint64_t mac) {

  std::stringstream stream;
  stream << std::setfill('0') << std::setw(12) << std::hex << mac;

  ::std::string hex_mac = stream.str();

  for (uint32_t i = 5; i > 0; i--) {
    ::std::string colon = ":";
    hex_mac.insert(i * 2, colon);
  }

  transform(hex_mac.begin(), hex_mac.end(), hex_mac.begin(), ::toupper);

  return hex_mac;
}

static uint64_t MacToInt(::std::string const& mac) {

  ::std::string mac_copy = mac;

  mac_copy.erase(std::remove(mac_copy.begin(), mac_copy.end(), ':'), mac_copy.end());

  return std::stoull(mac_copy, nullptr, 16);
}

::std::string DeviceTypeLabelProvider::GetIncrementedMac(uint32_t inc) const {

  ::std::string incremented_mac;
  if (inc < mac_increment_) {

    try {

      uint64_t mac = MacToInt(mac_);
      mac += inc;
      incremented_mac = IntToMac(mac);

      return incremented_mac;
    } catch (std::exception& e) {
      LogError("DeviceTypeLabelProvider: Failed to increment MAC. ("s.append(e.what()).append(")"));
    }
  }

  return mac_;
}

void DeviceTypeLabelProvider::GetMacFallback() {

  EthernetInterface eth0If { "eth0" };

  auto mac_bytes = eth0If.GetMac();
  std::array<char, 16> mac_str = {};

  snprintf(mac_str.data(), mac_str.size(), "%02x:%02x:%02x:%02x:%02x:%02x", mac_bytes[0], mac_bytes[1], mac_bytes[2],
           mac_bytes[3], mac_bytes[4], mac_bytes[5]);

  mac_.assign(mac_str.begin(), mac_str.end());
}

void DeviceTypeLabelProvider::GetOrderFallback() {
  order_number_ = "wago-pfc";
}

} /* namespace netconfd */
