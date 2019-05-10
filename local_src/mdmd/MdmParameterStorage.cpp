// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "MdmParameterStorage.h"
#include "ParameterStorage.h"
#include "mdm_config_types.h"
#include <boost/optional.hpp>
#include <string>

constexpr auto KEY_LOG_LEVEL =               "log_level";
constexpr auto KEY_PORT_STATE =              "port_state";
constexpr auto KEY_SIM_ICCID =               "sim_iccid";
constexpr auto KEY_SIM_PIN =                 "sim_pin";
constexpr auto KEY_NET_SELECTION_MODE =      "net_selection_mode";
constexpr auto KEY_NET_MANUAL_OPER =         "net_manual_oper";
constexpr auto KEY_NET_MANUAL_ACT =          "net_manual_act";
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

ModemManagementConfig MdmParameterStorage::getModemManagementConfig()
{
    ModemManagementConfig storedConfig;

    constexpr auto LOG_LEVEL_ERR_WRN_INF = 3;  
    boost::optional<const int> logLevel = _storage.get_integer(KEY_LOG_LEVEL);
    storedConfig.set_log_level(logLevel ? *logLevel : LOG_LEVEL_ERR_WRN_INF);

    constexpr auto PORT_STATE_ENABLED = 1;  
    boost::optional<const int> portState= _storage.get_integer(KEY_PORT_STATE);
    storedConfig.set_port_state(portState ? *portState : PORT_STATE_ENABLED);

    return storedConfig;
}

void MdmParameterStorage::setModemManagementConfig(const ModemManagementConfig &newConfig)
{
    _storage.set_integer(KEY_LOG_LEVEL, newConfig.get_log_level());
    _storage.set_integer(KEY_PORT_STATE, newConfig.get_port_state());
    _storage.store();
}

UserIdentityConfig MdmParameterStorage::getUserIdentityConfig()
{
    UserIdentityConfig storedConfig;

    constexpr auto DEFAULT_SIM_ICCID = "";
    boost::optional<const std::string> iccid = _storage.get_string(KEY_SIM_ICCID);
    storedConfig.set_iccid(iccid ? *iccid : DEFAULT_SIM_ICCID);

    constexpr auto DEFAULT_SIM_PIN = "";
    boost::optional<const std::string> simpin = _storage.get_string(KEY_SIM_PIN);
    storedConfig.set_pin(simpin ? *simpin: DEFAULT_SIM_PIN);

    return storedConfig;
}

void MdmParameterStorage::setUserIdentityConfig(const UserIdentityConfig &newConfig)
{
    _storage.set_string(KEY_SIM_ICCID, newConfig.get_iccid());
    _storage.set_string(KEY_SIM_PIN, newConfig.get_pin());
    _storage.store();
}

NetworkAccessConfig MdmParameterStorage::getNetworkAccessConfig()
{
    NetworkAccessConfig storedConfig;

    constexpr auto NET_SELECTION_MODE_AUTOMATIC =  0;  
    boost::optional<const int> mode = _storage.get_integer(KEY_NET_SELECTION_MODE);
    storedConfig.set_mode(mode ? *mode : NET_SELECTION_MODE_AUTOMATIC);

    constexpr auto DEFAULT_NET_MANUAL_OPER = 0;
    boost::optional<const int> oper = _storage.get_integer(KEY_NET_MANUAL_OPER);
    storedConfig.set_oper(oper ? *oper : DEFAULT_NET_MANUAL_OPER);

    constexpr auto DEFAULT_NET_MANUAL_ACT = 0;
    boost::optional<const int> act = _storage.get_integer(KEY_NET_MANUAL_ACT);
    storedConfig.set_act(act ? *act : DEFAULT_NET_MANUAL_ACT);

    return storedConfig;
}

void MdmParameterStorage::setNetworkAccessConfig(const NetworkAccessConfig &newConfig)
{
    _storage.set_integer(KEY_NET_SELECTION_MODE, newConfig.get_mode());
    _storage.set_integer(KEY_NET_MANUAL_OPER, newConfig.get_oper());
    _storage.set_integer(KEY_NET_MANUAL_ACT, newConfig.get_act());
    _storage.store();
}

