// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <map>
#include "BaseTypes.hpp"
#include "IDynamicIPClient.hpp"
#include "DynamicIPType.hpp"

namespace netconf {


class IDynamicIPClientAdministrator{
 public:
  IDynamicIPClientAdministrator() = default;
  virtual ~IDynamicIPClientAdministrator() = default;

  IDynamicIPClientAdministrator(const IDynamicIPClientAdministrator&) = delete;
  IDynamicIPClientAdministrator& operator=(const IDynamicIPClientAdministrator&) = delete;
  IDynamicIPClientAdministrator(IDynamicIPClientAdministrator&&) = delete;
  IDynamicIPClientAdministrator& operator=(IDynamicIPClientAdministrator&&) = delete;

  virtual IDynamicIPClientPtr AddClient(DynamicIPType type, const ::std::string &itf_name, const ::std::string &hostname)  = 0;
  virtual void DeleteClient(const ::std::string &itf_name)  = 0;
  virtual IDynamicIPClientPtr GetClient(const ::std::string &itf_name) const  = 0;
  virtual void RestartAllClients(const ::std::string& hostname) = 0;
};

} /* namespace netconf */
