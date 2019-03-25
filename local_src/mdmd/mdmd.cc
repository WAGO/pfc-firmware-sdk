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
///  \file     mdmd.cc
///
///  \brief    Modem Daemon main function and statemachine implementation
///
///  \author   KNu
//------------------------------------------------------------------------------
#include "mdm_cuse_worker.h"
#include "mdm_statemachine.h"
#include "events.h"
#include "statemachine_functors.h"
#include "mdm_functors.h"

#include <stdlib.h>
#include <algorithm>    // std::transform
#include <sstream>

#include "mdmd_log.h"

GMainLoop *ml;


/// statemachine timer in milliseconds
static const int timer_at_cmd_copn = 125000; //depends on the number of operator names
static const int timer_at_cmd_cops = 185000; //for network selection that is determined by the network
static const int timer_at_cmd_sms = 125000;  //for sms transmission that is determined by the network
static const int timer_at_cmd_cfun = 125000; //15s specified into UC20 AT command manual but 120s timeout with R04A07
static const int timer_at_cmd_short = 5000;  //for commands without mobile network dependency
static const int timer_dbus_retry = 5000;
static const int timer_wait_port = 10000;
static const int timer_wait_port_io_error = 10000; //wait time after modem port io error
static const int timer_wait_pb_done = 15000;
static const int timer_wait_cfun = 10000;
static const int timer_wait_simstate = 5000;
static const int timer_wait_suspendauto = 65000;
static const int timer_wait_ps_detach = 65000;
static const int timer_status_query = 10000;  //for periodic status queries
static const int timer_immediate = 10;
static const int timer_power_down = 65000;    //the maximum time for unregistering network is 60 seconds

#define MAX_AT_CMD_PRINT_LENGTH  128
#define MDM_GPIO__RESET_PIN                 130
#define MDM_GPIO__PWRKEY_PIN                132
#define MDM_MUSB_DRIVER_PORT                  1
#define MAX_PORT_WAIT_COUNT                  60  /*number of wait cycles before automatic modem reset*/
#define MAX_CFUN_WAIT_COUNT                  60  /*number of wait cycles before automatic modem reset*/


/*
 * Guard functions
 */
static bool gf_activation_required(const MdmStatemachine &sm)
{
    return (1 != sm.get_cfun_state());
}

static bool gf_dbus_offline(const MdmStatemachine &sm)
{
    return (false == sm.dbus_online());
}

static bool gf_modem_port_open(const MdmStatemachine &sm)
{
    return (sm.is_modem_port_open());
}

static bool gf_modem_port_disabled(const MdmStatemachine &sm)
{
    return (!sm.is_port_enabled());
}

static bool gf_have_no_manufacturer(const MdmStatemachine &sm)
{
    return (0 == sm.get_modem_manufacturer().length());
}

static bool gf_gprs_disabled(const MdmStatemachine &sm)
{
    return sm.is_gprs_disabled();
}

static bool gf_gprsprofile_set_oninit(const MdmStatemachine &sm)
{
    return sm.is_gprsaccess_set_oninit();
}

static bool gf_oper_stored(const MdmStatemachine &sm)
{
    int id, act;
    return sm.get_stored_oper(id,act);
}

static bool gf_pin_needed(const MdmStatemachine &sm)
{
    return (MdmSimState::SIM_PIN == sm.get_sim_state());
}

static bool gf_wait_modem_port_limit(const MdmStatemachine &sm)
{
    return (sm.get_port_wait_count() >= MAX_PORT_WAIT_COUNT);
}

static bool gf_wait_cfun_state_limit(const MdmStatemachine &sm)
{
    return (sm.get_cfun_wait_count() >= MAX_CFUN_WAIT_COUNT);
}
static bool gf_puk_needed(const MdmStatemachine &sm)
{
    return (MdmSimState::SIM_PUK == sm.get_sim_state());
}

static bool gf_set_stored_pin(const MdmStatemachine &sm)
{
    return ( !sm.is_new_sim_iccid() &&
             (MdmSimState::SIM_PIN == sm.get_sim_state()) &&
             (0 < sm.get_stored_sim_pin().length()) );
}

static bool gf_sim_ready(const MdmStatemachine &sm)
{
    return (MdmSimState::READY == sm.get_sim_state());
}

static bool gf_cme_command_not_allowed(const MdmStatemachine &sm)
{
    return (3 == sm.get_last_cme_error());
}

static bool gf_cme_incorrect_password(const MdmStatemachine &sm)
{
    switch (sm.get_last_cme_error())
    {
      case 12:
      case 16:
        return true;
      default:
        return false;
    }
}

static bool gf_cme_sim_failure(const MdmStatemachine &sm)
{
    switch (sm.get_last_cme_error())
    {
      case 10: /*SIM not inserted*/
      case 13: /*SIM failure*/
      case 14: /*SIM busy*/
      case 15: /*SIM wrong*/
        return true;
      default:
        return false;
    }
}

static bool gf_cms_sim_failure(const MdmStatemachine &sm)
{
    switch (sm.get_last_cms_error())
    {
      case 310: /*SIM not inserted*/
      case 313: /*SIM failure*/
      case 314: /*SIM busy*/
      case 315: /*SIM wrong*/
      case 512: /*SIM not ready*/
        return true;
      default:
        return false;
    }
}

static bool gf_sim_not_ready(const MdmStatemachine &sm)
{
    return (MdmSimState::READY != sm.get_sim_state());
}

static bool gf_have_no_operatornames(const MdmStatemachine &sm)
{
    return (sm.get_count_operator_names() == 0);
}

/*
 * Action functions
 */
static void af_dbus_getoperstate(MdmStatemachine &sm, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant * gvar;
    int selmode;
    const int operator_id = sm.get_current_oper().get_id();
    const std::string operator_name = sm.get_operator_name(operator_id);

    if (gf_oper_stored(sm))
    {
      selmode = 1;
    }
    else if (sm.get_oper_scan_mode())
    {
      selmode = 1 + sm.get_oper_scan_mode();
    }
    else if (sm.get_oper_scan_seq())
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
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}


static void af_dbus_getportstate(MdmStatemachine &sm, Event &ev)
{
    int state = sm.is_port_enabled() ? 1 : 0;
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(i)", state);
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    mdmd_Log(MD_LOG_INF, "%s: deprecated DBUS method \"GetPortState\"\n", sm.get_state().c_str());
    //todo: remove when FW 02.08.xx is not supported anymore
}

static void af_dbus_getportstate2(MdmStatemachine &sm, Event &ev)
{
    const int state = sm.is_port_enabled() ? 1 : 0;
    const int open = sm.is_modem_port_open() ? 1 : 0;
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(ii)", state, open);
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

static void af_dbus_getsimstate(MdmStatemachine &sm, Event &ev)
{
    int state;
    int attempts = -1;
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant * gvar;
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
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

static void af_dbus_getgprsaccess(MdmStatemachine &sm, Event &ev)
{
    GprsAccess gprsaccess = sm.get_gprsaccess();
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(isiss)",
                                    sm.get_gprs_reg_state(),
                                    gprsaccess.get_apn().c_str(),
                                    gprsaccess.get_auth(),
                                    gprsaccess.get_user().c_str(),
                                    gprsaccess.get_pass().c_str());
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    mdmd_Log(MD_LOG_INF, "%s: deprecated DBUS method \"GetGprsAccess\"\n", sm.get_state().c_str());
    //todo: remove when FW 02.08.xx is not supported anymore
}

static void af_dbus_getgprsaccess2(MdmStatemachine &sm, Event &ev)
{
    GprsAccess gprsaccess = sm.get_gprsaccess();
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(isissis)",
                                    sm.get_gprs_reg_state(),
                                    gprsaccess.get_apn().c_str(),
                                    gprsaccess.get_auth(),
                                    gprsaccess.get_user().c_str(),
                                    gprsaccess.get_pass().c_str(),
                                    sm.get_gprs_connectivity(),
                                    sm.get_pdp_address().c_str());
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }

}

static void af_dbus_getmodeminfo(MdmStatemachine &sm, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(sss)",
                                    sm.get_modem_manufacturer().c_str(),
                                    sm.get_modem_model().c_str(),
                                    sm.get_modem_revision().c_str() );
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }

}

