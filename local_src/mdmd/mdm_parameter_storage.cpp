// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "mdm_config_types.h"
#include <boost/optional.hpp>
#include <string>
#include "mdm_parameter_storage.h"
#include "parameter_storage.h"

constexpr auto KEY_LOG_LEVEL =               "log_level";
constexpr auto KEY_PORT_STATE =              "port_state";
constexpr auto KEY_SIM_ICCID =               "sim_iccid";
constexpr auto KEY_SIM_PIN =                 "sim_pin";
constexpr auto KEY_NET_SELECTION_MODE =      "net_selection_mode";
constexpr auto KEY_NET_MANUAL_OPER =         "net_manual_oper";
constexpr auto KEY_NET_MANUAL_ACT =          "net_manual_act";
constexpr auto KEY_NET_AUTOSELECT_SCANMODE = "net_autoselect_scanmode";
constexpr auto KEY_NET_AUTOSELECT_SCANSEQ =  "net_autoselect_scanseq";
constexpr auto KEY_GPRS_STATE =              "gprs_state";
constexpr auto KEY_GPRS_APN =                "gprs_apn";
constexpr auto KEY_GPRS_AUTH =               "gprs_auth_type";
constexpr auto KEY_GPRS_USER =               "gprs_username";
constexpr auto KEY_GPRS_PASS =               "gprs_password";
constexpr auto KEY_SMS_CNMI_MODE =           "sms_cnmi_mode";
constexpr auto KEY_SMS_CNMI_MT =             "sms_cnmi_mt";
constexpr auto KEY_SMS_CNMI_BM =             "sms_cnmi_bm";
constexpr auto KEY_SMS_CNMI_DS =             "sms_cnmi_ds";
constexpr auto KEY_SMS_CNMI_BFR =            "sms_cnmi_bfr";
constexpr auto KEY_SMS_CPMS_MEM1 =           "sms_cpms_mem1";
constexpr auto KEY_SMS_CPMS_MEM2 =           "sms_cpms_mem2";
constexpr auto KEY_SMS_CPMS_MEM3 =           "sms_cpms_mem3";

ModemManagementConfig MdmParameterStorage::get_modem_management_config() const
{
    ModemManagementConfig stored_config;

    constexpr auto LOG_LEVEL_ERR_WRN_INF = 3;  
    boost::optional<const int> logLevel = _storage.get_integer(KEY_LOG_LEVEL);
    stored_config.set_log_level(logLevel ? *logLevel : LOG_LEVEL_ERR_WRN_INF);

    constexpr auto PORT_STATE_ENABLED = 1;  
    boost::optional<const int> portState= _storage.get_integer(KEY_PORT_STATE);
    stored_config.set_port_state(portState ? *portState : PORT_STATE_ENABLED);

    return stored_config;
}

void MdmParameterStorage::set_modem_management_config(const ModemManagementConfig &new_config)
{
    _storage.set_integer(KEY_LOG_LEVEL, new_config.get_log_level());
    _storage.set_integer(KEY_PORT_STATE, new_config.get_port_state());
    _storage.store();
}

SimAutoActivation MdmParameterStorage::get_sim_autoactivation() const
{
    SimAutoActivation stored_config;

    constexpr auto DEFAULT_SIM_ICCID = "";
    boost::optional<const std::string> iccid = _storage.get_string(KEY_SIM_ICCID);
    stored_config.set_iccid(iccid ? *iccid : DEFAULT_SIM_ICCID);

    constexpr auto DEFAULT_SIM_PIN = "";
    boost::optional<const std::string> simpin = _storage.get_string(KEY_SIM_PIN);
    stored_config.set_pin(simpin ? *simpin: DEFAULT_SIM_PIN);

    return stored_config;
}

void MdmParameterStorage::set_sim_autoactivation(const SimAutoActivation &new_config)
{
    _storage.set_string(KEY_SIM_ICCID, new_config.get_iccid());
    _storage.set_string(KEY_SIM_PIN, new_config.get_pin());
    _storage.store();
}

