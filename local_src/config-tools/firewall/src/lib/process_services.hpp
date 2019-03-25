//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_services.hpp
/// 
/// \brief Function processing all service configuration files.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#ifndef WAGO_PROCESS_SERVICES_HPP
#define WAGO_PROCESS_SERVICES_HPP


#include "xmlhlp.hpp"
#include <string>
#include <vector>


namespace wago
{


//------------------------------------------------------------------------------
/// Process requests aimed at all services.
/// \param cmd requested command (see help of firewall config-tool for description)
/// \param dir directory path where all service configuration files are stored
/// \param argv parameters of requested command
//------------------------------------------------------------------------------
void process_services(const std::string& cmd,
                      const std::string& dir,
                      const std::vector<std::string>& argv);


} // namespace wago


#endif // WAGO_PROCESS_SERVICES_HPP

