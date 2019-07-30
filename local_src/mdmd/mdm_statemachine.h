// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


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
#include "mdm_config_types.h"
#include "persistent_storage.h"
#include "mdm_diagnostic.h"

#include "sysfs_lib.h"

enum class MdmSimState {UNKNOWN, SIM_PIN, SIM_PUK, READY, NOT_INSERTED, NOT_READY, SIM_ERROR};

class OperNameMap : public std::map<int, std::string>{};
class OperMap : public std::map<int, Oper>{};
class SmsList : public std::list<Sms>{};

class MdmSMPort;
class MdmTimeout;

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
    MdmDiagnostic _diagnostic;

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
    bool _gprs_wwan_state_change;
    bool _gprs_pdp_addr_change;
    MdmSimState _sim_state;
    int _cfun_state;

    std::string _iccid;
    std::string _pin;
    std::string _puk;
    int _pin_count;
    int _puk_count;

    Oper        _current_oper;
    OperMap     _oper_map;

    OperNameMap _operator_names;

    bool _gprs_temporary_disable;
    unsigned int _gprs_autoconnect_count;

    int _last_cme_error;
    int _last_cms_error;

    SmsList _sms_list;
    int _last_sms_msg_ref;
    std::string _pdp_addr;

    void init();
    int set_modem_powerkey(int value) const;
    int execute_command(const std::string &cmd, std::string &cmd_output, std::string &cmd_error);
    bool wwan_get_status(bool &is_enabled, bool &is_configured);
    bool wwan_stop();
    bool wwan_enable();
    bool wwan_start();
    void open_modem_port(const std::string& commandDevice);

    public:
    MdmStatemachine (transition_list_t &tl, State &s1, action_list_t &al,
                     int gpioPowerControl, int musbDriverPort);
    ~MdmStatemachine();

    void wwan_disable();
    void wwan_renew();

    void set_current_invocation( MethodInvocation invocation ) { _current_invocation = invocation; }
    MethodInvocation current_invocation() { return _current_invocation; }

    void write( const std::string &text );

    void deactivate_cmd_timeout();
    void kick_cmd_timeout(unsigned int milliseconds);
    bool dbus_online() const { return _dbus_server->online(); }
    bool is_modem_port_open() const { return (_port != 0); }
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

    int get_pin_count() const { return _pin_count; }
    void set_pin_count( int pin_count ) { _pin_count = pin_count; }
    int get_puk_count() const { return _puk_count; }
    void set_puk_count( int puk_count ) { _puk_count = puk_count; }

    bool get_stored_oper(int &id, int &act) const {return _storage.get_oper(id, act);}
    void set_stored_oper(int id, int act) { _storage.set_oper(id, act); _storage.save(); }
    void remove_stored_oper() { _storage.remove_oper(); _storage.save(); }

    bool get_stored_selection(int &mode) const {return _storage.get_selection(mode);}
    void set_stored_selection(int mode) { _storage.set_selection(mode); _storage.save(); }

    void set_gprsaccess(const GprsAccessConfig &newConfig);
    bool get_gprsaccess(GprsAccessConfig& gprs_access) const { return _storage.get_gprs_access(gprs_access); }
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

    void set_sms_report_config(const SmsEventReportingConfig& sms_config) { _storage.set_sms_reporting_config(sms_config);_storage.save(); }
    bool get_sms_report_config(SmsEventReportingConfig& sms_config) { return _storage.get_sms_reporting_config(sms_config);}
    void set_sms_storage_config(const SmsStorageConfig& sms_config) { _storage.set_sms_storage_config(sms_config);_storage.save(); }
    bool get_sms_storage_config(SmsStorageConfig& sms_config) { return _storage.get_sms_storage_config(sms_config);}
    
    void set_error_state(MdmErrorState newErrorState) { _diagnostic.set_error_state(newErrorState); }

    void event_report_last_read();

    void set_last_cme_error( int err) { _last_cme_error = err; }
    int get_last_cme_error() const { return _last_cme_error; }
    void set_last_cms_error( int err) { _last_cms_error = err; }
    int get_last_cms_error() const { return _last_cms_error; }

    void set_cfun_state( int state) { _cfun_state = state; }
    int get_cfun_state() const { return _cfun_state; }

    bool set_loglevel(const int loglevel);
    int  get_loglevel() const;

    size_t get_count_operator_names() const { return _operator_names.size(); }
    bool insert_operator_name(int id, const std::string& name);
    std::string get_operator_name(int id) const;
    void reset_gprs_change_flags() { _gprs_pdp_addr_change = false; _gprs_wwan_state_change = false; }
    bool gprs_pdp_addr_changed() const { return _gprs_pdp_addr_change; }
    bool gprs_wwan_state_changed() const { return _gprs_wwan_state_change; }

    bool is_gprs_autoconnect_count_exceeded() const;
    void set_gprs_autoconnect_count();
    void reset_gprs_autoconnect_count();
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
    ~MdmSMPort() = default; 

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
