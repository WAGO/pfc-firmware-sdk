// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "Status.hpp"
namespace netconfd {

template<class DataType>
class IPersistence {
 public:
  virtual ~IPersistence() = default;

  virtual Status Write(const DataType& d) = 0;
  virtual Status Read(DataType& d) = 0;
};

}  // namespace netconfd
