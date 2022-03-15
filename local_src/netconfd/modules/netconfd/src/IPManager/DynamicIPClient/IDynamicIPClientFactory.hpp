#pragma once

#include "IDynamicIPClient.hpp"
#include <string>
#include "BaseTypes.hpp"
#include "DynamicIPType.hpp"

namespace netconf {

class IDynamicIPClientFactory {
 public:
  IDynamicIPClientFactory() = default;
  virtual ~IDynamicIPClientFactory() = default;

  IDynamicIPClientFactory(const IDynamicIPClientFactory&) = delete;
  IDynamicIPClientFactory& operator=(const IDynamicIPClientFactory&) = delete;
  IDynamicIPClientFactory(const IDynamicIPClientFactory&&) = delete;
  IDynamicIPClientFactory& operator=(const IDynamicIPClientFactory&&) = delete;

  virtual IDynamicIPClientPtr Create(DynamicIPType type, const ::std::string &itf_name, const ::std::string &hostname="",
                                     const ::std::string &vendorclass="") = 0;

};

} /* namespace netconf */
