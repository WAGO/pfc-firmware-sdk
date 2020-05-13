// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>

namespace netconfd {
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
