// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "mdm_cuse_worker.h"
#include "mdm_statemachine.h"
#include "events.h"
#include "statemachine_functors.h"
#include "mdm_functors.h"
#include "mdmd_log.h"
#include <algorithm>    // std::transform, std::find
#include <sstream>
#include <glib.h>
#include <memory>
#include <string>
#include <vector>

GMainLoop *ml;

using namespace std::string_literals;

namespace {

/// statemachine timer in milliseconds
constexpr auto timer_at_cmd_copn = 125000; //depends on the number of operator names
constexpr auto timer_at_cmd_cops = 185000; //for network selection that is determined by the network
constexpr auto timer_at_cmd_sms = 125000;  //for sms transmission that is determined by the network
constexpr auto timer_at_cmd_cfun = 125000; //15s specified into UC20 AT command manual but 120s timeout with R04A07
constexpr auto timer_at_cmd_short = 5000;  //for commands without mobile network dependency
constexpr auto timer_at_cmd_cgatt = 15000; //for GPRS attachment / detachment
constexpr auto timer_dbus_retry = 5000;
constexpr auto timer_wait_port = 10000;
constexpr auto timer_wait_port_io_error = 10000; //wait time after modem port io error
constexpr auto timer_wait_pb_done = 15000;
constexpr auto timer_wait_cfun = 10000;
constexpr auto timer_wait_simstate = 5000;
constexpr auto timer_status_query = 10000;  //for periodic status queries
constexpr auto timer_immediate = 10;
constexpr auto timer_power_down = 65000;    //the maximum time for unregistering network is 60 seconds
/// timer are currently not in used
/// set as comment to keep the values in the source code
//constexpr auto timer_wait_suspendauto = 65000;
//constexpr auto timer_wait_ps_detach = 65000;

constexpr auto rmnet_profileid = '1';
constexpr auto sms_format = 0; //Format = PDU
constexpr auto MDM_GPIO__PWRKEY_PIN = 132;
constexpr auto MDM_MUSB_DRIVER_PORT = 1;
constexpr auto MAX_PORT_WAIT_COUNT = 60;  /*number of wait cycles before automatic modem reset*/
constexpr auto MAX_CFUN_WAIT_COUNT = 60;  /*number of wait cycles before automatic modem reset*/
///constexpr is currently not in used
///set as comment to keep the value in  the source code
//constexpr auto MDM_GPIO__RESET_PIN = 130;
//constexpr auto MAX_AT_CMD_PRINT_LENGTH = 128;

enum class CME_ERROR 
{
    SIM_NOT_INSERTED = 10,
    SIM_PUK_REQUIRED = 12,
    SIM_FAILURE = 13,     
    SIM_BUSY = 14,        
    SIM_WRONG = 15,       
    INCORRECT_PASSWORD = 16,
};

enum class CMS_ERROR 
{
    SIM_NOT_INSERTED = 310,
    SIM_FAILURE = 313,
    SIM_BUSY = 314,
    SIM_WRONG = 315,
    SIM_NOT_READY = 512,
};

enum class MODE 
{
    AUTOMATIC = 0,
    MANUAL = 1,
    ONLY_GSM = 2,
    ONLY_UMTS = 3,
    PREFER_GSM = 4,
    PREFER_UMTS = 5,
};

/*
 * Guard functions
 */
bool gf_activation_required(const MdmStatemachine &sm)
{
    return (1 != sm.get_cfun_state());
}

bool gf_dbus_offline(const MdmStatemachine &sm)
{
    return !sm.dbus_online();
}

bool gf_modem_port_open(const MdmStatemachine &sm)
{
    return (sm.is_modem_port_open());
}

bool gf_modem_port_disabled(const MdmStatemachine &sm)
{
    return (!sm.is_port_enabled());
}

bool gf_have_no_manufacturer(const MdmStatemachine &sm)
{
    return (0 == sm.get_modem_manufacturer().length());
}

bool gf_gprs_disabled(const MdmStatemachine &sm)
{
    return sm.is_gprs_disabled();
}

bool gf_oper_stored(const MdmStatemachine &sm)
{
    int id, act;
    return sm.get_stored_oper(id,act);
}

bool gf_pin_needed(const MdmStatemachine &sm)
{
    return (MdmSimState::SIM_PIN == sm.get_sim_state());
}

bool gf_wait_modem_port_limit(const MdmStatemachine &sm)
{
    return (sm.get_port_wait_count() >= MAX_PORT_WAIT_COUNT);
}

bool gf_wait_cfun_state_limit(const MdmStatemachine &sm)
{
    return (sm.get_cfun_wait_count() >= MAX_CFUN_WAIT_COUNT);
}
bool gf_puk_needed(const MdmStatemachine &sm)
{
    return (MdmSimState::SIM_PUK == sm.get_sim_state());
}

bool gf_set_stored_pin(const MdmStatemachine &sm)
{
    return ( !sm.is_new_sim_iccid() &&
             (MdmSimState::SIM_PIN == sm.get_sim_state()) &&
             (0 < sm.get_stored_sim_pin().length()) );
}

bool gf_sim_ready(const MdmStatemachine &sm)
{
    return (MdmSimState::READY == sm.get_sim_state());
}

bool gf_sim_state_not_ready(const MdmStatemachine &sm)
{
    return (MdmSimState::NOT_READY == sm.get_sim_state());
}

bool gf_sim_state_not_inserted(const MdmStatemachine &sm)
{
    return (MdmSimState::NOT_INSERTED == sm.get_sim_state());
}

bool gf_cme_command_not_allowed(const MdmStatemachine &sm)
{
    return (3 == sm.get_last_cme_error());
}

bool gf_cme_incorrect_password(const MdmStatemachine &sm)
{
    switch (sm.get_last_cme_error())
    {
        case static_cast<int>(CME_ERROR::SIM_PUK_REQUIRED): 
        case static_cast<int>(CME_ERROR::INCORRECT_PASSWORD): 
            return true;
        default:
            return false;
    }
}

bool gf_cme_sim_busy(const MdmStatemachine &sm)
{
    return (sm.get_last_cme_error() == static_cast<int>(CME_ERROR::SIM_BUSY));
}

bool gf_cme_sim_not_inserted(const MdmStatemachine &sm)
{
    return (sm.get_last_cme_error() == static_cast<int>(CME_ERROR::SIM_NOT_INSERTED));
}

bool gf_cme_sim_failure(const MdmStatemachine &sm)
{
    switch (sm.get_last_cme_error())
    {
        case static_cast<int>(CME_ERROR::SIM_NOT_INSERTED): 
        case static_cast<int>(CME_ERROR::SIM_FAILURE): 
        case static_cast<int>(CME_ERROR::SIM_BUSY): 
        case static_cast<int>(CME_ERROR::SIM_WRONG): 
            return true;
        default:
            return false;
    }
}

bool gf_cms_sim_failure(const MdmStatemachine &sm)
{
    switch (sm.get_last_cms_error())
    {
        case static_cast<int>(CMS_ERROR::SIM_NOT_INSERTED): 
        case static_cast<int>(CMS_ERROR::SIM_FAILURE): 
        case static_cast<int>(CMS_ERROR::SIM_BUSY): 
        case static_cast<int>(CMS_ERROR::SIM_WRONG): 
        case static_cast<int>(CMS_ERROR::SIM_NOT_READY): 
            return true;
        default:
            return false;
    }
}

bool gf_sim_not_ready(const MdmStatemachine &sm)
{
    return (MdmSimState::READY != sm.get_sim_state());
}

bool gf_have_no_operatornames(const MdmStatemachine &sm)
{
    return (sm.get_count_operator_names() == 0);
}

bool gf_renew_wwan_address(const MdmStatemachine &sm)
{
    //FIX for connectivity problem:
    //WWAN0 may hold up to one hour an invalid IP address due to long DHCP lease time.
    //This happens when modem gets a new PDP IP address without GPRS state change and restart of WWAN.
    //For fast reconnection an update of WWAN0 IP address is necessary in this case.
    return (sm.gprs_pdp_addr_changed() &&
            !sm.gprs_wwan_state_changed() &&
            sm.is_gprs_registered());
}

bool gf_gprs_autoconnect_timeout(const MdmStatemachine &sm)
{
    //FIX for connection establishment problem:
	//Autoconnect function seems to get stuck sometimes in case of roaming and in GSM networks.
	return sm.is_gprs_autoconnect_count_exceeded();
}


/*
 * Action functions
 */
void af_dbus_getsmsreportconfig(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    SmsEventReportingConfig sms_config;
    (void)sm.get_sms_report_config(sms_config);
    
    auto* gvar = g_variant_new("(iiiii)",
                                    sms_config.get_mode(),
                                    sms_config.get_mt(),
                                    sms_config.get_bm(),
                                    sms_config.get_ds(),
                                    sms_config.get_bfr());
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_getoperstate(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  gvar = static_cast<GVariant*>(nullptr);
    int selmode;
    const int operator_id = sm.get_current_oper().get_id();
    const std::string operator_name = sm.get_operator_name(operator_id);

    if (gf_oper_stored(sm))
    {
      selmode = 1;
    }
    else if (0 != sm.get_oper_scan_mode())
    {
      selmode = 1 + sm.get_oper_scan_mode();
    }
    else if (0 != sm.get_oper_scan_seq())
    {
      selmode = 3 + sm.get_oper_scan_seq();
    }
    else
    {
      selmode = 0;
    }
    gvar = g_variant_new("(iiiiisss)",
                         sm.get_oper_reg_state(),
                         selmode,
                         operator_id,
                         sm.get_current_oper().get_act(),
                         sm.get_current_oper().get_quality_percent(),
                         operator_name.c_str(),
                         sm.get_oper_reg_lac().c_str(),
                         sm.get_oper_reg_cid().c_str());
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}


void af_dbus_getportstate2(MdmStatemachine &sm, Event &ev)
{
    const int state = sm.is_port_enabled() ? 1 : 0;
    const int open = sm.is_modem_port_open() ? 1 : 0;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  gvar = g_variant_new("(ii)", state, open);
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_getsimstate(MdmStatemachine &sm, Event &ev)
{
    int state;
    int attempts = -1;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  gvar = static_cast<GVariant*>(nullptr);
    switch(sm.get_sim_state()) {
    case MdmSimState::SIM_PIN:
        state=1;
        attempts = sm.get_pin_count();
        break;
    case MdmSimState::SIM_PUK:
        state=2;
        attempts = sm.get_puk_count();
        break;
    case MdmSimState::READY:
        state=3;
        break;
    case MdmSimState::NOT_INSERTED:
        state=4;
        break;
    case MdmSimState::SIM_ERROR:
        state=5;
        break;
    case MdmSimState::UNKNOWN:
        /*no break*/
    default:
        state=0;
    }
    gvar = g_variant_new("(ii)", state, attempts);
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_getgprsaccess2(MdmStatemachine &sm, Event &ev)
{
    GprsAccessConfig gprsaccess;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    if(sm.get_gprsaccess(gprsaccess)) {
        auto* gvar = g_variant_new("(isissis)",
                                    sm.get_gprs_reg_state(),
                                    gprsaccess.get_apn().c_str(),
                                    gprsaccess.get_auth(),
                                    gprsaccess.get_user().c_str(),
                                    gprsaccess.get_pass().c_str(),
                                    gprsaccess.get_state(),
                                    sm.get_pdp_address().c_str());
        if (gvar != nullptr)
        {
          dev->invocation().return_value(gvar);
        }
        else
        {
          dev->invocation().return_error("de.wago.mdmdError", "ERROR");
        }
    } else {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_getmodeminfo(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  gvar = g_variant_new("(sss)",
                                    sm.get_modem_manufacturer().c_str(),
                                    sm.get_modem_model().c_str(),
                                    sm.get_modem_revision().c_str() );
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }

}

void af_dbus_getmodemidentity(MdmStatemachine &sm, Event &ev)
{
    auto* const dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  const gvar = g_variant_new("(s)", sm.get_modem_identity().c_str());
    if (gvar != nullptr) {
      dev->invocation().return_value(gvar);
    }
    else {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_getsmsformat(MdmStatemachine &sm, Event &ev)
{
    (void)sm;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  gvar = g_variant_new("(i)", sms_format) ;
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_setsmsformat(MdmStatemachine &sm, Event &ev)
{
    (void)sm;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    int format;
    g_variant_get(dev->invocation().parameters(), "(i)", &format);
    if (format == sms_format)
    {
      dev->invocation().return_value( 0 );
    }
    else
    { //change not allowed
      dev->invocation().return_error("de.wago.mdmdError", "NOT ALLOWED");
    }
}

void af_dbus_getsignalquality(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  gvar = g_variant_new("(ii)",
                                   sm.get_current_oper().get_rssi(),
                                   sm.get_current_oper().get_ber()) ;
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_getversion(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  gvar = g_variant_new("(s)", _MDMD_VERSION);
    (void)sm; 
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_getloglevel(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto*  gvar = g_variant_new("(i)", sm.get_loglevel());
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

void af_dbus_setloglevel(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    int loglevel;
    g_variant_get(dev->invocation().parameters(), "(i)", &loglevel);
    if (sm.set_loglevel(loglevel))
    {
      dev->invocation().return_value(nullptr);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "INVALID VALUE");
    }
}

void af_read_cops(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int id = std::stoi( rev->named_match("id"));
    int act = std::stoi( rev->named_match("act"));
    sm.set_current_oper(id, act);
}

void af_read_cops_null(MdmStatemachine &sm, Event &ev)
{
    (void)ev; 
    //sm.clear_current_oper(); //COPS:0 does not show an invalid operator, but indication for status change

    mdmd_Log(MD_LOG_DBG, "%s: COPS indication\n", sm.get_state().c_str()); //log message for debug purpose only
}

void af_read_cgreg(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int state = std::stoi(rev->named_match("state"));
    sm.set_gprs_reg_state(state);
}

void af_read_pdp_address(MdmStatemachine &sm, Event &ev)
{
  auto* rev = dynamic_cast<ModemEvent *>(&ev);
  //const int cid = atoi( rev->named_match("cid").c_str() );
  const std::string pdp_addr = rev->named_match("pdp_addr");
  sm.set_pdp_address(pdp_addr);
}

void af_read_creg(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int state = std::stoi(rev->named_match("state"));
    std::string location_area_code;  
    std::string cell_id;  
    int access_technology = -1; 
    sm.set_oper_reg_state(state, location_area_code, cell_id, access_technology);
}

void af_read_creg_full(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int state = std::stoi(rev->named_match("state"));
    std::string location_area_code = rev->named_match("lac"); 
    std::string cell_id = rev->named_match("cid"); 
    int access_technology = std::stoi(rev->named_match("act")); 
    sm.set_oper_reg_state(state, location_area_code, cell_id, access_technology);
}

void af_read_csq(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int bit_error_rate = std::stoi(rev->named_match("ber"));
    int received_signal_strength = std::stoi(rev->named_match("rssi"));
    sm.set_current_oper_csq(bit_error_rate, received_signal_strength);
}

void af_read_cme_error(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int err = std::stoi(rev->named_match("err"));
    sm.set_last_cme_error(err);
}

void af_read_cms_error(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int err = std::stoi(rev->named_match("err"));
    sm.set_last_cms_error(err);
}

void af_read_cfun_state(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int state = std::stoi(rev->named_match("state"));
    if (1 == state) 
    {
      sm.set_cfun_state(state);
    }
    else
    {
      sm.set_cfun_state(0);
    }
}

void af_read_cpin_state(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string state = rev->named_match("state");
    if (0 == state.compare("READY"))
    {
        sm.set_sim_state(MdmSimState::READY);
    }
    else if (0 == state.compare("SIM PIN"))
    {
        sm.set_sim_state(MdmSimState::SIM_PIN);
    }
    else if (0 == state.compare("SIM PUK"))
    {
        sm.set_sim_state(MdmSimState::SIM_PUK);
    }
    else if (0 == state.compare("NOT INSERTED"))
    {
        sm.set_sim_state(MdmSimState::NOT_INSERTED);
    }
    else if (0 == state.compare("NOT READY"))
    {
        sm.set_sim_state(MdmSimState::NOT_READY);
    }
    else
    {
        sm.set_sim_state(MdmSimState::UNKNOWN);
    }
}

void af_read_scan_mode(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent*>(&ev);
    int mode = std::stoi(rev->named_match("mode"));
    sm.set_oper_scan_mode(mode);
}

void af_read_scan_seq(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int scan_sequence = std::stoi(rev->named_match("seq"));
    sm.set_oper_scan_seq(scan_sequence);
}

void af_sms_event_report(MdmStatemachine &sm, Event &ev)
{
    (void)ev; 
    sm.event_report_last_read();
}


/*
 * Common transaction functions (used in different states)
 */
bool tf_query_reg_state_full(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    auto at_cmd{"at+cops?;+csq;+creg=2;+creg?;+creg=0;+cgreg?;+cgpaddr="s};
    at_cmd.push_back(rmnet_profileid);
    sm.reset_gprs_change_flags();
    sm.write(at_cmd);
    sm.set_gprs_autoconnect_count();
//    sm.set_current_invocation(MethodInvocation());
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool tf_do_nothing(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)sm; (void)src; (void)dst; (void)ev; 
  return true;
}

bool tf_openport(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.try_open_modem();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

bool tf_port_disabled(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_INF, "%s: modem port disabled\n", sm.get_state().c_str());
    sm.deactivate_cmd_timeout();
    return true;
}

bool tf_startinit(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_INF, "%s: modem port open\n", sm.get_state().c_str());
    sm.set_error_state(MdmErrorState::NONE);
    /*disable command echo mode*/
    sm.write("ATE0");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool tf_dbus_modemreset(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_value(nullptr);

    //disable wwan interface before power down and restart
    sm.wwan_disable();

    sm.write("at+qpowd");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool tf_powerdown_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_INF, "%s: wait modem shutdown...\n", sm.get_state().c_str());
    sm.kick_cmd_timeout(timer_power_down);
    return true;
}

bool tf_powerdown_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_ERR, "%s: modem shutdown error, restart with possible data corruption!\n", sm.get_state().c_str());
    //wwan interface should be already disabled before power down and restart
    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

bool tf_powerdown_finish(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_INF, "%s: modem shutdown successful\n", sm.get_state().c_str());
    //wwan interface should be already disabled before power down and restart
    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

bool tf_powerdown_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_ERR, "%s: modem shutdown timeout, restart with possible data corruption!\n", sm.get_state().c_str());
    //wwan interface should be already disabled before power down and restart
    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

bool tf_dbus_setpin_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_value(nullptr);
    return true;
}

void presetoper(MdmStatemachine &sm, int id, int act, int mode)
{
    switch(mode)
    {
        case static_cast<int>(MODE::PREFER_UMTS): 
            sm.set_oper_scan_mode(0);
            sm.set_oper_scan_seq(2);
            break;
        case static_cast<int>(MODE::PREFER_GSM): 
            sm.set_oper_scan_mode(0);
            sm.set_oper_scan_seq(1);
            break;
        case static_cast<int>(MODE::ONLY_UMTS): 
            sm.set_oper_scan_mode(2);
            sm.set_oper_scan_seq(0);
            break;
        case static_cast<int>(MODE::ONLY_GSM): 
            sm.set_oper_scan_mode(1);
            sm.set_oper_scan_seq(0);
            break;
        case static_cast<int>(MODE::MANUAL): 
            //no break;
        case static_cast<int>(MODE::AUTOMATIC): 
            //no break;
      default:
        sm.set_oper_scan_mode(0);
        sm.set_oper_scan_seq(0);
        break;
    }

    if (mode == static_cast<int>(MODE::MANUAL))
    {
        sm.set_stored_oper(id,act);
    }
    else
    {
        sm.remove_stored_oper();
    }
}

void dbus_presetoper(MdmStatemachine &sm, Event &ev)
{
    auto id = -1;
    auto act = -1;
    auto mode = -1;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    g_variant_get(dev->invocation().parameters(), "(iii)", &mode, &id, &act);
    presetoper(sm, id, act, mode);
    sm.set_stored_selection(mode);
}

bool tf_dbus_setoper_return_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dbus_presetoper(sm, ev);
    dev->invocation().return_value(nullptr);
    return true;
}

void setoper(MdmStatemachine &sm, int id, int act)
{
  auto at_cmd{""s};
  at_cmd.append(R"(at+qcfg="nwscanmode",)" + std::to_string(sm.get_oper_scan_mode()) + ",1;");
  at_cmd.append(R"(+qcfg="nwscanseq",)" + std::to_string(sm.get_oper_scan_seq()) + ",1;");
  if (sm.get_stored_oper(id, act))
  {
      at_cmd.append(R"(+cops=1,2,")" + std::to_string(id) + R"(",)" + std::to_string(act));
  }
  else
  {
      at_cmd.append("+cops=0,2");
  }
  sm.clear_current_oper();
  sm.write(at_cmd);
  sm.kick_cmd_timeout(timer_at_cmd_cops);
}

bool tf_dbus_setoper(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dbus_presetoper(sm, ev);
    sm.set_current_invocation(dev->invocation());

    int id = 0;
    int act = 0;
    setoper(sm, id, act);

    return true;
}

bool tf_dbus_setoper_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value(nullptr);
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setoper_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}


bool tf_gprsprofile_init(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    GprsAccessConfig gprsaccess;
    (void)sm.get_gprsaccess(gprsaccess);

    auto at_cmd{"at+qicsgp="s};
    at_cmd.push_back(rmnet_profileid);
    at_cmd.append(",1,");
    at_cmd.append(R"(")" + gprsaccess.get_apn() + R"(",)");
    at_cmd.append(R"(")" + gprsaccess.get_user() + R"(",)");
    at_cmd.append(R"(")" + gprsaccess.get_pass() + R"(",)");
    at_cmd.append(std::to_string(gprsaccess.get_auth()));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool extract_dbus_setgprsaccess2(GVariant *gvar, GprsAccessConfig& gprsaccess)
{
    gchar* apn = nullptr;
    auto auth = 0;
    gchar* user = nullptr;
    gchar* pass = nullptr;
    auto conn = 0;
    g_variant_get(gvar, "(sissi)", &apn,
                                   &auth,
                                   &user,
                                   &pass,
                                   &conn);

    if((apn != nullptr) && (user != nullptr) && (pass != nullptr))
    {
        gprsaccess.set_apn(apn);
        gprsaccess.set_auth(auth);
        gprsaccess.set_user(user);
        gprsaccess.set_pass(pass);
		gprsaccess.set_state(conn);
        g_free(apn);
        g_free(user);
        g_free(pass);

        return true;
    }
    return false;
}

bool tf_setgprsaccess2_to_parameter_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    GprsAccessConfig gprsaccess;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    if (extract_dbus_setgprsaccess2(dev->invocation().parameters(), gprsaccess))
    {
        sm.set_gprsaccess(gprsaccess);
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_WRN, "%s: DBUS method \"SetGprsAccess\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", "INVALID PARAMETER");
    }

    return true;
}

bool tf_setgprsaccess2_to_modem(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    GprsAccessConfig gprsaccess;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    if (extract_dbus_setgprsaccess2(dev->invocation().parameters(), gprsaccess))
    {
        auto at_cmd{"at+qicsgp="s};
        at_cmd.push_back(rmnet_profileid);
        at_cmd.append(",1,");
        at_cmd.append(R"(")" + gprsaccess.get_apn() + R"(",)");
        at_cmd.append(R"(")" + gprsaccess.get_user() + R"(",)");
        at_cmd.append(R"(")" + gprsaccess.get_pass() + R"(",)");
        at_cmd.append(std::to_string(gprsaccess.get_auth()));
        sm.write(at_cmd);
        sm.kick_cmd_timeout(timer_at_cmd_short);
        sm.set_current_invocation(dev->invocation());
        return true;
    }
    mdmd_Log(MD_LOG_WRN, "%s: DBUS method \"SetGprsAccess2\" with invalid parameter\n", sm.get_state().c_str());
    dev->invocation().return_error("de.wago.mdmdError", "INVALID PARAMETER");

    return false;
}

bool extract_dbus_setsmsreportconfig(GVariant *gvar, SmsEventReportingConfig& sms_report_config)
{
    int mode;
    int mt;
    int bm;
    int ds;
    int bfr;
    g_variant_get(gvar, "(iiiii)", &mode, &mt, &bm, &ds, &bfr);

    if ((mode < 0) || (mode > 2))
    {
        return false; 
    }
    if ((mt < 0) || (mt > 1))
    {
        return false; //full SMS DELIVERS (+CMT) not supported by mdmd
    }
    if (bm != 0)
    {
        return false; //broadcast message indications (+CBM) not supported by mdmd
    }
    if (ds != 0)
    {
        return false; //SMS status reports (+CDS) not supported by mdmd
    }
    if ((bfr < 0) || (bfr > 1))
    {
        return false; 
    }

    sms_report_config.set_mode(mode);
    sms_report_config.set_mt(mt);
    sms_report_config.set_bm(bm);
    sms_report_config.set_ds(ds);
    sms_report_config.set_bfr(bfr);
    
    return true;
}

bool tf_setsmsreportconfig_to_parameter_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    SmsEventReportingConfig sms_report_config;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    if (extract_dbus_setsmsreportconfig(dev->invocation().parameters(), sms_report_config))
    {
        sm.set_sms_report_config(sms_report_config);
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_WRN, "%s: DBUS method \"SetSmsReportConfig\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", "INVALID PARAMETER");
    }

    return true;
}

bool tf_setsmsreportconfig_to_modem(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    SmsEventReportingConfig sms_report_config;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    if (extract_dbus_setsmsreportconfig(dev->invocation().parameters(), sms_report_config))
    {
        auto at_cmd{"at+cnmi="s};
        at_cmd.append(std::to_string(sms_report_config.get_mode()) + ",");
        at_cmd.append(std::to_string(sms_report_config.get_mt()) + ",");
        at_cmd.append(std::to_string(sms_report_config.get_bm()) + ",");
        at_cmd.append(std::to_string(sms_report_config.get_ds()) + ",");
        at_cmd.append(std::to_string(sms_report_config.get_bfr()));
        sm.write(at_cmd);
        sm.kick_cmd_timeout(timer_at_cmd_short);
        sm.set_current_invocation(dev->invocation()); //set current invocation for following parameter handling

        return true;
    }
    mdmd_Log(MD_LOG_WRN, "%s: DBUS method \"SetSmsReportConfig\" with invalid parameter\n", sm.get_state().c_str());
    dev->invocation().return_error("de.wago.mdmdError", "INVALID PARAMETER");

    return false;
}

bool tf_dbus_setsmsreport_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    SmsEventReportingConfig sms_report_config;
    if (extract_dbus_setsmsreportconfig(sm.current_invocation().parameters(), sms_report_config))
    {
        sm.set_sms_report_config(sms_report_config);
        sm.current_invocation().return_value(nullptr);
    }
    else
    {
        //this should not happen when current invocation is set on DBusEvent, see above tf_setsmsstorage_to_modem
        mdmd_Log(MD_LOG_ERR, "%s: DBUS method invocation \"SetSmsReportConfig\" invalid\n", sm.get_state().c_str());
        sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setsmsreport_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setportstate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    bool do_transition = false; //do transition only on value change
    int state = sm.is_port_enabled() ? 1 : 0;
    bool request_port_enabled;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    g_variant_get(dev->invocation().parameters(), "(i)", &state);
    request_port_enabled = (state != 0);
    if (request_port_enabled != sm.is_port_enabled())
    {
      sm.set_port_enabled(request_port_enabled);
      sm.kick_cmd_timeout(timer_immediate);
      do_transition = true;
    }
    dev->invocation().return_value(nullptr);

    return do_transition;
}

bool tf_init_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_ERR, "%s: modem initialization failed\n", sm.get_state().c_str());
    sm.set_error_state(MdmErrorState::INIT_FAILED);
    sm.deactivate_cmd_timeout();
    return true;
}