static void af_dbus_getmodemidentity(MdmStatemachine &sm, Event &ev)
{
    DBusEvent* const dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* const gvar = g_variant_new("(s)", sm.get_modem_identity().c_str());
    if (gvar != NULL) {
      dev->invocation().return_value(gvar);
    }
    else {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

static void af_dbus_getsmsformat(MdmStatemachine &sm, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(i)", sm.get_sms_format()) ;
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

static void af_dbus_setsmsformat(MdmStatemachine &sm, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    int format;
    g_variant_get(dev->invocation().parameters(), "(i)", &format);
    if (format == sm.get_sms_format())
    {
      dev->invocation().return_value( 0 );
    }
    else
    { //change not allowed
      dev->invocation().return_error("de.wago.mdmdError", "NOT ALLOWED");
    }
}

static void af_dbus_getsignalquality(MdmStatemachine &sm, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(ii)",
                                   sm.get_current_oper().get_rssi(),
                                   sm.get_current_oper().get_ber()) ;
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

static void af_dbus_getversion(MdmStatemachine &sm, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(s)", _MDMD_VERSION);
    (void)sm; //unused parameter
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

static void af_dbus_getloglevel(MdmStatemachine &sm, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    GVariant* gvar = g_variant_new("(i)", sm.get_loglevel());
    if (gvar != NULL)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "ERROR");
    }
}

static void af_dbus_setloglevel(MdmStatemachine &sm, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    int loglevel;
    g_variant_get(dev->invocation().parameters(), "(i)", &loglevel);
    if (sm.set_loglevel(loglevel))
    {
      dev->invocation().return_value( 0 );
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "INVALID VALUE");
    }
}

static void af_read_cops(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int id = atoi( rev->named_match("id").c_str() );
    int act = atoi( rev->named_match("act").c_str() );
    sm.set_current_oper(id, act);
}

static void af_read_cops_clear(MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    sm.clear_current_oper();
}

static void af_read_cgreg(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int state = atoi( rev->named_match("state").c_str() );
    sm.set_gprs_reg_state(state);
}

static void af_read_pdp_address(MdmStatemachine &sm, Event &ev)
{
  ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
  //const int cid = atoi( rev->named_match("cid").c_str() );
  const std::string pdp_addr = rev->named_match("pdp_addr");
  sm.set_pdp_address(pdp_addr);
}

static void af_read_creg(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int state = atoi( rev->named_match("state").c_str() );
    std::string lac = "";  //location area code
    std::string cid = "";  //cell id
    int act = -1; //access technology
    sm.set_oper_reg_state(state, lac, cid, act);
}

static void af_read_creg_full(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int state = atoi( rev->named_match("state").c_str() );
    std::string lac = rev->named_match("lac"); //location area code
    std::string cid = rev->named_match("cid"); //cell id
    int act = atoi( rev->named_match("act").c_str() ); //access technology
    sm.set_oper_reg_state(state, lac, cid, act);
}

static void af_read_csq(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int ber = atoi( rev->named_match("ber").c_str() );
    int rssi = atoi( rev->named_match("rssi").c_str() );
    sm.set_current_oper_csq(ber, rssi);
}

static void af_read_cme_error(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int err = atoi( rev->named_match("err").c_str() );
    sm.set_last_cme_error(err);
}

static void af_read_cms_error(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int err = atoi( rev->named_match("err").c_str() );
    sm.set_last_cms_error(err);
}

static void af_read_cfun_state(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    if (1 == atoi( rev->named_match("state").c_str() ))
    {
      sm.set_cfun_state(1);
    }
    else
    {
      sm.set_cfun_state(0);
    }
}

static void af_read_cpin_state(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
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

static void af_read_scan_mode(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int mode = atoi( rev->named_match("mode").c_str() );
    sm.set_oper_scan_mode(mode);
}

static void af_read_scan_seq(MdmStatemachine &sm, Event &ev)
{
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int seq = atoi( rev->named_match("seq").c_str() );
    sm.set_oper_scan_seq(seq);
}

static void af_sms_event_report(MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    sm.event_report_last_read();
}


/*
 * Common transaction functions (used in different states)
 */
static bool tf_query_reg_state_full(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
    std::string at_cmd;
    snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH,
              "at+cops?;+csq;+creg=2;+creg?;+creg=0;+cgreg?;+cgpaddr=%d",
              sm.get_rmnet_profileid() );
    at_cmd = cmd_buf;
    sm.write(at_cmd);

//    sm.set_current_invocation(MethodInvocation());
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_do_nothing(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)sm; (void)src; (void)dst; (void)ev; //unused parameter
  return true;
}

static bool tf_openport(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.try_open_modem();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

static bool tf_port_disabled(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_INF, "%s: modem port disabled\n", sm.get_state().c_str());
    sm.deactivate_cmd_timeout();
    return true;
}

static bool tf_startinit(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_INF, "%s: modem port open\n", sm.get_state().c_str());
    //sm.log_event(DIAG_3GMM_OPEN_SUCCESS );
    sm.stop_net_led_blink_code();
    /*disable command echo mode*/
    sm.write("ATE0");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_dbus_modemreset(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_value(0);

    //disable wwan interface before power down and restart
    sm.wwan_disable();

    sm.write("at+qpowd");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_powerdown_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_INF, "%s: wait modem shutdown...\n", sm.get_state().c_str());
    sm.kick_cmd_timeout(timer_power_down);
    return true;
}

static bool tf_powerdown_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_ERR, "%s: modem shutdown error, restart with possible data corruption!\n", sm.get_state().c_str());
    //wwan interface should be already disabled before power down and restart
    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

static bool tf_powerdown_finish(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_INF, "%s: modem shutdown successful\n", sm.get_state().c_str());
    //wwan interface should be already disabled before power down and restart
    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

static bool tf_powerdown_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_ERR, "%s: modem shutdown timeout, restart with possible data corruption!\n", sm.get_state().c_str());
    //wwan interface should be already disabled before power down and restart
    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

static bool tf_dbus_setpin_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_value(0);
    return true;
}

static void presetoper(MdmStatemachine &sm, int id, int act, int mode)
{
    switch(mode)
    {
      case 5: /*prefer UMTS*/
        sm.set_oper_scan_mode(0);
        sm.set_oper_scan_seq(2);
        break;
      case 4: /*prefer GSM*/
        sm.set_oper_scan_mode(0);
        sm.set_oper_scan_seq(1);
        break;
      case 3: /*only UMTS*/
        sm.set_oper_scan_mode(2);
        sm.set_oper_scan_seq(0);
        break;
      case 2: /*only GSM*/
        sm.set_oper_scan_mode(1);
        sm.set_oper_scan_seq(0);
        break;
      case 1: /*MANUAL*/
        //no break;
      case 0: /*AUTOMATIC*/
        //no break;
      default:
        sm.set_oper_scan_mode(0);
        sm.set_oper_scan_seq(0);
        break;
    }
    if (mode==1) /*MANUAL*/
    {
        sm.set_stored_oper(id,act);
    }
    else
    {
        sm.remove_stored_oper();
    }
}

static void dbus_presetoper(MdmStatemachine &sm, Event &ev)
{
    int id = -1;
    int act = -1;
    int mode = -1;
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    g_variant_get(dev->invocation().parameters(), "(iii)", &mode, &id, &act);
    presetoper(sm, id, act, mode);
    sm.set_stored_selection(mode);
}

static bool tf_dbus_setoper_return_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dbus_presetoper(sm, ev);
    dev->invocation().return_value( 0 );
    return true;
}

static void setoper(MdmStatemachine &sm, int id, int act)
{
  std::string at_cmd;
  char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
  if (sm.get_stored_oper(id, act))
  {
      snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "at+qcfg=\"nwscanmode\",%d,1;+qcfg=\"nwscanseq\",%d,1;+cops=1,2,\"%d\",%d",
                sm.get_oper_scan_mode(), sm.get_oper_scan_seq(), id, act );
  }
  else
  {
      snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "at+cops=0,2;+qcfg=\"nwscanmode\",%d,1;+qcfg=\"nwscanseq\",%d,1",
                sm.get_oper_scan_mode(), sm.get_oper_scan_seq() );
  }
  at_cmd = cmd_buf;
  sm.clear_current_oper();
  sm.write(at_cmd);
  sm.kick_cmd_timeout(timer_at_cmd_cops);
}

static bool tf_dbus_setoper(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    int id = 0;
    int act = 0;
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dbus_presetoper(sm, ev);
    sm.set_current_invocation(dev->invocation());
    setoper(sm, id, act);
    return true;
}

static bool tf_dbus_setoper_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value( 0 );
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

