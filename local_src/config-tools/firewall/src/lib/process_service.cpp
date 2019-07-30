//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_service.cpp
/// 
/// \brief Function processing service's configuration file.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include "process_service.hpp"

#include "error.hpp"
#include "process.hpp"
#include "regex.hpp"
#include <libxml/parser.h>


namespace wago
{


static void set_if(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (2 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& ifname(argv[1]);

    if (!is_match_std(regex::rex_onoff, state) ||
        !is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode interface(get_node(ctx,
                               "/f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if='" + ifname + "']",
                               false));

    if (interface.is_empty())
    {
        xmlnode interfaces(get_node(ctx, "/f:firewall/f:ipv4/f:service/f:interfaces"));
        interface = append_node(interfaces, "interface");

        append_attribute(interface, "state", state);
        append_attribute(interface, "if", ifname);
    }
    else
    {
        update_attribute(interface, "state", state);
        update_attribute(interface, "if", ifname);
    }
}

static void rem_if(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& ifname(argv[0]);

    if (!is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode interface(get_node(ctx,
                               "/f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if='" + ifname + "']",
                               false));

    if (!interface.is_empty())
        remove_node(std::move(interface));
}

static void add_rule(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (4 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& proto(argv[1]);
    const std::string& src_port(argv[2]);
    const std::string& dst_port(argv[3]);

    if (!is_match_std(regex::rex_onoff, state) ||
        !is_match_std(regex::rex_tcpudp, proto) ||
        !is_match_opt(regex::rex_port_range, src_port) ||
        !is_match_std(regex::rex_port_range, dst_port))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode rules(get_node(ctx, "/f:firewall/f:ipv4/f:service/f:rules"));
    xmlnode rule(append_node(rules, "rule"));

    append_attribute(rule, "state", state);
    append_attribute(rule, "proto", proto);
    if ("-" != src_port)
        append_attribute(rule, "src_port", src_port);
    append_attribute(rule, "dst_port", dst_port);
}

static void upd_rule(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (5 != argv.size())
        throw invalid_param_error();

    const std::string& index(argv[0]);
    const std::string& state(argv[1]);
    const std::string& proto(argv[2]);
    const std::string& src_port(argv[3]);
    const std::string& dst_port(argv[4]);

    if (!is_match_std(regex::rex_number, index) ||
        !is_match_std(regex::rex_onoff, state) ||
        !is_match_std(regex::rex_tcpudp, proto) ||
        !is_match_opt(regex::rex_port_range, src_port) ||
        !is_match_std(regex::rex_port_range, dst_port))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode rule(get_node(ctx, "/f:firewall/f:ipv4/f:service/f:rules/f:rule[" + index + "]", false));

    if (!rule.is_empty())
    {
        update_attribute(rule, "state", state);
        update_attribute(rule, "proto", proto);
        updrem_attribute(rule, "src_port", src_port);
        update_attribute(rule, "dst_port", dst_port);
    }
}

static void rem_rule(xmldoc& doc, const std::vector<std::string>& argv)
{
    remove(doc, argv, "/f:firewall/f:ipv4/f:service/f:rules/f:rule");
}

void process_service(xmldoc& doc, const std::string& cmd, const std::vector<std::string>& argv)
{
    if ("--set-if" == cmd)
        set_if(doc, argv);
    else if ("--rem-if" == cmd)
        rem_if(doc, argv);
    else if ("--add-rule" == cmd)
        add_rule(doc, argv);
    else if ("--upd-rule" == cmd)
        upd_rule(doc, argv);
    else if ("--rem-rule" == cmd)
        rem_rule(doc, argv);
    else
        throw invalid_param_error(cmd);
}


} // namespace wago