bool tf_init_cme_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int code = std::stoi(rev->named_match("err"));
    mdmd_Log(MD_LOG_ERR, "%s: CME ERROR %d\n", sm.get_state().c_str(), code);
    sm.set_error_state(MdmErrorState::INIT_FAILED);
    sm.deactivate_cmd_timeout();
    return true;
}

bool tf_init_sim_state_unknown(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)src; (void)dst; (void)ev;
  mdmd_Log(MD_LOG_ERR, "%s: SIM state unknown\n", sm.get_state().c_str());
  sm.deactivate_cmd_timeout();
  return true;
}

bool tf_modem_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_ERR, "%s: command timeout, restart modem with possible data corruption!\n", sm.get_state().c_str());
    sm.wwan_disable();
    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);

    return true;
}

bool tf_io_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.kick_cmd_timeout(timer_wait_port_io_error);

    return true;
}

bool tf_io_error_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.current_invocation().return_error("de.wago.mdmdError", "MODEM IO ERROR");
    sm.kick_cmd_timeout(timer_wait_port_io_error);

    return true;
}
bool tf_io_error_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_ERR, "%s: io error timeout, restart modem with possible data corruption!\n", sm.get_state().c_str());

    //disable wwan interface before restart
    sm.wwan_disable();

    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);

    return true;
}

