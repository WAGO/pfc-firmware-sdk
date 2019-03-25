// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef SMS_H_
#define SMS_H_

#include <string>

class Sms
{
    private:
    int         _index;
    int         _state;
    int         _length;
    std::string _body;

    public:
    Sms()
        : _index(-1)
        , _state(-1)
        , _length(-1)
        , _body("")
    {}

    Sms(int index)
        : _index(index)
        , _state(-1)
        , _length(-1)
        , _body("")
    {}

    Sms(int index, int state, int length)
        : _index(index)
        , _state(state)
        , _length(length)
        , _body("")
    {}

    Sms( const Sms &sms )
        : _index(sms.get_index())
        , _state(sms.get_state())
        , _length(sms.get_length())
        , _body(sms.get_body())
    {}

    ~Sms() {}

    Sms& operator=( const Sms& sms )
    {
        _index = sms.get_index();
        _state = sms.get_state();
        _length = sms.get_length();
        _body  = sms.get_body();
        return *this;
    }

    int                get_index() const  { return _index; }
    int                get_state() const  { return _state; }
    int                get_length() const  { return _length; }
    const std::string &get_body() const   { return _body; }

    void set_state( int state ) { _state = state; }
    void set_length( int length ) { _length = length; }
    void set_body( const std::string &body ) { _body = body; }

    bool is_valid() const { return ((_index >= 0) && (_state >=0) && (_length >= 0) && (_body.length() > 0)); }
};

#endif /* SMS_H_ */
