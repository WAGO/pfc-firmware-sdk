//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file regex.hpp
/// 
/// \brief Helper variables and funtions for regular expressions processing.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef WAGO_FIREWALL_REGEX_HPP_
#define WAGO_FIREWALL_REGEX_HPP_

#include <glib.h>
#include <string>
#include <vector>


namespace wago {
namespace firewall {
namespace regex {


/// Various regular expression strings.
extern const std::string rex_name;
extern const std::string rex_number;
extern const std::string rex_conn_count;
extern const std::string rex_yesno;
extern const std::string rex_onoff;
extern const std::string rex_dir;
extern const std::string rex_ifname;
extern const std::string rex_ifstate;
extern const std::string rex_ifmac;
extern const std::string rex_ifmac_mask;
extern const std::string rex_limit;
extern const std::string rex_burst;
extern const std::string rex_tag;
extern const std::string rex_level;
extern const std::string rex_mode;
extern const std::string rex_policy;
extern const std::string rex_eproto;
extern const std::string rex_proto;
extern const std::string rex_tcpudp;
extern const std::string rex_port;
extern const std::string rex_port_range;
extern const std::string rex_ip4;
extern const std::string rex_ip4mask;


// RAII class.
class regex
{
public:
    regex()
        : rexobj(NULL) {}
    regex(const std::string& rex);
    regex(const regex& other) = delete;
    regex(regex&& other);
    ~regex();

    regex& operator=(const regex& rhs) = delete;
    regex& operator=(regex&& rhs);

    GRegex* get(void) const
            { return rexobj; }
    GRegex* release(void)
            { GRegex* const _rexobj(rexobj); rexobj = NULL; return _rexobj; }

private:
    GRegex* rexobj;
};


//
/// Class containing all regexes in compiled form.
/// IMPORTANT: The class should be create AFTER glib framework is initialized
/// and before it is deinitialized.
//
class regexs
{
public:
    regex name;
    regex conn_count;
    regex yesno;
    regex onoff;
    regex dir;
    regex ifname;
    regex ifstate;
    regex ifmac;
    regex ifmac_mask;
    regex limit;
    regex burst;
    regex tag;
    regex level;
    regex mode;
    regex policy;
    regex eproto;
    regex proto;
    regex tcpudp;
    regex port;
    regex port_range;
    regex ip4;
    regex ip4mask;

    regexs();
};


// RAII class.
class match_info
{
public:
    match_info()
        : info(NULL) {}
    match_info(const match_info& other) = delete;
    match_info(match_info&& other);
    ~match_info();

    match_info& operator=(const match_info& rhs) = delete;
    match_info& operator=(match_info&& rhs);

    GMatchInfo* get(void) const
            { return info; }
    GMatchInfo* release(void)
            { GMatchInfo* const _info(info); info = NULL; return _info; }
    GMatchInfo** ptr(void);

private:
    GMatchInfo* info;
};


//------------------------------------------------------------------------------
/// Checks regex's match agains a given line.
/// \param rex regular expression to match
/// \param line line to do the match against
/// \param empty true treats empty lines as a match, false does the opposite
/// \return true if match was found, false if not
//------------------------------------------------------------------------------
bool is_match(const std::string& rex, const std::string& line, const bool empty = false);

//------------------------------------------------------------------------------
/// Checks regex's match agains a given line.
/// \param rex a pre-compiled regular expression to match
/// \param line line to do the match against
/// \param empty true treats empty lines as a match, false does the opposite
/// \return true if match was found, false if not
//------------------------------------------------------------------------------
bool is_match(const regex& rexobj, const std::string& line, const bool empty = false);

//------------------------------------------------------------------------------
/// Returns single mached part of a given line.
/// \param rex regular expression to match
/// \param line line to do the match against
/// \param mix index of the matched element which shall be returned
/// \return mached substrings
//------------------------------------------------------------------------------
std::string get_match(const std::string& rex, const std::string& line, const int mix);

//------------------------------------------------------------------------------
/// Returns single mached part of a given line.
/// \param rex a pre-compiled regular expression to match
/// \param line line to do the match against
/// \param mix index of the matched element which shall be returned
/// \return mached substrings
//------------------------------------------------------------------------------
std::string get_match(const regex& rexobj, const std::string& line, const int mix);

//------------------------------------------------------------------------------
/// Returns all mached parts of a given line.
/// \param rex regular expression to match
/// \param line line to do the match against
/// \return all mached substrings
//------------------------------------------------------------------------------
std::vector<std::string> get_match(const std::string& rex, const std::string& line);

//------------------------------------------------------------------------------
/// Returns all mached parts of a given line.
/// For each match there are multiple results possible. First is the full match
/// with sub-matches (paranthesis) following it.
/// \param rex a pre-compiled regular expression to match
/// \param line line to do the match against
/// \return all mached substrings
//------------------------------------------------------------------------------
std::vector<std::string> get_match(const regex& rexobj, const std::string& line);


} // namespace regex
} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_REGEX_HPP_
