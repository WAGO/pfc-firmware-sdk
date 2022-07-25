// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG



#ifndef REGEX_H
#define REGEX_H

#include "glib.h"
#include <string>
#include <memory>


class Regex;

class MatchInfo
{
    private:
	std::shared_ptr<GMatchInfo> _gmatchinfo_ptr;

	MatchInfo( std::shared_ptr<GMatchInfo> gmatchinfo_ptr )
	    : _gmatchinfo_ptr(gmatchinfo_ptr)
	{}

	friend class Regex;

    public:
	MatchInfo()
	    : _gmatchinfo_ptr()
	{}

	MatchInfo( const MatchInfo &other )
	    : _gmatchinfo_ptr(other._gmatchinfo_ptr)
	{
	}

	~MatchInfo() {
	}

	MatchInfo & operator=(const MatchInfo &other) {
	    _gmatchinfo_ptr = other._gmatchinfo_ptr;
	    return *this;
	}

	gboolean matches() {
	    return g_match_info_matches(_gmatchinfo_ptr.get());
	}

	std::string fetch_named( const std::string &name ) {
	    gchar *substring = g_match_info_fetch_named( _gmatchinfo_ptr.get(), name.c_str() );
	    std::string retval((substring != nullptr) ? substring : "");
      if (substring != nullptr)
      {
        g_free(substring);
      }
	    return retval;
	}

	std::string fetch_number( int num ) {
	    gchar *substring = g_match_info_fetch( _gmatchinfo_ptr.get(), num );
      std::string retval((substring != nullptr) ? substring : "");
      if (substring != nullptr)
      {
        g_free(substring);
      }
	    return retval;
	}

	int num_matches() {
	    return g_match_info_get_match_count( _gmatchinfo_ptr.get() );
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
	}

	Regex( const Regex &other )
	    : _gregex(other._gregex)
	{
      if (_gregex != nullptr)
      {
          g_regex_ref(_gregex);
      }
	}

	~Regex()
	{
      if (_gregex != nullptr)
      {
          g_regex_unref(_gregex);
      }
	}

	Regex & operator=(const Regex &other) {
    if (this != &other) { // self-assignment check
	    _gregex = other._gregex;
      if (_gregex != nullptr)
      {
          g_regex_ref(_gregex);
      }
    }
	  return *this;
	}

	MatchInfo match( const std::string &str) {
	    MatchInfo result;
	    GMatchInfo *gmatchinfo;
	    g_regex_match(_gregex, str.c_str(), (GRegexMatchFlags)0, &gmatchinfo);
	    if (gmatchinfo != nullptr)
	    {
	      //memory of gmatchinfo will be released with last instance of shared_ptr
	      std::shared_ptr<GMatchInfo> gmatchinfo_ptr(gmatchinfo, g_match_info_free);
	      result = MatchInfo(gmatchinfo_ptr);
	    }
	    return result;
	}
};

#endif
