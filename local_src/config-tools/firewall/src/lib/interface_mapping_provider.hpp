//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#ifndef WAGO_FIREWALL_INTERFACE_MAPPING_PROVIDER_HPP_
#define WAGO_FIREWALL_INTERFACE_MAPPING_PROVIDER_HPP_

#include <vector>
#include <tuple>

namespace wago {
namespace firewall {

class InterfaceMappingProvider {
 public:
  InterfaceMappingProvider() = default;
  virtual ~InterfaceMappingProvider() = default;
  InterfaceMappingProvider(const InterfaceMappingProvider& other) = delete;
  InterfaceMappingProvider& operator=(const InterfaceMappingProvider& other) = delete;
  InterfaceMappingProvider(const InterfaceMappingProvider&&) = delete;
  InterfaceMappingProvider& operator=(const InterfaceMappingProvider&&) = delete;

  ::std::string get_interface(const ::std::string &interface) const;
  ::std::pair<::std::string, ::std::string> get_interface(
      const ::std::string &interface1,
      const ::std::string &interface2) const;
};

} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_INTERFACE_MAPPING_PROVIDER_HPP_
