//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#ifndef WAGO_FIREWALL_RULE_FILE_EDITOR_HPP_
#define WAGO_FIREWALL_RULE_FILE_EDITOR_HPP_

#include <string>

namespace wago {
namespace firewall {

class RuleFileEditor {
 public:
  RuleFileEditor() = default;
  virtual~RuleFileEditor() = default;
  RuleFileEditor(const RuleFileEditor& other) = delete;
  RuleFileEditor& operator=(const RuleFileEditor& other) = delete;
  RuleFileEditor(const RuleFileEditor&&) = delete;
  RuleFileEditor& operator=(const RuleFileEditor&&) = delete;

  void remove_duplicate_lines(const ::std::string& rule_file) const;
};

} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_RULE_FILE_EDITOR_HPP_