static bool tf_gprsprofile_set(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    GprsAccess gprsaccess = sm.get_gprsaccess();
    char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
    std::string at_cmd;
    int const profileid = sm.get_rmnet_profileid();

    at_cmd = "at+qicsgp="; //set profile parameter
    snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "%d,1,\"%s\",\"%s\",\"%s\",%d", profileid,
                                                                    gprsaccess.get_apn().c_str(),
                                                                    gprsaccess.get_user().c_str(),
                                                                    gprsaccess.get_pass().c_str(),
                                                                    gprsaccess.get_auth() );
    at_cmd += cmd_buf;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static void dbus_presetgprsaccess(MdmStatemachine &sm, DBusEvent *dev)
{
    char *c_apn = NULL;
    int auth;
    char *c_user = NULL;
    char *c_pass = NULL;
    std::string apn;
    std::string user;
    std::string pass;
    g_variant_get(dev->invocation().parameters(), "(siss)", &c_apn, &auth, &c_user, &c_pass);
    if (c_apn)
    {
      apn = c_apn;
      g_free(c_apn);
    }
    if (c_user)
    {
      user = c_user;
      g_free(c_user);
    }
    if (c_pass)
    {
      pass = c_pass;
      g_free(c_pass);
    }
    sm.set_gprsaccess(GprsAccess(apn, auth, user, pass));
    mdmd_Log(MD_LOG_INF, "%s: deprecated DBUS method \"SetGprsAccess\"\n", sm.get_state().c_str());
    //todo: remove when FW 02.08.xx is not supported anymore
}

static void dbus_presetgprsaccess2(MdmStatemachine &sm, DBusEvent *dev)
{
    char *c_apn = NULL;
    int auth;
    char *c_user = NULL;
    char *c_pass = NULL;
    std::string apn;
    std::string user;
    std::string pass;
    int connectivity;
    g_variant_get(dev->invocation().parameters(), "(sissi)", &c_apn, &auth, &c_user, &c_pass, &connectivity);
    if (c_apn)
    {
      apn = c_apn;
      g_free(c_apn);
    }
    if (c_user)
    {
      user = c_user;
      g_free(c_user);
    }
    if (c_pass)
    {
      pass = c_pass;
      g_free(c_pass);
    }
    sm.set_gprsaccess(GprsAccess(apn, auth, user, pass));
    sm.set_gprs_connectivity(connectivity);
}

static bool tf_dbus_setgprsaccess_oninit(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dbus_presetgprsaccess(sm, dev);
    sm.set_gprsaccess_oninit();
    dev->invocation().return_value( 0 );
    return true;
    //todo: remove when FW 02.08.xx is not supported anymore
}

static bool tf_dbus_setgprsaccess_oninit2(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dbus_presetgprsaccess2(sm, dev);
    sm.set_gprsaccess_oninit();
    dev->invocation().return_value( 0 );
    return true;
}

static bool tf_dbus_setgprsaccess(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dbus_presetgprsaccess(sm, dev);
    sm.set_current_invocation(dev->invocation());
    (void)tf_gprsprofile_set(sm, src, dst, ev);
    return true;
    //todo: remove when FW 02.08.xx is not supported anymore
}

static bool tf_dbus_setgprsaccess2(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dbus_presetgprsaccess2(sm, dev);
    sm.set_current_invocation(dev->invocation());
    (void)tf_gprsprofile_set(sm, src, dst, ev);
    return true;
}

static bool tf_dbus_setsmsstorage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    std::string at_cmd("at+cpms=");
    char *c_mem1 = NULL;
    char *c_mem2 = NULL;
    char *c_mem3 = NULL;
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(sss)", &c_mem1, &c_mem2, &c_mem3);
    at_cmd += "\"";
    if (c_mem1 != NULL)
    {
      at_cmd += c_mem1;
      g_free(c_mem1);
    }
    at_cmd += "\",\"";
    if (c_mem2 != NULL)
    {
      at_cmd += c_mem2;
      g_free(c_mem2);
    }
    at_cmd += "\",\"";
    if (c_mem3 != NULL)
    {
      at_cmd += c_mem3;
      g_free(c_mem3);
    }
    at_cmd += "\"";
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_dbus_setsmsreport(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
    std::string at_cmd;
    int mode;
    int mt;
    int bm;
    int ds;
    int bfr;
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);

    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(iiiii)", &mode, &mt, &bm, &ds, &bfr);

    if (mt > 1) mt = -1; //full SMS DELIVERS (+CMT) not supported by mdmd
    if (bm > 0) bm = -1; //broadcast message indications (+CBM) not supported by mdmd
    if (ds > 0) ds = -1; //SMS status reports (+CDS) not supported by mdmd

    snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "at+cnmi=%d,%d,%d,%d,%d", mode, mt, bm, ds, bfr );
    at_cmd = cmd_buf;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_dbus_setportstate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    bool do_transition = false; //do transition only on value change
    int state = sm.is_port_enabled() ? 1 : 0;
    bool request_port_enabled;
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);

    g_variant_get(dev->invocation().parameters(), "(i)", &state);
    request_port_enabled = (state != 0);
    if (request_port_enabled != sm.is_port_enabled())
    {
      sm.set_port_enabled(request_port_enabled);
      sm.kick_cmd_timeout(timer_immediate);
      do_transition = true;
    }
    dev->invocation().return_value( 0 );
    return do_transition;
}

static bool tf_init_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_ERR, "%s: modem initialization failed\n", sm.get_state().c_str());
    sm.log_event( DIAG_3GMM_ERR_INIT_FAIL);
    sm.deactivate_cmd_timeout();
    return true;
}

static bool tf_cme_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int code = atoi( rev->named_match("err").c_str() );
    switch (code)
    {
      case 10:
        sm.set_sim_state(MdmSimState::NOT_INSERTED);
        mdmd_Log(MD_LOG_WRN, "%s: SIM card not inserted\n", sm.get_state().c_str());
        sm.log_event( DIAG_3GMM_ERR_NOSIM );
        break;
      case 13:
      case 14:
      case 15:
        sm.set_sim_state(MdmSimState::SIM_ERROR);
        mdmd_Log(MD_LOG_WRN, "%s: SIM card failure\n", sm.get_state().c_str());
        sm.log_event( DIAG_3GMM_ERR_SIMAUTH  );
        break;
      default:
        mdmd_Log(MD_LOG_ERR, "%s: CME ERROR %d\n", sm.get_state().c_str(), code);
        sm.log_event( DIAG_3GMM_ERR_INIT_FAIL);
        break;
    }
    sm.deactivate_cmd_timeout();
    return true;
}

// TODO: Reduce code doubling?
static bool tf_init_sim_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    switch (sm.get_sim_state())
    {
      case MdmSimState::NOT_INSERTED:
        mdmd_Log(MD_LOG_WRN, "%s: SIM card not inserted\n", sm.get_state().c_str());
        sm.log_event( DIAG_3GMM_ERR_NOSIM );
        break;
      case MdmSimState::NOT_READY:
        mdmd_Log(MD_LOG_WRN, "%s: SIM card not ready\n", sm.get_state().c_str());
        sm.log_event( DIAG_3GMM_ERR_NOSIM );
        break;
      default:
        mdmd_Log(MD_LOG_WRN, "%s: SIM card failure\n", sm.get_state().c_str());
        sm.log_event( DIAG_3GMM_ERR_SIMAUTH  );
        break;
    }
    sm.deactivate_cmd_timeout();
    return true;
}

static bool tf_modem_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_ERR, "%s: command timeout, restart modem with possible data corruption!\n", sm.get_state().c_str());

    //disable wwan interface before restart
    sm.wwan_disable();

    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

static bool tf_io_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.kick_cmd_timeout(timer_wait_port_io_error);
    return true;
}

static bool tf_io_error_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_ERR, "%s: io error timeout, restart modem with possible data corruption!\n", sm.get_state().c_str());

    //disable wwan interface before restart
    sm.wwan_disable();

    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}

static bool tf_modem_reconfig_restart(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_INF, "%s: reconfiguration, restart modem\n", sm.get_state().c_str());

    //disable wwan interface before power down and restart
    sm.wwan_disable();

    sm.write("at+qpowd");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_dbus_modem_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "TIMEOUT");
    (void)tf_modem_timeout(sm, src, dst, ev);
    return true;
}

static bool tf_void_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; (void)ev; //unused parameter
    sm.deactivate_cmd_timeout();
    return true;
}

static bool tf_wait_pin(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_DBG, "%s: SIM PIN required, %d attempts remaining\n", sm.get_state().c_str(), sm.get_pin_count());
    sm.deactivate_cmd_timeout();
    return true;
}

