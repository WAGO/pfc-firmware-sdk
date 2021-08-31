// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <vector>

#include "../../extern/Status.hpp"
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


  virtual Status AddBridge(const Bridge& bridge) const = 0;
  virtual Status DeleteBridge(const Bridge& bridge) const = 0;
  virtual Status AddInterface(const Bridge& bridge, const Interface& interface) const = 0;
  virtual Status DeleteInterface(const Bridge& bridge, const Interface& interface) const = 0;

  virtual Bridges GetBridges() const = 0;
  virtual Interfaces GetBridgeInterfaces(const Bridge& bridge) const = 0;

  virtual Interfaces GetInterfaces() const = 0;

  virtual Status SetInterfaceUp(const ::std::string& name) const = 0;
  virtual Status SetInterfaceDown(const ::std::string& name) const = 0;

  virtual void SetAgetime(const Bridge& bridge, int seconds) const = 0;
};

}
