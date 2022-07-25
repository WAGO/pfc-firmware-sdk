// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DynamicIPClientFactory.hpp"
#include "IDynamicIPClientAdministrator.hpp"

#include <string>


namespace netconf {

class DynamicIPClientAdministrator : public IDynamicIPClientAdministrator {
 public:
  DynamicIPClientAdministrator(::std::string vendorclass);
  virtual ~DynamicIPClientAdministrator() = default;

  DynamicIPClientAdministrator(const DynamicIPClientAdministrator &other) = delete;
  DynamicIPClientAdministrator(DynamicIPClientAdministrator &&other) = delete;
  DynamicIPClientAdministrator& operator=(const DynamicIPClientAdministrator &other) = delete;
  DynamicIPClientAdministrator& operator=(DynamicIPClientAdministrator &&other) = delete;

  IDynamicIPClientPtr AddClient(DynamicIPType type, const ::std::string &itf_name, const ::std::string &hostname) override;
  void DeleteClient(const ::std::string &itf_name) override;
  IDynamicIPClientPtr GetClient(const ::std::string &itf_name) const override;
  void RestartAllClients(const ::std::string& hostname) override;

 private:
  ::std::string vendorclass_;

  DynamicIPClientFactory factory_;
  IDynamicIPClients clients_;

  void OnHostnameChanged();
};

}
