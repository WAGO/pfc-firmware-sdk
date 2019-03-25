// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG



#ifndef REGEX_H
#define REGEX_H

#include "glib.h"
#include <string>
#include <memory>


class Regex;

class MatchInfo
{
    private:
	std::shared_ptr<GMatchInfo> _matchinfo;

	MatchInfo( GMatchInfo *info )
	    : _matchinfo(info,g_match_info_free)
	{}

	friend class Regex;

    public:
	MatchInfo()
	    : _matchinfo()
	{}

	MatchInfo( const MatchInfo &other )
	    : _matchinfo(other._matchinfo)
	{
	}

	~MatchInfo() {
	}

	MatchInfo & operator=(const MatchInfo &other) {
	    _matchinfo = other._matchinfo;
	    return *this;
	}

	gboolean matches() {
	    return g_match_info_matches(_matchinfo.get());
	}

	std::string fetch_named( const std::string &name ) {
	    gchar *substring = g_match_info_fetch_named( _matchinfo.get(), name.c_str() );
	    if (substring == 0)
		throw std::exception();

	    std::string retval(substring);
	    g_free(substring);
	    return retval;
	}

	std::string fetch_number( int num ) {
	    gchar *substring = g_match_info_fetch( _matchinfo.get(), num );
	    if (substring == 0)
		throw std::exception();

	    std::string retval(substring);
	    g_free(substring);
	    return retval;
	}

	int num_matches() {
	    return g_match_info_get_match_count( _matchinfo.get() );
	}
};


class Regex
{
    private:
	GRegex *_gregex;

    public:
	Regex( const std::string &pattern ) {
	    GError *err = 0;
	    _gregex = g_regex_new(pattern.c_str(), (GRegexCompileFlags)0, (GRegexMatchFlags)0, &err);
	    if (_gregex == 0)
		throw std::exception();
	}

	Regex( const Regex &other )
	    : _gregex(other._gregex)
	{
	    g_regex_ref(_gregex);
	}

	~Regex() {
	    g_regex_unref(_gregex);
	}

	Regex & operator=(const Regex &other) {
    if (this != &other) { // self-assignment check
	    _gregex = other._gregex;
	    g_regex_ref(_gregex);
    }
	  return *this;
	}

	MatchInfo match( const std::string &str) {
	    GMatchInfo *matchinfo;
	    g_regex_match(_gregex, str.c_str(), (GRegexMatchFlags)0, &matchinfo);

	    if (matchinfo == 0)
		throw std::exception();

	    return MatchInfo(matchinfo);
	}
};

#endif
