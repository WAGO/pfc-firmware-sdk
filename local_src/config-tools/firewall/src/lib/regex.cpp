//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file regex.cpp
/// 
/// \brief Helper variables and funtions for regular expressions processing
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


#include "regex.hpp"
#include <stdexcept>

namespace wago {
namespace firewall {
namespace regex {


/*
 * Regex's below are used to validate ebtables/iptables configuration files.
 * These regex's MUST be synchronized with patterns.xsd file.
 */
const std::string rex_name("^[a-zA-Z_ ][a-zA-Z0-9_\\-]{0,19}$");
const std::string rex_number("^([0-9]|[1-9][0-9]{1,3})$");
const std::string rex_conn_count("^([0-9]|[1-9][0-9]{1,2}|[1-3][0-9]{3}|4000)$");
const std::string rex_yesno("^(yes|no)$");
const std::string rex_onoff("^(on|off)$");
const std::string rex_dir("^(uni|bi)$");
const std::string rex_ifname("^[a-zA-Z]{1}[a-zA-Z0-9]{1,19}$");
const std::string rex_ifstate("^(open|filtered)$");
const std::string rex_ifmac("^[a-fA-F0-9]{1,2}(:[a-fA-F0-9]{1,2}){5}(/[a-fA-F0-9]{1,2}(:[a-fA-F0-9]{1,2}){5}){0,1}$");
const std::string rex_ifmac_mask("^[a-fA-F0-9]{1,2}(:[a-fA-F0-9]{1,2}){5}(/[a-fA-F0-9]{1,2}(:[a-fA-F0-9]{1,2}){5}){0,1}$");
const std::string rex_limit("^[1-9][0-9]{0,5}/(second|minute|hour|day)$");
const std::string rex_burst("^[1-9][0-9]{0,5}$");
const std::string rex_tag("^[a-zA-Z_ ][a-zA-Z0-9_\\- ]{0,19}$");
const std::string rex_level("^(emerg|alert|crit|error|warning|notice|info|debug)$");
const std::string rex_mode("^(all-allow|whitelist)$");
const std::string rex_policy("^(drop|accept)$");
const std::string rex_eproto("^(IPv4|ARP|IPv6|0x(0[a-zA-Z6-9][a-zA-Z0-9]{2}|[a-zA-Z1-9][a-zA-Z0-9]{3}))$");
const std::string rex_proto("^(tcp|udp|udplite|icmp|esp|ah|sctp|mh|all|"
                            "([1-9][0-9]{0,1}|1[0-9]{2}|2[0-4][0-9]|25[0-5]))$");
const std::string rex_tcpudp("^(tcp|udp|tcpudp)$");
const std::string rex_port("^([0-9]|[1-9][0-9]{1,3}|[1-5][0-9]{4}|6([0-4][0-9]{3}|"
                           "5([0-4][0-9]{2}|5([0-2][0-9]|3[0-5]))))$");
const std::string rex_port_range("^([0-9]|[1-9][0-9]{1,3}|[1-5][0-9]{4}|"
                                 "6([0-4][0-9]{3}|5([0-4][0-9]{2}|5([0-2][0-9]|"
                                 "3[0-5]))))(:([0-9]|[1-9][0-9]{1,3}|[1-5][0-9]{4}|"
                                 "6([0-4][0-9]{3}|5([0-4][0-9]{2}|5([0-2][0-9]|"
                                 "3[0-5]))))){0,1}$");
const std::string rex_ip4("^([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])"
                          "(\\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])){3}$");
const std::string rex_ip4mask("^([0-9]|[1-2][0-9]|3[0-2])|"
                              "((255.){3}(0|128|192|224|240|248|252|254|255))|"
                              "((255.){2}(0|128|192|224|240|248|252|254|255).0)|"
                              "(255.(0|128|192|224|240|248|252|254|255).0.0)|"
                              "((0|128|192|224|240|248|252|254|255).0.0.0)$");


regex::regex(const std::string& rex)
    : rexobj(NULL)
{
    rexobj = g_regex_new(rex.c_str(),
                         G_REGEX_OPTIMIZE,
                         G_REGEX_MATCH_NEWLINE_ANY,
                         NULL);

    if (NULL == rexobj)
        throw std::runtime_error("Can't compile requested regex.");
}

regex::regex(regex&& other)
    : rexobj(other.rexobj)
{
    other.rexobj = NULL;
}

regex::~regex()
{
    if (NULL != rexobj)
        g_regex_unref(rexobj);
}

regex& regex::operator=(regex&& rhs)
{
    if (NULL != rexobj)
        g_regex_unref(rexobj);
    rexobj = rhs.rexobj;
    rhs.rexobj = NULL;
    return *this;
}


regexs::regexs()
    : name(rex_name),
      conn_count(rex_conn_count),
      yesno(rex_yesno),
      onoff(rex_onoff),
      dir(rex_dir),
      ifname(rex_ifname),
      ifstate(rex_ifstate),
      ifmac(rex_ifmac),
      ifmac_mask(rex_ifmac_mask),
      limit(rex_limit),
      burst(rex_burst),
      tag(rex_tag),
      level(rex_level),
      mode(rex_mode),
      policy(rex_policy),
      eproto(rex_eproto),
      proto(rex_proto),
      tcpudp(rex_tcpudp),
      port(rex_port),
      port_range(rex_port_range),
      ip4(rex_ip4),
      ip4mask(rex_ip4mask)
{
}


match_info::match_info(match_info&& other)
    : info(other.info)
{
    other.info = NULL;
}

match_info::~match_info()
{
    if (NULL != info)
        g_match_info_free(info);
}

match_info& match_info::operator=(match_info&& rhs)
{
    if (NULL != info)
        g_match_info_free(info);
    info = rhs.info;
    rhs.info = NULL;
    return *this;
}

GMatchInfo** match_info::ptr(void)
{
    if (NULL != info)
    {
        g_match_info_free(info);
        info = NULL;
    }

    return &info;
}


bool is_match(const std::string& rex, const std::string& line, const bool empty)
{
    return is_match(regex(rex), line, empty);
}

bool is_match(const regex& rexobj, const std::string& line, const bool empty)
{
    if (empty && 0U == line.size())
        return true;

    match_info info;

    // Caused by glib ugly cast to non-existing enum value...
    g_regex_match(rexobj.get(), line.c_str(), (GRegexMatchFlags)0, info.ptr());

    return g_match_info_matches(info.get());
}

std::string get_match(const std::string& rex, const std::string& line, const int mix)
{
    return get_match(regex(rex), line, mix);
}

std::string get_match(const regex& rexobj, const std::string& line, const int mix)
{
    if (0U == line.size())
        return std::string();

    const gint spcount = g_regex_get_capture_count(rexobj.get());

    if (spcount < mix)
        return std::string();

    std::string res;
    match_info info;

    // Caused by glib ugly cast to non-existing enum value...
    g_regex_match(rexobj.get(), line.c_str(), (GRegexMatchFlags)0, info.ptr());

    if (g_match_info_matches(info.get()))
    {
        gchar* match = g_match_info_fetch(info.get(), mix);
        res = nullptr != match ? match : std::string();
        g_free(match);
    }

    return res;
}

std::vector<std::string> get_match(const std::string& rex, const std::string& line)
{
    return get_match(regex(rex), line);
}

std::vector<std::string> get_match(const regex& rexobj, const std::string& line)
{
    if (0U == line.size())
        return std::vector<std::string>();

    std::vector<std::string> res;
    match_info info;
    const gint spcount = g_regex_get_capture_count(rexobj.get());

    // Caused by glib ugly cast to non-existing enum value...
    g_regex_match(rexobj.get(), line.c_str(), (GRegexMatchFlags)0, info.ptr());

    while (g_match_info_matches(info.get()))
    {
        for (gint i = 0; i <= spcount; ++i)
        {
            gchar* match = g_match_info_fetch(info.get(), i);
            res.push_back(nullptr != match ? match : std::string());
            g_free(match);
        }
        g_match_info_next(info.get(), nullptr);
    }

    return res;
}

} // namespace regex
} // namespace firewall
} // namespace wago