bool tf_modem_reconfig_restart(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_INF, "%s: reconfiguration, restart modem\n", sm.get_state().c_str());

    //disable wwan interface before power down and restart
    sm.wwan_disable();

    sm.write("at+qpowd");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_dbus_modem_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "TIMEOUT");
    (void)tf_modem_timeout(sm, src, dst, ev);

    return true;
}

bool tf_void_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; (void)ev; 
    sm.deactivate_cmd_timeout();

    return true;
}

bool tf_wait_pin(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_DBG, "%s: SIM PIN required, %d attempts remaining\n", sm.get_state().c_str(), sm.get_pin_count());
    sm.deactivate_cmd_timeout();

    return true;
}

bool tf_wait_puk(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_DBG, "%s: SIM PUK required, %d attempts remaining\n", sm.get_state().c_str(), sm.get_puk_count());
    sm.deactivate_cmd_timeout();

    return true;
}

bool tf_get_network_registration_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.kick_cmd_timeout(timer_status_query);

    return true;
}

bool tf_get_network_registration_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.kick_cmd_timeout(timer_status_query);

    return true;
}

bool tf_init_network_state(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    const auto at_cmd{R"(at+creg=0;+cgreg=0;+qcfg="nwscanmode";+qcfg="nwscanseq";+qindcfg="csq",0)"s};
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_query_simstate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    const auto at_cmd(R"(at+cpin?;+qpinc="SC";+qccid)"s);
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_sim_not_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.clear_current_oper();
    (void)tf_query_simstate(sm, src, dst, ev);

    return true;
}

bool tf_dbus_return_sim_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "SIM FAILURE");
    sm.clear_current_oper();
    (void)tf_query_simstate(sm, src, dst, ev);

    return true;
}

/*
 * Specific transaction functions
 */
/*i01*/
bool tf_wait_dbus(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.kick_cmd_timeout(timer_dbus_retry);

    return true;
}

bool tf_dbus_up(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.kick_cmd_timeout(timer_immediate);

    return true;
}

/*i02_1*/
bool tf_modem_port_not_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.set_error_state(MdmErrorState::PORT_NOT_READY);
    sm.set_port_wait_count(1);
    sm.kick_cmd_timeout(timer_wait_port);
    mdmd_Log(MD_LOG_WRN, "%s: modem port not ready (%d)\n", sm.get_state().c_str(), sm.get_port_wait_count());

    return true;
}

/*i02_2*/
bool tf_openport_retry(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.try_open_modem();
    if (sm.is_modem_port_open())
    {
      sm.kick_cmd_timeout(timer_immediate);
    }
    else
    {
      sm.inc_port_wait_count();
      sm.kick_cmd_timeout(timer_wait_port);
      mdmd_Log(MD_LOG_WRN, "%s: modem port not ready (%d)\n", sm.get_state().c_str(), sm.get_port_wait_count());
    }

    return true;
}

bool tf_wait_modem_port_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_ERR, "%s: wait modem port timeout, restart modem with possible data corruption!\n", sm.get_state().c_str());

    sm.wwan_disable();
    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);

    return true;
}

/*s03*/
bool tf_getmodeminfo_revision(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto revision = rev->named_match("revision");
    sm.set_modem_revision(revision);

    return true;
}

/*s03*/
bool tf_getmodeminfo_manufacturer(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto manufacturer = rev->named_match("line");
    sm.set_modem_manufacturer( manufacturer );

    return true;
}

/*s03*/
bool tf_getmodeminfo_model(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto model = rev->named_match("line");
    sm.set_modem_model(model);

    return true;
}