NetworkAccessConfig MdmParameterStorage::get_network_access_config() const
{
    NetworkAccessConfig stored_config;

    constexpr auto NET_SELECTION_MODE_AUTOMATIC =  0; //0=AUTOMATIC, 1=MANUAL
    boost::optional<const int> selection_mode = _storage.get_integer(KEY_NET_SELECTION_MODE);
    stored_config.set_selection_mode(selection_mode ? *selection_mode : NET_SELECTION_MODE_AUTOMATIC);

    constexpr auto DEFAULT_NET_MANUAL_OPER_NONE = 0; //0=NONE, other values see 3GPP TS 27.007
    boost::optional<const int> manual_oper = _storage.get_integer(KEY_NET_MANUAL_OPER);
    stored_config.set_manual_oper(manual_oper ? *manual_oper : DEFAULT_NET_MANUAL_OPER_NONE);

    constexpr auto DEFAULT_NET_MANUAL_ACT_GSM = 0; //0=GSM, 2=UMTS, other values see 3GPP TS 27.007
    boost::optional<const int> manual_act = _storage.get_integer(KEY_NET_MANUAL_ACT);
    stored_config.set_manual_act(manual_act ? *manual_act : DEFAULT_NET_MANUAL_ACT_GSM);

    constexpr auto NET_AUTOSELECT_SCANMODE_AUTO = 0; //0=AUTO, 1=GSM_ONLY, 2=UMTS_ONLY
    boost::optional<const int> autoselect_scanmode = _storage.get_integer(KEY_NET_AUTOSELECT_SCANMODE);
    stored_config.set_autoselect_scanmode(autoselect_scanmode ? *autoselect_scanmode : NET_AUTOSELECT_SCANMODE_AUTO);

    constexpr auto NET_AUTOSELECT_SCANSEQ_AUTO = 0; //0=AUTO, 1=GSM_PRIOR, 2=UMTS_PRIOR
    boost::optional<const int> autoselect_scanseq = _storage.get_integer(KEY_NET_AUTOSELECT_SCANSEQ);
    stored_config.set_autoselect_scanseq(autoselect_scanseq ? *autoselect_scanseq : NET_AUTOSELECT_SCANSEQ_AUTO);

    return stored_config;
}

void MdmParameterStorage::set_network_access_config(const NetworkAccessConfig &new_config)
{
    _storage.set_integer(KEY_NET_SELECTION_MODE, new_config.get_selection_mode());
    _storage.set_integer(KEY_NET_MANUAL_OPER, new_config.get_manual_oper());
    _storage.set_integer(KEY_NET_MANUAL_ACT, new_config.get_manual_act());
    _storage.set_integer(KEY_NET_AUTOSELECT_SCANMODE, new_config.get_autoselect_scanmode());
    _storage.set_integer(KEY_NET_AUTOSELECT_SCANSEQ, new_config.get_autoselect_scanseq());
    _storage.store();
}

GprsAccessConfig MdmParameterStorage::get_gprs_access_config() const
{
    GprsAccessConfig stored_config;

    //variable parameters:
    constexpr auto GPRS_STATE_ENABLED = 1; 
    boost::optional<const int> state = _storage.get_integer(KEY_GPRS_STATE);
    stored_config.set_state(state ? *state : GPRS_STATE_ENABLED);

    constexpr auto DEFAULT_GPRS_APN = "";
    boost::optional<const std::string> apn = _storage.get_string(KEY_GPRS_APN);
    stored_config.set_apn(apn ? *apn : DEFAULT_GPRS_APN);

    constexpr auto DEFAULT_GPRS_USER = "";
    boost::optional<const std::string> user = _storage.get_string(KEY_GPRS_USER);
    stored_config.set_user(user ? *user : DEFAULT_GPRS_USER);

    constexpr auto DEFAULT_GPRS_PASS = "";
    boost::optional<const std::string> pass = _storage.get_string(KEY_GPRS_PASS);
    stored_config.set_pass(pass ? * pass : DEFAULT_GPRS_PASS);

    constexpr auto GPRS_NO_AUTH = 0;  
    boost::optional<const int> auth = _storage.get_integer(KEY_GPRS_AUTH);
    stored_config.set_auth(auth ? *auth : GPRS_NO_AUTH);

    //constant parameters:
    constexpr auto DEFAULT_PROFILE = 1;
    stored_config.set_profile(DEFAULT_PROFILE);

    return stored_config;
}

