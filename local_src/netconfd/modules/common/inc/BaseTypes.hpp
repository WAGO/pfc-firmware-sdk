// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>
#include <utility>

#include "MacAddress.hpp"

namespace netconf {

// TODO(SB): modernize with constexpr type when new cpp-std is available.
// Use static string to prevent Lint warning [cppcoreguidelines-pro-bounds-array-to-pointer-decay]
static ::std::string ZeroIP = "0.0.0.0";

struct Interface : public ::std::string {
  Interface()
      : ::std::string() {
  }
  Interface(const ::std::string &interface)
      : ::std::string(interface) {
  }
  using ::std::string::string;
};

struct Bridge : public ::std::string {
  Bridge()
      : ::std::string() {
  }
  ;
  Bridge(const ::std::string &str)
      : ::std::string(str) {
  }
  ;
  using ::std::string::string;
};

struct Address : public ::std::string {
  Address()
      : ::std::string() {
  }
  ;
  Address(const ::std::string &str)
      : ::std::string(str) {
  }
  ;
  using ::std::string::string;
};

struct Netmask : public ::std::string {
  Netmask()
      : ::std::string() {
  }
  ;
  Netmask(const ::std::string &str)
      : ::std::string(str) {
  }
  ;
  using ::std::string::string;
};

enum class IPSource {
  UNKNOWN,
  NONE,
  STATIC,
  DHCP,
  BOOTP,
  EXTERNAL,
  FIXIP
};

/**
 * @brief POCO class to hold the ip-config for one interface
 * @note The fields must be filled depending on the @see IPSource.
 *  The @see IPConfig::interface_ field must be set always.
 *  For the sources @see IPSource::NONE, @see IPSource::DHCP, @see IPSource::BOOTP and @see IPSource::EXTERNAL
 *  setting set the @see IPConfig::source_ field is sufficient.
 *  For the sources @see IPSource::STATIC and @see IPConfig::address_ fields needs to be
 * set also.
 *
 */
struct IPConfig {
  IPConfig()
      : interface_ {},
        source_ { IPSource::NONE },
        address_ { ZeroIP },
        netmask_ { ZeroIP } {
  }

  IPConfig(Interface interface, const IPSource source = IPSource::NONE)
      : interface_ { ::std::move(interface) },
        source_ { source },
        address_ { ZeroIP },
        netmask_ { ZeroIP } {
  }

  IPConfig(Interface interface, const IPSource source, const Address &address, const Netmask &netmask)
      : interface_ { ::std::move(interface) },
        source_ { source },
        address_ { address },
        netmask_ { netmask } {
  }

  static IPConfig CreateDefault(Interface interface) {
    return IPConfig(::std::move(interface), IPSource::STATIC, ZeroIP, ZeroIP);
  }

  void Clear() {
    source_ = IPSource::NONE;
    interface_.clear();
    address_ = ZeroIP;
    netmask_ = ZeroIP;
  }

  bool operator==(const IPConfig &config) const {
    return ((this->interface_ == config.interface_) && (this->source_ == config.source_)
        && (this->address_ == config.address_) && (this->netmask_ == config.netmask_));
  }

  bool operator <(const IPConfig &config) const {
    return (interface_ < config.interface_);
  }

  template<typename ... Args>
  static constexpr bool SourceChangesToAnyOf(const IPConfig &old, const IPConfig &new_, Args ... sources) {
    if (old.source_ != new_.source_) {
      if (sizeof...(sources) > 0) {
        return SourceIsAnyOf(new_, sources...);
      } else {
        return true;
      }
    }
    return false;
  }

  template<typename ... Args>
  static constexpr bool SourceIsAnyOf(const IPConfig &config, Args ... sources) {
    bool match = false;
    (void) std::initializer_list<int> { (match |= (config.source_ == (sources)),0)... };
    return match;
  }

  Interface interface_;
  IPSource source_;
  Address address_;
  Netmask netmask_;

};

enum class DipSwitchMode {
  OFF,
  STATIC,
  DHCP,
  HW_NOT_AVAILABLE
};

struct DipSwitchIpConfig {

  DipSwitchIpConfig() {}

  DipSwitchIpConfig(const Address &address, const Netmask &netmask)
      : address_(address),
        netmask_(netmask) {
  }

  bool IsIncomplete() const {
    return (address_.empty() || netmask_.empty());
  }

  void Clear() {
    address_.clear();
    netmask_.clear();
  }

  bool operator==(const DipSwitchIpConfig &config) const {
    return ((this->address_ == config.address_) && (this->netmask_ == config.netmask_));
  }

  Address address_;
  Netmask netmask_;
};

struct DipSwitchConfig {
  DipSwitchConfig()
      : DipSwitchConfig(DipSwitchIpConfig { }) {
  }