/*s03*/
bool tf_getmodeminfo_imei(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto imei = rev->named_match("line");
    sm.set_modem_identity( imei );

    return true;
}
/*s04_1*/
bool tf_wait_cfun(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.inc_cfun_wait_count();
    sm.kick_cmd_timeout(timer_wait_cfun);
    mdmd_Log(MD_LOG_INF, "%s: modem not ready (%d)\n", sm.get_state().c_str(), sm.get_cfun_wait_count());

    return true;
}

/*s08*/
bool tf_getsimstate_setstoredpin(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    const auto pin = sm.get_stored_sim_pin();
    auto at_cmd{"at+cpin="s};
    at_cmd += pin;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    sm.set_pin(pin);

    return true;
}

/*s08*/
bool tf_getsim_iccid(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto iccid = rev->named_match("id");
    sm.set_iccid(iccid);

    return true;
}

/*s08*/
bool tf_sim_busy(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.set_sim_state(MdmSimState::NOT_READY);
    sm.kick_cmd_timeout(timer_wait_simstate);

    return true;
}

/*s08*/
bool tf_sim_not_inserted(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.set_sim_state(MdmSimState::NOT_INSERTED);
    sm.set_error_state(MdmErrorState::SIM_REMOVED);
    sm.deactivate_cmd_timeout();
    return true;
}

/*s08*/
bool tf_sim_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.set_sim_state(MdmSimState::SIM_ERROR);
    sm.set_error_state(MdmErrorState::SIM_FAILURE);
    sm.deactivate_cmd_timeout();
    return true;
}

/*s08*/
bool tf_init_sms(MdmStatemachine &sm, State &src, State &dst, Event &ev);
bool tf_sim_ready_init_sms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_sim_state(MdmSimState::READY);
    sm.set_pin_count(-1);
    sm.set_puk_count(-1);
    (void)tf_init_sms(sm, src, dst, ev);

    return true;
}

/*s08*/
bool tf_getsim_pincount(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    sm.set_pin_count(std::stoi(rev->named_match("s1")));
    sm.set_puk_count(std::stoi( rev->named_match("s2")));

    return true;
}

/*s10*/
bool tf_waitpin_dbus_setpin(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    gchar* pin = nullptr;
    g_variant_get(sm.current_invocation().parameters(), "(s)", &pin);

    auto at_cmd{"at+cpin="s};
    if (pin != nullptr)
    {
      std::string pin_str{pin};
      sm.set_pin(pin_str);
      at_cmd += pin_str;
      g_free(pin);
    }
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

/*s11*/
bool tf_waitpin_dbus_setpuk(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    gchar* pin = nullptr;
    gchar* puk = nullptr;
    auto at_cmd("at+cpin="s);

    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(ss)", &puk, &pin);
    if (puk != nullptr)
    {
      at_cmd.append(std::string(puk));
      g_free(puk);
    }
    at_cmd += ",";
    if (pin != nullptr)
    {
      std::string pin_str{pin};
      sm.set_pin(pin_str);
      at_cmd += pin_str;
      g_free(pin);
    }
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

/*s12_1*/
bool tf_setpin_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.set_sim_state(MdmSimState::READY);
    sm.set_pin_count(-1);
    sm.set_puk_count(-1);
    sm.kick_cmd_timeout(timer_wait_pb_done);

    return true;
}

/*s12_1*/
bool tf_setpin_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.reset_stored_sim_pin();
    (void)tf_query_simstate(sm, src, dst, ev);

    return true;
}

/*s12_2*/
bool tf_dbus_setpin_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.store_sim_pin();
    sm.current_invocation().return_value(nullptr);
    (void)tf_setpin_ok(sm, src, dst, ev);

    return true;
}

/*s12_2*/
bool tf_dbus_setpin_cme_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    if (gf_cme_incorrect_password(sm))
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "WRONG PASSWORD");
    }
    else if (gf_cme_sim_failure(sm))
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "SIM FAILURE");
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    (void)tf_setpin_err(sm, src, dst, ev);

    return true;
}

/*s12_2*/
bool tf_dbus_setpin_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_setpin_err(sm, src, dst, ev);

    return true;
}

/*s14*/
bool tf_init_sms_not_allowed(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)src; (void)dst; (void)ev; 
  sm.kick_cmd_timeout(timer_wait_pb_done);

  return true;
}

/*s14*/
bool tf_init_sms_sim_busy(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)src; (void)dst; (void)ev; 
  sm.kick_cmd_timeout(timer_wait_pb_done);

  return true;
}

/*s13*/
bool tf_init_sms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)src; (void)dst; (void)ev; 
  SmsEventReportingConfig sms_config;
  (void)sm.get_sms_report_config(sms_config);//default if get_sms_config fails
  auto at_cmd{"at+cmgf="s};

  at_cmd.append(std::to_string(sms_format) + ";");
  at_cmd.append("+cnmi=");
  at_cmd.append(std::to_string(sms_config.get_mode()) + ",");
  at_cmd.append(std::to_string(sms_config.get_mt()) + ",");
  at_cmd.append(std::to_string(sms_config.get_bm()) + ",");
  at_cmd.append(std::to_string(sms_config.get_ds()) + ",");
  at_cmd.append(std::to_string(sms_config.get_bfr()));
  sm.write(at_cmd);
  sm.kick_cmd_timeout(timer_at_cmd_short);

  return true;
}

/*s17*/
bool tf_readoperatorname(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto id = std::stoi(rev->named_match("id"));
    const auto name = rev->named_match("name");
    const auto inserted = sm.insert_operator_name(id, name);
    mdmd_Log(MD_LOG_DBG, "%s: operator %s %d %s\n", sm.get_state().c_str(),
             inserted ? ":" : "already exists:", id, name.c_str());

    return true;
}

/*s18*/
bool tf_initoper(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    auto id = 0;
    auto act = 0;
    auto mode = 0;
    sm.get_stored_selection(mode);
    sm.get_stored_oper(id, act);
    presetoper(sm, id, act, mode);
    setoper(sm, id, act);

    return true;
}

/*o06_2*/
bool tf_getopers(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    auto line = rev->named_match("cops");
    Regex re("(?U)\\((?<state>[0-9]),\"(?<long>.*)\",\"(?<short>.*)\",\"(?<id>[0-9]+)\",(?<act>[0-9])\\),(?<remains>.*)$");

    do {
        MatchInfo mi = re.match(line);
        if (!static_cast<bool>(mi.matches()))
        {
            break;
        }

        const auto state      = mi.fetch_named("state");
        const auto id_oper    = mi.fetch_named("id");
        const auto act        = mi.fetch_named("act");

        sm.add_opermap(std::stoi(id_oper), std::stoi(act), std::stoi(state));

        line = mi.fetch_named("remains");
    } while (!line.empty());

    return true;
}

/*o10*/
bool tf_listsms_next(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto sms_index = std::stoi(rev->named_match("index"));
    const auto sms_state = std::stoi(rev->named_match("state"));
    size_t pos = sm.get_last_read().find_last_of(',');
    const auto sms_length = (pos == std::string::npos) ? 0 : std::stoi(sm.get_last_read().substr(pos + 1));
    sm.add_smslist(sms_index, sms_state, sms_length);

    return true;
}

/*o10*/
bool tf_sms_get_body(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.set_sms_body_last_read();

    return true;
}

bool tf_dbus_listsms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto at_cmd{"at+cmgl="s};
    auto state = -1;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(i)", &state);
    at_cmd.append(std::to_string(state));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    sm.clear_smslist();

    return true;
}

bool tf_dbus_listsms_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto*  gvar = sm.get_var_smslist();
    if (gvar != nullptr)
    {
      sm.current_invocation().return_value( gvar );
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_listsms_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_deletesms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto at_cmd{"at+cmgd="s};
    auto index = -1;
    auto delflag = -1;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(ii)", &index, &delflag);
    at_cmd.append(std::to_string(index) + ',' + std::to_string(delflag));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_dbus_deletesms_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value(nullptr);
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_deletesms_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}


bool tf_dbus_readsms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto at_cmd{"at+cmgr="s};
    int sms_index = -1;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(i)", &sms_index);
    at_cmd.append(std::to_string(sms_index));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    sm.clear_smslist();
    sm.add_smslist(sms_index);

    return true;
}

bool tf_readsms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    auto sms_state = std::stoi(rev->named_match("state"));
    const auto pos = sm.get_last_read().find_last_of(',');
    const auto sms_length = (pos == std::string::npos) ? 0 : std::stoi(sm.get_last_read().substr(pos + 1));

    sm.set_last_sms(sms_state, sms_length);

    return true;
}

bool tf_dbus_readsms_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto* gvar = sm.get_var_smsread();
    if (gvar != nullptr)
    {
      sm.current_invocation().return_value( gvar );
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_readsms_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_sendsms_head(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto at_cmd{"at+cmgs="s};
    auto pdu_length{-1};
    gchar* body = nullptr;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(is)", &pdu_length, &body);
    g_free(body);
    at_cmd.append(std::to_string(pdu_length));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    sm.set_last_sms_msg_ref(0);

    return true;
}

bool tf_sendsms_body(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    auto pdu_length{-1};
    gchar* c_body = nullptr;
    g_variant_get(sm.current_invocation().parameters(), "(is)", &pdu_length, &c_body);

    std::string body;
    if (c_body != nullptr)
    {
      constexpr auto ctrl_plus_z_key = "\x1a";
      body = std::string(c_body);
      body += ctrl_plus_z_key; 
      g_free(c_body);
    }
    else
    {
      constexpr auto esc_key = "\x1b";
      body = esc_key; 
    }

    sm.write(body);
    sm.kick_cmd_timeout(timer_at_cmd_sms);

    return true;
}

bool tf_sendsms_result(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    auto msgref = std::stoi(rev->named_match("msgref"));

    sm.set_last_sms_msg_ref(msgref);

    return true;
}

bool tf_dbus_sendsms_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto*  gvar = g_variant_new("(i)", sm.get_last_sms_msg_ref());
    if (gvar != nullptr)
    {
      sm.current_invocation().return_value( gvar );
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_sendsms_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_get_smsstorage_default(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    SmsStorageConfig default_sms_storage;
    (void)sm.get_sms_storage_config(default_sms_storage);
    auto* gvar = g_variant_new("(siisiisii)", default_sms_storage.get_mem1().c_str(),
                                              -1,
                                              -1,
                                              default_sms_storage.get_mem2().c_str(),
                                              -1,
                                              -1,
                                              default_sms_storage.get_mem3().c_str(),
                                              -1,
                                              -1);
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }

    return true;
}

bool tf_dbus_getsmsstorage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    sm.set_current_invocation(dev->invocation());
    sm.write("at+cpms?");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_get_sms_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);

    const auto mem1 = rev->named_match("mem1");
    const auto used1 = std::stoi(rev->named_match("used1"));
    const auto total1 = std::stoi(rev->named_match("total1"));

    const auto mem2 = rev->named_match("mem2");
    const auto used2 = std::stoi(rev->named_match("used2"));
    const auto total2 = std::stoi(rev->named_match("total2"));

    const auto mem3 = rev->named_match("mem3");
    const auto used3 = std::stoi(rev->named_match("used3"));
    const auto total3 = std::stoi(rev->named_match("total3"));

    auto*  gvar = g_variant_new("(siisiisii)", mem1.c_str(),
                                                  used1,
                                                  total1,
                                                  mem2.c_str(),
                                                  used2,
                                                  total2,
                                                  mem3.c_str(),
                                                  used3,
                                                  total3);
                                                
    if (gvar != nullptr)
    {
      sm.current_invocation().return_value(gvar);
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    
    return true;
}

