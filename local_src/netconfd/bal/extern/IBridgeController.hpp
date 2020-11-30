// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <vector>

#include "Error.hpp"
#include "Types.hpp"


namespace netconf {


class IBridgeController {
 public:

  IBridgeController() = default;
  virtual ~IBridgeController() = default;

  IBridgeController(const IBridgeController&) = default;
  IBridgeController& operator=(const IBridgeController&) = default;
  IBridgeController(IBridgeController&&) = default;
  IBridgeController& operator=(IBridgeController&&) = default;


  virtual Error AddBridge(const Bridge& bridge) const = 0;
  virtual Error DeleteBridge(const Bridge& bridge) const = 0;
  virtual Error AddInterface(const Bridge& bridge, const Interface& interface) const = 0;
  virtual Error DeleteInterface(const Bridge& bridge, const Interface& interface) const = 0;

  virtual Bridges GetBridges() const = 0;
  virtual Interfaces GetBridgeInterfaces(const Bridge& bridge) const = 0;

  virtual Interfaces GetInterfaces() const = 0;

  virtual Error SetInterfaceUp(const ::std::string& name) const = 0;
  virtual Error SetInterfaceDown(const ::std::string& name) const = 0;

  virtual void SetAgetime(const Bridge& bridge, int seconds) const = 0;
};

}
