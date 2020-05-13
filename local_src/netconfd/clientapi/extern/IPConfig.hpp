// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "Status.hpp"

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <vector>

namespace netconf {

enum class IPSource {
  UNKNOWN,
  NONE,    //< Used to delete an existing config
  STATIC,  //< Statically applied ip config. The IP address will set when the ethnet link is up and removed on link down
           //(LOWER_UP/LOWER_DOWN).
  DHCP,    //< IP config over dhcp; starts the dhcp client
  BOOTP,   //< IP config over bootp; starts the bootp client
  EXTERNAL,  //< IP config is managed externally, netconfd does not interfere in any way
  TEMPORARY  //< IP config is applied temporarly, the dhcp client uses this to set the ip-config. Temporary IP addresses
             // will be removed on link down.
};

/**
 * @brief POCO class to hold the ip-config for one interface
 * @note The fields must be filled depending on the @see IPSource.
 *  The @see IPConfig::interface_ field must be set always.
 *  For the sources @see IPSource::NONE, @see IPSource::DHCP, @see IPSource::BOOTP and @see IPSource::EXTERNAL
 *  setting set the @see IPConfig::source_ field is sufficient.
 *  For the sources @see IPSource::STATIC and @see IPSource::TEMPORARY the @see IPConfig::address_ fields needs to be
 * set also.
 *
 */
struct IPConfig {
  bool operator==(const IPConfig& config) const {
    return ((this->interface_ == config.interface_) && (this->source_ == config.source_) &&
            (this->address_ == config.address_) && (this->netmask_ == config.netmask_) &&
            (this->broadcast_ == config.broadcast_));
  }

  ::std::string interface_ = "";
  IPSource source_ = IPSource::UNKNOWN;
  ::std::string address_ = "";
  ::std::string netmask_ = "";
  ::std::string broadcast_ = "";
};

/**
 * @brief Container class to manage the @see IPConfig
 *
 */
class IPConfigs {
 public:
  IPConfigs();
  IPConfigs(const IPConfigs&) noexcept;
  IPConfigs(IPConfigs&&) noexcept;
  explicit IPConfigs(const ::std::string& jsonstr);
  ~IPConfigs();

  /**
   * @brief Add or replace an @see IPConfig
   * @note The config is not validated.
   *
   * @param config The IPConfig
   */
  void AddIPConfig(IPConfig config);

  /**
   * @brief Remove the configuration of an interface.
   * @note When there is no for the interface, this is a no-op.
   *
   * @param interface_name Name of the interface.
   */
  void RemoveIPConfig(const ::std::string& interface_name);

  /**
   * @brief Return the @see IPConfig of an interface
   *
   * @param interface_name Name of the interface
   * @return boost::optional<IPConfig>
   */
  boost::optional<IPConfig> GetIPConfig(const ::std::string& interface_name) const;

  /**
   * @brief Return the IPConfigs of an interface
   *
   * @return ::std::vector<IPConfig>
   * */
  ::std::vector<IPConfig> Get() const;

  /**
   * @brief Retruns the json representation of the ip configurations.
   *
   * @return ::std::string
   */
  ::std::string ToJson() const;

 private:
  class Impl;
  ::std::unique_ptr<Impl> impl_;
};

/**
 * @brief Returns the @see IPConfigs from the netconfd network configuration daemon.
 *
 * @return IPConfigs The @see IPConfigs
 */
IPConfigs GetIPConfigs();

/**
 * @brief Returns the current @see IPConfigs from the netconfd network configuration daemon.
 *
 * @return IPConfigs The @see IPConfigs
 */
IPConfigs GetCurrentIPConfigs();

/**
 * @brief Set the ip configuration for the netconfd network configuration daemon.
 * @note check /var/log/messages when the ip config failed to apply.
 *
 * @param config The configuration to apply.
 * @return Status @see Status::Ok on success.
 */
Status SetIPConfigs(const IPConfigs& config);

/**
 * @brief Delete the ip configuration for one interface.
 *
 * @param interface_name
 */
void DeleteIPConfig(::std::string interface_name);


/**
 * @brief Set the Temp Fix Ip
 *
 */
void SetTempFixIp();

/**
 * @brief Get DIP switch configuration.
 *
 * @return DIP switch configuration containing:
 *         - IP address, e.g. 192.168.1.0 (the first three octets matter)
 *         - netmask
 *         - mode, (off, dhcp, static, hw-not-available)
 *         - value, the value of the last octet (0 - 255)
 */
::std::string GetDipSwitchConfig();

/**
 * @brief Set DIP switch configuration.
 *
 * @param config The DIP switch configuration containing IP address and netmask.
 * @return Status @see Status::Ok on success.
 */
Status SetDipSwitchConfig(const ::std::string& config);


}  // namespace netconf
