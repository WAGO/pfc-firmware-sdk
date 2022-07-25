//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_ebtables.hpp
/// 
/// \brief Function processing ebtables configuration file.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef WAGO_FIREWALL_PROCESS_EBTABLES_HPP_
#define WAGO_FIREWALL_PROCESS_EBTABLES_HPP_

#include "xmlhlp.hpp"
#include <string>
#include <vector>


namespace wago {
namespace firewall {
namespace ebtables {

//------------------------------------------------------------------------------
/// Process ebtables's configuration change request.
/// \param doc xml document to be operated on
/// \param cmd requested command (see help of firewall config-tool for description)
/// \param argv parameters of requested command
//------------------------------------------------------------------------------
void process(xmldoc& doc, const std::string& cmd, const std::vector<std::string>& argv);

namespace impl {

void set_if(xmldoc& doc, const std::vector<std::string>& argv);
void rem_if(xmldoc& doc, const std::vector<std::string>& argv);

} // namespace impl
} // namespace ebtables
} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_PROCESS_EBTABLES_HPP_