static bool tf_wait_puk(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_DBG, "%s: SIM PUK required, %d attempts remaining\n", sm.get_state().c_str(), sm.get_puk_count());
    sm.deactivate_cmd_timeout();
    return true;
}

static bool tf_dbus_return_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}

static bool tf_get_network_registration_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}

static bool tf_get_network_registration_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}

static bool tf_init_network_state(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    std::string at_cmd("at+creg=0;+cgreg=0;+qcfg=\"nwscanmode\";+qcfg=\"nwscanseq\";+qindcfg=\"csq\",0");
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_query_simstate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    std::string at_cmd("at+cpin?;+qpinc=\"SC\";+qccid");
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_sim_not_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.clear_current_oper();
    (void)tf_query_simstate(sm, src, dst, ev);
    return true;
}

static bool tf_dbus_return_sim_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
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
static bool tf_wait_dbus(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.kick_cmd_timeout(timer_dbus_retry);
    return true;
}
static bool tf_dbus_up(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}
/*i02_1*/
static bool tf_modem_port_not_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.log_event( DIAG_3GMM_ERR_PORT_NOT_READY );
    sm.set_port_wait_count(1);
    sm.kick_cmd_timeout(timer_wait_port);
    mdmd_Log(MD_LOG_WRN, "%s: modem port not ready (%d)\n", sm.get_state().c_str(), sm.get_port_wait_count());
    return true;
}
/*i02_2*/
static bool tf_openport_retry(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
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
static bool tf_wait_modem_port_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    mdmd_Log(MD_LOG_ERR, "%s: wait modem port timeout, restart modem with possible data corruption!\n", sm.get_state().c_str());

    //disable wwan interface before restart
    sm.wwan_disable();

    sm.modem_hard_reset();
    sm.kick_cmd_timeout(timer_immediate);
    return true;
}
/*s03*/
static bool tf_getmodeminfo_revision(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    std::string revision = rev->named_match("revision");
    sm.set_modem_revision( revision );
    return true;
}
/*s03*/
static bool tf_getmodeminfo_manufacturer(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    std::string manufacturer = rev->named_match("line");
    sm.set_modem_manufacturer( manufacturer );
    return true;
}
/*s03*/
static bool tf_getmodeminfo_model(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    std::string model = rev->named_match("line");
    sm.set_modem_model( model );
    return true;
}
/*s03*/
static bool tf_getmodeminfo_imei(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    std::string imei = rev->named_match("line");
    sm.set_modem_identity( imei );
    return true;
}
/*s04_1*/
static bool tf_wait_cfun(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.inc_cfun_wait_count();
    sm.kick_cmd_timeout(timer_wait_cfun);
    mdmd_Log(MD_LOG_INF, "%s: modem not ready (%d)\n", sm.get_state().c_str(), sm.get_cfun_wait_count());
    return true;
}
/*s08*/
static bool tf_getsimstate_setstoredpin(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    std::string pin = sm.get_stored_sim_pin();

    std::string at_cmd("at+cpin=");
    at_cmd += pin;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    sm.set_pin(pin);
    return true;
}
/*s08*/
static bool tf_getsim_iccid(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    std::string iccid = rev->named_match("id");
    sm.set_iccid( iccid );
    return true;
}
/*s08*/
static bool tf_sim_busy(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.kick_cmd_timeout(timer_wait_simstate);
    return true;
}
/*s08*/
static bool tf_init_sms(MdmStatemachine &sm, State &src, State &dst, Event &ev);
static bool tf_sim_ready_init_sms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_sim_state(MdmSimState::READY);
    sm.set_pin_count(-1);
    sm.set_puk_count(-1);
    (void)tf_init_sms(sm, src, dst, ev);
    return true;
}
/*s08*/
static bool tf_getsim_pincount(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    sm.set_pin_count( atoi( rev->named_match("s1").c_str() ) );
    sm.set_puk_count( atoi( rev->named_match("s2").c_str() ) );
    return true;
}
/*s10*/
static bool tf_waitpin_dbus_setpin(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    char *pin = NULL;
    std::string at_cmd("at+cpin=");

    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(s)", &pin);
    if (pin)
    {
      sm.set_pin(pin);
      at_cmd += pin;
      g_free(pin);
    }
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}
/*s11*/
static bool tf_waitpin_dbus_setpuk(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    char *pin = NULL;
    char *puk = NULL;
    std::string at_cmd("at+cpin=");

    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(ss)", &puk, &pin);
    if (puk)
    {
      at_cmd += puk;
      g_free(puk);
    }
    at_cmd += ",";
    if (pin)
    {
      sm.set_pin(pin);
      at_cmd += pin;
      g_free(pin);
    }
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}
/*s12_1*/
static bool tf_setpin_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.set_sim_state(MdmSimState::READY);
    sm.set_pin_count(-1);
    sm.set_puk_count(-1);
    sm.kick_cmd_timeout(timer_wait_pb_done);
    return true;
}
/*s12_1*/
static bool tf_setpin_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.reset_stored_sim_pin();
    (void)tf_query_simstate(sm, src, dst, ev);
    return true;
}
/*s12_2*/
static bool tf_dbus_setpin_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.store_sim_pin();
    sm.current_invocation().return_value( 0 );
    (void)tf_setpin_ok(sm, src, dst, ev);
    return true;
}
/*s12_2*/
static bool tf_dbus_setpin_cme_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
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
static bool tf_dbus_setpin_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_setpin_err(sm, src, dst, ev);
    return true;
}
/*s14*/
static bool tf_init_sms_not_allowed(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)src; (void)dst; (void)ev; //unused parameter
  sm.kick_cmd_timeout(timer_wait_pb_done);
  return true;
}
/*s14*/
static bool tf_init_sms_sim_busy(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)src; (void)dst; (void)ev; //unused parameter
  sm.kick_cmd_timeout(timer_wait_pb_done);
  return true;
}
/*s13*/
static bool tf_init_sms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
  (void)src; (void)dst; (void)ev; //unused parameter
  sm.write("at+cmgf=0;+cnmi=0,0,0,0,0");
  sm.set_sms_format(0);
  sm.set_sms_event_report(0,0,0,0,0);
  sm.kick_cmd_timeout(timer_at_cmd_short);
  return true;
}
/*s16_1*/
static bool tf_getprofileid(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int profileid = atoi( rev->named_match("id").c_str() );
    sm.set_rmnet_profileid(profileid);
    return true;
}
/*s16_1*/
static bool tf_gprsprofile_get(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    std::string at_cmd;
    char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
    int const profileid = sm.get_rmnet_profileid();

    snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "at+qicsgp=%d", profileid );
    at_cmd = cmd_buf;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}
/*s16_2*/
static bool tf_getprofileparams(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    std::string apn = rev->named_match("apn");
    int auth = atoi( rev->named_match("auth").c_str() );
    std::string user = rev->named_match("user");
    std::string pass = rev->named_match("pass");
    sm.set_gprsaccess(GprsAccess(apn, auth, user, pass));
    return true;
}
/*s17*/
static bool tf_readoperatorname(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    const int id = atoi( rev->named_match("id").c_str() );
    const std::string name = rev->named_match("name");
    const bool inserted = sm.insert_operator_name(id, name);
    mdmd_Log(MD_LOG_DBG, "%s: operator %s %d %s\n", sm.get_state().c_str(),
             inserted ? ":" : "already exists:", id, name.c_str());
    return true;
}
/*s18*/
static bool tf_initoper(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    int id = 0;
    int act = 0;
    int mode = 0;
    sm.get_stored_selection(mode);
    sm.get_stored_oper(id, act);
    presetoper(sm, id, act, mode);
    setoper(sm, id, act);
    return true;
}
/*o05*/
static bool tf_dbus_return_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.current_invocation().return_value( 0 );
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}
/*o06_2*/
static bool tf_getopers(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    std::string line = rev->named_match("cops");
    Regex re("(?U)\\((?<state>[0-9]),\"(?<long>.*)\",\"(?<short>.*)\",\"(?<id>[0-9]+)\",(?<act>[0-9])\\),(?<remains>.*)$");

    do {
        MatchInfo mi = re.match(line);
        if (! mi.matches())
            break;

        std::string state      = mi.fetch_named("state");
        std::string id_oper    = mi.fetch_named("id");
        std::string act        = mi.fetch_named("act");

        sm.add_opermap(atoi(id_oper.c_str()), atoi(act.c_str()), atoi(state.c_str()));

        line = mi.fetch_named("remains");
    } while (line.size() != 0);
    return true;
}
/*o10*/
static bool tf_listsms_next(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int sms_index = atoi( rev->named_match("index").c_str() );
    int sms_state = atoi( rev->named_match("state").c_str() );
    size_t pos = sm.get_last_read().find_last_of(',');
    int sms_length = (pos == std::string::npos) ? 0 : atoi( sm.get_last_read().substr(pos + 1).c_str() );
    sm.add_smslist(sms_index, sms_state, sms_length);
    return true;
}
/*o10*/
static bool tf_sms_get_body(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.set_sms_body_last_read();
    return true;
}

