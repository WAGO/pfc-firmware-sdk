// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG


#ifndef GERROR_EXCEPTION_H
#define GERROR_EXCEPTION_H

#include <string>
#include <exception>
#include "glib.h"

class GErrorException : public std::exception
{
    private:
	std::string _message;
	int _code;
	std::string _domain;

    public:
	explicit GErrorException( GError *err )
	  : _message( err->message )
	  , _code( err->code )
      , _domain( g_quark_to_string( err->domain ) )
	{
	    g_error_free(err);
	}

	const char* what() const noexcept override { return _message.c_str(); }
	const std::string & domain() { return _domain; }
	int code() { return _code; }
};

#endif
