// SPDX-License-Identifier: GPL-2.0-or-later

#include "SwitchConfigLegacy.hpp"
#include "FileEditor.hpp"

namespace netconf {


SwitchConfigLegacy::SwitchConfigLegacy(::std::string filename): filename_(std::move(filename))
{}

bool SwitchConfigLegacy::GetFastAgeing() const {
  FileEditor fe;
  ::std::string data;

  fe.Read(filename_, data);

  return data.find("FAST_AGING=1") != ::std::string::npos;
}

}  // namespace netconf
