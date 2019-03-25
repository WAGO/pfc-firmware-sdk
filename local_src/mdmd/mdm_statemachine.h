//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project mdmd (PTXdist package mdmd).
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     mdm_statemachine.cc
///
///  \brief    Specific statemachine declaration
///
///  \author   KNu
//------------------------------------------------------------------------------
#ifndef MDM_STATEMACHINE_H
#define MDM_STATEMACHINE_H

#include <list>
#include <map>
#include "glib.h"

#include "statemachine.h"
#include "serialport.h"
#include "timeout.h"

#include "oper.h"
#include "sms.h"
#include "mdm_dbusserver.h"
#include "persistent_storage.h"

#include <diagnostic/mdmd_diag.h>
#include "sysfs_lib.h"

enum class MdmSimState {UNKNOWN, SIM_PIN, SIM_PUK, READY, NOT_INSERTED, NOT_READY, SIM_ERROR};

class OperNameMap : public std::map<int, std::string>{};
class OperMap : public std::map<int, Oper>{};
class SmsList : public std::list<Sms>{};

class MdmSMPort;
class MdmTimeout;

class GprsAccess
{
  private:
    std::string _apn;
    int _auth;
    std::string _user;
    std::string _pass;

  public:
  GprsAccess()
    : _apn("")
    , _auth(0)
    , _user("")
    , _pass("")
  {}

  GprsAccess( const std::string &apn, int auth, const std::string &user, const std::string &pass )
    : _apn(apn)
    , _auth(auth)
    , _user(user)
    , _pass(pass)
  {}

  ~GprsAccess() {}

  GprsAccess& operator=( const GprsAccess& gprsaccess )
  {
    _apn=gprsaccess.get_apn();
    _auth=gprsaccess.get_auth();
    _user=gprsaccess.get_user();
    _pass=gprsaccess.get_pass();
    return *this;
  }

  void clear()
  {
    _apn="";
    _auth=0;
    _user="";
    _pass="";
  }

  const std::string & get_apn() const   { return _apn; }
  int                 get_auth() const  { return _auth; }
  const std::string & get_user() const  { return _user; }
  const std::string & get_pass() const  { return _pass; }

  void set_apn(const std::string &apn) { _apn = apn; }
  void set_auth(int auth) { _auth = auth; }
  void set_user(const std::string &user) { _user = user; }
  void set_pass(const std::string &pass) { _pass = pass; }

};


class MdmStatemachine : public StateMachine<MdmStatemachine>
{
    private:
    MdmSMPort *_port;
    std::string _port_read_buffer;
    int        _port_wait_count;
    int        _cfun_wait_count;
    MdmTimeout *_timeout;

    MdmDBusServer *_dbus_server;
    MethodInvocation _current_invocation;
    PersistentStorage _storage;

    GPIO_Control _gpioPowerKey;
    MUSB_Control _musbDriverPort;

    std::string _modem_manufacturer;
    std::string _modem_model;
    std::string _modem_revision;
    std::string _modem_identity;

    int _oper_reg_state;
    std::string _oper_reg_lac;
    std::string _oper_reg_cid;
    int _oper_scan_mode;
    int _oper_scan_seq;
    int _gprs_reg_state;
    MdmSimState _sim_state;
    int _cfun_state;

    std::string _iccid;
    std::string _pin;
    std::string _puk;
    int _pin_count;
    int _puk_count;
    int _current_signal_step;

    Oper        _current_oper;
    OperMap     _oper_map;

    OperNameMap _operator_names;

    int _rmnet_profileid;
    GprsAccess _gprsaccess;
    bool _gprsaccess_set_oninit;
    bool _gprs_temporary_disable;
    bool _net_led_blink_code_output;

    int _last_cme_error;
    int _last_cms_error;

    SmsList _sms_list;
    int _sms_format;
    int _sms_event_report_mode;
    int _sms_event_report_mt;
    int _sms_event_report_bm;
    int _sms_event_report_ds;
    int _sms_event_report_bfr;
    std::string _sms_storage_mem[3];
    int _sms_storage_used[3];
    int _sms_storage_total[3];
    int _last_sms_msg_ref;
    std::string _pdp_addr;

    void init();
    void update_status_leds();
    int set_modem_powerkey(int value) const;
    int execute_command(const std::string &cmd, std::string &cmd_output, std::string &cmd_error);
    bool wwan_get_status(bool &is_enabled, bool &is_configured);
    void wwan_stop();
    void wwan_enable();
    void wwan_start();

