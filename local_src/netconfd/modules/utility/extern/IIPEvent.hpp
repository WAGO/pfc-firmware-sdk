// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

namespace netconf {
class IIPEvent{

 public:
  enum class ChangeType {
    Add,
    Delete,
    Modify,
  };

  virtual ~IIPEvent() = default;

  virtual void OnAddressChange(ChangeType change_type, ::std::uint32_t if_index, ::std::string address, ::std::string netmask) = 0;

};

}  // namespace pfcspecific
