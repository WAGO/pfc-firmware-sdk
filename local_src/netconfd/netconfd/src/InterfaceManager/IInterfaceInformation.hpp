// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconfd {

class IInterfaceInformation {
 public:
  IInterfaceInformation() = default;
  virtual ~IInterfaceInformation() = default;

  IInterfaceInformation(const IInterfaceInformation&) = delete;
  IInterfaceInformation& operator=(const IInterfaceInformation&) = delete;
  IInterfaceInformation(const IInterfaceInformation&&) = delete;
  IInterfaceInformation& operator=(const IInterfaceInformation&&) = delete;

  virtual Interfaces GetBridgeAssignedInterfaces() const = 0;
  virtual Bridge GetBridgeOfInterface(const Interface& itf) const = 0;

  virtual bool IsInterfaceUp(const Interface& itf) const = 0;


};

} /* namespace netconfd */
