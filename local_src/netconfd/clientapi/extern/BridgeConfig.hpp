// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <memory>
#include <string>
#include "Status.hpp"

namespace netconf {

/**
 * @brief Class representing the bridge configuration
 *
 */
class BridgeConfig {
 public:
  /**
   * @brief Construct an empty Bridge Config object
   */
  BridgeConfig();
  BridgeConfig(const BridgeConfig&) noexcept;
  BridgeConfig& operator=(const BridgeConfig&);
  BridgeConfig(BridgeConfig&&) noexcept;
  BridgeConfig& operator=(BridgeConfig&&) noexcept;

  /**
   * @brief Construct a new Bridge Config object from the json string
   *
   * @param jsonstr json string representing the bridge configuration
   * @example
   * {
   *  "br0": ["X1","X2"],
   *   "br1": []
   *  },
   *
   */
  explicit BridgeConfig(const ::std::string &jsonstr);
  ~BridgeConfig();

  /**
   * @brief Adds a bridge to the configuration
   * @note The nameing sheme is brX, where X is a number equal or greater than 0.
   *
   * @param bridge_name Name of the bridge to add.
   */
  void AddBridge(const ::std::string &bridge_name);

  /**
   * @brief Deletes a bridge from the configuration
   *
   * @param bridge_name Name of the bridge to delete.
   */
  void DeleteBridge(const ::std::string &bridge_name);

  /**
   * @brief Assigns an interface to a bridge.
   *
   * @param interface_name Name of the interface
   * @param bridge_name Name of the bridge
   */
  void AssignInterfaceToBridge(const ::std::string &interface_name, const ::std::string &bridge_name);

  /**
   * @brief Removes an interface from the bridge configuration
   * @note Has no effect when the interface is not part of the bridge configuration
   *
   * @param interface_name Name of the interface to remove.
   */
  void DeleteInterface(const ::std::string &interface_name);

  /**
   * @brief Returns the bridge assigned interface names.
   *
   * @param bridge_name Name of the interface.
   * @return ::std::string Name of the interfaces assigned to the bridge or empty when the bridge does not exist.
   */
  ::std::string GetBridgeInterfaces(const ::std::string &bridge_name) const;

  /**
   * @brief Returns the bridge name the interface is assigned to.
   *
   * @param interface_name Name of the interface.
   * @return ::std::string Name of the bridge the interface is assigned to or empty when the interface is not part of the bridge configuration.
   */
  ::std::string GetBridgeOfInterface(const ::std::string &interface_name) const;

  /**
   * @brief Check if the given bridge contains any interfaces.
   * @param bridge_name The bridge to be checked
   * @return true is there are any interfaces assigned to the bridge, false if not.
   */
  bool BridgeIsEmpty(const ::std::string &bridge_name) const;

  /**
   * @brief Converts the bridge configuration to its json representation.
   *
   * @return ::std::string The json string
   */
  ::std::string ToJson() const;
  ::std::string ToString() const;

 private:
  class Impl;
  ::std::unique_ptr<Impl> impl_;
};

/**
 * @brief Get the Bridge Config from the netconfd network config daemon *
 * @return BridgeConfig The current bridge configuration
 */
BridgeConfig GetBridgeConfig();

/**
 * @brief Set the Bridge Config for the netconfd network config daemon.
 * @see /var/log/messages for details when this operation failed.
 *
 * @param config The configuration to set.
 * @return Status @see Status::OK on success.
 */
Status SetBridgeConfig(const BridgeConfig &config);

} /* namespace netconf */
