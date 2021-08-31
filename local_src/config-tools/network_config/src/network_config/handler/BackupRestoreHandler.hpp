// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>

namespace network_config {

class BackupRestoreHandler : public IHandler {
 public:
  explicit BackupRestoreHandler(const ::boost::program_options::variables_map &vm);
  ~BackupRestoreHandler() override = default;

  BackupRestoreHandler(const BackupRestoreHandler&) = delete;
  BackupRestoreHandler& operator=(const BackupRestoreHandler&) = delete;
  BackupRestoreHandler(const BackupRestoreHandler&&) = delete;
  BackupRestoreHandler& operator=(const BackupRestoreHandler&&) = delete;

  void Execute() override;
 private:
  ::std::function<void()> execute_;
  const ::boost::program_options::variables_map &vm_;
};

} /* namespace network_config */
