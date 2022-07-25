//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process.hpp
/// 
/// \brief Common helper functions for configuration processing routines.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef WAGO_FIREWALL_PROCESS_HPP_
#define WAGO_FIREWALL_PROCESS_HPP_

#include "regex.hpp"
#include "xmlhlp.hpp"
#include <string>
#include <vector>


//
//  Please note that this module is a set of random functions common for all
//  process_* modules, and can change reapidly and without any warning.
//  THIS MODULE IS NOT MEANT TO BE USED OUTSIDE OF process_* MODULES!
//  DO NOT RELY ON THE MODULE ANYWHERE ELSE. IN AN EXTREME CASE IT CAN SIMPLY
//  GET DELETED.
//


namespace wago {
namespace firewall {

//------------------------------------------------------------------------------
/// Creates xpath contest for a given xml document.
/// The function automatically sets standard xml namespace.
/// \param doc xml document for which the xpath context should be created
/// \return created xpath context
//------------------------------------------------------------------------------
xmlctx get_ctx(const xmldoc& doc);

//------------------------------------------------------------------------------
/// Marker wrapper for regex::is_match function.
/// Its sole funtions is to provide identical syntax as 'is_match_opt' does.
/// For parameters description and results see regex::is_match function
//------------------------------------------------------------------------------
bool is_match_std(const std::string& rex, const std::string& line);

//------------------------------------------------------------------------------
/// Wrapper for regex::is_match function.
/// It extends given 'rex' expression with the possibility to match '^-$' as well.
/// For parameters description and results see regex::is_match function
//------------------------------------------------------------------------------
bool is_match_opt(const std::string& rex, const std::string& line);

//------------------------------------------------------------------------------
/// Updates or removes an attribute based on a new supplied value.
/// \param node node containing the attribute
/// \param name name of the attribute
/// \param value new value or if equal '-' the attribute will get deleted
//------------------------------------------------------------------------------
void updrem_attribute(xmlnode& node,
                      const std::string& name,
                      const std::string& value);

//------------------------------------------------------------------------------
/// Removes an entry in the configuration file based on supplied parameters.
/// \param doc xml document to be operated on
/// \param argv argv[0] index of a node to be deleted, there can be only one entry
/// \param xpath xpath expression pointing to a set of nodes
//------------------------------------------------------------------------------
void remove(xmldoc& doc, const std::vector<std::string>& argv, const std::string& xpath);

} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_PROCESS_HPP_
