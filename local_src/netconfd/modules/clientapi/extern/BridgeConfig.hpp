// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Status.hpp"
#include "Types.hpp"


namespace netconf {
namespace api {
/**
 * @brief Class representing the bridge configuration
 *
 */
class BridgeConfig {
 public:
  /**
   * @brief Construct an empty Bridge Config object
   */
  BridgeConfig() = default;

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
  explicit BridgeConfig(const netconf::BridgeConfig&);
  ~BridgeConfig() = default;

  BridgeConfig& operator=(const BridgeConfig&) = default;


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
   * The assigned interface will be removed from other bridges if already assigned.
   *
   * @param interface_name Name of the interface
   * @param bridge_name Name of the bridge
   */
  void AssignInterfaceToBridge(const ::std::string &interface_name, const ::std::string &bridge_name, const bool remove_bridge_if_empty = false);


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
   * @return vector of interface names in the specified bridge.
   */
  Interfaces GetBridgeInterfaces(const ::std::string &bridge_name) const;

  /**
   * @brief Returns the bridge names in the config.
   *
   * @return vector of bridge names.
   */
  ::std::vector<Bridge> GetBridges() const;

  /**
   * @brief Returns the bridge name the interface is assigned to.
   *
   * @param interface_name Name of the interface.
   * @return ::std::string Name of the bridge the interface is assigned to or empty when the interface is not part of the bridge configuration.
   */
  Bridge GetBridgeOfInterface(const Interface &interface_name) const;

  /**
   * @brief Check if the given bridge contains any interfaces.
   * @param bridge_name The bridge to be checked
   * @return true is there are any interfaces assigned to the bridge, false if not.
   */
  bool BridgeIsEmpty(const ::std::string &bridge_name) const;

  /**
   * Test if the given interfaces are in the same bridge.
   * @param interfaces The interfaces to be checked
   * @return true, if the interfaces are in the same bridge.
   */
  bool AreInSameBridge(const ::std::vector<::std::string> &interfaces) const;

  netconf::BridgeConfig GetConfig() const noexcept;

  friend bool operator==(const BridgeConfig& rhs, const BridgeConfig& lhs);

 private:
  netconf::BridgeConfig configs_;
};

/**
 * @brief Converts the bridge configuration to its compact json representation.
 *
 * @return ::std::string The json string
 */
::std::string ToJson(const BridgeConfig& config) noexcept;

/**
 * @brief Converts the bridge configuration to its human readable json representation.
 *
 * @return ::std::string The json string
 */
::std::string ToPrettyJson(const BridgeConfig& config) noexcept;

/**
 * Convert the bridge configuration to a textual representation
 * e.g. "br0=X1,X2 br1=X11,X12"
 * @param config
 * @return
 */
::std::string ToString(const BridgeConfig& config) noexcept;


Status MakeBridgeConfig(const std::string& json_str, BridgeConfig& config);

/**
 * @brief Get the Bridge Config from the netconfd network config daemon *
 * @param config
 * @return Status
 */
Status GetBridgeConfig(BridgeConfig& config);

/**
 * @brief Set the Bridge Config for the netconfd network config daemon.
 * @see /var/log/messages for details when this operation failed.
 *
 * @param config The configuration to set.
 * @return Status @see Status::OK on success.
 */
Status SetBridgeConfig(const BridgeConfig &config);

/**
 * Validates a given bridge configuration
 *
 * @param config The config to be validated
 * @return Result of the validataion
 */
Status ValidateBridgeConfig(const BridgeConfig &config, const InterfaceInformations& interface_information);


bool operator==(const BridgeConfig& rhs, const BridgeConfig& lhs);

} /* namespace api */
} /* namespace netconf */

