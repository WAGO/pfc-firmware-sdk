// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG


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
#include "mdm_diagnostic.h"
#include "mdm_parameter_storage.h"
#include "sysfs_lib.h"

enum class ModemResetMode {DEFAULT, MUSB_RESET};

enum class CFUN_STATE
{
    MINIMUM = 0,
    FULL = 1,
    TX_RX_DISABLE = 4,
};

enum class SERVICE_REG_STATE
{
    STOPPED = 0,
    HOMENET = 1,
    STARTED = 2,
    DENIED = 3,
    UNKNOWN = 4,
    ROAMING = 5,
    NOSERVICE = 6,
};

enum class SIM_STATE
{
    UNKNOWN = 0,
    PIN_REQUIRED = 1,
    PUK_REQUIRED = 2,
    READY = 3,
    NOT_INSERTED = 4,
    ERROR = 5,
    NOT_READY = 6,
};

enum class SIM_INIT_STATUS
{
    INITIAL = 0,
    CPIN_READY = 1,
    SMS_DONE = 3,
    PHB_DONE = 5,
    COMPLETED = 7,
};


enum class PDP_CONTEXT_STATE
{
    DEACTIVATED = 0,
    ACTIVATED = 1,
};

enum class AUTOCONNECT_STATE
{
    DEACTIVATED = 0,
    ACTIVATED = 1,
};

enum class GPRS_STATE
{
    DETACHED = 0,
    ATTACHED = 1,
};


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
    bool       _cfun_wait_flag;
    bool       _siminit_wait_flag;
    bool       _sim_sms_support_flag;
    MdmTimeout *_timeout;

    GPIO_Control _gpioPowerKey;
    MUSB_Control _musbDriverPort;

    MdmDBusServer *_dbus_server;
    MethodInvocation _current_invocation;

    MdmDiagnostic _diagnostic;
    MdmParameterStorage _parameter_storage;

    std::string _modem_manufacturer;
    std::string _modem_model;
    std::string _modem_revision;
    std::string _modem_identity;

    int _oper_reg_state;
    std::string _oper_reg_lac;
    std::string _oper_reg_cid;
    int _gprs_reg_state;
    bool _gprs_wwan_state_change;
    bool _gprs_pdp_addr_change;
    bool _rmnet_autoconnect_with_dtrset;

    bool _continue_getoperlist;

    Oper        _current_oper;
    OperMap     _oper_map;

    OperNameMap _operator_names;

    unsigned int _gprs_autoconnect_count;

    SmsList _sms_list;
    std::string _pdp_addr;
/* Vorbereitung Abfrage ServingCellState */
//    std::string _servingcell_state;

    int execute_command(const std::string &cmd, std::string &cmd_output, std::string &cmd_error);
    bool wwan_get_status(bool &is_enabled, bool &is_configured);
    bool wwan_disable();
    bool wwan_stop();
    bool wwan_enable();
    bool wwan_start();
    void open_modem_port(const std::string& commandDevice);

    public:
    MdmStatemachine (transition_list_t &tl, State &s1, action_list_t &al,
                     int gpioPowerControl, int musbDriverPort,
                     ParameterStorage& storage);
    ~MdmStatemachine();

    void wwan_renew();

    void set_current_invocation( MethodInvocation invocation ) { _current_invocation = invocation; }
    MethodInvocation current_invocation() { return _current_invocation; }

    void write( const std::string &text );

    void deactivate_cmd_timeout();
    void kick_cmd_timeout(unsigned int milliseconds);
    bool dbus_online() const { return ((_dbus_server != nullptr) ? _dbus_server->online() : false); }
    bool is_modem_port_open() const { return (_port != nullptr); }
    void try_open_modem();
    int  get_port_wait_count() const { return _port_wait_count; }
    int  inc_port_wait_count() { _port_wait_count++; return _port_wait_count; }
    void set_port_wait_count(int value) { _port_wait_count = value; }
    void clear_modem_port();
    bool is_port_enabled() const;
    void set_port_enabled(bool enabled);
    void modem_reset(ModemResetMode reset_mode);
    bool get_siminit_wait_flag() const { return _siminit_wait_flag; }
    void set_siminit_wait_flag() { _siminit_wait_flag = true; }
    bool get_cfun_wait_flag() const { return _cfun_wait_flag; }
    void set_cfun_wait_flag() { _cfun_wait_flag = true; }
    bool get_sim_sms_support_flag() const { return _sim_sms_support_flag; }
    void set_sim_sms_support_flag() { _sim_sms_support_flag = true; }

    void set_oper_reg_state(int state, const std::string &lac, const std::string &cid, int act );
    int get_oper_reg_state() const { return _oper_reg_state; }
    bool is_oper_registered() const { return ((1 == _oper_reg_state) || (5 == _oper_reg_state)); }
    std::string get_oper_reg_lac() const { return _oper_reg_lac; }
    std::string get_oper_reg_cid() const { return _oper_reg_cid; }

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

    std::string get_stored_sim_iccid() const;
    std::string get_stored_sim_pin() const;
    void store_sim_pin(const std::string &iccid, const std::string &pin);

    void set_network_access_config(const NetworkAccessConfig &config) { _parameter_storage.set_network_access_config(config); }
    NetworkAccessConfig get_network_access_config() const { return _parameter_storage.get_network_access_config(); }

    void set_gprs_access_config(const GprsAccessConfig &config);
    GprsAccessConfig get_gprs_access_config() const { return _parameter_storage.get_gprs_access_config(); }

    bool is_gprs_disabled() const;
    void set_pdp_address( const std::string &pdp_addr );
    std::string get_pdp_address() const { return _pdp_addr; }