static bool tf_dbus_listsms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
    std::string at_cmd;
    int state = -1;

    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(i)", &state);

    snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "at+cmgl=%d", state );
    at_cmd = cmd_buf;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    sm.clear_smslist();
    return true;
}

static bool tf_dbus_listsms_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    GVariant* gvar = sm.get_var_smslist();
    if (gvar != NULL)
    {
      sm.current_invocation().return_value( gvar );
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}

static bool tf_dbus_deletesms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
    std::string at_cmd;
    int index;
    int delflag;

    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(ii)", &index, &delflag);

    snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "at+cmgd=%d,%d", index, delflag );
    at_cmd = cmd_buf;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_dbus_readsms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
    std::string at_cmd;
    int sms_index = -1;

    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(i)", &sms_index);

    snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "at+cmgr=%d", sms_index);
    at_cmd = cmd_buf;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    sm.clear_smslist();
    sm.add_smslist(sms_index);
    return true;
}

static bool tf_readsms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int sms_state = atoi( rev->named_match("state").c_str() );
    size_t pos = sm.get_last_read().find_last_of(',');
    int sms_length = (pos == std::string::npos) ? 0 : atoi( sm.get_last_read().substr(pos + 1).c_str() );
    sm.set_last_sms(sms_state, sms_length);
    return true;
}

static bool tf_dbus_readsms_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    GVariant* gvar = sm.get_var_smsread();
    if (gvar != NULL)
    {
      sm.current_invocation().return_value( gvar );
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}

static bool tf_dbus_sendsms_head(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    char cmd_buf[MAX_AT_CMD_PRINT_LENGTH];
    std::string at_cmd;
    int pdu_length = -1;
    char *c_body = NULL;
    std::string body;

    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(is)", &pdu_length, &c_body);

    if (c_body)
    {
      g_free(c_body);
    }

    snprintf( cmd_buf, MAX_AT_CMD_PRINT_LENGTH, "at+cmgs=%d", pdu_length);
    at_cmd = cmd_buf;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    sm.set_last_sms_msg_ref(0);
    return true;
}

static bool tf_sendsms_body(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    int pdu_length = -1;
    char *c_body = NULL;
    std::string body;

    g_variant_get(sm.current_invocation().parameters(), "(is)", &pdu_length, &c_body);

    if (c_body)
    {
      body = c_body;
      body += "\x1a";  /*CTRL+Z*/
      g_free(c_body);
    }
    else
    {
      body = "\x1b";  /*ESC*/
    }

    sm.write(body);
    sm.kick_cmd_timeout(timer_at_cmd_sms);
    return true;
}

static bool tf_sendsms_result(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    int msgref = atoi( rev->named_match("msgref").c_str() );
    sm.set_last_sms_msg_ref(msgref);
    return true;
}

static bool tf_dbus_sendsms_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    GVariant* gvar = g_variant_new("(i)", sm.get_last_sms_msg_ref());
    if (gvar != NULL)
    {
      sm.current_invocation().return_value( gvar );
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}

static bool tf_dbus_getsmsstorage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    sm.write("at+cpms?");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_get_sms_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    std::string mem;
    int used;
    int total;
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    mem = rev->named_match("mem1");
    used = atoi( rev->named_match("used1").c_str() );
    total = atoi( rev->named_match("total1").c_str() );
    sm.set_sms_storage(0, mem, used, total);
    mem = rev->named_match("mem2");
    used = atoi( rev->named_match("used2").c_str() );
    total = atoi( rev->named_match("total2").c_str() );
    sm.set_sms_storage(1, mem, used, total);
    mem = rev->named_match("mem3");
    used = atoi( rev->named_match("used3").c_str() );
    total = atoi( rev->named_match("total3").c_str() );
    sm.set_sms_storage(2, mem, used, total);
    return true;
}

static bool tf_dbus_getsmsstorage_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    GVariant* gvar = g_variant_new("(siisiisii)",
                                    sm.get_sms_storage_mem(0).c_str(),
                                    sm.get_sms_storage_used(0),
                                    sm.get_sms_storage_total(0),
                                    sm.get_sms_storage_mem(1).c_str(),
                                    sm.get_sms_storage_used(1),
                                    sm.get_sms_storage_total(1),
                                    sm.get_sms_storage_mem(2).c_str(),
                                    sm.get_sms_storage_used(2),
                                    sm.get_sms_storage_total(2));
    if (gvar != NULL)
    {
      sm.current_invocation().return_value(gvar);
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}

static bool tf_dbus_getsmsreportconfig(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    sm.write("at+cnmi?");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_get_smsreportconfig(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    int mode;
    int mt;
    int bm;
    int ds;
    int bfr;
    ModemEvent *rev = dynamic_cast<ModemEvent *>(&ev);
    mode = atoi( rev->named_match("mode").c_str() );
    mt = atoi( rev->named_match("mt").c_str() );
    bm = atoi( rev->named_match("bm").c_str() );
    ds = atoi( rev->named_match("ds").c_str() );
    bfr = atoi( rev->named_match("bfr").c_str() );
    sm.set_sms_event_report(mode, mt, bm, ds, bfr);
    return true;
}

static bool tf_dbus_getsmsreportconfig_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    GVariant* gvar = g_variant_new("(iiiii)",
                                    sm.get_sms_event_report_mode(),
                                    sm.get_sms_event_report_mt(),
                                    sm.get_sms_event_report_bm(),
                                    sm.get_sms_event_report_ds(),
                                    sm.get_sms_event_report_bfr());
    if (gvar != NULL)
    {
      sm.current_invocation().return_value(gvar);
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    }
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}


static bool tf_gprs_detach(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.write("at+cgatt=0"); //detach PS as trigger for rmnet/autoconnect
    sm.kick_cmd_timeout(timer_at_cmd_short);
    //sm.set_gprs_reg_state(0); //necessary for "ifdown" trigger
    return true;
}

static bool tf_gprs_detach_ok_getopernames(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.set_gprs_reg_state(0); //trigger for wwan interface restart or stop
    sm.write("at+copn");
    sm.kick_cmd_timeout(timer_at_cmd_copn);
    return true;
}

static bool tf_gprs_detach_ok_initoper(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_gprs_reg_state(0); //trigger for wwan interface restart or stop
    (void)tf_initoper(sm, src, dst, ev);
    return true;
}

static bool tf_dbus_setgprsaccess_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value( 0 );
    sm.set_gprs_reg_state(0); //trigger for wwan interface restart or stop
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

static bool tf_getoperlist_prepare(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());

    /* temporarily disable packet data service for a controlled disconnection of possible IP traffic */
    sm.set_gprs_temporary_disable();
    sm.write("at+cgatt=0");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

static bool tf_getoperlist_prepare_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    /* PS detached now */
    sm.set_gprs_reg_state(0);  //trigger for wwan interface stop

    sm.clear_opermap();
    sm.write("at+cops=?");
    sm.kick_cmd_timeout(timer_at_cmd_cops);
    return true;
}

static bool tf_getoperlist(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());

    sm.clear_opermap();
    sm.write("at+cops=?");
    sm.kick_cmd_timeout(timer_at_cmd_cops);
    return true;
}

static bool tf_getoperlist_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
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

static bool tf_getoperlist_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_gprs_temporary_enable();
    sm.current_invocation().return_error("de.wago.mdmdError", "ERROR");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

static bool tf_getoperlist_fail_not_allowed(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_gprs_temporary_enable();
    sm.current_invocation().return_error("de.wago.mdmdError", "NOT ALLOWED");
    (void)tf_query_reg_state_full(sm, src, dst, ev);
    return true;
}

static bool tf_getoperlist_fail_sim_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_gprs_temporary_enable();
    (void)tf_dbus_return_sim_failure(sm, src, dst, ev);
    return true;
}

static bool tf_dbus_not_allowed(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "NOT ALLOWED");
    return true;
}