void MdmParameterStorage::set_gprs_access_config(const GprsAccessConfig &new_config)
{
    _storage.set_integer(KEY_GPRS_STATE, new_config.get_state());
    _storage.set_string(KEY_GPRS_APN, new_config.get_apn());
    _storage.set_string(KEY_GPRS_USER, new_config.get_user());
    _storage.set_string(KEY_GPRS_PASS, new_config.get_pass());
    _storage.set_integer(KEY_GPRS_AUTH, new_config.get_auth());
    _storage.store();
}

SmsEventReportingConfig MdmParameterStorage::get_sms_event_reporting_config() const
{
    SmsEventReportingConfig stored_config;

    constexpr auto BUFFER_MSG_IN_TA = 0;  
    boost::optional<const int> mode = _storage.get_integer(KEY_SMS_CNMI_MODE);
    stored_config.set_mode(mode ? *mode : BUFFER_MSG_IN_TA);

    constexpr auto STORE_MSG_WITHOUT_INDICATION = 0;  
    boost::optional<const int> mt = _storage.get_integer(KEY_SMS_CNMI_MT);
    stored_config.set_mt(mt ? *mt : STORE_MSG_WITHOUT_INDICATION);

    constexpr auto NO_INIDICATION_FOR_BROADCAST_MSG = 0;  
    boost::optional<const int> bm = _storage.get_integer(KEY_SMS_CNMI_BM);
    stored_config.set_bm(bm ? *bm : NO_INIDICATION_FOR_BROADCAST_MSG);

    constexpr auto NO_INDICATION_FOR_STATUS = 0;  
    boost::optional<const int> ds = _storage.get_integer(KEY_SMS_CNMI_DS);
    stored_config.set_ds(ds ? *ds : NO_INDICATION_FOR_STATUS);

    constexpr auto KEEP_MSG_IN_BFR = 0;  
    boost::optional<const int> bfr = _storage.get_integer(KEY_SMS_CNMI_BFR);
    stored_config.set_bfr(bfr ? *bfr : KEEP_MSG_IN_BFR);

    return stored_config;
}

void MdmParameterStorage::set_sms_event_reporting_config(const SmsEventReportingConfig &new_config)
{
    _storage.set_integer(KEY_SMS_CNMI_MODE, new_config.get_mode());
    _storage.set_integer(KEY_SMS_CNMI_MT, new_config.get_mt());
    _storage.set_integer(KEY_SMS_CNMI_BM, new_config.get_bm());
    _storage.set_integer(KEY_SMS_CNMI_DS, new_config.get_ds());
    _storage.set_integer(KEY_SMS_CNMI_BFR, new_config.get_bfr());
    _storage.store();
}

SmsStorageConfig MdmParameterStorage::get_sms_storage_config() const
{
    SmsStorageConfig stored_config;
    constexpr auto STORE_SMS_ON_SIM = "SM"; 

    boost::optional<const std::string> mem1 = _storage.get_string(KEY_SMS_CPMS_MEM1);
    stored_config.set_mem1(mem1 ? *mem1 : STORE_SMS_ON_SIM);

    boost::optional<const std::string> mem2 = _storage.get_string(KEY_SMS_CPMS_MEM2);
    stored_config.set_mem2(mem2 ? *mem2 : STORE_SMS_ON_SIM);

    boost::optional<const std::string> mem3 = _storage.get_string(KEY_SMS_CPMS_MEM3);
    stored_config.set_mem3(mem3 ? *mem3 : STORE_SMS_ON_SIM);

    return stored_config;
}

void MdmParameterStorage::set_sms_storage_config(const SmsStorageConfig &new_config)
{
    _storage.set_string(KEY_SMS_CPMS_MEM1, new_config.get_mem1());
    _storage.set_string(KEY_SMS_CPMS_MEM2, new_config.get_mem2());
    _storage.set_string(KEY_SMS_CPMS_MEM3, new_config.get_mem3());
    _storage.store();
}

MessageServiceConfig MdmParameterStorage::get_message_service_config() const
{
    MessageServiceConfig stored_config;

    //constant parameters:
    constexpr auto DEFAULT_SMS_FORMAT = 0; //0=PDU, 1=TEXT
    stored_config.set_sms_format(DEFAULT_SMS_FORMAT);

    return stored_config;
}
