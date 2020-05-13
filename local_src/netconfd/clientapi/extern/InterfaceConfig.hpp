// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "Status.hpp"

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <cstring>

namespace netconf {

constexpr size_t mac_addr_length = 6;

struct MacAddress {
  uint8_t addr_[mac_addr_length] = {0};

  MacAddress(const uint8_t *mac) {

    ::std::memcpy(addr_, mac, mac_addr_length);
  }
};

enum class InterfaceState {
  UNKNOWN,  //< not determined yet
  DOWN,    //< either logical down or no link is established
  UP,      //< link is established and the interface is logical up
};

enum class Duplex {
  UNKNOWN,
  HALF,
  FULL
};

enum class Autonegotiation {
  UNKNOWN,
  OFF,
  ON
};

MacAddress GetMacAddress(const ::std::string &interface_name);

/**
 * @brief Represents the interfaces' physical properties
 */
struct InterfaceConfig {
  static constexpr int UNKNOWN_SPEED = -1;

  explicit InterfaceConfig(::std::string device_name, InterfaceState state = InterfaceState::UNKNOWN,
                           Autonegotiation autoneg = Autonegotiation::UNKNOWN, int speed = UNKNOWN_SPEED,
                           Duplex duplex = Duplex::UNKNOWN) noexcept
      :
      interface_ { std::move(device_name) },
      state_ { state },
      autoneg_ { autoneg },
      speed_ { speed },
      duplex_ { duplex } {
  }

  InterfaceConfig() noexcept
      :
      interface_ { "" },
      state_ { InterfaceState::UNKNOWN },
      autoneg_ { Autonegotiation::UNKNOWN },
      speed_ { UNKNOWN_SPEED },
      duplex_ { Duplex::UNKNOWN } {
  }

  ::std::string interface_;
  InterfaceState state_;
  Autonegotiation autoneg_;
  int speed_;
  Duplex duplex_;

  bool operator==(const InterfaceConfig &config) const {
    return ((this->interface_ == config.interface_) && (this->state_ == config.state_)
        && (this->autoneg_ == config.autoneg_) && (this->duplex_ == config.duplex_) && (this->speed_ == config.speed_));
  }

  static InterfaceConfig DefaultConfig(::std::string device_name) {
    return InterfaceConfig { ::std::move(device_name), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL };
  }

  void FillUpDefaults() {
    state_ = (state_ == InterfaceState::UNKNOWN) ? InterfaceState::UP : state_;
    autoneg_ = (autoneg_ == Autonegotiation::UNKNOWN) ? Autonegotiation::ON : autoneg_;
    speed_ = (speed_ == UNKNOWN_SPEED) ? 100 : speed_;
    duplex_ = (duplex_ == Duplex::UNKNOWN) ? Duplex::FULL : duplex_;
  }
};

/**
 * @brief Container class for the @see InterfaceConfig
 *
 */
class InterfaceConfigs {
 public:
  InterfaceConfigs();
  explicit InterfaceConfigs(const ::std::string &json_str);
  ~InterfaceConfigs();

  InterfaceConfigs(const InterfaceConfigs&) noexcept;
  InterfaceConfigs& operator=(const InterfaceConfigs&);
  InterfaceConfigs(InterfaceConfigs&&) noexcept;
  InterfaceConfigs& operator=(const InterfaceConfigs&&) = delete;

  void AddInterfaceConfig(InterfaceConfig config);
  void RemoveInterfaceConfig(const ::std::string &interface_name);
  boost::optional<InterfaceConfig> GetInterfaceConfig(const ::std::string &interface_name);

  ::std::string ToJson() const;
  ::std::string ToString() const;

 private:
  class Impl;
  ::std::unique_ptr<Impl> impl_;
};

class InterfaceInfo {
 public:
  InterfaceInfo();
  explicit InterfaceInfo(const ::std::string &json_str);
  ~InterfaceInfo();

  InterfaceInfo(const InterfaceInfo&) noexcept;
  InterfaceInfo& operator=(const InterfaceInfo&) = delete;
  InterfaceInfo(InterfaceInfo&&) noexcept;
  InterfaceInfo& operator=(const InterfaceInfo&&) = delete;

  ::std::string ToJson() const;
 private:
 private:
  class Impl;
  ::std::unique_ptr<Impl> impl_;
};

/**
 * @brief Get the Interface Configs object from the netconfd network configuration daemon.
 *
 * @return InterfaceConfigs Current @see InterfaceConfigs
 */
InterfaceConfigs GetInterfaceConfigs();

/**
 * @brief Set the Interface Configs object for the netconfd network
 *
 * @param config
 * @return Status
 */
Status SetInterfaceConfigs(const InterfaceConfigs &config);

InterfaceInfo GetInterfaceInfo();

}  // namespace netconf