/****************************/

typedef TransitionFunctionFunctor<MdmStatemachine> TF;
typedef ActionFunctionFunctor<MdmStatemachine> AF;
typedef positive_guard<MdmStatemachine> PG;
typedef GuardFunctionFunctor<MdmStatemachine> GF;

int main(int argc, char **argv)
{
    (void)argc; (void)argv; //unused parameter
#if !GLIB_CHECK_VERSION(2,36,0)
    /* g_type_init has been deprecated since version 2.36 and should not be used in newly-written code.
       The type system is now initialised automatically. */
    g_type_init ();
#endif

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
    State s05_2("s05_2", "GPRS init: get profile id");
    State s05_3("s05_3", "GPRS init: get profile parameter");
    State s06_1("s06_1", "get autoconnect state");
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
    State s14("s14", "init SMS");
    State s16_1("s16_1", "GPRS init: set profile parameter");
    State s16_2("s16_2", "GPRS init: detach PS");
    State s17("s17", "get operator names");
    State s18("s18", "init operator");
    /*Operation states*/
    State o01("o01", "set operator");
    State o04("o04", "wait user command", true);
    State o05("o05", "common command");
    State o06_1("o06_1", "get OPER list: prepare");
    State o06_2("o06_2", "get OPER list: list available networks");
    State o10("o10", "get SMS list");
    State o11("o11", "read SMS");
    State o12("o12", "send SMS");
    State o13("o13", "get SMS storage");
    State o14("o14", "get SMS event report config");
    State o15_2("o15_2", "get network registration details (periodic)");
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
    { i02_2, new DBusEvent("SetGprsAccess"), i02_2, new TF(tf_dbus_setgprsaccess_oninit), new PG },
    { i02_2, new DBusEvent("SetGprsAccess2"), i02_2, new TF(tf_dbus_setgprsaccess_oninit2), new PG },
    { i02_2, new DBusEvent("SetOper"), i02_2, new TF(tf_dbus_setoper_return_ok), new PG },
    { i02_2, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { i02_2, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    /*wait modem port enable*/
    { i03, new DBusEvent("SetGprsAccess"), i03, new TF(tf_dbus_setgprsaccess_oninit), new PG },
    { i03, new DBusEvent("SetGprsAccess2"), i03, new TF(tf_dbus_setgprsaccess_oninit2), new PG },
    { i03, new DBusEvent("SetOper"), i03, new TF(tf_dbus_setoper_return_ok), new PG },
    { i03, new DBusEvent("ModemReset"), i03, new TF(tf_dbus_not_allowed), new PG },
    { i03, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { i03, new TimeoutEvent(), i03, new TF(tf_void_timeout), new PG },
    /*unrecoverable initialization error*/
    { e01, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { e01, new DBusEvent("SetGprsAccess"), e01, new TF(tf_dbus_setgprsaccess_oninit), new PG },
    { e01, new DBusEvent("SetGprsAccess2"), e01, new TF(tf_dbus_setgprsaccess_oninit2), new PG },
    { e01, new DBusEvent("SetOper"), e01, new TF(tf_dbus_setoper_return_ok), new PG },
    { e01, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { e01, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { e01, new TimeoutEvent(), e01, new TF(tf_void_timeout), new PG },
    /*e02: modem port io error*/
    { e02, new DBusEvent("SetGprsAccess"), e02, new TF(tf_dbus_setgprsaccess_oninit), new PG },
    { e02, new DBusEvent("SetGprsAccess2"), e02, new TF(tf_dbus_setgprsaccess_oninit2), new PG },
    { e02, new DBusEvent("SetOper"), e02, new TF(tf_dbus_setoper_return_ok), new PG },
    { e02, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_not_allowed), new PG },
    { e02, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { e02, new TimeoutEvent(), i01, new TF(tf_io_error_timeout), new PG },
    /*set command echo mode*/
    { s01, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s01, new ModemEvent("^ATE0$"), s01, new TF(tf_do_nothing), new PG },
    { s01, new ModemEvent("^OK$"), s02, new swrite("at+cmee=1", timer_at_cmd_short), new PG },
    { s01, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s01, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set error format*/
    { s02, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s02, new ModemEvent("^OK$"), s03_1, new swrite("ati", timer_at_cmd_short), new PG },
    { s02, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s02, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get modem information*/
    { s03_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s03_1, new ModemEvent("^OK$"), s03_2, new swrite("at+gsn", timer_at_cmd_short), new PG },
    { s03_1, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s03_1, new ModemEvent("^Revision: (?<revision>.*)"), s03_1, new TF(tf_getmodeminfo_revision), new PG },
    { s03_1, new ModemEvent("(?<line>.*)"), s03_1, new TF(tf_getmodeminfo_manufacturer), new GF(gf_have_no_manufacturer) },
    { s03_1, new ModemEvent("(?<line>.*)"), s03_1, new TF(tf_getmodeminfo_model), new PG },
    { s03_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get IMEI*/
    { s03_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s03_2, new ModemEvent("^OK$"), s04_1, new swrite("at+cfun?", timer_at_cmd_cfun), new PG },
    { s03_2, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s03_2, new ModemEvent("(?<line>.*)"), s03_2, new TF(tf_getmodeminfo_imei), new PG },
    { s03_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s04_0: wait cfun state*/
    { s04_0, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s04_0, new DBusEvent("SetGprsAccess"), s04_0, new TF(tf_dbus_setgprsaccess_oninit), new PG },
    { s04_0, new DBusEvent("SetGprsAccess2"), s04_0, new TF(tf_dbus_setgprsaccess_oninit2), new PG },
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
    { s04_1, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s04_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s04_2: set modem full functionality*/
    { s04_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s04_2, new ModemEvent("^OK$"), s05_1, new TF(tf_init_network_state), new PG },
    { s04_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s04_0, new TF(tf_wait_cfun), new GF(gf_wait_cfun_state_limit) },
    { s04_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_init_err), new PG },
    { s04_2, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s04_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s05_1: get network state*/
    { s05_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s05_1, new ModemEvent("^OK$"), s05_2, new swrite("at+qcfg=\"rmnet/profileid\"", timer_at_cmd_short), new PG },
    { s05_1, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s05_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s05_2: GPRS init: get profile id*/
    { s05_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s05_2, new ModemEvent("^\\+QCFG: \"rmnet/profileid\",(?<id>[0-9]+)"), s05_2, new TF(tf_getprofileid), new PG },
    { s05_2, new ModemEvent("^OK$"), s05_3, new TF(tf_gprsprofile_get), new PG },
    //Note: ignore error, rmnet/profileid may be not supported by the modem, default profile id 1 is used in this case
    { s05_2, new ModemEvent("^ERROR$"), s05_3, new TF(tf_gprsprofile_get), new PG },
    { s05_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s05_3: GPRS init: get profile parameter*/
    { s05_3, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s05_3, new ModemEvent("^\\+QICSGP: (?<type>[0-9]+),\"(?<apn>.*)\",\"(?<user>.*)\",\"(?<pass>.*)\",(?<auth>[0-9]+)"), s05_3, new TF(tf_getprofileparams), new PG },
    { s05_3, new ModemEvent("^OK$"), s06_1, new swrite("at+qcfg=\"rmnet/autoconnect\"", timer_at_cmd_short), new PG },
    { s05_3, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s05_3, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get autoconnect state*/
    { s06_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s06_1, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",1,0,1"), s06_1, new TF(tf_do_nothing), new PG },
    { s06_1, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",(?<status>[0-9]),(?<linkprot>[0-9]),(?<dtrset>[0-9])"), s06_2, new TF(tf_do_nothing), new PG },
    { s06_1, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",1,0"), s06_1, new TF(tf_do_nothing), new PG }, //DTR set in qmiwwan driver
    { s06_1, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",(?<status>[0-9]),(?<linkprot>[0-9])"), s06_3, new TF(tf_do_nothing), new PG },
    { s06_1, new ModemEvent("^OK$"), s08, new TF(tf_query_simstate), new PG },
    { s06_1, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s06_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set autoconnect required*/
    { s06_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s06_2, new ModemEvent("^OK$"), s07, new swrite("at+qcfg=\"rmnet/autoconnect\",1,0,1", timer_at_cmd_short), new PG },
    { s06_2, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s06_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set autoconnect legacy required*/
    { s06_3, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s06_3, new ModemEvent("^OK$"), s07, new swrite("at+qcfg=\"rmnet/autoconnect\",1,0", timer_at_cmd_short), new PG },
    { s06_3, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s06_3, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set autoconnect*/
    { s07, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s07, new ModemEvent("^OK$"), x01, new TF(tf_modem_reconfig_restart), new PG },
    { s07, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s07, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get SIM state*/
    { s08, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s08, new ModemEvent("^\\+QCCID: (?<id>[0-9]+)"), s08, new TF(tf_getsim_iccid), new PG },
    { s08, new ModemEvent("^\\+QPINC: \"SC\",(?<s1>[0-9]+),(?<s2>[0-9]+)"), s08, new TF(tf_getsim_pincount), new PG },
    { s08, new ModemEvent("^OK$"), s14, new TF(tf_sim_ready_init_sms), new GF(gf_sim_ready) },
    { s08, new ModemEvent("^OK$"), s12_1, new TF(tf_getsimstate_setstoredpin), new GF(gf_set_stored_pin) },
    { s08, new ModemEvent("^OK$"), s11, new TF(tf_wait_puk), new GF(gf_puk_needed) },
    { s08, new ModemEvent("^OK$"), s10, new TF(tf_wait_pin), new GF(gf_pin_needed) },
    { s08, new ModemEvent("^OK$"), e01, new TF(tf_init_sim_err), new PG },
    //Note: SIM card may be busy when modem set to full functionality (at+cfun=1) quite recently
    { s08, new ModemEvent("^\\+CME ERROR: 14"), s09, new TF(tf_sim_busy), new PG },
    { s08, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_cme_error), new PG },
    { s08, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s08, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*wait SIM state*/
    { s09, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s09, new ModemEvent("^\\+CPIN: (?<state>.*)"), s13, new TF(tf_setpin_ok), new GF(gf_sim_ready) },
    { s09, new ModemEvent("^\\+CPIN: (?<state>.*)"), s08, new TF(tf_query_simstate), new PG },
    { s09, new DBusEvent("SetGprsAccess"), s09, new TF(tf_dbus_setgprsaccess_oninit), new PG },
    { s09, new DBusEvent("SetGprsAccess2"), s09, new TF(tf_dbus_setgprsaccess_oninit2), new PG },
    { s09, new DBusEvent("SetOper"), s09, new TF(tf_dbus_setoper_return_ok), new PG },
    { s09, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { s09, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { s09, new TimeoutEvent(), s08, new TF(tf_query_simstate), new PG },
    /*wait PIN*/
    { s10, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s10, new DBusEvent("SetGprsAccess"), s10, new TF(tf_dbus_setgprsaccess_oninit), new PG },
    { s10, new DBusEvent("SetGprsAccess2"), s10, new TF(tf_dbus_setgprsaccess_oninit2), new PG },
    { s10, new DBusEvent("SetOper"), s10, new TF(tf_dbus_setoper_return_ok), new PG },
    { s10, new DBusEvent("SetSimPin"), s12_2, new TF(tf_waitpin_dbus_setpin), new PG },
    { s10, new DBusEvent("SetSimPuk"), s12_2, new TF(tf_waitpin_dbus_setpuk), new PG },
    { s10, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { s10, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { s10, new TimeoutEvent(), s10, new TF(tf_void_timeout), new PG },
    /*wait PUK*/
    { s11, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s11, new DBusEvent("SetGprsAccess"), s11, new TF(tf_dbus_setgprsaccess_oninit), new PG },
    { s11, new DBusEvent("SetGprsAccess2"), s11, new TF(tf_dbus_setgprsaccess_oninit2), new PG },
    { s11, new DBusEvent("SetOper"), s11, new TF(tf_dbus_setoper_return_ok), new PG },
    { s11, new DBusEvent("SetSimPuk"), s12_2, new TF(tf_waitpin_dbus_setpuk), new PG },
    { s11, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { s11, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { s11, new TimeoutEvent(), s11, new TF(tf_void_timeout), new PG },
    /*set PIN (stored)*/
    { s12_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s12_1, new ModemEvent("^OK$"), s13, new TF(tf_setpin_ok), new PG },
    { s12_1, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_setpin_err), new PG },
    { s12_1, new ModemEvent("^ERROR$"), s08, new TF(tf_setpin_err), new PG },
    { s12_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set PIN (dbus)*/
    { s12_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s12_2, new ModemEvent("^OK$"), s13, new TF(tf_dbus_setpin_ok), new PG },
    { s12_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_setpin_cme_err), new PG },
    { s12_2, new ModemEvent("^ERROR$"), s08, new TF(tf_dbus_setpin_err), new PG },
    { s12_2, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*wait SMS / PB done*/
    { s13, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s13, new ModemEvent("^\\+QUSIM: (?<simtype>[0-9]+)"), s13, new TF(tf_do_nothing), new PG },
    { s13, new ModemEvent("^\\+QIND: SMS DONE"), s13, new TF(tf_do_nothing), new PG },
    { s13, new ModemEvent("^\\+QIND: PB DONE"), s14, new TF(tf_init_sms), new PG },
    { s13, new TimeoutEvent(), s14, new TF(tf_init_sms), new PG },
    /*init SMS*/
    { s14, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s14, new ModemEvent("^OK$"), s16_1, new TF(tf_gprsprofile_set), new GF(gf_gprsprofile_set_oninit) },
    { s14, new ModemEvent("^OK$"), s17, new swrite("at+copn", timer_at_cmd_copn), new GF(gf_have_no_operatornames) },
    { s14, new ModemEvent("^OK$"), s18, new TF(tf_initoper), new PG },
    { s14, new ModemEvent("^\\+CME ERROR: 3"), s13, new TF(tf_init_sms_not_allowed), new PG },
    { s14, new ModemEvent("^\\+CME ERROR: 14"), s13, new TF(tf_init_sms_sim_busy), new PG },
    { s14, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s14, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s16_1: GPRS init: set profile parameter*/
    { s16_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s16_1, new ModemEvent("^OK$"), s16_2, new TF(tf_gprs_detach), new PG },
    { s16_1, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s16_1, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*s16_2: GPRS init: detach PS*/
    { s16_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s16_2, new ModemEvent("^OK$"), s17, new TF(tf_gprs_detach_ok_getopernames), new GF(gf_have_no_operatornames) },
    { s16_2, new ModemEvent("^OK$"), s18, new TF(tf_gprs_detach_ok_initoper), new PG },
    { s16_2, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s16_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { s16_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_cme_error), new PG },
    { s16_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*get operator names*/
    { s17, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s17, new ModemEvent("^\\+COPN: \"(?<id>[0-9]+)\",\"(?<name>.*)\""), s17, new TF(tf_readoperatorname), new PG },
    { s17, new ModemEvent("^OK$"), s18, new TF(tf_initoper), new PG },
    { s17, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { s17, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_cme_error), new PG },
    { s17, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s17, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*init operator*/
    { s18, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { s18, new ModemEvent("^OK$"), o15_2, new TF(tf_query_reg_state_full), new PG },
    { s18, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { s18, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), e01, new TF(tf_cme_error), new PG },
    { s18, new ModemEvent("^ERROR$"), e01, new TF(tf_init_err), new PG },
    { s18, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*set operator*/
    { o01, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o01, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_setoper_ok), new PG },
    { o01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o01, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o01, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*wait user command*/
    { o04, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o04, new ModemEvent("^\\+CPIN: (?<state>.*)"), s08, new TF(tf_sim_not_ready), new GF(gf_sim_not_ready) },
    { o04, new DBusEvent("GetOperList"), o06_2, new TF(tf_getoperlist), new GF(gf_gprs_disabled) },
    { o04, new DBusEvent("GetOperList"), o06_1, new TF(tf_getoperlist_prepare), new PG },
    { o04, new DBusEvent("ListSms"), o10, new TF(tf_dbus_listsms), new PG },
    { o04, new DBusEvent("ReadSms"), o11, new TF(tf_dbus_readsms), new PG },
    { o04, new DBusEvent("SendSms"), o12, new TF(tf_dbus_sendsms_head), new PG },
    { o04, new DBusEvent("GetSmsStorage"), o13, new TF(tf_dbus_getsmsstorage), new PG },
    { o04, new DBusEvent("GetSmsReportConfig"), o14, new TF(tf_dbus_getsmsreportconfig), new PG },
    { o04, new DBusEvent("SetGprsAccess"), o16_1, new TF(tf_dbus_setgprsaccess), new PG },
    { o04, new DBusEvent("SetGprsAccess2"), o16_1, new TF(tf_dbus_setgprsaccess2), new PG },
    { o04, new DBusEvent("SetSmsStorage"), o05, new TF(tf_dbus_setsmsstorage), new PG },
    { o04, new DBusEvent("SetSmsReportConfig"), o05, new TF(tf_dbus_setsmsreport), new PG },
    { o04, new DBusEvent("DeleteSms"), o05, new TF(tf_dbus_deletesms), new PG },
    { o04, new DBusEvent("SetOper"), o01, new TF(tf_dbus_setoper), new PG },
    { o04, new DBusEvent("SetSimPin"), o04, new TF(tf_dbus_setpin_ready), new PG },
    { o04, new DBusEvent("SetSimPuk"), o04, new TF(tf_dbus_setpin_ready), new PG },
    { o04, new DBusEvent("ModemReset"), x01, new TF(tf_dbus_modemreset), new PG },
    { o04, new DBusEvent("SetPortState"), i01, new TF(tf_dbus_setportstate), new PG },
    { o04, new TimeoutEvent(), o15_2, new TF(tf_query_reg_state_full), new PG },
    /*common command*/
    { o05, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o05, new ModemEvent("^OK$"), o04, new TF(tf_dbus_return_ok), new PG },
    { o05, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o05, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o05, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o05, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*o06_1: get OPER list: prepare*/
    { o06_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o06_1, new ModemEvent("^OK$"), o06_2, new TF(tf_getoperlist_prepare_ok), new PG },
    { o06_1, new ModemEvent("ERROR"), o15_2, new TF(tf_getoperlist_fail), new PG },
    { o06_1, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*o06_2: get OPER list: list available networks*/
    { o06_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
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
    { o10, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o10, new ModemEvent("^\\+CMGL: (?<index>[0-9]+),(?<state>[0-9]+)"), o10, new TF(tf_listsms_next), new PG },
    { o10, new ModemEvent("^OK$"), o04, new TF(tf_dbus_listsms_ok), new PG },
    { o10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o10, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o10, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o10, new ModemEvent("(?<line>.*)"), o10, new TF(tf_sms_get_body), new PG },
    { o10, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*read SMS*/
    { o11, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o11, new ModemEvent("^\\+CMGR: (?<state>[0-9]+)"), o11, new TF(tf_readsms), new PG },
    { o11, new ModemEvent("^OK$"), o04, new TF(tf_dbus_readsms_ok), new PG },
    { o11, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o11, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o11, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o11, new ModemEvent("(?<line>.*)"), o11, new TF(tf_sms_get_body), new PG },
    { o11, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*send SMS*/
    { o12, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o12, new ModemEvent("^(?<prompt>[>])"), o12, new TF(tf_sendsms_body), new PG },
    { o12, new ModemEvent("^\\+CMGS: (?<msgref>[0-9]+)"), o12, new TF(tf_sendsms_result), new PG },
    { o12, new ModemEvent("^\\+CMGS: (?<msgref>[0-9]+)"), o12, new TF(tf_sendsms_result), new PG },
    { o12, new ModemEvent("^OK$"), o04, new TF(tf_dbus_sendsms_ok), new PG },
    { o12, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o12, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o12, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o12, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*get SMS storage*/
    { o13, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o13, new ModemEvent("^\\+CPMS: \"(?<mem1>.*)\",(?<used1>[0-9]+),(?<total1>[0-9]+),\"(?<mem2>.*)\",(?<used2>[0-9]+),(?<total2>[0-9]+),\"(?<mem3>.*)\",(?<used3>[0-9]+),(?<total3>[0-9]+)"), o13, new TF(tf_get_sms_storage), new PG },
    { o13, new ModemEvent("^OK$"), o04, new TF(tf_dbus_getsmsstorage_ok), new PG },
    { o13, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o13, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o13, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o13, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*get SMS event report config*/
    { o14, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o14, new ModemEvent("^\\+CNMI: (?<mode>[0-9]+),(?<mt>[0-9]+),(?<bm>[0-9]+),(?<ds>[0-9]+),(?<bfr>[0-9]+)"), o14, new TF(tf_get_smsreportconfig), new PG },
    { o14, new ModemEvent("^OK$"), o04, new TF(tf_dbus_getsmsreportconfig_ok), new PG },
    { o14, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o14, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o14, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o14, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*get network registration details (periodic)*/
    { o15_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o15_2, new ModemEvent("^OK$"), o04, new TF(tf_get_network_registration_ok), new PG },
    { o15_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cme_sim_failure) },
    { o15_2, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_query_simstate), new GF(gf_cms_sim_failure) },
    { o15_2, new ModemEvent("^ERROR$"), o04, new TF(tf_get_network_registration_error), new PG },
    { o15_2, new TimeoutEvent(), i01, new TF(tf_modem_timeout), new PG },
    /*o16_1: GPRS config: set profile parameter*/
    { o16_1, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o16_1, new ModemEvent("^OK$"), o16_2, new TF(tf_gprs_detach), new PG },
    { o16_1, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o16_1, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*o16_2: GPRS config: detach PS*/
    { o16_2, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { o16_2, new ModemEvent("^OK$"), o15_2, new TF(tf_dbus_setgprsaccess_ok), new PG },
    { o16_2, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cme_sim_failure) },
    { o16_2, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), s08, new TF(tf_dbus_return_sim_failure), new GF(gf_cms_sim_failure) },
    { o16_2, new ModemEvent("^ERROR$"), o04, new TF(tf_dbus_return_err), new PG },
    { o16_2, new TimeoutEvent(), i01, new TF(tf_dbus_modem_timeout), new PG },
    /*x01: trigger power down*/
    { x01, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { x01, new ModemEvent("^OK$"), x02, new TF(tf_powerdown_ok), new PG },
    { x01, new ModemEvent("^ERROR$"), i01, new TF(tf_powerdown_err), new PG },
    { x01, new TimeoutEvent(), i01, new TF(tf_powerdown_err), new PG },
    /*x02: wait power down*/
    { x02, new RegexEvent("ModemPort_IO_error"), e02, new TF(tf_io_error), new PG },
    { x02, new ModemEvent("^POWERED DOWN$"), i01, new TF(tf_powerdown_finish), new PG },
    { x02, new TimeoutEvent(), i01, new TF(tf_powerdown_timeout), new PG },
  };

  MdmStatemachine::action_list_t al {
    { new ModemEvent("^\\+CSQ: (?<rssi>[0-9]+),(?<ber>[0-9]+)"), new AF(af_read_csq) },
    { new ModemEvent("^\\+COPS: (?<mode>[0-9]+),2,\"(?<id>[0-9]+)\",(?<act>[0-9]+)"), new AF(af_read_cops)},
    { new ModemEvent("^\\+COPS: 0"), new AF(af_read_cops_clear)},
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
    { new DBusEvent("GetPortState"), new AF(af_dbus_getportstate) },
    { new DBusEvent("GetPortState2"), new AF(af_dbus_getportstate2) },
    { new DBusEvent("GetSimState"), new AF(af_dbus_getsimstate) },
    { new DBusEvent("GetGprsAccess"), new AF(af_dbus_getgprsaccess) },
    { new DBusEvent("GetGprsAccess2"), new AF(af_dbus_getgprsaccess2) },
    { new DBusEvent("GetModemInfo"), new AF(af_dbus_getmodeminfo) },
    { new DBusEvent("GetModemIdentity"), new AF(af_dbus_getmodemidentity) },
    { new DBusEvent("GetSmsFormat"), new AF(af_dbus_getsmsformat) },
    { new DBusEvent("SetSmsFormat"), new AF(af_dbus_setsmsformat) },
    { new DBusEvent("GetSignalQuality"), new AF(af_dbus_getsignalquality) },
    { new DBusEvent("GetVersion"), new AF(af_dbus_getversion) },
    { new DBusEvent("GetLogLevel"), new AF(af_dbus_getloglevel) },
    { new DBusEvent("SetLogLevel"), new AF(af_dbus_setloglevel) },
  };

    GThread* worker_thread = g_thread_new("mdm_cuse_worker",
                                          (GThreadFunc)mdm_cuse_worker_main,
                                          NULL);
    MdmStatemachine sm(tl, i01, al, MDM_GPIO__PWRKEY_PIN, MDM_MUSB_DRIVER_PORT );
    //trigger initial timeout for statemachine entry
    sm.do_timeout(0,false);

    ml = g_main_loop_new(0, true);
    g_main_loop_run(ml);

    g_thread_join(worker_thread);

    exit(EXIT_SUCCESS);
}

