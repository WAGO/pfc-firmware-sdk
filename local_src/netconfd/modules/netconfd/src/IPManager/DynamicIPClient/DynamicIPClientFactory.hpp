#pragma once

#include "IDynamicIPClientFactory.hpp"

namespace netconf {

class DynamicIPClientFactory : public IDynamicIPClientFactory {
 public:
  DynamicIPClientFactory() = default;
  virtual ~DynamicIPClientFactory() = default;

  DynamicIPClientFactory(const DynamicIPClientFactory&) = delete;
  DynamicIPClientFactory& operator=(const DynamicIPClientFactory&) = delete;
  DynamicIPClientFactory(const DynamicIPClientFactory&&) = delete;
  DynamicIPClientFactory& operator=(const DynamicIPClientFactory&&) = delete;

  IDynamicIPClientPtr Create(DynamicIPType type, const ::std::string &itf_name, const ::std::string &hostname,
                             const ::std::string &vendorclass) override;
};

} /* namespace netconf */
