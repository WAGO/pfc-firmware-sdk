// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef MDM_CONFIG_TYPES_H
#define MDM_CONFIG_TYPES_H

#include <string>

enum class OPER_SEL_MODE
{
    AUTOMATIC = 0,
    MANUAL = 1,
    ONLY_GSM = 2,
    ONLY_UMTS = 3,
    PREFER_GSM = 4,  /*WAT26409: deprecated option, not removed for settings restore compatibility*/
    PREFER_UMTS = 5, /*WAT26409: deprecated option, not removed for settings restore compatibility*/
};

enum class OPER_SEL_ACT
{
    GSM = 0,
    GSM_COMPACT = 1,
    UMTS = 2,
    GSM_EGPRS = 3,
    UMTS_HSDPA = 4,
    UMTS_HSUPA = 5,
    UMTS_HSPA = 6,
    LTE = 7,
};

enum class NW_SCAN_MODE
{
    AUTO = 0,
    GSM_ONLY = 1,
    UMTS_ONLY = 2,
};

enum class NW_SCAN_SEQ
{
    AUTO = 0,
    GSM_PRIOR = 1,
    UMTS_PRIOR = 2,
};

enum class GPRS_ACCESS_STATE
{
    DISABLED = 0,
    ENABLED = 1,
};

enum class MODEM_PORT_STATE
{
    DISABLED = 0,
    ENABLED = 1,
};


class GprsAccessConfig
{
public:
    explicit GprsAccessConfig(int state = 0,
                              std::string apn = "",
                              int auth = 0,
                              std::string  user = "",
                              std::string  pass = "",
                              int profile = 0)
        : _state(state),
          _apn(std::move(apn)),
          _auth(auth),
          _user(std::move(user)),
          _pass(std::move(pass)),
          _profile(profile)
    {}

    int         get_state() const noexcept { return _state; }
    std::string get_apn()   const noexcept { return _apn; }
    int         get_auth()  const noexcept { return _auth; }
    std::string get_user()  const noexcept { return _user; }
    std::string get_pass()  const noexcept { return _pass; }
    int         get_profile() const noexcept { return _profile; }

    void set_state(const int state)        noexcept { _state = state; }
    void set_apn(const std::string& apn)   noexcept { _apn = apn; }
    void set_auth(const int auth)          noexcept { _auth = auth; }
    void set_user(const std::string& user) noexcept { _user = user; }
    void set_pass(const std::string& pass) noexcept { _pass = pass; }
    void set_profile(const int profile)    noexcept { _profile = profile; }

    int compare(const GprsAccessConfig &config) const noexcept
    {
        int nbr_of_differences = 0;
        if (_state != config.get_state()) {
            nbr_of_differences++;
        }
        if (_apn   != config.get_apn()) {
            nbr_of_differences++;
        }
        if (_auth  != config.get_auth()) {
            nbr_of_differences++;
        }
        if (_user  != config.get_user()) {
            nbr_of_differences++;
        }
        if (_pass  != config.get_pass()) {
            nbr_of_differences++;
        }
        if (_profile != config.get_profile()) {
            nbr_of_differences++;
        }
        return nbr_of_differences;
    };

private:
    int         _state;
    std::string _apn;
    int         _auth;
    std::string _user;
    std::string _pass;
    int         _profile;
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

    int compare(const SmsEventReportingConfig &config) const noexcept
    {
        int nbr_of_differences = 0;
        if (_mode != config.get_mode()) {
            nbr_of_differences++;
        }
        if (_mt   != config.get_mt()) {
            nbr_of_differences++;
        }
        if (_bm  != config.get_bm()) {
            nbr_of_differences++;
        }
        if (_ds  != config.get_ds()) {
            nbr_of_differences++;
        }
        if (_bfr  != config.get_bfr()) {
            nbr_of_differences++;
        }
        return nbr_of_differences;
    };

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

    int compare(const SmsStorageConfig &config) const noexcept
    {
        int nbr_of_differences = 0;
        if (_mem1 != config.get_mem1()) {
            nbr_of_differences++;
        }
        if (_mem2 != config.get_mem2()) {
            nbr_of_differences++;
        }
        if (_mem3 != config.get_mem3()) {
            nbr_of_differences++;
        }
        return nbr_of_differences;
    };

private:
    std::string _mem1;
    std::string _mem2;
    std::string _mem3;
};

