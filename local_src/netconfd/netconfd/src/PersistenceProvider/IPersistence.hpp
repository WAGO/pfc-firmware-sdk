// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "Error.hpp"
namespace netconf {

template<class DataType>
class IPersistence {
 public:
  virtual ~IPersistence() = default;

  virtual Error Write(const DataType& d) = 0;
  virtual Error Read(DataType& d) = 0;
};

}  // namespace netconf
