// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef MDM_CONFIG_TYPES_H
#define MDM_CONFIG_TYPES_H

#include <string>

class GprsAccessConfig
{
public:
    explicit GprsAccessConfig(int state = 0,
                              std::string apn = "",
                              int auth = 0,
                              std::string  user = "",
                              std::string  pass = "")
        : _state(state),
          _apn(std::move(apn)),
          _auth(auth),
          _user(std::move(user)),
          _pass(std::move(pass))
    {}

    int         get_state() const noexcept { return _state; }
    std::string get_apn()   const noexcept { return _apn; }
    int         get_auth()  const noexcept { return _auth; }
    std::string get_user()  const noexcept { return _user; }
    std::string get_pass()  const noexcept { return _pass; }

    void set_state(const int state)        noexcept { _state = state; }
    void set_apn(const std::string& apn)   noexcept { _apn = apn; }
    void set_auth(const int auth)          noexcept { _auth = auth; }
    void set_user(const std::string& user) noexcept { _user = user; }
    void set_pass(const std::string& pass) noexcept { _pass = pass; }
private:
    int         _state;
    std::string _apn;
    int         _auth;
    std::string _user;
    std::string _pass;
};

class SmsEventReportingConfig 
{
public:
    explicit SmsEventReportingConfig(int mode = 0,
                                     int mt = 0,
                                     int bm = 0,
                                     int ds = 0,
                                     int bfr = 0)
        : _mode(mode),
          _mt(mt),
          _bm(bm),
          _ds(ds),
          _bfr(bfr)
    {}

    int get_mode() const noexcept { return _mode; }
    int get_mt()   const noexcept { return _mt; }
    int get_bm()   const noexcept { return _bm; }
    int get_ds()   const noexcept { return _ds; }
    int get_bfr()  const noexcept { return _bfr; }

    void set_mode(const int mode) noexcept { _mode = mode; }
    void set_mt(const int mt)     noexcept { _mt = mt; }
    void set_bm(const int bm)     noexcept { _bm = bm; }
    void set_ds(const int ds)     noexcept { _ds = ds; }
    void set_bfr(const int bfr)   noexcept { _bfr = bfr; }
private:
    int _mode;
    int _mt;
    int _bm;
    int _ds;
    int _bfr;
};

class SmsStorageConfig 
{
public:
    explicit SmsStorageConfig(std::string mem1 = "",
                              std::string mem2 = "",
                              std::string mem3 = "")
        : _mem1(std::move(mem1)),
          _mem2(std::move(mem2)),
          _mem3(std::move(mem3))
    {}

    std::string get_mem1() const noexcept { return _mem1; }
    std::string get_mem2() const noexcept { return _mem2; }
    std::string get_mem3() const noexcept { return _mem3; }

    void set_mem1(const std::string& mem1) noexcept { _mem1 = mem1; }
    void set_mem2(const std::string& mem2) noexcept { _mem2 = mem2; }
    void set_mem3(const std::string& mem3) noexcept { _mem3 = mem3; }
private:
    std::string _mem1;
    std::string _mem2;
    std::string _mem3;
};

class NetworkAccessConfig 
{
public:
    explicit NetworkAccessConfig(int mode = 0,
                                 int oper = 0,
                                 int act = 0)
        : _mode(mode),
          _oper(oper),
          _act(act)
    {}

    int get_mode() const noexcept { return _mode; }
    int get_oper() const noexcept { return _oper; }
    int get_act()  const noexcept { return _act; }

    void set_mode(const int mode) noexcept { _mode = mode; }
    void set_oper(const int oper) noexcept { _oper = oper; }
    void set_act(const int act)   noexcept { _act = act; }
private:
    int _mode;
    int _oper;
    int _act;
};

class UserIdentityConfig 
{
public:
    explicit UserIdentityConfig(std::string iccid = "",
                                std::string pin = "")
        : _iccid(std::move(iccid)),
          _pin(std::move(pin))
    {}

    std::string get_iccid() const noexcept { return _iccid; }
    std::string get_pin()   const noexcept { return _pin; }

    void set_iccid(const std::string& iccid) noexcept { _iccid = iccid; }
    void set_pin(const std::string& pin)     noexcept { _pin = pin; }
private:
    std::string _iccid;
    std::string _pin;
};

class ModemManagementConfig 
{
public:
    explicit ModemManagementConfig(int log_level = 0,
                                   int port_state = 0)
        : _log_level(log_level),
          _port_state(port_state)
    {}

    int get_log_level()  const noexcept { return _log_level; }
    int get_port_state() const noexcept { return _port_state; }

    void set_log_level(const int log_level)   noexcept { _log_level = log_level; }
    void set_port_state(const int port_state) noexcept { _port_state = port_state; }
private:
    int _log_level;
    int _port_state;
};

#endif