bool tf_dbus_getsmsstorage_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_gprs_detach(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.write("at+cgatt=0"); //detach PS as trigger for rmnet/autoconnect
    sm.kick_cmd_timeout(timer_at_cmd_cgatt);
    //sm.set_gprs_reg_state(0); //necessary for "ifdown" trigger

    return true;
}

bool tf_gprs_detach_ok_getopernames(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.set_gprs_reg_state(0); //trigger for wwan interface restart or stop
    sm.write("at+copn");
    sm.kick_cmd_timeout(timer_at_cmd_copn);

    return true;
}

bool tf_gprs_detach_ok_initoper(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_gprs_reg_state(0); //trigger for wwan interface restart or stop
    (void)tf_initoper(sm, src, dst, ev);

    return true;
}

bool tf_dbus_setgprsaccess_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
	//set gprs acccess to storage
    GprsAccessConfig gprsaccess;
    if (extract_dbus_setgprsaccess2(sm.current_invocation().parameters(), gprsaccess))
    {
        sm.set_gprsaccess(gprsaccess);
    }
    else { //internal error that should not happen, current_invocation is possibly invalid
        mdmd_Log(MD_LOG_ERR, "%s: Could not store GPRS access parameter\n", sm.get_state().c_str());
    }
    sm.current_invocation().return_value(nullptr);
    sm.set_gprs_reg_state(0); //trigger for wwan interface restart or stop
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_dbus_setgprsaccess_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_getoperlist_prepare(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    sm.set_current_invocation(dev->invocation());
    /* temporarily disable packet data service for a controlled disconnection of possible IP traffic */
    sm.set_gprs_temporary_disable();
    sm.write("at+cgatt=0");
    sm.kick_cmd_timeout(timer_at_cmd_cgatt);

    return true;
}

bool tf_getoperlist_prepare_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    /* PS detached now */
    sm.set_gprs_reg_state(0);  //trigger for wwan interface stop
    sm.clear_opermap();
    sm.write("at+cops=?");
    sm.kick_cmd_timeout(timer_at_cmd_cops);

    return true;
}

bool tf_getoperlist(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    sm.set_current_invocation(dev->invocation());
    sm.clear_opermap();
    sm.write("at+cops=?");
    sm.kick_cmd_timeout(timer_at_cmd_cops);

    return true;
}

bool tf_getoperlist_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value( sm.get_var_opermap() );
    sm.set_gprs_temporary_enable();
    if (!sm.is_gprs_disabled())
    {
      sm.set_gprs_reg_state(0);  //trigger for wwan interface restart
    }
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_gprs_temporary_enable();
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_fail_not_allowed(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_gprs_temporary_enable();
    sm.current_invocation().return_error("de.wago.mdmdError", "NOT ALLOWED");
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_fail_sim_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_gprs_temporary_enable();
    (void)tf_dbus_return_sim_failure(sm, src, dst, ev);

    return true;
}

bool tf_dbus_not_allowed(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "NOT ALLOWED");

    return true;
}

bool tf_renew_wwan_address(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    //renew WWAN IP address
    sm.wwan_renew();
    sm.kick_cmd_timeout(timer_status_query);

    return true;
}

bool tf_gprs_autoconnect_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.write("at+cgatt=0"); //detach PS as trigger for rmnet/autoconnect
    sm.kick_cmd_timeout(timer_at_cmd_cgatt);

    return true;
}

bool tf_gprs_autoconnect_timeout_detach_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.reset_gprs_autoconnect_count();
    mdmd_Log(MD_LOG_WRN, "%s: Autoconnect timeout\n", sm.get_state().c_str());
    sm.kick_cmd_timeout(timer_status_query);

    return true;
}

bool tf_gprs_autoconnect_timeout_detach_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    //don't reset autoconnect_timeout counter to try again
    mdmd_Log(MD_LOG_ERR, "%s: Autoconnect timeout not resolved\n", sm.get_state().c_str());
    sm.kick_cmd_timeout(timer_status_query);

    return true;
}

bool tf_set_rmnet_profileid(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    auto at_cmd{R"(at+qcfg="rmnet/profileid",)"s};
    at_cmd.push_back(rmnet_profileid);
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_smsstorage_init(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    auto at_cmd{"at+cpms="s};
    SmsStorageConfig sms_storage_config;

    (void)sm.get_sms_storage_config(sms_storage_config);
    at_cmd.append(R"(")" + sms_storage_config.get_mem1() + R"(",)");
    at_cmd.append(R"(")" + sms_storage_config.get_mem2() + R"(",)");
    at_cmd.append(R"(")" + sms_storage_config.get_mem3() + R"(")");
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_sms_report_config_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 

    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto code = std::stoi(rev->named_match("err"));

    mdmd_Log(MD_LOG_WRN, "%s: Sms report configuration failed: cms_error=%d\n", sm.get_state().c_str(), code);

    return tf_smsstorage_init(sm, src, dst, ev); //continue with sms storage configuration
}

bool tf_sms_storage_config_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 

    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto code = std::stoi(rev->named_match("err"));

    mdmd_Log(MD_LOG_WRN, "%s: Sms storage configuration failed: cms_error=%d\n", sm.get_state().c_str(), code);

    return tf_gprsprofile_init(sm, src, dst, ev); //continue with gprs configuration
}

bool extract_dbus_setsmsstorage(GVariant *gvar, SmsStorageConfig& sms_storage_config, const std::vector<std::string> allowed_cpms_parameter)
{
    gchar* c_mem1 = nullptr;
    gchar* c_mem2 = nullptr;
    gchar* c_mem3 = nullptr;
    g_variant_get(gvar, "(sss)", &c_mem1, &c_mem2, &c_mem3);

    std::string mem1{c_mem1};
    std::string mem2{c_mem2};
    std::string mem3{c_mem3};
    bool result = ( (c_mem1 && c_mem2 && c_mem3) &&
                    (std::find(allowed_cpms_parameter.begin(), allowed_cpms_parameter.end(), mem1) != allowed_cpms_parameter.end()) &&
                    (std::find(allowed_cpms_parameter.begin(), allowed_cpms_parameter.end(), mem2) != allowed_cpms_parameter.end()) &&
                    (std::find(allowed_cpms_parameter.begin(), allowed_cpms_parameter.end(), mem3) != allowed_cpms_parameter.end()));  
    if (result)
    {
        sms_storage_config.set_mem1(mem1);
        sms_storage_config.set_mem2(mem2);
        sms_storage_config.set_mem3(mem3);
    }
    
    g_free(c_mem1);
    g_free(c_mem2);
    g_free(c_mem3);

    return result;
}

bool tf_setsmsstorage_to_parameter_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    SmsStorageConfig sms_storage_config;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    if (extract_dbus_setsmsstorage(dev->invocation().parameters(), sms_storage_config, {"SM", "ME", "MT", ""}))
    {
        sm.set_sms_storage_config(sms_storage_config);
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_WRN, "%s: DBUS method \"SetSmsStorage\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", "INVALID PARAMETER");
    }

    return true;
}

bool tf_setsmsstorage_to_modem(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    SmsStorageConfig sms_storage_config;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);

    if (extract_dbus_setsmsstorage(dev->invocation().parameters(), sms_storage_config, {"SM", "ME", "MT"}))
    {
        auto at_cmd{"at+cpms="s};
        at_cmd.append(R"(")" + sms_storage_config.get_mem1() + R"(",)");
        at_cmd.append(R"(")" + sms_storage_config.get_mem2() + R"(",)");
        at_cmd.append(R"(")" + sms_storage_config.get_mem3() + R"(")");
        sm.write(at_cmd);
        sm.kick_cmd_timeout(timer_at_cmd_short);
        sm.set_current_invocation(dev->invocation()); //set current invocation for following parameter handling
    }
    else
    {
        mdmd_Log(MD_LOG_WRN, "%s: DBUS method \"SetSmsStorage\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", "INVALID PARAMETER");

        return false;
    }

    return true;
}

bool tf_dbus_setsmsstorage_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    SmsStorageConfig sms_storage_config;
    if (extract_dbus_setsmsstorage(sm.current_invocation().parameters(), sms_storage_config, {"SM", "ME", "MT"}))
    {
        sm.set_sms_storage_config(sms_storage_config);
        sm.current_invocation().return_value(nullptr);
    }
    else
    {
        //this should not happen when current invocation is set on DBusEvent, see above tf_setsmsstorage_to_modem
        mdmd_Log(MD_LOG_ERR, "%s: DBUS method invocation \"SetSmsStorage\" invalid\n", sm.get_state().c_str());
        sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setsmsstorage_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

} //namespace

/****************************/

using TF = TransitionFunctionFunctor<MdmStatemachine>;
using AF = ActionFunctionFunctor<MdmStatemachine>;
using PG = positive_guard<MdmStatemachine>;
using GF = GuardFunctionFunctor<MdmStatemachine>;

