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
/// \version $Id: system.hpp 65689 2022-03-11 14:37:43Z falk.werner@wago.com $ 
/// 
/// \brief System related helper functions.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


#ifndef WAGO_SYSTEM_HPP
#define WAGO_SYSTEM_HPP


#include <string>


namespace wago
{


//------------------------------------------------------------------------------
/// Executes an external command (can be shell call).
/// \param cmd command to be executed
/// \return string returned by the executed command
//------------------------------------------------------------------------------
std::string exe_cmd(const std::string& cmd);


} // namespace wago


#endif // WAGO_SYSTEM_HPP