GprsAccessConfig MdmParameterStorage::getGprsAccessConfig()
{
    GprsAccessConfig storedConfig;

    constexpr auto GPRS_STATE_ENABLED = 1; 
    boost::optional<const int> state = _storage.get_integer(KEY_GPRS_STATE);
    storedConfig.set_state(state ? *state : GPRS_STATE_ENABLED);

    constexpr auto DEFAULT_GPRS_APN = "";
    boost::optional<const std::string> apn = _storage.get_string(KEY_GPRS_APN);
    storedConfig.set_apn(apn ? *apn : DEFAULT_GPRS_APN);

    constexpr auto DEFAULT_GPRS_USER = "";
    boost::optional<const std::string> user = _storage.get_string(KEY_GPRS_USER);
    storedConfig.set_user(user ? *user : DEFAULT_GPRS_USER);

    constexpr auto DEFAULT_GPRS_PASS = "";
    boost::optional<const std::string> pass = _storage.get_string(KEY_GPRS_PASS);
    storedConfig.set_pass(pass ? * pass : DEFAULT_GPRS_PASS);

    constexpr auto GPRS_NO_AUTH = 0;  
    boost::optional<const int> auth = _storage.get_integer(KEY_GPRS_AUTH);
    storedConfig.set_auth(auth ? *auth : GPRS_NO_AUTH);

    return storedConfig;
}

void MdmParameterStorage::setGprsAccessConfig(const GprsAccessConfig &newConfig)
{
    _storage.set_integer(KEY_GPRS_STATE, newConfig.get_state());
    _storage.set_string(KEY_GPRS_APN, newConfig.get_apn());
    _storage.set_string(KEY_GPRS_USER, newConfig.get_user());
    _storage.set_string(KEY_GPRS_PASS, newConfig.get_pass());
    _storage.set_integer(KEY_GPRS_AUTH, newConfig.get_auth());
    _storage.store();
}

SmsEventReportingConfig MdmParameterStorage::getSmsEventReportingConfig()
{
    SmsEventReportingConfig storedConfig;

    constexpr auto BUFFER_MSG_IN_TA = 0;  
    boost::optional<const int> mode = _storage.get_integer(KEY_SMS_CNMI_MODE);
    storedConfig.set_mode(mode ? *mode : BUFFER_MSG_IN_TA);

    constexpr auto STORE_MSG_WITHOUT_INDICATION = 0;  
    boost::optional<const int> mt = _storage.get_integer(KEY_SMS_CNMI_MT);
    storedConfig.set_mt(mt ? *mt : STORE_MSG_WITHOUT_INDICATION);

    constexpr auto NO_INIDICATION_FOR_BROADCAST_MSG = 0;  
    boost::optional<const int> bm = _storage.get_integer(KEY_SMS_CNMI_BM);
    storedConfig.set_bm(bm ? *bm : NO_INIDICATION_FOR_BROADCAST_MSG);

    constexpr auto NO_INDICATION_FOR_STATUS = 0;  
    boost::optional<const int> ds = _storage.get_integer(KEY_SMS_CNMI_DS);
    storedConfig.set_ds(ds ? *ds : NO_INDICATION_FOR_STATUS);

    constexpr auto KEEP_MSG_IN_BFR = 0;  
    boost::optional<const int> bfr = _storage.get_integer(KEY_SMS_CNMI_BFR);
    storedConfig.set_bfr(bfr ? *bfr : KEEP_MSG_IN_BFR);

    return storedConfig;
}

void MdmParameterStorage::setSmsEventReportingConfig(const SmsEventReportingConfig &newConfig)
{
    _storage.set_integer(KEY_SMS_CNMI_MODE, newConfig.get_mode());
    _storage.set_integer(KEY_SMS_CNMI_MT, newConfig.get_mt());
    _storage.set_integer(KEY_SMS_CNMI_BM, newConfig.get_bm());
    _storage.set_integer(KEY_SMS_CNMI_DS, newConfig.get_ds());
    _storage.set_integer(KEY_SMS_CNMI_BFR, newConfig.get_bfr());
    _storage.store();
}

SmsStorageConfig MdmParameterStorage::getSmsStorageConfig()
{
    SmsStorageConfig storedConfig;
    constexpr auto STORE_SMS_ON_SIM = "SM"; 

    boost::optional<const std::string> mem1 = _storage.get_string(KEY_SMS_CPMS_MEM1);
    storedConfig.set_mem1(mem1 ? *mem1 : STORE_SMS_ON_SIM);

    boost::optional<const std::string> mem2 = _storage.get_string(KEY_SMS_CPMS_MEM2);
    storedConfig.set_mem2(mem2 ? *mem2 : STORE_SMS_ON_SIM);

    boost::optional<const std::string> mem3 = _storage.get_string(KEY_SMS_CPMS_MEM3);
    storedConfig.set_mem3(mem3 ? *mem3 : STORE_SMS_ON_SIM);

    return storedConfig;
}

void MdmParameterStorage::setSmsStorageConfig(const SmsStorageConfig &newConfig)
{
    _storage.set_string(KEY_SMS_CPMS_MEM1, newConfig.get_mem1());
    _storage.set_string(KEY_SMS_CPMS_MEM2, newConfig.get_mem2());
    _storage.set_string(KEY_SMS_CPMS_MEM3, newConfig.get_mem3());
    _storage.store();
}
