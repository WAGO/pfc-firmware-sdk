//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process.cpp
/// 
/// \brief Common helper functions for configuration processing routines.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


#include "process.hpp"
#include "error.hpp"


namespace wago {
namespace firewall {

xmlctx get_ctx(const xmldoc& doc)
{
    return create_xpath_ctx(doc, "f", "http://www.wago.com/security/firewall");
}

bool is_match_std(const std::string& rex, const std::string& line)
{
    return regex::is_match(rex, line);
}

bool is_match_opt(const std::string& rex, const std::string& line)
{
    return regex::is_match("(^-$|" + rex + ")", line);
}

void updrem_attribute(xmlnode& node,
                      const std::string& name,
                      const std::string& value)
{
    if ("-" != value)
        update_attribute(node, name, value);
    else
        remove_attribute(node, name);
}

void remove(xmldoc& doc, const std::vector<std::string>& argv, const std::string& xpath)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& index(argv[0]);

    if (!is_match_std(regex::rex_number, index))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    xmlnode node(get_node(ctx, xpath + "[" + index + "]", false));

    if (!node.is_empty())
        remove_node(std::move(node));
}

} // namespace firewall
} // namespace wago

