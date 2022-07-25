//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file system.hpp
/// 
/// \brief System related helper functions.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef WAGO_FIREWALL_SYSTEM_HPP_
#define WAGO_FIREWALL_SYSTEM_HPP_

#include <string>

namespace wago {
namespace firewall {


 //------------------------------------------------------------------------------
 /// Executes an external command (can be shell call).
 /// \param cmd command to be executed
 /// \param exit_code reference to return status of executed command
 /// (last exited child).
 /// \return string returned by the executed command
 //------------------------------------------------------------------------------
std::string exe_cmd(const std::string& cmd, int &exit_code);


} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_SYSTEM_HPP_
