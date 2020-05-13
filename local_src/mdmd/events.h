// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


#include "statemachine.h"
#include "dbus_server.h"

class RegexEvent : public Event
{
  private:
    Regex _re;
    MatchInfo _info;
  public:
    bool match_string(const std::string &text ) override;
    std::string named_match( const std::string &name ) { return _info.fetch_named(name); }
    std::string get_match( int num ) { return _info.fetch_number(num); }
    int num_matches() { return _info.num_matches(); }

    RegexEvent(const std::string &re);
    ~RegexEvent();
};

class ModemEvent : public RegexEvent
{
  public:
    bool match_read( const std::string &text ) override { return match_string(text); }

    ModemEvent(const std::string &re) : RegexEvent(re) {}
    ~ModemEvent() {}
};

class TimeoutEvent : public Event
{
  private:
    int _id;
  public:
    bool match_timeout( int i ) override { return _id == i; }

    TimeoutEvent(int id=0);
    ~TimeoutEvent();
};

class DBusEvent : public Event
{
  private:
    std::string _method_name;
    bool _match_any;
    MethodInvocation _invocation;
  public:
    bool match_dbus(MethodInvocation dbus_invocation) override;
    MethodInvocation invocation() { return _invocation; }

    DBusEvent( const std::string &method_name );
    DBusEvent();
    ~DBusEvent();
};
