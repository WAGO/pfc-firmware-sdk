// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     BridgeConfig.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef EXTERN_BRIDGECONFIG_HPP_
#define EXTERN_BRIDGECONFIG_HPP_

#include <utility>
#include <map>
#include <string>
#include <vector>

namespace netconfd {

// TODO(JSo): modernize constexpr type when new cpp-std is available
constexpr char ZeroIP[] = "0.0.0.0";

//strong typedef
struct Interface : public ::std::string {
  Interface()
      :
      ::std::string() {
  }
  Interface(const ::std::string &interface)
      :
      ::std::string(interface) {
  }
  using ::std::string::string;
};

struct Bridge : public ::std::string {
  Bridge()
      :
      ::std::string() {
  }
  ;
  Bridge(const ::std::string &str)
      :
      ::std::string(str) {
  }
  ;
  using ::std::string::string;
};

struct Address : public ::std::string {
  Address()
      :
      ::std::string() {
  }
  ;
  Address(const ::std::string &str)
      :
      ::std::string(str) {
  }
  ;
  using ::std::string::string;
};

struct Netmask : public ::std::string {
  Netmask()
      :
      ::std::string() {
  }
  ;
  Netmask(const ::std::string &str)
      :
      ::std::string(str) {
  }
  ;
  using ::std::string::string;
};

struct Broadcast : public ::std::string {
  Broadcast()
      :
      ::std::string() {
  }
  ;
  Broadcast(const ::std::string &str)
      :
      ::std::string(str) {
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
  TEMPORARY
};

struct IPConfig {
  IPConfig()
      :
      interface_ { "" },
      source_ { IPSource::NONE },
      address_ { "" },
      netmask_ { "" },
      broadcast_ { "" } {
  }

  IPConfig(const Interface &interface, const IPSource source)
      :
      interface_ { interface },
      source_ { source },
      address_ { "" },
      netmask_ { "" },
      broadcast_ { "" } {
  }

  IPConfig(const Interface &interface, const IPSource source, const Address &address, const Netmask &netmask)
      :
      interface_ { interface },
      source_ { source },
      address_ { address },
      netmask_ { netmask },
      broadcast_ { "" } {
  }

  IPConfig(const Interface &interface, const IPSource source, const Address &address, const Netmask &netmask,
           const Broadcast &broadcast)
      :
      interface_ { interface },
      source_ { source },
      address_ { address },
      netmask_ { netmask },
      broadcast_ { broadcast } {
  }

  static IPConfig CreateDefault(const Interface &interface) {
    return IPConfig(interface, IPSource::STATIC, ZeroIP, ZeroIP, ZeroIP);
  }

  void Clear() {
    source_ = IPSource::NONE;
    interface_ = "";
    address_ = "";
    netmask_ = "";
    broadcast_ = "";
  }

  bool operator==(const IPConfig &config) const {
    return ((this->interface_ == config.interface_) && (this->source_ == config.source_)
        && (this->address_ == config.address_) && (this->netmask_ == config.netmask_)
        && (this->broadcast_ == config.broadcast_));
  }

  bool operator <(const IPConfig &config) const {
    return (interface_ < config.interface_);
  }

  template<typename ... Args>
  static constexpr bool SourceChangesToAnyOf(const IPConfig &old, const IPConfig &new_, Args ... sources) {
    if (old.source_ != new_.source_) {
      if (sizeof...(sources) > 0) {
        return SourceIsAnyOf(new_, sources...);
      }
      else {
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
  Broadcast broadcast_;

};

enum DipSwitchMode {
  OFF,
  STATIC,
  DHCP,
  HW_NOT_AVAILABLE
};

struct DipSwitchIpConfig {

  DipSwitchIpConfig()
      :
      address_(""),
      netmask_("") {
  }

  DipSwitchIpConfig(const Address &address, const Netmask &netmask)
      :
      address_(address),
      netmask_(netmask) {
  }

  bool IsIncomplete() const {
    return (address_.empty() || netmask_.empty());
  }

  void Clear() {
    address_ = "";
    netmask_ = "";
  }

  bool operator==(const DipSwitchIpConfig &config) const {
    return ((this->address_ == config.address_) && (this->netmask_ == config.netmask_));
  }

  Address address_;
  Netmask netmask_;
};

struct DipSwitchConfig {
  DipSwitchConfig(const DipSwitchIpConfig &ip_config, const DipSwitchMode &mode, const int &value)
      :
      ip_config_(ip_config),
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

enum class Duplex {
  UNKNOWN,
  HALF,
  FULL
};

enum class Autonegotiation {
  UNKNOWN,
  ON,
  OFF
};

struct InterfaceConfig {
  static constexpr int UNKNOWN_SPEED = -1;

  InterfaceConfig(::std::string device_name, InterfaceState state = InterfaceState::UNKNOWN, Autonegotiation autoneg =
                      Autonegotiation::UNKNOWN,
                  int speed = UNKNOWN_SPEED, Duplex duplex = Duplex::UNKNOWN) noexcept
      :
      device_name_ { std::move(device_name) },
      state_ { state },
      autoneg_ { autoneg },
      speed_ { speed },
      duplex_ { duplex } {
  }

  InterfaceConfig() noexcept
      :
      device_name_ { },
      state_ { InterfaceState::UNKNOWN },
      autoneg_ { Autonegotiation::UNKNOWN },
      speed_ { UNKNOWN_SPEED },
      duplex_ { Duplex::UNKNOWN } {
  }

  bool operator==(const InterfaceConfig &config) const {
    return ((this->autoneg_ == config.autoneg_) && (this->device_name_ == config.device_name_)
        && (this->duplex_ == config.duplex_) && (this->speed_ == config.speed_) && (this->state_ == config.state_));
  }

  void FillUpDefaults() {
    state_ = (state_ == InterfaceState::UNKNOWN) ? InterfaceState::UP : state_;
    autoneg_ = (autoneg_ == Autonegotiation::UNKNOWN) ? Autonegotiation::ON : autoneg_;
    speed_ = (speed_ == UNKNOWN_SPEED) ? 100 : speed_;
    duplex_ = (duplex_ == Duplex::UNKNOWN) ? Duplex::FULL : duplex_;
  }

  static InterfaceConfig DefaultConfig(::std::string device_name) {
    return InterfaceConfig { device_name, InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL };
  }

  ::std::string device_name_;
  InterfaceState state_;
  Autonegotiation autoneg_;
  int speed_;
  Duplex duplex_;

};

using Interfaces = ::std::vector<Interface>;
using Bridges = ::std::vector<Bridge>;
using BridgePair = std::pair<Bridge, Interfaces>;
using BridgeConfig = ::std::map<Bridge, Interfaces>;

using InterfaceNamesPair = std::pair<Interface, Interface>;
using InterfaceNameMapping = ::std::map<Interface, Interface>;

using IPConfigs = ::std::vector<IPConfig>;
class InterfaceConfigs : public ::std::vector<InterfaceConfig> {
  using ::std::vector<InterfaceConfig>::vector;
};

}

#endif /* EXTERN_BRIDGECONFIG_HPP_ */
//---- End of source file ------------------------------------------------------