  explicit DipSwitchConfig(const DipSwitchIpConfig &ip_config)
      : DipSwitchConfig(ip_config, DipSwitchMode::OFF, 0) {
  }

  DipSwitchConfig(const DipSwitchIpConfig &ip_config, DipSwitchMode mode, int value)
      : ip_config_(ip_config),
        mode_(mode),
        value_(value) {
  }

  DipSwitchIpConfig ip_config_;
  DipSwitchMode mode_;
  int value_;
};

enum class InterfaceState {
  UNKNOWN,
  DOWN,
  UP,
};

enum class LinkState {
  UNKNOWN,
  DOWN,
  UP,
};

enum class Duplex {
  UNKNOWN = 0xFF,
  HALF = 0,
  FULL = 1
};

enum class Autonegotiation {
  UNKNOWN,
  ON,
  OFF
};

enum class AutonegotiationSupported {
  UNKNOWN,
  NO,
  YES
};

enum class MacLearning {
  UNKNOWN,
  ON,
  OFF,
};

struct InterfaceBase {
  static constexpr int UNKNOWN_SPEED = -1;

  InterfaceBase(
      ::std::string device_name,
      InterfaceState state = InterfaceState::UNKNOWN,
      Autonegotiation autoneg = Autonegotiation::UNKNOWN,
      int speed = UNKNOWN_SPEED,
      Duplex duplex = Duplex::UNKNOWN,
      MacLearning mac_learning = MacLearning::UNKNOWN) noexcept

      : device_name_ { std::move(device_name) },
        state_ { state },
        autoneg_ { autoneg },
        speed_ { speed },
        duplex_ { duplex },
        mac_learning_{ mac_learning}{
  }

  InterfaceBase() noexcept
      : device_name_ { },
        state_ { InterfaceState::UNKNOWN },
        autoneg_ { Autonegotiation::UNKNOWN },
        speed_ { UNKNOWN_SPEED },
        duplex_ { Duplex::UNKNOWN },
        mac_learning_ { MacLearning::UNKNOWN }{
  }

  bool operator==(const InterfaceBase &config) const {
    return ((this->autoneg_ == config.autoneg_)
        && (this->device_name_ == config.device_name_)
        && (this->duplex_ == config.duplex_)
        && (this->speed_ == config.speed_)
        && (this->state_ == config.state_)
        && (this->mac_learning_ == config.mac_learning_));
  }

  void FillUpDefaults() {
    state_ = (state_ == InterfaceState::UNKNOWN) ? InterfaceState::UP : state_;
    autoneg_ = (autoneg_ == Autonegotiation::UNKNOWN) ? Autonegotiation::ON : autoneg_;
    speed_ = (speed_ == UNKNOWN_SPEED) ? 100 : speed_;
    duplex_ = (duplex_ == Duplex::UNKNOWN) ? Duplex::FULL : duplex_;
    mac_learning_ = (mac_learning_ == MacLearning::UNKNOWN) ? MacLearning::ON : mac_learning_;
  }

  static InterfaceBase DefaultConfig(::std::string device_name) {
    return InterfaceBase { device_name, InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON };
  }

  ::std::string device_name_;
  InterfaceState state_;
  Autonegotiation autoneg_;
  int speed_;
  Duplex duplex_;
  MacLearning mac_learning_;

};

using InterfaceConfig = InterfaceBase;

struct InterfaceStatus : InterfaceBase {

  InterfaceStatus(
      ::std::string device_name,
      InterfaceState state = InterfaceState::UNKNOWN,
      Autonegotiation autoneg = Autonegotiation::UNKNOWN,
      int speed = UNKNOWN_SPEED,
      Duplex duplex = Duplex::UNKNOWN,
      LinkState link_state = LinkState::UNKNOWN,
      MacAddress mac = MacAddress { }) noexcept

      : InterfaceBase { device_name, state, autoneg, speed, duplex },
        link_state_ { link_state },
        mac_ { mac } {
  }

  InterfaceStatus(InterfaceBase base) noexcept
      : InterfaceBase { base },
        link_state_ { LinkState::UNKNOWN },
        mac_ { } {
  }

  InterfaceStatus() noexcept
      : InterfaceBase { "", InterfaceState::UNKNOWN, Autonegotiation::UNKNOWN, UNKNOWN_SPEED, Duplex::UNKNOWN },
        link_state_ { LinkState::UNKNOWN },
        mac_ { } {
  }

  bool operator==(const InterfaceStatus &config) const {
    return ((this->autoneg_ == config.autoneg_)
        && (this->device_name_ == config.device_name_)
        && (this->duplex_ == config.duplex_)
        && (this->speed_ == config.speed_)
        && (this->state_ == config.state_)
        && (this->link_state_ == config.link_state_)
        && (this->mac_ == config.mac_));
  }

  LinkState link_state_;
  MacAddress mac_;
};

}  // netconf