/* Vorbereitung Abfrage ServingCellState */
//    void set_servingcell_state( const std::string &servingcell_state );
//    std::string get_servingcell_state() const { return _servingcell_state; }

    void clear_current_oper();
    void set_current_oper(int mode, int id, int act);
    void set_current_oper_csq(int ber, int rssi);
    Oper get_current_oper() const { return _current_oper; }

    void add_opermap(int id, int act, int state);
    GVariant* get_var_opermap() const;
    void clear_opermap() { _oper_map.clear(); }

    void clear_smslist() { _sms_list.clear(); }
    void add_smslist(int index);
    void add_smslist(int index, int state, int length);
    void set_sms_body_last_read();
    void set_last_sms(int state, int length);
    GVariant* get_var_smslist() const;
    GVariant* get_var_smsread() const;

    void set_sms_report_config(const SmsEventReportingConfig& config) { _parameter_storage.set_sms_event_reporting_config(config); }
    SmsEventReportingConfig get_sms_report_config() const { return _parameter_storage.get_sms_event_reporting_config(); }
    void set_sms_storage_config(const SmsStorageConfig& config) { _parameter_storage.set_sms_storage_config(config); }
    SmsStorageConfig get_sms_storage_config() const { return _parameter_storage.get_sms_storage_config(); }
    MessageServiceConfig get_message_service_config() const { return _parameter_storage.get_message_service_config(); }

    
    MdmErrorState get_error_state() const { return _diagnostic.get_error_state(); }
    bool set_error_state(MdmErrorState newErrorState) { return _diagnostic.set_error_state(newErrorState); }

    void event_report_last_read();

    bool set_loglevel(const int loglevel);
    int  get_loglevel() const;

    size_t get_count_operator_names() const { return _operator_names.size(); }
    bool insert_operator_name(int id, const std::string& name);
    std::string get_operator_name(int id) const;
    void reset_gprs_change_flags() { _gprs_pdp_addr_change = false; _gprs_wwan_state_change = false; }
    bool gprs_pdp_addr_changed() const { return _gprs_pdp_addr_change; }
    bool gprs_wwan_state_changed() const { return _gprs_wwan_state_change; }

    unsigned int get_gprs_autoconnect_count() const { return _gprs_autoconnect_count; }
    void update_gprs_autoconnect_count();
    void reset_gprs_autoconnect_count();

    void set_rmnet_autoconnect_with_dtrset(bool value) { _rmnet_autoconnect_with_dtrset = value; }
    bool get_rmnet_autoconnect_with_dtrset() const { return _rmnet_autoconnect_with_dtrset; }

    void set_continue_getoperlist(bool value) { _continue_getoperlist = value; }
    bool get_continue_getoperlist() const { return _continue_getoperlist; }

    void reset_service_states();
    void reset_sim_states();
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

    void read_line_cb( const std::string &line ) override;
    void io_error() override;
};

class MdmTimeout : public Timeout
{
    private:
    MdmStatemachine *_sm;
    int _id;

    public:
    MdmTimeout( MdmStatemachine *sm, unsigned int seconds, int id )
        : Timeout(seconds)
        , _sm(sm)
        , _id(id)
    {}

    void callback() override;
};

#endif