    public:
    MdmStatemachine (transition_list_t &tl, State &s1, action_list_t &al,
                     int gpioPowerControl, int musbDriverPort);
    ~MdmStatemachine();

    void wwan_disable();

    void set_current_invocation( MethodInvocation invocation ) { _current_invocation = invocation; }
    MethodInvocation current_invocation() { return _current_invocation; }

    void write( const std::string &text );

    void deactivate_cmd_timeout();
    void kick_cmd_timeout(unsigned int milliseconds);
    bool dbus_online() const { return _dbus_server->online(); }
    bool is_modem_port_open() const { return (_port != 0); }
    bool get_modem_command_device(std::string &commandDevice) const;
    void try_open_modem();
    int  get_port_wait_count() const { return _port_wait_count; }
    int  inc_port_wait_count() { _port_wait_count++; return _port_wait_count; }
    void set_port_wait_count(int value) { _port_wait_count = value; }
    void clear_modem_port();
    bool is_port_enabled() const { return _storage.get_port_enabled(); };
    void set_port_enabled(bool enabled);
    void modem_hard_reset();
    int  get_cfun_wait_count() const { return _cfun_wait_count; }
    int  inc_cfun_wait_count() { _cfun_wait_count++; return _cfun_wait_count; }
    void set_cfun_wait_count(int value) { _cfun_wait_count = value; }

    void set_sim_state(MdmSimState state) { _sim_state = state; }
    MdmSimState get_sim_state() const { return _sim_state; }

    void set_oper_reg_state(int state, const std::string &lac, const std::string &cid, int act );
    int get_oper_reg_state() const { return _oper_reg_state; }
    bool is_oper_registered() const { return ((1 == _oper_reg_state) || (5 == _oper_reg_state)); }
    std::string get_oper_reg_lac() const { return _oper_reg_lac; }
    std::string get_oper_reg_cid() const { return _oper_reg_cid; }

    void set_oper_scan_mode(int value) { _oper_scan_mode = value; }
    int get_oper_scan_mode() const { return _oper_scan_mode; }
    void set_oper_scan_seq(int value) { _oper_scan_seq = value; }
    int get_oper_scan_seq() const { return _oper_scan_seq; }

    void set_gprs_reg_state(int state);
    int get_gprs_reg_state() const { return _gprs_reg_state; }
    bool is_gprs_registered() const { return ((1 == _gprs_reg_state) || (5 == _gprs_reg_state)); }

    void set_modem_manufacturer( const std::string &value ) { _modem_manufacturer = value; }
    std::string get_modem_manufacturer() const { return _modem_manufacturer; }
    void set_modem_model( const std::string &value ) { _modem_model = value; }
    std::string get_modem_model() const { return _modem_model; }
    void set_modem_revision( const std::string &value ) { _modem_revision = value; }
    std::string get_modem_revision() const { return _modem_revision; }
    void set_modem_identity( const std::string &value ) { _modem_identity = value; }
    std::string get_modem_identity() const { return _modem_identity; }

    void set_iccid( const std::string &iccid ) { _iccid = iccid; }
    bool is_new_sim_iccid() const;
    std::string get_stored_sim_pin() const;
    void reset_stored_sim_pin() { _pin="";_storage.set_sim(_iccid,_pin);_storage.save();}
    void store_sim_pin() { _storage.set_sim(_iccid,_pin);_storage.save();}

    void set_pin( const std::string &pin ) { _pin = pin; }

    void set_rmnet_profileid( int profileid ) { _rmnet_profileid = profileid; }
    int get_rmnet_profileid() const { return _rmnet_profileid; }

    int get_pin_count() const { return _pin_count; }
    void set_pin_count( int pin_count ) { _pin_count = pin_count; }
    int get_puk_count() const { return _puk_count; }
    void set_puk_count( int puk_count ) { _puk_count = puk_count; }

    bool get_stored_oper(int &id, int &act) const {return _storage.get_oper(id, act);}
    void set_stored_oper(int id, int act) { _storage.set_oper(id, act); _storage.save(); }
    void remove_stored_oper() { _storage.remove_oper(); _storage.save(); }

    bool get_stored_selection(int &mode) const {return _storage.get_selection(mode);}
    void set_stored_selection(int mode) { _storage.set_selection(mode); _storage.save(); }

