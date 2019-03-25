//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_iptables.hpp
/// 
/// \brief Function processing iptables configuration file.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#ifndef WAGO_PROCESS_IPTABLES_HPP
#define WAGO_PROCESS_IPTABLES_HPP


#include "xmlhlp.hpp"
#include <string>
#include <vector>


namespace wago
{


//------------------------------------------------------------------------------
/// Process iptables's configuration change request.
/// \param doc xml document to be operated on
/// \param cmd requested command (see help of firewall config-tool for description)
/// \param argv parameters of requested command
//------------------------------------------------------------------------------
void process_iptables(xmldoc& doc, const std::string& cmd, const std::vector<std::string>& argv);


} // namespace wago


#endif // WAGO_PROCESS_IPTABLES_HPP

