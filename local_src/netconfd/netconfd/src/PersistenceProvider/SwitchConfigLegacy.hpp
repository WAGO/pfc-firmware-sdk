// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     SwitchConfigLegacy.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include <string>

namespace netconfd {

class SwitchConfigLegacy {
 public:
  explicit SwitchConfigLegacy(::std::string filename);
  ~SwitchConfigLegacy() = default;

  bool GetFastAgeing() const;

 private:
  ::std::string filename_;
};

}  // namespace netconfd

//---- End of header file ------------------------------------------------------