    void set_gprsaccess(const GprsAccess &gprsaccess) { _gprsaccess = gprsaccess; }
    void set_gprsaccess_oninit() { _gprsaccess_set_oninit = true; }
    bool is_gprsaccess_set_oninit() const { return _gprsaccess_set_oninit; }
    const GprsAccess & get_gprsaccess() const { return _gprsaccess; }
    void set_gprs_connectivity(const int connectivity);
    int  get_gprs_connectivity() const;
    void set_gprs_temporary_disable();
    void set_gprs_temporary_enable();
    bool is_gprs_temporary_disabled() const;
    bool is_gprs_disabled() const;
    void set_pdp_address( const std::string &pdp_addr );
    std::string get_pdp_address() const { return _pdp_addr; }

    void clear_current_oper();
    void set_current_oper(int id, int act);
    void set_current_oper(const Oper & oper) { _current_oper = oper; }
    void set_current_oper_csq(int ber, int rssi);
    const Oper & get_current_oper() const { return _current_oper; }

    void add_opermap(int id, int act, int state);
    void clear_opermap() { _oper_map.clear(); }
    GVariant* get_var_opermap() const;

    void clear_smslist() { _sms_list.clear(); }
    void add_smslist(int index);
    void add_smslist(int index, int state, int length);
    void set_sms_body_last_read();
    void set_last_sms(int state, int length);
    GVariant* get_var_smslist() const;
    GVariant* get_var_smsread() const;
    int get_last_sms_msg_ref() const { return _last_sms_msg_ref; }
    void set_last_sms_msg_ref(int msg_ref) { _last_sms_msg_ref = msg_ref; }

    void set_sms_format(int format) { _sms_format = format; }
    int get_sms_format() { return _sms_format; }
    void set_sms_event_report(int mode, int mt, int bm, int ds, int bfr) { _sms_event_report_mode = mode;
                                                                           _sms_event_report_mt = mt;
                                                                           _sms_event_report_bm = bm;
                                                                           _sms_event_report_ds = ds;
                                                                           _sms_event_report_bfr = bfr; }
    int get_sms_event_report_mode() { return _sms_event_report_mode; }
    int get_sms_event_report_mt() { return _sms_event_report_mt; }
    int get_sms_event_report_bm() { return _sms_event_report_bm; }
    int get_sms_event_report_ds() { return _sms_event_report_ds; }
    int get_sms_event_report_bfr() { return _sms_event_report_bfr; }
    void set_sms_storage(unsigned int id, const std::string &mem, int used, int total)
        { if (id < 3) { _sms_storage_mem[id] = mem; _sms_storage_used[id] = used; _sms_storage_total[id] = total; }}
    std::string get_sms_storage_mem(unsigned int id) { return ((id < 3) ? _sms_storage_mem[id] : ""); }
    int get_sms_storage_used(unsigned int id) { return ((id < 3) ? _sms_storage_used[id] : 0); }
    int get_sms_storage_total(unsigned int id) { return ((id < 3) ? _sms_storage_total[id] : 0); }

    void log_event(unsigned int id);

    void event_report_last_read();

    void set_last_cme_error( int err) { _last_cme_error = err; }
    int get_last_cme_error() const { return _last_cme_error; }
    void set_last_cms_error( int err) { _last_cms_error = err; }
    int get_last_cms_error() const { return _last_cms_error; }

    void set_cfun_state( int state) { _cfun_state = state; }
    int get_cfun_state() const { return _cfun_state; }

    bool set_loglevel(const int loglevel);
    int  get_loglevel() const;

    void stop_net_led_blink_code();

    size_t get_count_operator_names() const { return _operator_names.size(); }
    bool insert_operator_name(int id, const std::string& name);
    std::string get_operator_name(int id) const;
};

class MdmSMPort : public SerialPort
{
    private:
    MdmStatemachine *_sm;
    public:
    MdmSMPort( MdmStatemachine *sm, const std::string &fname, std::string *read_buffer )
        : SerialPort (fname, read_buffer)
        , _sm(sm)
    {}
    ~MdmSMPort() {}

    void read_line_cb( const std::string &line );
    void io_error();
};

class MdmTimeout : public Timeout
{
    private:
    MdmStatemachine *_sm;
    int _id;

    public:
    MdmTimeout( MdmStatemachine *sm, int seconds, int id )
        : Timeout(seconds)
        , _sm(sm)
        , _id(id)
    {}

    void callback();
};

#endif