class MessageServiceConfig
{
public:
    explicit MessageServiceConfig(int sms_format = 0)
        : _sms_format(sms_format)
    {}

    int get_sms_format() const noexcept { return _sms_format; }

    void set_sms_format(const int sms_format) noexcept { _sms_format = sms_format; }

    int compare(const MessageServiceConfig &config) const noexcept
    {
        int nbr_of_differences = 0;
        if (_sms_format != config.get_sms_format()) {
            nbr_of_differences++;
        }
        return nbr_of_differences;
    };

private:
    int _sms_format;
};


class NetworkAccessConfig 
{
public:
    explicit NetworkAccessConfig(int selection_mode = 0,
                                 int manual_oper = 0,
                                 int manual_act = 0,
                                 int autoselect_scanmode = 0,
                                 int autoselect_scanseq = 0)
        : _selection_mode(selection_mode),
          _manual_oper(manual_oper),
          _manual_act(manual_act),
          _autoselect_scanmode(autoselect_scanmode),
          _autoselect_scanseq(autoselect_scanseq)
    {}

    int get_selection_mode() const noexcept { return _selection_mode; }
    int get_manual_oper() const noexcept { return _manual_oper; }
    int get_manual_act()  const noexcept { return _manual_act; }
    int get_autoselect_scanmode()  const noexcept { return _autoselect_scanmode; }
    int get_autoselect_scanseq()  const noexcept { return _autoselect_scanseq; }

    void set_selection_mode(const int selection_mode) noexcept { _selection_mode = selection_mode; }
    void set_manual_oper(const int manual_oper) noexcept { _manual_oper = manual_oper; }
    void set_manual_act(const int manual_act)   noexcept { _manual_act = manual_act; }
    void set_autoselect_scanmode(const int autoselect_scanmode)   noexcept { _autoselect_scanmode = autoselect_scanmode; }
    void set_autoselect_scanseq(const int autoselect_scanseq)   noexcept { _autoselect_scanseq = autoselect_scanseq; }

    int compare(const NetworkAccessConfig &config) const noexcept
    {
        int nbr_of_differences = 0;
        if (_selection_mode != config.get_selection_mode()) {
            nbr_of_differences++;
        }
        if (_manual_oper != config.get_manual_oper()) {
            nbr_of_differences++;
        }
        if (_manual_act != config.get_manual_act()) {
            nbr_of_differences++;
        }
        if (_autoselect_scanmode != config.get_autoselect_scanmode()) {
            nbr_of_differences++;
        }
        if (_autoselect_scanseq != config.get_autoselect_scanseq()) {
            nbr_of_differences++;
        }
        return nbr_of_differences;
    };

private:
    int _selection_mode;
    int _manual_oper;
    int _manual_act;
    int _autoselect_scanmode;
    int _autoselect_scanseq;
};

class SimAutoActivation 
{
public:
    explicit SimAutoActivation(std::string iccid = "",
                               std::string pin = "")
        : _iccid(std::move(iccid)),
          _pin(std::move(pin))
    {}

    std::string get_iccid() const noexcept { return _iccid; }
    std::string get_pin()   const noexcept { return _pin; }

    void set_iccid(const std::string& iccid) noexcept { _iccid = iccid; }
    void set_pin(const std::string& pin)     noexcept { _pin = pin; }

    int compare(const SimAutoActivation &config) const noexcept
    {
        int nbr_of_differences = 0;
        if (_iccid != config.get_iccid()) {
            nbr_of_differences++;
        }
        if (_pin != config.get_pin()) {
            nbr_of_differences++;
        }
        return nbr_of_differences;
    };

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

    int compare(const ModemManagementConfig &config) const noexcept
    {
        int nbr_of_differences = 0;
        if (_log_level != config.get_log_level()) {
            nbr_of_differences++;
        }
        if (_port_state != config.get_port_state()) {
            nbr_of_differences++;
        }
        return nbr_of_differences;
    };

private:
    int _log_level;
    int _port_state;
};

#endif

