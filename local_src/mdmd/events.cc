// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


#include "statemachine.h"
#include "events.h"

RegexEvent::RegexEvent(const std::string &re)
    : _re(re)
{}

RegexEvent::~RegexEvent()
{}

bool
RegexEvent::match_string(const std::string &str)
{
    _info = _re.match(str);
    return _info.matches();
}

TimeoutEvent::TimeoutEvent(int id)
    : _id(id)
{}

TimeoutEvent::~TimeoutEvent()
{}

DBusEvent::DBusEvent( const std::string &method_name )
    : _method_name(method_name)
    , _match_any(false)
    , _invocation(0)
{}

DBusEvent::DBusEvent()
    : _method_name("")
    , _match_any(true)
    , _invocation(0)
{}

DBusEvent::~DBusEvent()
{}

bool
DBusEvent::match_dbus(MethodInvocation dbus_invocation)
{
    if (_match_any) {
	_invocation = dbus_invocation;
	return true;
    }

    if (dbus_invocation.method_name() == _method_name) {
	_invocation = dbus_invocation;
	return true;
    }

    // Fall through to false
    return false;
}