int main(int argc, char **argv)
{
    // (void)argc; (void)argv; 
#if !GLIB_CHECK_VERSION(2,36,0)
    /* g_type_init has been deprecated since version 2.36 and should not be used in newly-written code.
       The type system is now initialised automatically. */
    g_type_init ();
#endif

    (void)argc; (void)argv;

    /*Init states*/
    State i01("i01", "wait dbus");
    State i02_1("i02_1", "open modem port");
    State i02_2("i02_2", "wait modem port", true);
    State i03("i03", "wait modem port enable", true);
    /*Error states*/
    State e01("e01", "init failed", true);
    State e02("e02", "modem port io error", true);
    /*Start-up states*/
    State s01("s01", "set command echo mode");
    State s02("s02", "set error format");
    State s03_1("s03_1", "get modem information");
    State s03_2("s03_2", "get IMEI");
    State s04_0("s04_0", "wait cfun state", true);
    State s04_1("s04_1", "get cfun state");
    State s04_2("s04_2", "set modem full functional");
    State s05_1("s05_1", "get network state");
    State s05_2("s05_2", "set rmnet profile id");
    State s06_1("s06_1", "get rmnet autoconnect state");
    State s06_2("s06_2", "set autoconnect required");
    State s06_3("s06_3", "set autoconnect legacy required");
    State s07("s07", "set autoconnect");
    State s08("s08", "get SIM state");
    State s09("s09", "wait SIM state", true);
    State s10("s10", "wait SIM PIN", true);
    State s11("s11", "wait SIM PUK", true);
    State s12_1("s12_1", "set SIM PIN (stored)");
    State s12_2("s12_2", "set SIM PIN (dbus)");
    State s13("s13", "wait SMS / PB done");
    State s14("s14", "init SMS report");
    State s15("s15", "init SMS storage");
    State s16_1("s16_1", "GPRS init: set profile parameter");
    State s16_2("s16_2", "GPRS init: detach PS");
    State s17("s17", "get operator names");
    State s18("s18", "init operator");
    /*Operation states*/
    State o01("o01", "set operator");
    State o04("o04", "wait user command", true);
    //State o05("o05", "common command");
    State o05_1("o05_1", "set sms storage");
    State o05_2("o05_2", "set sms report config");
    State o05_3("o05_3", "delete sms");
    State o06_1("o06_1", "get OPER list: prepare");
    State o06_2("o06_2", "get OPER list: list available networks");
    State o10("o10", "get SMS list");
    State o11("o11", "read SMS");
    State o12("o12", "send SMS");
    State o13_1("o13_1", "get SMS storage");
    State o13_2("o13_2", "got SMS storage");
    State o15_2("o15_2", "get network registration details (periodic)");
    State o15_3("o15_3", "GPRS autoconnect timeout: detach PS");
    State o16_1("o16_1", "GPRS config: set profile parameter");
    State o16_2("o16_2", "GPRS config: detach PS");
    /*Shutdown states*/
    State x01("x01", "trigger power down");
    State x02("x02", "wait power down");

  MdmStatemachine::transition_list_t tl {

    /*wait dbus*/
    { i01, new RegexEvent("DBus_Connection_up"), i01, new TF(tf_dbus_up), new PG },
    { i01, new RegexEvent("DBus_Connection_down"), i01, new TF(tf_wait_dbus), new PG },
    { i01, new TimeoutEvent(), i01, new TF(tf_wait_dbus), new GF(gf_dbus_offline) },
    { i01, new TimeoutEvent(), i03, new TF(tf_port_disabled), new GF(gf_modem_port_disabled) },
    { i01, new TimeoutEvent(), s01, new TF(tf_startinit), new GF(gf_modem_port_open) },
    { i01, new TimeoutEvent(), i02_1, new TF(tf_openport), new PG },
    /*i02_1: open modem port*/
    { i02_1, new TimeoutEvent(), s01, new TF(tf_startinit), new GF(gf_modem_port_open) },
    { i02_1, new TimeoutEvent(), i02_2, new TF(tf_modem_port_not_ready), new PG },
    /*wait modem port*/
    { i02_2, new TimeoutEvent(), s01, new TF(tf_startinit), new GF(gf_modem_port_open) },
    { i02_2, new TimeoutEvent(), i01, new TF(tf_wait_modem_port_timeout), new GF(gf_wait_modem_port_limit) },
    { i02_2, new TimeoutEvent(), i02_2, new TF(tf_openport_retry), new PG },
    { i02_2, new DBusEvent("SetGprsAccess2"), i02_2, new TF(tf_setgprsaccess2_to_parameter_storage), new PG },
    { i02_2, new DBusEvent("GetSmsStorage"), i02_2, new TF(tf_get_smsstorage_default), new PG },
    { i02_2, new DBusEvent("SetSmsStorage"), i02_2, new TF(tf_setsmsstorage_to_parameter_storage), new PG },
    { i02_2, new DBusEvent("SetSmsReportConfig"), i02_2, new TF(tf_setsmsreportconfig_to_parameter_storage), new PG },
    { i02_2, new DBusEvent("SetOper"), i02_2, new TF(tf_dbus_setoper_return_ok), new PG },
    { i02_2, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { i02_2, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    /*wait modem port enable*/
    { i03, new DBusEvent("SetGprsAccess2"), i03, new TF(tf_setgprsaccess2_to_parameter_storage), new PG },
    { i03, new DBusEvent("GetSmsStorage"), i03, new TF(tf_get_smsstorage_default), new PG },
    { i03, new DBusEvent("SetSmsStorage"), i03, new TF(tf_setsmsstorage_to_parameter_storage), new PG },
    { i03, new DBusEvent("SetSmsReportConfig"), i03, new TF(tf_setsmsreportconfig_to_parameter_storage), new PG },
    { i03, new DBusEvent("SetOper"), i03, new TF(tf_dbus_setoper_return_ok), new PG },
    { i03, new DBusEvent("ModemReset"), i03, new TF(tf_dbus_not_allowed), new PG },
    { i03, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { i03, new TimeoutEvent(), i03, new TF(tf_void_timeout), new PG },
    /*unrecoverable initialization error*/
    { e01, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { e01, new DBusEvent("SetGprsAccess2"), e01, new TF(tf_setgprsaccess2_to_parameter_storage), new PG },
    { e01, new DBusEvent("GetSmsStorage"), e01, new TF(tf_get_smsstorage_default), new PG },
    { e01, new DBusEvent("SetSmsStorage"), e01, new TF(tf_setsmsstorage_to_parameter_storage), new PG },
    { e01, new DBusEvent("SetSmsReportConfig"), e01, new TF(tf_setsmsreportconfig_to_parameter_storage), new PG },
    { e01, new DBusEvent("SetOper"), e01, new TF(tf_dbus_setoper_return_ok), new PG },
    { e01, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { e01, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { e01, new TimeoutEvent(), e01, new TF(tf_void_timeout), new PG },
    /*e02: modem port io error*/
    { e02, new DBusEvent("SetGprsAccess2"), e02, new TF(tf_setgprsaccess2_to_parameter_storage), new PG },
    { e02, new DBusEvent("GetSmsStorage"), e02, new TF(tf_get_smsstorage_default), new PG },
    { e02, new DBusEvent("SetSmsStorage"), e02, new TF(tf_setsmsstorage_to_parameter_storage), new PG },
    { e02, new DBusEvent("SetSmsReportConfig"), e02, new TF(tf_setsmsreportconfig_to_parameter_storage), new PG },
    { e02, new DBusEvent("SetOper"), e02, new TF(tf_dbus_setoper_return_ok), new PG },
    { e02, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_not_allowed), new PG },
    { e02, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { e02, new TimeoutEvent(), i01, new TF(tf_io_error_timeout), new PG },
    /*set command echo mode*/
    { s01, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s01, new ModemEvent("^ATE0$"), s01, new TF(tf_do_nothing), new PG },
    { s01, new ModemEvent("^OK$"), s02, new swrite("at+cmee=1", timer_at_cmd_short), new PG },
    { s01, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s01, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set error format*/
    { s02, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s02, new ModemEvent("^OK$"), s03_1, new swrite("ati", timer_at_cmd_short), new PG },
    { s02, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s02, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get modem information*/
    { s03_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s03_1, new ModemEvent("^OK$"), s03_2, new swrite("at+gsn", timer_at_cmd_short), new PG },
    { s03_1, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s03_1, new ModemEvent("^Revision: (?<revision>.*)"), s03_1, new TF(tf_getmodeminfo_revision), new PG },
    { s03_1, new ModemEvent("(?<line>.*)"), s03_1, new TF(tf_getmodeminfo_manufacturer), new GF(gf_have_no_manufacturer) },
    { s03_1, new ModemEvent("(?<line>.*)"), s03_1, new TF(tf_getmodeminfo_model), new PG },
    { s03_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get IMEI*/
    { s03_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s03_2, new ModemEvent("^OK$"), s04_1, new swrite("at+cfun?", timer_at_cmd_cfun), new PG },
    { s03_2, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s03_2, new ModemEvent("(?<line>.*)"), s03_2, new TF(tf_getmodeminfo_imei), new PG },
    { s03_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s04_0: wait cfun state*/
    { s04_0, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s04_0, new DBusEvent("SetGprsAccess2"), s04_0, new TF(tf_setgprsaccess2_to_parameter_storage), new PG },
    { s04_0, new DBusEvent("GetSmsStorage"), s04_0, new TF(tf_get_smsstorage_default), new PG },
    { s04_0, new DBusEvent("SetSmsStorage"), s04_0, new TF(tf_setsmsstorage_to_parameter_storage), new PG },
    { s04_0, new DBusEvent("SetSmsReportConfig"), s04_0, new TF(tf_setsmsreportconfig_to_parameter_storage), new PG },
    { s04_0, new DBusEvent("SetOper"), s04_0, new TF(tf_dbus_setoper_return_ok), new PG },
    { s04_0, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { s04_0, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { s04_0, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), s04_1, new swrite("at+cfun?", timer_at_cmd_cfun), new PG },
    { s04_0, new TimeoutEvent(), s04_1, new swrite("at+cfun?", timer_at_cmd_cfun), new PG },
    /*s04_1: get cfun state*/
    { s04_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s04_1, new ModemEvent("^OK$"), s04_2, new swrite("at+cfun=1", timer_at_cmd_cfun), new GF(gf_activation_required) },
    { s04_1, new ModemEvent("^OK$"), s05_1, new TF(tf_init_network_state), new PG },
    //Note: after modem hard reset it may be not yet allowed to set cfun -> wait for CFUN indication or timeout
    { s04_1, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s04_0, new TF(tf_wait_cfun), new GF(gf_wait_cfun_state_limit) },
    { s04_1, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_init_err), new PG },
    { s04_1, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s04_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s04_2: set modem full functionality*/
    { s04_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s04_2, new ModemEvent("^OK$"), s05_1, new TF(tf_init_network_state), new PG },
    { s04_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s04_0, new TF(tf_wait_cfun), new GF(gf_wait_cfun_state_limit) },
    { s04_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_init_err), new PG },
    { s04_2, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s04_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s05_1: get network state*/
    { s05_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s05_1, new ModemEvent("^OK$"), s05_2, new TF(tf_set_rmnet_profileid), new PG },
    { s05_1, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s05_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s05_2: set rmnet profile id*/
    { s05_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s05_2, new ModemEvent("^OK$"), s06_1, new swrite("at+qcfg=\"rmnet/autoconnect\"", timer_at_cmd_short), new PG },
    //Note: ignore error, rmnet/profileid may be not supported by the modem, assume default profileid 1 in this case
    { s05_2, new ModemEvent("ERROR"), s06_1, new swrite("at+qcfg=\"rmnet/autoconnect\"", timer_at_cmd_short), new PG },
    { s05_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s06_1: get rmnet autoconnect state*/
    { s06_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s06_1, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",1,0,1"), s06_1, new TF(tf_do_nothing), new PG },
    { s06_1, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",(?<status>[0-9]),(?<linkprot>[0-9]),(?<dtrset>[0-9])"), s06_2, new TF(tf_do_nothing), new PG },
    { s06_1, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",1,0"), s06_1, new TF(tf_do_nothing), new PG }, //DTR set in qmiwwan driver
    { s06_1, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",(?<status>[0-9]),(?<linkprot>[0-9])"), s06_3, new TF(tf_do_nothing), new PG },
    { s06_1, new ModemEvent("^OK$"), s08, new TF(tf_query_simstate), new PG },
    { s06_1, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s06_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set autoconnect required*/
    { s06_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s06_2, new ModemEvent("^OK$"), s07, new swrite("at+qcfg=\"rmnet/autoconnect\",1,0,1", timer_at_cmd_short), new PG },
    { s06_2, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s06_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set autoconnect legacy required*/
    { s06_3, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s06_3, new ModemEvent("^OK$"), s07, new swrite("at+qcfg=\"rmnet/autoconnect\",1,0", timer_at_cmd_short), new PG },
    { s06_3, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s06_3, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set autoconnect*/
    { s07, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s07, new ModemEvent("^OK$"), x01, new TF(tf_modem_reconfig_restart), new PG },
    { s07, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s07, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get SIM state*/
    { s08, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s08, new ModemEvent("^\\+QCCID: (?<id>[0-9]+)"), s08, new TF(tf_getsim_iccid), new PG },
    { s08, new ModemEvent("^\\+QPINC: \"SC\",(?<s1>[0-9]+),(?<s2>[0-9]+)"), s08, new TF(tf_getsim_pincount), new PG },
    { s08, new ModemEvent("^OK$"), s14, new TF(tf_sim_ready_init_sms), new GF(gf_sim_ready) },
    { s08, new ModemEvent("^OK$"), s12_1, new TF(tf_getsimstate_setstoredpin), new GF(gf_set_stored_pin) },
    { s08, new ModemEvent("^OK$"), s11, new TF(tf_wait_puk), new GF(gf_puk_needed) },
    { s08, new ModemEvent("^OK$"), s10, new TF(tf_wait_pin), new GF(gf_pin_needed) },
    { s08, new ModemEvent("^OK$"), s09, new TF(tf_sim_busy), new GF(gf_sim_state_not_ready) },
    { s08, new ModemEvent("^OK$"), e01, new TF(tf_sim_not_inserted), new GF(gf_sim_state_not_inserted) },
    { s08, new ModemEvent("^OK$"), e01, new TF(tf_init_sim_state_unknown), new PG },
    //Note: SIM card may be busy when modem set to full functionality (at+cfun=1) quite recently
    { s08, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s09, new TF(tf_sim_busy), new GF(gf_cme_sim_busy) },
    { s08, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_sim_not_inserted), new GF(gf_cme_sim_not_inserted) },
    { s08, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { s08, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_init_cme_error), new PG },
    { s08, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s08, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*wait SIM state*/
    { s09, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s09, new ModemEvent("^\\+CPIN: (?<state>.*)"), s13, new TF(tf_setpin_ok), new GF(gf_sim_ready) },
    { s09, new ModemEvent("^\\+CPIN: (?<state>.*)"), s08, new TF(tf_query_simstate), new PG },
    { s09, new DBusEvent("SetGprsAccess2"), s09, new TF(tf_setgprsaccess2_to_parameter_storage), new PG },
    { s09, new DBusEvent("GetSmsStorage"), s09, new TF(tf_get_smsstorage_default), new PG },
    { s09, new DBusEvent("SetSmsStorage"), s09, new TF(tf_setsmsstorage_to_parameter_storage), new PG },
    { s09, new DBusEvent("SetSmsReportConfig"), s09, new TF(tf_setsmsreportconfig_to_parameter_storage), new PG },
    { s09, new DBusEvent("SetOper"), s09, new TF(tf_dbus_setoper_return_ok), new PG },
    { s09, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { s09, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { s09, new TimeoutEvent(), s08, new TF(tf_query_simstate), new PG },
    /*wait PIN*/
    { s10, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s10, new DBusEvent("SetGprsAccess2"), s10, new TF(tf_setgprsaccess2_to_parameter_storage), new PG },
    { s10, new DBusEvent("GetSmsStorage"), s10, new TF(tf_get_smsstorage_default), new PG },
    { s10, new DBusEvent("SetSmsStorage"), s10, new TF(tf_setsmsstorage_to_parameter_storage), new PG },
    { s10, new DBusEvent("SetSmsReportConfig"), s10, new TF(tf_setsmsreportconfig_to_parameter_storage), new PG },
    { s10, new DBusEvent("SetOper"), s10, new TF(tf_dbus_setoper_return_ok), new PG },
    { s10, new DBusEvent("SetSimPin"), s12_2, new TF(tf_waitpin_dbus_setpin), new PG },
    { s10, new DBusEvent("SetSimPuk"), s12_2, new TF(tf_waitpin_dbus_setpuk), new PG },
    { s10, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { s10, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { s10, new TimeoutEvent(), s10, new TF(tf_void_timeout), new PG },
    /*wait PUK*/
    { s11, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s11, new DBusEvent("SetGprsAccess2"), s11, new TF(tf_setgprsaccess2_to_parameter_storage), new PG },
    { s11, new DBusEvent("GetSmsStorage"), s11, new TF(tf_get_smsstorage_default), new PG },
    { s11, new DBusEvent("SetSmsStorage"), s11, new TF(tf_setsmsstorage_to_parameter_storage), new PG },
    { s11, new DBusEvent("SetSmsReportConfig"), s11, new TF(tf_setsmsreportconfig_to_parameter_storage), new PG },
    { s11, new DBusEvent("SetOper"), s11, new TF(tf_dbus_setoper_return_ok), new PG },
    { s11, new DBusEvent("SetSimPuk"), s12_2, new TF(tf_waitpin_dbus_setpuk), new PG },
    { s11, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { s11, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { s11, new TimeoutEvent(), s11, new TF(tf_void_timeout), new PG },
    /*set PIN (stored)*/
    { s12_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s12_1, new ModemEvent("^OK$"), s13, new TF(tf_setpin_ok), new PG },
    { s12_1, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_setpin_err), new PG },
    { s12_1, new ModemEvent("ERROR"), s08, new TF(tf_setpin_err), new PG },
    { s12_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set PIN (dbus)*/
    { s12_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s12_2, new ModemEvent("^OK$"), s13, new TF(tf_dbus_setpin_ok), new PG },
    { s12_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_setpin_cme_err), new PG },
    { s12_2, new ModemEvent("ERROR"), s08, new TF(tf_dbus_setpin_err), new PG },
    { s12_2, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*wait SMS / PB done*/
    { s13, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s13, new ModemEvent("^\\+QUSIM: (?<simtype>[0-9]+)"), s13, new TF(tf_do_nothing), new PG },
    { s13, new ModemEvent("^\\+QIND: SMS DONE"), s13, new TF(tf_do_nothing), new PG },
    { s13, new ModemEvent("^\\+QIND: PB DONE"), s14, new TF(tf_init_sms), new PG },
    { s13, new TimeoutEvent(), s14, new TF(tf_init_sms), new PG },
    /*init SMS report*/
    { s14, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s14, new ModemEvent("^OK$"), s15, new TF(tf_smsstorage_init), new PG },
    { s14, new ModemEvent("^\\+CME ERROR: 3"), s13, new TF(tf_init_sms_not_allowed), new PG },
    { s14, new ModemEvent("^\\+CME ERROR: 14"), s13, new TF(tf_init_sms_sim_busy), new PG },
    { s14, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s15, new TF(tf_sms_report_config_error), new PG },
    { s14, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s14, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*init SMS storage*/
    { s15, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s15, new ModemEvent("^OK$"), s16_1, new TF(tf_gprsprofile_init), new PG },
    { s15, new ModemEvent("^\\+CPMS: (?<used1>[0-9]+),(?<total1>[0-9]+),(?<used2>[0-9]+),(?<total2>[0-9]+),(?<used3>[0-9]+),(?<total3>[0-9]+)"), s15, new TF(tf_do_nothing), new PG },
    { s15, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s16_1, new TF(tf_sms_storage_config_error), new PG },
    { s15, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s15, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s16_1: GPRS init: set profile parameter*/
    { s16_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s16_1, new ModemEvent("^OK$"), s16_2, new TF(tf_gprs_detach), new PG },
    { s16_1, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s16_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s16_2: GPRS init: detach PS*/
    { s16_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s16_2, new ModemEvent("^OK$"), s17, new TF(tf_gprs_detach_ok_getopernames), new GF(gf_have_no_operatornames) },
    { s16_2, new ModemEvent("^OK$"), s18, new TF(tf_gprs_detach_ok_initoper), new PG },
    { s16_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { s16_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_init_cme_error), new PG },
    { s16_2, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s16_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get operator names*/
    { s17, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s17, new ModemEvent("^\\+COPN: \"(?<id>[0-9]+)\",\"(?<name>.*)\""), s17, new TF(tf_readoperatorname), new PG },
    { s17, new ModemEvent("^OK$"), s18, new TF(tf_initoper), new PG },
    { s17, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { s17, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_init_cme_error), new PG },
    { s17, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s17, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*init operator*/
    { s18, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s18, new ModemEvent("^OK$"), o15_2, new TF(tf_query_reg_state_full), new PG },
    { s18, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { s18, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_init_cme_error), new PG },
    { s18, new ModemEvent("ERROR"), e01, new TF(tf_init_err), new PG },
    { s18, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set operator*/
    { o01, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o01, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_setoper_ok), new PG },
    { o01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o01, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_setoper_err), new PG },
    { o01, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*wait user command*/
    { o04, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o04, new ModemEvent("^\\+CPIN: (?<state>.*)"), s08, new TF(tf_sim_not_ready), new GF(gf_sim_not_ready) },
    { o04, new DBusEvent("GetOperList"), o06_2, new TF(tf_getoperlist), new GF(gf_gprs_disabled) },
    { o04, new DBusEvent("GetOperList"), o06_1, new TF(tf_getoperlist_prepare), new PG },
    { o04, new DBusEvent("ListSms"), o10, new TF(tf_dbus_listsms), new PG },
    { o04, new DBusEvent("ReadSms"), o11, new TF(tf_dbus_readsms), new PG },
    { o04, new DBusEvent("SendSms"), o12, new TF(tf_dbus_sendsms_head), new PG },
    { o04, new DBusEvent("GetSmsStorage"), o13_1, new TF(tf_dbus_getsmsstorage), new PG },
    { o04, new DBusEvent("SetGprsAccess2"), o16_1, new TF(tf_setgprsaccess2_to_modem), new PG },
    { o04, new DBusEvent("SetSmsStorage"), o05_1, new TF(tf_setsmsstorage_to_modem), new PG },
    { o04, new DBusEvent("SetSmsReportConfig"), o05_2, new TF(tf_setsmsreportconfig_to_modem), new PG },
    { o04, new DBusEvent("DeleteSms"), o05_3, new TF(tf_dbus_deletesms), new PG },
    { o04, new DBusEvent("SetOper"), o01, new TF(tf_dbus_setoper), new PG },
    { o04, new DBusEvent("SetSimPin"), o04, new TF(tf_dbus_setpin_ready), new PG },
    { o04, new DBusEvent("SetSimPuk"), o04, new TF(tf_dbus_setpin_ready), new PG },
    { o04, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { o04, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { o04, new TimeoutEvent(), o15_2, new TF(tf_query_reg_state_full), new PG },
    /*set sms storage*/
    { o05_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o05_1, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_setsmsstorage_ok), new PG },
    { o05_1, new ModemEvent("^\\+CPMS: (?<used1>[0-9]+),(?<total1>[0-9]+),(?<used2>[0-9]+),(?<total2>[0-9]+),(?<used3>[0-9]+),(?<total3>[0-9]+)"), o05_1, new TF(tf_do_nothing), new PG },
    { o05_1, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o05_1, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o05_1, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_setsmsstorage_err), new PG },
    { o05_1, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*set sms report config*/
    { o05_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o05_2, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_setsmsreport_ok), new PG },
    { o05_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o05_2, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o05_2, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_setsmsreport_err), new PG },
    { o05_2, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*delete sms*/
    { o05_3, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o05_3, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_deletesms_ok), new PG },
    { o05_3, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o05_3, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o05_3, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_deletesms_err), new PG },
    { o05_3, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*o06_1: get OPER list: prepare*/
    { o06_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o06_1, new ModemEvent("^OK$"), o06_2, new TF(tf_getoperlist_prepare_ok), new PG },
    { o06_1, new ModemEvent("ERROR"), o15_2, new TF(tf_getoperlist_fail), new PG },
    { o06_1, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*o06_2: get OPER list: list available networks*/
    { o06_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o06_2, new ModemEvent("^\\+COPS: (?<cops>\\([0-9],\".*\",\".*\",\"[0-9]+\",[0-9]\\),)"), o06_2, new TF(tf_getopers), new PG },
    { o06_2, new ModemEvent("^OK$"), o15_2, new TF(tf_getoperlist_ok), new PG },
    { o06_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_getoperlist_fail_sim_failure), new GF(gf_cme_sim_failure) },
    { o06_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), o15_2, new TF(tf_getoperlist_fail_not_allowed), new GF(gf_cme_command_not_allowed) },
    /*todo: WAT21951 - PFC200 3G: Keine Aktualisierung der Provider Liste
     *      Sometimes CME ERROR 3 (Operation not allowed) is returned when autoconnect does an internal action.
     *      The problem is that autoconnect can not be disabled temporarily because the suitable command
     *      "at+qndisop=suspendauto=1" returns an error on UC20G devices. It seems not supported anymore.
     *      A solution would be to handle connection completely in mdmd instead using Quectel specific autoconnect.
     */
    { o06_2, new ModemEvent("ERROR"), o15_2, new TF(tf_getoperlist_fail), new PG },
    { o06_2, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*get SMS list*/
    { o10, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o10, new ModemEvent("^\\+CMGL: (?<index>[0-9]+),(?<state>[0-9]+)"), o10, new TF(tf_listsms_next), new PG },
    { o10, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_listsms_ok), new PG },
    { o10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o10, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o10, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_listsms_err), new PG },
    { o10, new ModemEvent("(?<line>.*)"), o10, new TF(tf_sms_get_body), new PG },
    { o10, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*read SMS*/
    { o11, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o11, new ModemEvent("^\\+CMGR: (?<state>[0-9]+)"), o11, new TF(tf_readsms), new PG },
    { o11, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_readsms_ok), new PG },
    { o11, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o11, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o11, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_readsms_err), new PG },
    { o11, new ModemEvent("(?<line>.*)"), o11, new TF(tf_sms_get_body), new PG },
    { o11, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*send SMS*/
    { o12, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o12, new ModemEvent("^(?<prompt>[>])"), o12, new TF(tf_sendsms_body), new PG },
    { o12, new ModemEvent("^\\+CMGS: (?<msgref>[0-9]+)"), o12, new TF(tf_sendsms_result), new PG },
    { o12, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_sendsms_ok), new PG },
    { o12, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o12, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o12, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_sendsms_err), new PG },
    { o12, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*get SMS storage (dbus invocation not yet returned here)*/
    { o13_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o13_1, new ModemEvent("^\\+CPMS: \"(?<mem1>.*)\",(?<used1>[0-9]+),(?<total1>[0-9]+),\"(?<mem2>.*)\",(?<used2>[0-9]+),(?<total2>[0-9]+),\"(?<mem3>.*)\",(?<used3>[0-9]+),(?<total3>[0-9]+)"), o13_2, new TF(tf_get_sms_storage), new PG },
    { o13_1, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_getsmsstorage_fail), new PG },
    { o13_1, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o13_1, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o13_1, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_getsmsstorage_fail), new PG },
    { o13_1, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*got SMS storage (dbus invocation already returned here)*/
    { o13_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o13_2, new ModemEvent("^OK$"), o15_2, new TF(tf_query_reg_state_full), new PG },
    { o13_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { o13_2, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cms_sim_failure) },
    { o13_2, new ModemEvent("ERROR"), o15_2, new TF(tf_query_reg_state_full), new PG },
    { o13_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get network registration details (periodic)*/
    { o15_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o15_2, new ModemEvent("^OK$"), o04, new TF(tf_renew_wwan_address), new GF(gf_renew_wwan_address) },
    { o15_2, new ModemEvent("^OK$"), o15_3, new TF(tf_gprs_autoconnect_timeout), new GF(gf_gprs_autoconnect_timeout) },
    { o15_2, new ModemEvent("^OK$"), o04, new TF(tf_get_network_registration_ok), new PG },
    { o15_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { o15_2, new ModemEvent("ERROR"), o04, new TF(tf_renew_wwan_address), new GF(gf_renew_wwan_address) },
    { o15_2, new ModemEvent("ERROR"), o15_3, new TF(tf_gprs_autoconnect_timeout), new GF(gf_gprs_autoconnect_timeout) },
    { o15_2, new ModemEvent("ERROR"), o04, new TF(tf_get_network_registration_error), new PG },
    { o15_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*o15_3: GPRS autoconnect timeout: detach PS*/
    { o15_3, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o15_3, new ModemEvent("^OK$"), o04, new TF(tf_gprs_autoconnect_timeout_detach_ok), new PG },
    { o15_3, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { o15_3, new ModemEvent("ERROR"), o04, new TF(tf_gprs_autoconnect_timeout_detach_err), new PG },
    { o15_3, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*o16_1: GPRS config: set profile parameter*/
    { o16_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o16_1, new ModemEvent("^OK$"), o16_2, new TF(tf_gprs_detach), new PG },
    { o16_1, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_setgprsaccess_err), new PG },
    { o16_1, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*o16_2: GPRS config: detach PS*/
    { o16_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error_dbus_return), new PG },
    { o16_2, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_setgprsaccess_ok), new PG },
    { o16_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o16_2, new ModemEvent("ERROR"), o15_2, new TF(tf_dbus_setgprsaccess_err), new PG },
    { o16_2, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*x01: trigger power down*/
    { x01, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { x01, new ModemEvent("^OK$"), x02, new TF(tf_powerdown_ok), new PG },
    { x01, new ModemEvent("ERROR"), i01, new TF(tf_powerdown_err), new PG },
    { x01, new TimeoutEvent(), i01, new TF(tf_powerdown_err), new PG },
    /*x02: wait power down*/
    { x02, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { x02, new ModemEvent("^POWERED DOWN$"), i01, new TF(tf_powerdown_finish), new PG },
    { x02, new TimeoutEvent(), i01, new TF(tf_powerdown_timeout), new PG },
  };

  MdmStatemachine::action_list_t al {
    { new ModemEvent("^\\+CSQ: (?<rssi>[0-9]+),(?<ber>[0-9]+)"), new AF(af_read_csq) },
    { new ModemEvent("^\\+COPS: (?<mode>[0-9]+),2,\"(?<id>[0-9]+)\",(?<act>[0-9]+)"), new AF(af_read_cops)},
    { new ModemEvent("^\\+COPS: 0"), new AF(af_read_cops_null)},
    { new ModemEvent("^\\+CREG: (?<mode>[0-9]+),(?<state>[0-9]+),\"(?<lac>.*)\",\"(?<cid>.*)\",(?<act>[0-9]+)"), new AF(af_read_creg_full)},
    { new ModemEvent("^\\+CREG: (?<mode>[0-9]+),(?<state>[0-9]+)"), new AF(af_read_creg)},
    { new ModemEvent("^\\+CGREG: (?<mode>[0-9]+),(?<state>[0-9]+)"), new AF(af_read_cgreg)},
    { new ModemEvent("^\\+CGPADDR: (?<cid>[0-9]+),\"(?<pdp_addr>.*)\""), new AF(af_read_pdp_address) },
    { new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), new AF(af_read_cme_error) },
    { new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), new AF(af_read_cms_error) },
    { new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), new AF(af_read_cfun_state) },
    { new ModemEvent("^\\+CPIN: (?<state>.*)"), new AF(af_read_cpin_state)},
    { new ModemEvent("^\\+QCFG: \"nwscanmode\",(?<mode>.*)"), new AF(af_read_scan_mode) },
    { new ModemEvent("^\\+QCFG: \"nwscanseq\",(?<seq>.*)"), new AF(af_read_scan_seq) },
    { new ModemEvent("^\\+CMTI: "), new AF(af_sms_event_report) },
    { new DBusEvent("GetOperState"), new AF(af_dbus_getoperstate) },
    { new DBusEvent("GetPortState2"), new AF(af_dbus_getportstate2) },
    { new DBusEvent("GetSimState"), new AF(af_dbus_getsimstate) },
    { new DBusEvent("GetGprsAccess2"), new AF(af_dbus_getgprsaccess2) },
    { new DBusEvent("GetModemInfo"), new AF(af_dbus_getmodeminfo) },
    { new DBusEvent("GetModemIdentity"), new AF(af_dbus_getmodemidentity) },
    { new DBusEvent("GetSmsFormat"), new AF(af_dbus_getsmsformat) },
    { new DBusEvent("SetSmsFormat"), new AF(af_dbus_setsmsformat) },
    { new DBusEvent("GetSignalQuality"), new AF(af_dbus_getsignalquality) },
    { new DBusEvent("GetVersion"), new AF(af_dbus_getversion) },
    { new DBusEvent("GetLogLevel"), new AF(af_dbus_getloglevel) },
    { new DBusEvent("SetLogLevel"), new AF(af_dbus_setloglevel) },
    { new DBusEvent("GetSmsReportConfig"), new AF(af_dbus_getsmsreportconfig) },
  };

    GThread* worker_thread = g_thread_new("mdm_cuse_worker",
                                          static_cast<GThreadFunc>(mdm_cuse_worker_main),
                                          nullptr);
    MdmStatemachine sm(tl, i01, al, MDM_GPIO__PWRKEY_PIN, MDM_MUSB_DRIVER_PORT );
    //trigger initial timeout for statemachine entry
    sm.do_timeout(0,false);

    ml = g_main_loop_new(nullptr, TRUE);
    g_main_loop_run(ml);

    g_thread_join(worker_thread);

    exit(EXIT_SUCCESS);
}

