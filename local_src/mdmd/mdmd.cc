// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "mdm_config_types.h"
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
#include "gkeyfile_storage.h"

GMainLoop *ml;

using namespace std::string_literals;

namespace {

/// statemachine timer in milliseconds
constexpr auto timer_at_cmd_copn = 125000; //depends on the number of operator names
constexpr auto timer_at_cmd_cops = 185000; //for network selection that is determined by the network
constexpr auto timer_at_cmd_sms = 125000;  //for sms transmission that is determined by the network
constexpr auto timer_at_cmd_cfun = 125000; //15s specified into UC20 AT command manual but 120s timeout with R04A07
constexpr auto timer_at_cmd_short = 5000;  //for commands without mobile network dependency
constexpr auto timer_dbus_retry = 5000;
constexpr auto timer_wait_port = 10000;
constexpr auto timer_wait_pb_done = 15000;
constexpr auto timer_wait_cfun = 10000;
constexpr auto timer_poll_simstate = 10000;
constexpr auto timer_sim_busy = 60000;
constexpr auto timer_wait_network_service = 20000; //wait time for operator selection retry when service is unavailable
constexpr auto timer_retry_network_access = 1000;  //wait time for operator selection retry after network timeout
constexpr auto timer_status_query = 10000;  //for periodic status queries
constexpr auto timer_immediate = 10;
constexpr auto timer_power_down = 65000;    //the maximum time for unregistering network is 60 seconds
/// timer are currently not in used
/// set as comment to keep the values in the source code
//constexpr auto timer_wait_suspendauto = 65000;
//constexpr auto timer_wait_ps_detach = 65000;

constexpr auto MDM_GPIO__PWRKEY_PIN = 132;
constexpr auto MDM_MUSB_DRIVER_PORT = 1;
constexpr auto MAX_PORT_WAIT_COUNT = 3;  /*number of wait cycles before automatic modem reset*/
constexpr auto MAX_CFUN_WAIT_COUNT = 12;  /*number of wait cycles before automatic modem reset*/
constexpr auto MAX_SIMINIT_WAIT_COUNT = 3;  /*number of wait cycles before init failure*/

constexpr auto MAX_GPRS_AUTOCONNECT_COUNT = 30; /*number of status update periods for autoconnect*/

///constexpr is currently not in used
///set as comment to keep the value in  the source code
//constexpr auto MDM_GPIO__RESET_PIN = 130;
//constexpr auto MAX_AT_CMD_PRINT_LENGTH = 128;

constexpr auto MDMD_CONFIG_FILE_GROUP = "DEFAULT";
constexpr auto MDMD_CONFIG_FILE_PATH =  "/etc/specific/mdmd_init.conf";

enum class CME_ERROR 
{
    PHONE_FAILURE = 0,
    NO_CONNECTION_TO_PHONE = 1,
    PHONE_LINK_RESERVED = 2,
    OPERATION_NOT_ALLOWED = 3,
    OPERATION_NOT_SUPPORTED = 4,
    PH_SIM_PIN_REQUIRED = 5,
    PH_FSIM_PIN_REQUIRED = 6,
    PH_FSIM_PUK_REQUIRED = 7,
    SIM_NOT_INSERTED = 10,
    SIM_PIN_REQUIRED = 11,
    SIM_PUK_REQUIRED = 12,
    SIM_FAILURE = 13,     
    SIM_BUSY = 14,        
    SIM_WRONG = 15,       
    INCORRECT_PASSWORD = 16,
    SIM_PIN2_REQUIRED = 17,
    SIM_PUK2_REQUIRED = 18,
    MEMORY_FULL = 20,
    INVALID_INDEX = 21,
    NOT_FOUND = 22,
    MEMORY_FAILURE = 23,
    TEXT_STRING_TOO_LONG = 24,
    INVALID_CHARACTERS_IN_TEXT_STRING = 25,
    DIAL_STRING_TOO_LONG = 26,
    INVALID_CHARACTERS_IN_DIAL_STRING = 27,
    NO_NETWORK_SERVICE = 30,
    NETWORK_TIMEOUT = 31,
    EMERGENCY_CALLS_ONLY = 32,
    NETWORK_PIN_REQUIRED = 40,
    NETWORK_PUK_REQUIRED = 41,
    NETWORK_SUBSET_PIN_REQUIRED = 42,
    NETWORK_SUBSET_PUK_REQUIRED = 43,
    SERVICE_PROVIDER_PIN_REQUIRED = 44,
    SERVICE_PROVIDER_PUK_REQUIRED = 45,
    CORPORATE_PIN_REQUIRED = 46,
    CORPORATE_PUK_REQUIRED = 47,
};

std::string cme_to_string( int err_id )
{
    switch (err_id)
    {
      case static_cast<int>(CME_ERROR::PHONE_FAILURE):                     return "PHONE_FAILURE";
      case static_cast<int>(CME_ERROR::NO_CONNECTION_TO_PHONE):            return "NO_CONNECTION_TO_PHONE";
      case static_cast<int>(CME_ERROR::PHONE_LINK_RESERVED):               return "PHONE_LINK_RESERVED";
      case static_cast<int>(CME_ERROR::OPERATION_NOT_ALLOWED):             return "OPERATION_NOT_ALLOWED";
      case static_cast<int>(CME_ERROR::OPERATION_NOT_SUPPORTED):           return "OPERATION_NOT_SUPPORTED";
      case static_cast<int>(CME_ERROR::PH_SIM_PIN_REQUIRED):               return "PH_SIM_PIN_REQUIRED";
      case static_cast<int>(CME_ERROR::PH_FSIM_PIN_REQUIRED):              return "PH_FSIM_PIN_REQUIRED";
      case static_cast<int>(CME_ERROR::PH_FSIM_PUK_REQUIRED):              return "PH_FSIM_PUK_REQUIRED";
      case static_cast<int>(CME_ERROR::SIM_NOT_INSERTED):                  return "SIM_NOT_INSERTED";
      case static_cast<int>(CME_ERROR::SIM_PIN_REQUIRED):                  return "SIM_PIN_REQUIRED";
      case static_cast<int>(CME_ERROR::SIM_PUK_REQUIRED):                  return "SIM_PUK_REQUIRED";
      case static_cast<int>(CME_ERROR::SIM_FAILURE):                       return "SIM_FAILURE";
      case static_cast<int>(CME_ERROR::SIM_BUSY):                          return "SIM_BUSY";
      case static_cast<int>(CME_ERROR::SIM_WRONG):                         return "SIM_WRONG";
      case static_cast<int>(CME_ERROR::INCORRECT_PASSWORD):                return "INCORRECT_PASSWORD";
      case static_cast<int>(CME_ERROR::SIM_PIN2_REQUIRED):                 return "SIM_PIN2_REQUIRED";
      case static_cast<int>(CME_ERROR::SIM_PUK2_REQUIRED):                 return "SIM_PUK2_REQUIRED";
      case static_cast<int>(CME_ERROR::MEMORY_FULL):                       return "MEMORY_FULL";
      case static_cast<int>(CME_ERROR::INVALID_INDEX):                     return "INVALID_INDEX";
      case static_cast<int>(CME_ERROR::NOT_FOUND):                         return "NOT_FOUND";
      case static_cast<int>(CME_ERROR::MEMORY_FAILURE):                    return "MEMORY_FAILURE";
      case static_cast<int>(CME_ERROR::TEXT_STRING_TOO_LONG):              return "TEXT_STRING_TOO_LONG";
      case static_cast<int>(CME_ERROR::INVALID_CHARACTERS_IN_TEXT_STRING): return "INVALID_CHARACTERS_IN_TEXT_STRING";
      case static_cast<int>(CME_ERROR::DIAL_STRING_TOO_LONG):              return "DIAL_STRING_TOO_LONG";
      case static_cast<int>(CME_ERROR::INVALID_CHARACTERS_IN_DIAL_STRING): return "INVALID_CHARACTERS_IN_DIAL_STRING";
      case static_cast<int>(CME_ERROR::NO_NETWORK_SERVICE):                return "NO_NETWORK_SERVICE";
      case static_cast<int>(CME_ERROR::NETWORK_TIMEOUT):                   return "NETWORK_TIMEOUT";
      case static_cast<int>(CME_ERROR::EMERGENCY_CALLS_ONLY):              return "EMERGENCY_CALLS_ONLY";
      case static_cast<int>(CME_ERROR::NETWORK_PIN_REQUIRED):              return "NETWORK_PIN_REQUIRED";
      case static_cast<int>(CME_ERROR::NETWORK_PUK_REQUIRED):              return "NETWORK_PUK_REQUIRED";
      case static_cast<int>(CME_ERROR::NETWORK_SUBSET_PIN_REQUIRED):       return "NETWORK_SUBSET_PIN_REQUIRED";
      case static_cast<int>(CME_ERROR::NETWORK_SUBSET_PUK_REQUIRED):       return "NETWORK_SUBSET_PUK_REQUIRED";
      case static_cast<int>(CME_ERROR::SERVICE_PROVIDER_PIN_REQUIRED):     return "SERVICE_PROVIDER_PIN_REQUIRED";
      case static_cast<int>(CME_ERROR::SERVICE_PROVIDER_PUK_REQUIRED):     return "SERVICE_PROVIDER_PUK_REQUIRED";
      case static_cast<int>(CME_ERROR::CORPORATE_PIN_REQUIRED):            return "CORPORATE_PIN_REQUIRED";
      case static_cast<int>(CME_ERROR::CORPORATE_PUK_REQUIRED):            return "CORPORATE_PUK_REQUIRED";
      default:
        return std::to_string(err_id);
    }
}

enum class CMS_ERROR 
{
    ME_FAILURE = 300,
    SMS_ME_RESERVED = 301,
    OPERATION_NOT_ALLOWED = 302,
    OPERATION_NOT_SUPPORTED = 303,
    INVALID_PDU_MODE = 304,
    INVALID_TEXT_MODE = 305,
    SIM_NOT_INSERTED = 310,
    SIM_PIN_REQUIRED = 311,
    PH_SIM_PIN_REQUIRED = 312,
    SIM_FAILURE = 313,
    SIM_BUSY = 314,
    SIM_WRONG = 315,
    SIM_PUK_REQUIRED = 316,
    SIM_PIN2_REQUIRED = 317,
    SIM_PUK2_REQUIRED = 318,
    MEMORY_FAILURE = 320,
    INVALID_MEMORY_INDEX = 321,
    MEMORY_FULL = 322,
    SMSC_ADDRESS_UNKNOWN = 330,
    NO_NETWORK = 331,
    NETWORK_TIMEOUT = 332,
    UNKNOWN = 500,
    SIM_NOT_READY = 512,
    MESSAGE_LENGTH_EXCEEDS = 513,
    INVALID_REQUEST_PARAMETERS = 514,
    ME_STORAGE_FAILURE = 515,
    INVALID_SERVICE_MODE = 517,
    MORE_MESSAGE_TO_SEND_STATE_ERROR = 528,
    MO_SMS_IS_NOT_ALLOW = 529,
    GPRS_IS_SUSPENDED = 530,
    ME_STORAGE_FULL = 531,
};

std::string cms_to_string( int err_id )
{
    switch (err_id)
    {
      case static_cast<int>(CMS_ERROR::ME_FAILURE):                        return "ME_FAILURE";
      case static_cast<int>(CMS_ERROR::SMS_ME_RESERVED):                   return "SMS_ME_RESERVED";
      case static_cast<int>(CMS_ERROR::OPERATION_NOT_ALLOWED):             return "OPERATION_NOT_ALLOWED";
      case static_cast<int>(CMS_ERROR::OPERATION_NOT_SUPPORTED):           return "OPERATION_NOT_SUPPORTED";
      case static_cast<int>(CMS_ERROR::INVALID_PDU_MODE):                  return "INVALID_PDU_MODE";
      case static_cast<int>(CMS_ERROR::INVALID_TEXT_MODE):                 return "INVALID_TEXT_MODE";
      case static_cast<int>(CMS_ERROR::SIM_NOT_INSERTED):                  return "SIM_NOT_INSERTED";
      case static_cast<int>(CMS_ERROR::SIM_PIN_REQUIRED):                  return "SIM_PIN_REQUIRED";
      case static_cast<int>(CMS_ERROR::PH_SIM_PIN_REQUIRED):               return "PH_SIM_PIN_REQUIRED";
      case static_cast<int>(CMS_ERROR::SIM_FAILURE):                       return "SIM_FAILURE";
      case static_cast<int>(CMS_ERROR::SIM_BUSY):                          return "SIM_BUSY";
      case static_cast<int>(CMS_ERROR::SIM_WRONG):                         return "SIM_WRONG";
      case static_cast<int>(CMS_ERROR::SIM_PUK_REQUIRED):                  return "SIM_PUK_REQUIRED";
      case static_cast<int>(CMS_ERROR::SIM_PIN2_REQUIRED):                 return "SIM_PIN2_REQUIRED";
      case static_cast<int>(CMS_ERROR::SIM_PUK2_REQUIRED):                 return "SIM_PUK2_REQUIRED";
      case static_cast<int>(CMS_ERROR::MEMORY_FAILURE):                    return "MEMORY_FAILURE";
      case static_cast<int>(CMS_ERROR::INVALID_MEMORY_INDEX):              return "INVALID_MEMORY_INDEX";
      case static_cast<int>(CMS_ERROR::MEMORY_FULL):                       return "MEMORY_FULL";
      case static_cast<int>(CMS_ERROR::SMSC_ADDRESS_UNKNOWN):              return "SMSC_ADDRESS_UNKNOWN";
      case static_cast<int>(CMS_ERROR::NO_NETWORK):                        return "NO_NETWORK";
      case static_cast<int>(CMS_ERROR::NETWORK_TIMEOUT):                   return "NETWORK_TIMEOUT";
      case static_cast<int>(CMS_ERROR::UNKNOWN):                           return "UNKNOWN";
      case static_cast<int>(CMS_ERROR::SIM_NOT_READY):                     return "SIM_NOT_READY";
      case static_cast<int>(CMS_ERROR::MESSAGE_LENGTH_EXCEEDS):            return "MESSAGE_LENGTH_EXCEEDS";
      case static_cast<int>(CMS_ERROR::INVALID_REQUEST_PARAMETERS):        return "INVALID_REQUEST_PARAMETERS";
      case static_cast<int>(CMS_ERROR::ME_STORAGE_FAILURE):                return "ME_STORAGE_FAILURE";
      case static_cast<int>(CMS_ERROR::INVALID_SERVICE_MODE):              return "INVALID_SERVICE_MODE";
      case static_cast<int>(CMS_ERROR::MORE_MESSAGE_TO_SEND_STATE_ERROR):  return "MORE_MESSAGE_TO_SEND_STATE_ERROR";
      case static_cast<int>(CMS_ERROR::MO_SMS_IS_NOT_ALLOW):               return "MO_SMS_IS_NOT_ALLOW";
      case static_cast<int>(CMS_ERROR::GPRS_IS_SUSPENDED):                 return "GPRS_IS_SUSPENDED";
      case static_cast<int>(CMS_ERROR::ME_STORAGE_FULL):                   return "ME_STORAGE_FULL";
      default:
        return std::to_string(err_id);
    }
}


/*
 * Guard functions
 */
bool gf_is_cfun_full(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto state = std::stoi(rev->named_match("state"));
    return (state == static_cast<int>(CFUN_STATE::FULL));
}

bool gf_dbus_offline(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return !sm.dbus_online();
}

bool gf_modem_port_open(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (sm.is_modem_port_open());
}

bool gf_modem_port_open_continue_getoperlist(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (sm.is_modem_port_open() && sm.get_continue_getoperlist());
}


bool gf_modem_port_disabled(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (!sm.is_port_enabled());
}

bool gf_have_no_manufacturer(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (0 == sm.get_modem_manufacturer().length());
}

bool gf_wait_cfun_abort(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (sm.get_cfun_wait_count() >= MAX_CFUN_WAIT_COUNT);
}

bool gf_siminit_timeout(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    //timeout already occurred, avoid second wait loop
    return sm.get_siminit_wait_flag();
}

bool gf_wait_modem_port_abort(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (sm.get_port_wait_count() >= MAX_PORT_WAIT_COUNT);
}

bool gf_set_rmnet_profileid_required(const MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto id = std::stoi(rev->named_match("id"));
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();
    return (id != gprs_access_config.get_profile());
}

bool gf_set_rmnet_autoconnect_required(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto status = std::stoi(rev->named_match("status"));
    const auto linkprot = std::stoi(rev->named_match("linkprot"));
    //DTR set in qmiwwan driver
    return ((status != 1) || (linkprot != 0));
}

bool gf_set_rmnet_autoconnect_required2(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto status = std::stoi(rev->named_match("status"));
    const auto linkprot = std::stoi(rev->named_match("linkprot"));
    const auto dtrset = std::stoi(rev->named_match("dtrset"));
    return ((status != 1) || (linkprot != 0) || (dtrset != 1));
}

bool gf_set_gprs_profile_required(const MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto apn = rev->named_match("apn");
    const auto user = rev->named_match("user");
    const auto pass = rev->named_match("pass");
    const auto auth = std::stoi(rev->named_match("auth"));
    const GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();

    return ( (0 != apn.compare(gprs_access_config.get_apn())) ||
             (0 != user.compare(gprs_access_config.get_user())) ||
             (0 != pass.compare(gprs_access_config.get_pass())) ||
             (auth != gprs_access_config.get_auth()) );
}

bool gf_sim_without_sms_support(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (false == sm.get_sim_sms_support_flag());
}

bool gf_set_sms_format_required(const MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto mode = std::stoi(rev->named_match("mode"));
    const MessageServiceConfig message_service_config = sm.get_message_service_config();

    return ( mode != message_service_config.get_sms_format() );
}

bool gf_set_sms_report_config_required(const MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto mode = std::stoi(rev->named_match("mode"));
    const auto mt = std::stoi(rev->named_match("mt"));
    const auto bm = std::stoi(rev->named_match("bm"));
    const auto ds = std::stoi(rev->named_match("ds"));
    const auto bfr = std::stoi(rev->named_match("bfr"));
    SmsEventReportingConfig sms_report_config = sm.get_sms_report_config();

    return ( mode != sms_report_config.get_mode() ||
             mt != sms_report_config.get_mt() ||
             bm != sms_report_config.get_bm() ||
             ds != sms_report_config.get_ds() ||
             bfr != sms_report_config.get_bfr() );
}

bool gf_set_sms_storage_config_required(const MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto mem1 = rev->named_match("mem1");
    const auto mem2 = rev->named_match("mem2");
    const auto mem3 = rev->named_match("mem3");
    SmsStorageConfig sms_storage_config = sm.get_sms_storage_config();

    return ( (0 != mem1.compare(sms_storage_config.get_mem1())) ||
             (0 != mem2.compare(sms_storage_config.get_mem2())) ||
             (0 != mem3.compare(sms_storage_config.get_mem3())) );
}

bool gf_get_opernames_required(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (sm.get_count_operator_names() == 0);
}

bool gf_cme_sim_not_inserted(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto err = std::stoi(rev->named_match("err"));
    return (err == static_cast<int>(CME_ERROR::SIM_NOT_INSERTED));
}

bool gf_cme_sim_pin_required(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto err = std::stoi(rev->named_match("err"));
    return (err == static_cast<int>(CME_ERROR::SIM_PIN_REQUIRED));
}

bool gf_cme_sim_puk_required(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto err = std::stoi(rev->named_match("err"));
    return (err == static_cast<int>(CME_ERROR::SIM_PUK_REQUIRED));
}

bool gf_cme_sim_failure(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto err = std::stoi(rev->named_match("err"));
    return ((err == static_cast<int>(CME_ERROR::SIM_FAILURE)) || (err == static_cast<int>(CME_ERROR::SIM_WRONG)));
}


bool gf_cme_sim_busy(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto err = std::stoi(rev->named_match("err"));
    return (err == static_cast<int>(CME_ERROR::SIM_BUSY));
}

bool gf_cme_incorrect_password(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto err = std::stoi(rev->named_match("err"));
    return (err == static_cast<int>(CME_ERROR::INCORRECT_PASSWORD));
}

bool gf_cme_no_network_service(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto err = std::stoi(rev->named_match("err"));
    switch (err)
    {
        case static_cast<int>(CME_ERROR::NO_NETWORK_SERVICE):
        case static_cast<int>(CME_ERROR::EMERGENCY_CALLS_ONLY):
            return true;
        default:
            return false;
    }
}

bool gf_err_no_network_service(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    return (MdmErrorState::NET_NO_SERVICE == sm.get_error_state());
}

bool gf_cpin_ready(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string state = rev->named_match("state");
    return (0 == state.compare("READY"));
}

bool gf_cpin_sim_pin(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string state = rev->named_match("state");
    return (0 == state.compare("SIM PIN"));
}

bool gf_cpin_sim_puk(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string state = rev->named_match("state");
    return (0 == state.compare("SIM PUK"));
}

bool gf_cpin_not_inserted(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string state = rev->named_match("state");
    return (0 == state.compare("NOT INSERTED"));
}

bool gf_cpin_not_ready(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string state = rev->named_match("state");
    return (0 == state.compare("NOT READY"));
}

bool gf_qccid_match_storage(const MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string iccid_modem = rev->named_match("iccid");
    std::string iccid_stored = sm.get_stored_sim_iccid();
    return (((iccid_modem.length() > 0) && (0 == iccid_modem.compare(iccid_stored))));
}

bool gf_qinistat_complete(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto state = std::stoi(rev->named_match("state"));
    //Phonebook functionality is not used, therefore complete means also SIM READY and SMS done
    return ((state == static_cast<int>(SIM_INIT_STATUS::SMS_DONE)) ||
            (state == static_cast<int>(SIM_INIT_STATUS::COMPLETED)));
}

bool gf_qinistat_abort_siminit(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto state = std::stoi(rev->named_match("state"));

    /* SIM cards may have no SMS support (see WAT32412) and no Phonebook support (see WAT31951) *
     * SIM-Initialization must be aborted without error when SIM is ready and timeout occurred  *
    */
    return ((state != static_cast<int>(SIM_INIT_STATUS::INITIAL)) && sm.get_siminit_wait_flag());
}


bool gf_renew_wwan_address(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    //FIX for connectivity problem:
    //WWAN0 may hold up to one hour an invalid IP address due to long DHCP lease time.
    //This happens when modem gets a new PDP IP address without GPRS state change and restart of WWAN.
    //For fast reconnection an update of WWAN0 IP address is necessary in this case.
    return (sm.gprs_pdp_addr_changed() &&
            !sm.gprs_wwan_state_changed() &&
            sm.is_gprs_registered());
}

bool gf_gprs_autoconnect_abort(const MdmStatemachine &sm, Event &ev)
{
    (void)ev; //unused parameter
    //FIX for connection establishment problem:
    //Autoconnect function seems to get stuck sometimes in case of roaming and in GSM networks.
    return (sm.get_gprs_autoconnect_count() >= MAX_GPRS_AUTOCONNECT_COUNT);
}

bool set_operselection_required(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; (void)ev; //unused parameter
    NetworkAccessConfig network_access_config = sm.get_network_access_config();
    Oper current_oper = sm.get_current_oper();
    return (current_oper.get_selection_mode() != network_access_config.get_selection_mode());
}

bool extract_dbus_setoper(GVariant *gvar, NetworkAccessConfig& network_access_config, std::string &error_message)
{
    int setoper_id = -1;
    int setoper_act = -1;
    int setoper_mode = -1;
    g_variant_get(gvar, "(iii)", &setoper_mode, &setoper_id, &setoper_act);

    int selection_mode;               //0=AUTOMATIC, 1=MANUAL
    int autoselect_scanseq;           //0=AUTO, 1=GSM_PRIOR, 2=UMTS_PRIOR
    int autoselect_scanmode;          //0=AUTO, 1=GSM_ONLY, 2=UMTS_ONLY
    int manual_selection_act = 0;     //0=NONE, 1=GSM, 2=UMTS
    int manual_selection_oper = 0;    //0=NONE, other values see 3GPP TS 27.007

    /* WAT26409:
     * Selection modes PREFER_GSM and PREFER_UMTS are deprecated.
     * Specific scan sequence does not work with Quectel UC20.
    */
    if ((setoper_mode == static_cast<int>(OPER_SEL_MODE::PREFER_GSM)) ||
        (setoper_mode == static_cast<int>(OPER_SEL_MODE::PREFER_UMTS)))
    {
        //ignore deprecated option and set selection mode AUTOMATIC instead
        setoper_mode = static_cast<int>(OPER_SEL_MODE::AUTOMATIC);
    }

    //mapping of setoper mode to internal network access configuration
    switch(setoper_mode)
    {
      case static_cast<int>(OPER_SEL_MODE::ONLY_UMTS):
        selection_mode = static_cast<int>(OPER_SEL_MODE::AUTOMATIC);
        autoselect_scanmode = static_cast<int>(NW_SCAN_MODE::UMTS_ONLY);
        autoselect_scanseq = static_cast<int>(NW_SCAN_SEQ::UMTS_PRIOR);
        break;
      case static_cast<int>(OPER_SEL_MODE::ONLY_GSM):
        selection_mode = static_cast<int>(OPER_SEL_MODE::AUTOMATIC);
        autoselect_scanmode = static_cast<int>(NW_SCAN_MODE::GSM_ONLY);
        autoselect_scanseq = static_cast<int>(NW_SCAN_SEQ::GSM_PRIOR);
        break;
      case static_cast<int>(OPER_SEL_MODE::MANUAL):
        selection_mode = static_cast<int>(OPER_SEL_MODE::MANUAL);
        autoselect_scanmode = static_cast<int>(NW_SCAN_MODE::AUTO);
        autoselect_scanseq = static_cast<int>(NW_SCAN_SEQ::AUTO);
        break;
      case static_cast<int>(OPER_SEL_MODE::AUTOMATIC):
        selection_mode = static_cast<int>(OPER_SEL_MODE::AUTOMATIC);
        autoselect_scanmode = static_cast<int>(NW_SCAN_MODE::AUTO);
        autoselect_scanseq = static_cast<int>(NW_SCAN_SEQ::AUTO);
        break;
      default:
        //network access selection mode not supported by mdmd and UC20
        error_message = "INVALID PARAMETER: selmode (not supported)";
        return false;
    }
    if (selection_mode == static_cast<int>(OPER_SEL_MODE::MANUAL))
    {
        if (setoper_id < 0)
        {
						error_message = "INVALID PARAMETER: operid (out of range)";
            return false;
        }
        manual_selection_oper = setoper_id;
        switch (setoper_act)
        {
          case static_cast<int>(OPER_SEL_ACT::GSM):
              //no break;
          case static_cast<int>(OPER_SEL_ACT::GSM_COMPACT):
              //no break;
          case static_cast<int>(OPER_SEL_ACT::GSM_EGPRS):
              manual_selection_act = static_cast<int>(OPER_SEL_ACT::GSM);
              break;
          case static_cast<int>(OPER_SEL_ACT::UMTS):
              //no break;
          case static_cast<int>(OPER_SEL_ACT::UMTS_HSDPA):
              //no break;
          case static_cast<int>(OPER_SEL_ACT::UMTS_HSUPA):
              //no break;
          case static_cast<int>(OPER_SEL_ACT::UMTS_HSPA):
              manual_selection_act = static_cast<int>(OPER_SEL_ACT::UMTS);
              break;
          default:
              //network access type not supported by mdmd and UC20
              error_message = "INVALID PARAMETER: act (not supported)";
              return false;
        }
    }
    network_access_config.set_autoselect_scanmode(autoselect_scanmode);
    network_access_config.set_autoselect_scanseq(autoselect_scanseq);
    network_access_config.set_manual_act(manual_selection_act);
    network_access_config.set_manual_oper(manual_selection_oper);
    network_access_config.set_selection_mode(selection_mode);
    return true;
}

bool gf_is_oper_selection_manual(const MdmStatemachine &sm, Event &ev)
{
    (void)sm; (void)ev; //unused parameter
    NetworkAccessConfig network_access_config = sm.get_network_access_config();
    return (network_access_config.get_selection_mode() == static_cast<int>(OPER_SEL_MODE::MANUAL));
}

/*
 * Action functions
 */

void af_do_nothing(MdmStatemachine &sm, Event &ev)
{
    (void)sm; (void)ev;
    //do nothing, just avoid warning about not handled event
}

void af_dbus_getsmsreportconfig(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    SmsEventReportingConfig sms_config = sm.get_sms_report_config();
    
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
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
}


void af_dbus_getoperstate(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto* gvar = static_cast<GVariant*>(nullptr);
    NetworkAccessConfig oper_config = sm.get_network_access_config();
    Oper current_oper               = sm.get_current_oper();
    int setoper_mode;
    int setoper_id;
    int setoper_act;

    //reverse mapping of setoper mode to internal network access configuration (see extract_dbus_setoper method)
    if (oper_config.get_selection_mode() == static_cast<int>(OPER_SEL_MODE::MANUAL))
    {
        setoper_id   = oper_config.get_manual_oper();
        setoper_act  = oper_config.get_manual_act();
        setoper_mode = static_cast<int>(OPER_SEL_MODE::MANUAL);
    }
    else
    {
        setoper_id   = current_oper.get_id();
        setoper_act  = current_oper.get_act();
        if (oper_config.get_autoselect_scanmode() == static_cast<int>(NW_SCAN_MODE::GSM_ONLY))
        {
            setoper_mode = static_cast<int>(OPER_SEL_MODE::ONLY_GSM);
        }
        else if (oper_config.get_autoselect_scanmode() == static_cast<int>(NW_SCAN_MODE::UMTS_ONLY))
        {
            setoper_mode = static_cast<int>(OPER_SEL_MODE::ONLY_UMTS);
        }
        else
        {
            setoper_mode = static_cast<int>(OPER_SEL_MODE::AUTOMATIC);
        }
    }
    const int oper_state = ((sm.get_error_state() == MdmErrorState::NET_NO_SERVICE) ? static_cast<int>(SERVICE_REG_STATE::NOSERVICE) :
                                                                                      sm.get_oper_reg_state());
    std::string oper_name = sm.get_operator_name(setoper_id);
    std::string oper_lac  = sm.get_oper_reg_lac();
    std::string oper_cid  = sm.get_oper_reg_cid();
    gvar = g_variant_new("(iiiiisss)",
                         oper_state,
                         setoper_mode,
                         setoper_id,
                         setoper_act,
                         current_oper.get_quality_percent(),
                         oper_name.c_str(),
                         oper_lac.c_str(),
                         oper_cid.c_str());
    if (gvar != nullptr)
    {
        dev->invocation().return_value(gvar);
    }
    else
    {
        dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
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
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
}

void af_dbus_getgprsaccess2(MdmStatemachine &sm, Event &ev)
{
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto* gvar = g_variant_new("(isissis)",
                                sm.get_gprs_reg_state(),
                                gprs_access_config.get_apn().c_str(),
                                gprs_access_config.get_auth(),
                                gprs_access_config.get_user().c_str(),
                                gprs_access_config.get_pass().c_str(),
                                gprs_access_config.get_state(),
                                sm.get_pdp_address().c_str());
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
}

void af_dbus_getmodeminfo(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto* gvar = g_variant_new("(sss)",
                               sm.get_modem_manufacturer().c_str(),
                               sm.get_modem_model().c_str(),
                               sm.get_modem_revision().c_str() );
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
}

void af_dbus_getmodemidentity(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto* gvar = g_variant_new("(s)", sm.get_modem_identity().c_str());
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
}

void af_dbus_getsmsformat(MdmStatemachine &sm, Event &ev)
{
    (void)sm;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    const MessageServiceConfig message_service_config = sm.get_message_service_config();

    auto*  gvar = g_variant_new("(i)", message_service_config.get_sms_format()) ;
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
}

void af_dbus_setsmsformat(MdmStatemachine &sm, Event &ev)
{
    (void)sm;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    const MessageServiceConfig message_service_config = sm.get_message_service_config();

    int format;
    g_variant_get(dev->invocation().parameters(), "(i)", &format);
    if (format == message_service_config.get_sms_format())
    {
      dev->invocation().return_value( 0 );
    }
    else
    { //change not allowed
      dev->invocation().return_error("de.wago.mdmdError", "INVALID PARAMETER: mode (not supported)");
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
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
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
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
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
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
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
      dev->invocation().return_error("de.wago.mdmdError", "INVALID PARAMETER: loglevel (not supported)");
    }
}

void af_dbus_getsimautoactivation(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    auto* gvar = g_variant_new("(ss)", sm.get_stored_sim_iccid().c_str(),
                                       sm.get_stored_sim_pin().c_str());
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
}

void af_dbus_setsimautoactivation(MdmStatemachine &sm, Event &ev)
{
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    //extract dbus SetSimAutoAuth
    gchar* iccid = nullptr;
    gchar* pin = nullptr;
    g_variant_get(dev->invocation().parameters(), "(ss)", &iccid, &pin);
    if((iccid != nullptr) && (pin != nullptr))
    {
        sm.store_sim_pin(iccid, pin);
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetSimAutoActivation\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
    if (iccid != nullptr)
    {
        g_free(iccid);
    }
    if (pin != nullptr)
    {
        g_free(pin);
    }
}

void af_read_cops(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    int mode = std::stoi( rev->named_match("mode"));
    int id = std::stoi( rev->named_match("id"));
    int act = std::stoi( rev->named_match("act"));
    sm.set_current_oper(mode, id, act);
}

void af_read_cops_null(MdmStatemachine &sm, Event &ev)
{
    (void)sm; (void)ev; //unused parameter
    //COPS: 0 indicates automatic selection is ongoing, currently no operator is selected
    sm.clear_current_oper();
    Oper current_oper = sm.get_current_oper();
    sm.set_current_oper(static_cast<int>(OPER_SEL_MODE::AUTOMATIC), current_oper.get_id(), current_oper.get_act());
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

void af_sms_event_report(MdmStatemachine &sm, Event &ev)
{
    (void)ev; 
    sm.event_report_last_read();
}

void af_sms_done(MdmStatemachine &sm, Event &ev)
{
    (void)ev;
    sm.set_sim_sms_support_flag();
}

void af_cme_error(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string cme_string = cme_to_string(std::stoi(rev->named_match("err")));
    mdmd_Log(MD_LOG_DBG, "%s: CME ERROR: %s\n", sm.get_state().c_str(), cme_string.c_str());
}

void af_cms_error(MdmStatemachine &sm, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string cms_string = cms_to_string(std::stoi(rev->named_match("err")));
    mdmd_Log(MD_LOG_DBG, "%s: CMS ERROR: %s\n", sm.get_state().c_str(), cms_string.c_str());
}

/*
 * Transaction functions
 */
bool tf_trigger_modem_shutdown(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void) src; (void)dst; (void)ev; //unused parameter
    sm.write("at+qpowd");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool tf_dbus_modem_reset_immediate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void) src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_value(nullptr);
    mdmd_Log(MD_LOG_WRN, "%s: Reset request from user while modem not detected, restart without recommended shutdown!\n", sm.get_state().c_str());
    sm.modem_reset(ModemResetMode::MUSB_RESET);
    sm.kick_cmd_timeout(timer_wait_port);
    return true;
}
bool tf_dbus_modem_reset(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst;
    //user trigger for modem reset
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    mdmd_Log(MD_LOG_INF, "%s: Reset request from user -> trigger modem restart\n", sm.get_state().c_str());
    (void)tf_trigger_modem_shutdown(sm, src, dst, ev);
    return true;
}

bool tf_wait_modem_port_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.set_error_state(MdmErrorState::PORT_NOT_READY);
    mdmd_Log(MD_LOG_ERR, "%s: modem port timeout, restart modem without recommended shutdown!\n", sm.get_state().c_str());
    sm.modem_reset(ModemResetMode::MUSB_RESET);
    sm.kick_cmd_timeout(timer_wait_port);
    return true;
}

bool tf_shutdown_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //modem shutdown successfully triggered
    (void)src; (void)dst; (void)ev;
    sm.kick_cmd_timeout(timer_power_down);
    return true;
}

bool tf_shutdown_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //shutdown command not supported or not allowed -> reset modem immediately
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_ERR, "%s: modem shutdown error, restart without recommended shutdown!\n", sm.get_state().c_str());
    sm.modem_reset(ModemResetMode::DEFAULT);
    sm.kick_cmd_timeout(timer_wait_port);
    return true;
}

bool tf_shutdown_err_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value(nullptr);
    (void)tf_shutdown_err(sm, src, dst, ev);
    return true;
}

bool tf_shutdown_finish(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //modem shutdown completed -> reset modem now
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_INF, "%s: modem shutdown successful\n", sm.get_state().c_str());
    sm.modem_reset(ModemResetMode::DEFAULT);
    sm.kick_cmd_timeout(timer_wait_port);
    return true;
}

bool tf_getoperlist_shutdown_finish(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)tf_shutdown_finish(sm, src, dst, ev);
    //set flag to continue after reset
    sm.set_continue_getoperlist(true);
    return true;
}

bool tf_shutdown_finish_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value(nullptr);
    (void)tf_shutdown_finish(sm, src, dst, ev);
    return true;
}

bool tf_shutdown_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //modem hang-up -> reset modem including USB stack
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_ERR, "%s: modem shutdown timeout, restart without recommended shutdown!\n", sm.get_state().c_str());
    sm.modem_reset(ModemResetMode::MUSB_RESET);
    sm.kick_cmd_timeout(timer_wait_port);
    return true;
}

bool tf_shutdown_timeout_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value(nullptr);
    (void)tf_shutdown_timeout(sm, src, dst, ev);
    return true;
}

bool tf_modem_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //modem freeze -> reset modem including USB stack
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_ERR, "%s: modem command timeout, restart modem without recommended shutdown!\n", sm.get_state().c_str());
    sm.modem_reset(ModemResetMode::MUSB_RESET);
    sm.kick_cmd_timeout(timer_wait_port);
    return true;
}

bool tf_modem_timeout_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "MODEM COMMAND TIMEOUT");
    (void)tf_modem_timeout(sm, src, dst, ev);
    return true;
}

bool tf_io_error(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //modem crash -> reset modem including USB stack
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_WRN, "%s: modem io error, restart modem without recommended shutdown!\n", sm.get_state().c_str());
    sm.modem_reset(ModemResetMode::MUSB_RESET);
    sm.kick_cmd_timeout(timer_wait_port);
    return true;
}

bool tf_io_error_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: Modem IO error");
    (void)tf_io_error(sm, src, dst, ev);
    return true;
}


bool tf_query_reg_state_full(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();
    auto at_cmd{"at+cops?;+csq;+creg=2;+creg?;+creg=0;+cgreg?;+cgpaddr="s};
    at_cmd.append(std::to_string(gprs_access_config.get_profile()));
    sm.reset_gprs_change_flags();
    sm.write(at_cmd);
    sm.update_gprs_autoconnect_count();
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool tf_operation_failure_cme_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    auto error_message{"MODEM COMMAND FAILURE: "s};
    error_message.append(cme_to_string(std::stoi(rev->named_match("err"))));
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_operation_failure_cms_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    auto error_message{"MODEM COMMAND FAILURE: "s};
    error_message.append(cms_to_string(std::stoi(rev->named_match("err"))));
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_operation_failure_dbus_return(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "MODEM COMMAND FAILURE");
    (void)tf_query_reg_state_full(sm, src, dst, ev);

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

void return_dbus_getsimstate(DBusEvent *dev, int state, int attempts)
{
    auto* gvar = g_variant_new("(ii)", state, attempts);
    if (gvar != nullptr)
    {
      dev->invocation().return_value(gvar);
    }
    else
    {
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
}

bool tf_dbus_getsimstate_unknown(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    return_dbus_getsimstate(dynamic_cast<DBusEvent *>(&ev), static_cast<int>(SIM_STATE::UNKNOWN), -1);

    return true;
}

bool tf_dbus_getsimstate_activation_required(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    sm.set_current_invocation(dev->invocation());
    sm.write("at+qpinc=\"SC\"");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_getsim_pincount(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    auto* gvar = g_variant_new("(ii)", static_cast<int>(SIM_STATE::PIN_REQUIRED), std::stoi(rev->named_match("s1")));
    if (gvar != nullptr)
    {
        sm.current_invocation().return_value(gvar);
    }
    else
    {
        sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
    sm.deactivate_cmd_timeout();

    return true;
}

bool tf_getsim_nopincount(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    auto* gvar = g_variant_new("(ii)", static_cast<int>(SIM_STATE::PIN_REQUIRED), -1);
    if (gvar != nullptr)
    {
        sm.current_invocation().return_value(gvar);
    }
    else
    {
        sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
    sm.deactivate_cmd_timeout();

    return true;
}

bool tf_getsim_pukcount(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    auto* gvar = g_variant_new("(ii)", static_cast<int>(SIM_STATE::PUK_REQUIRED), std::stoi(rev->named_match("s2")));
    if (gvar != nullptr)
    {
        sm.current_invocation().return_value(gvar);
    }
    else
    {
        sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
    sm.deactivate_cmd_timeout();

    return true;
}

bool tf_getsim_nopukcount(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    auto* gvar = g_variant_new("(ii)", static_cast<int>(SIM_STATE::PUK_REQUIRED), -1);
    if (gvar != nullptr)
    {
        sm.current_invocation().return_value(gvar);
    }
    else
    {
        sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
    sm.deactivate_cmd_timeout();

    return true;
}

bool tf_dbus_getsimstate_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    return_dbus_getsimstate(dynamic_cast<DBusEvent *>(&ev), static_cast<int>(SIM_STATE::READY), -1);

    return true;
}

bool tf_dbus_getsimstate_not_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    return_dbus_getsimstate(dynamic_cast<DBusEvent *>(&ev), static_cast<int>(SIM_STATE::NOT_READY), -1);

    return true;
}

bool tf_dbus_getsimstate_not_inserted(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    return_dbus_getsimstate(dynamic_cast<DBusEvent *>(&ev), static_cast<int>(SIM_STATE::NOT_INSERTED), -1);

    return true;
}

bool tf_dbus_getsimstate_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    return_dbus_getsimstate(dynamic_cast<DBusEvent *>(&ev), static_cast<int>(SIM_STATE::ERROR), -1);

    return true;
}
bool tf_dbus_setpin_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_value(nullptr);
    return true;
}


bool tf_dbus_setoper_to_parameter_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    NetworkAccessConfig network_access_config = sm.get_network_access_config();
    std::string error_message;
    if (extract_dbus_setoper(dev->invocation().parameters(), network_access_config, error_message))
    {
        sm.set_network_access_config(network_access_config);
        dev->invocation().return_value(nullptr);
    }
    else
    {
				mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetOper\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
    }

    return true;
}

bool tf_set_operselection(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    NetworkAccessConfig network_access_config = sm.get_network_access_config();
    auto at_cmd{"at+cops="s};
    if (network_access_config.get_selection_mode() == 0)
    {
        at_cmd.append("0,2");
    }
    else
    {
        at_cmd.append("1,2,");
        at_cmd.append(std::to_string(network_access_config.get_manual_oper()) + ",");
        at_cmd.append(std::to_string(network_access_config.get_manual_act()));
    }
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_cops);

    return true;
}

bool tf_dbus_setoper_to_modem(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    bool do_transition;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    NetworkAccessConfig network_access_config = sm.get_network_access_config();
    std::string error_message;
    if (extract_dbus_setoper(dev->invocation().parameters(), network_access_config, error_message))
    {
        //compare new and actual configuration to avoid unnecessary actions
        if (0 != network_access_config.compare(sm.get_network_access_config()))
        {
            //set parameter to storage
            sm.set_network_access_config(network_access_config);
            (void)tf_set_operselection(sm, src, dst, ev);
            do_transition = true;
        }
        else
        {
            //no configuration change -> nothing to do
            do_transition = false;
        }
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetOper\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
        do_transition = false;
    }
    return do_transition;
}


bool tf_set_nwscan_config(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //nwscan parameter must be set again when operator selection is set to automatic
    (void)src; (void)dst; (void)ev;
    NetworkAccessConfig network_access_config = sm.get_network_access_config();
    auto at_cmd{"at"s};
    at_cmd.append(R"(+qcfg="nwscanmode",)" + std::to_string(network_access_config.get_autoselect_scanmode()) + ",1;");
    /* WAT26409:
     * Specific scan sequence does not work with Quectel UC20.
     * Ignore value from config file.
    */
    at_cmd.append(R"(+qcfg="nwscanseq",)" + std::to_string(static_cast<int>(NW_SCAN_SEQ::AUTO)) + ",1");
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_no_network_service(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //The access to mobile network may be temporarily not allowed. We have to wait and try operator selection again.
    (void)src; (void)dst; (void)ev;
    sm.reset_service_states();
    sm.set_error_state(MdmErrorState::NET_NO_SERVICE);
    sm.kick_cmd_timeout(timer_wait_network_service);
    return true;
}

bool tf_get_oper_regstate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+csq;+creg=2;+creg?;+creg=0");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool tf_set_operator_selection_manual_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //Reset possible network access failure
    sm.set_error_state(MdmErrorState::NONE);
    (void)tf_get_oper_regstate(sm, src, dst, ev);
    return true;
}

bool tf_set_operator_selection_automatic_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //Reset possible network access failure
    sm.set_error_state(MdmErrorState::NONE);
    //automatic network selection requires always scan configuration (fix WAT20902)
    (void)tf_set_nwscan_config(sm, src, dst, ev);
    return true;
}

bool tf_get_gprsprofile(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();
    auto at_cmd{"at+qicsgp="s};
    at_cmd.append(std::to_string(gprs_access_config.get_profile()));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_set_gprsprofile(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();

    auto at_cmd{"at+qicsgp="s};
    at_cmd.append(std::to_string(gprs_access_config.get_profile()));
    at_cmd.append(",1,");
    at_cmd.append(R"(")" + gprs_access_config.get_apn() + R"(",)");
    at_cmd.append(R"(")" + gprs_access_config.get_user() + R"(",)");
    at_cmd.append(R"(")" + gprs_access_config.get_pass() + R"(",)");
    at_cmd.append(std::to_string(gprs_access_config.get_auth()));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_get_gprs_regstate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();
    auto at_cmd{"at+cgreg?;+cgpaddr="s};
    at_cmd.append(std::to_string(gprs_access_config.get_profile()));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool extract_dbus_setgprsaccess2(GVariant *gvar, GprsAccessConfig& gprs_access_config, std::string &error_message)
{
    bool result = false;
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
        gprs_access_config.set_apn(apn);
        gprs_access_config.set_auth(auth);
        gprs_access_config.set_user(user);
        gprs_access_config.set_pass(pass);
        gprs_access_config.set_state(conn);
        result = true;
    }
    else
    {
        error_message = "INTERNAL FAILURE: var nullptr";
    }
    if (apn != nullptr)
    {
        g_free(apn);
    }
    if (user != nullptr)
    {
        g_free(user);
    }
    if (pass != nullptr)
    {
        g_free(pass);
    }

    return result;
}

bool tf_dbus_setgprsaccess_to_parameter_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();
    std::string error_message;
    if (extract_dbus_setgprsaccess2(dev->invocation().parameters(), gprs_access_config, error_message))
    {
        sm.set_gprs_access_config(gprs_access_config);
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetGprsAccess2\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
    }

    return true;
}

bool tf_dbus_setgprsaccess_to_modem(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    bool do_transition;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();
    std::string error_message;
    if (extract_dbus_setgprsaccess2(dev->invocation().parameters(), gprs_access_config, error_message))
    {
        //compare new and actual configuration to avoid unnecessary actions
        if (0 != gprs_access_config.compare(sm.get_gprs_access_config()))
        {
            //set parameter to storage
            sm.set_gprs_access_config(gprs_access_config);
            (void)tf_set_gprsprofile(sm, src, dst, ev);
            do_transition = true;
        }
        else
        {
            //no configuration change -> nothing to do
            do_transition = false;
        }
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetGprsAccess2\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
        do_transition = false;
    }

    return do_transition;
}

bool extract_dbus_setsmsreportconfig(GVariant *gvar, SmsEventReportingConfig& sms_report_config, std::string &error_message)
{
    int mode;
    int mt;
    int bm;
    int ds;
    int bfr;
    g_variant_get(gvar, "(iiiii)", &mode, &mt, &bm, &ds, &bfr);

    if ((mode < 0) || (mode > 2))
    {
        error_message = "INVALID PARAMETER: mode (not supported)";
        return false;  //SMS report mode not supported by mdmd and UC20
    }
    if ((mt < 0) || (mt > 1))
    {
        error_message = "INVALID PARAMETER: mt (not supported)";
        return false; //full SMS DELIVERS (+CMT) not supported by mdmd
    }
    if (bm != 0)
    {
        error_message = "INVALID PARAMETER: bm (not supported)";
        return false; //broadcast message indications (+CBM) not supported by mdmd
    }
    if (ds != 0)
    {
        error_message = "INVALID PARAMETER: ds (not supported)";
        return false; //SMS status reports (+CDS) not supported by mdmd
    }
    if ((bfr < 0) || (bfr > 1))
    {
        error_message = "INVALID PARAMETER: bfr (not supported)";
        return false;  //SMS buffer mode not supported by mdmd and UC20
    }

    sms_report_config.set_mode(mode);
    sms_report_config.set_mt(mt);
    sms_report_config.set_bm(bm);
    sms_report_config.set_ds(ds);
    sms_report_config.set_bfr(bfr);
    
    return true;
}

bool tf_dbus_setsmsreportconfig_to_parameter_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    SmsEventReportingConfig sms_report_config = sm.get_sms_report_config();
    std::string error_message;
    if (extract_dbus_setsmsreportconfig(dev->invocation().parameters(), sms_report_config, error_message))
    {
        sm.set_sms_report_config(sms_report_config);
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetSmsReportConfig\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
    }

    return true;
}

bool tf_set_sms_report_config(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    SmsEventReportingConfig sms_config = sm.get_sms_report_config();
    auto at_cmd{"at+cnmi="s};
    at_cmd.append(std::to_string(sms_config.get_mode()) + ",");
    at_cmd.append(std::to_string(sms_config.get_mt()) + ",");
    at_cmd.append(std::to_string(sms_config.get_bm()) + ",");
    at_cmd.append(std::to_string(sms_config.get_ds()) + ",");
    at_cmd.append(std::to_string(sms_config.get_bfr()));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_dbus_setsmsreportconfig_to_modem(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    bool do_transition;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    SmsEventReportingConfig sms_report_config = sm.get_sms_report_config();
    std::string error_message;
    if (extract_dbus_setsmsreportconfig(dev->invocation().parameters(), sms_report_config, error_message))
    {
        //compare new and actual configuration to avoid unnecessary actions
        if (0 != sms_report_config.compare(sm.get_sms_report_config()))
        {
            //set parameter to storage
            sm.set_sms_report_config(sms_report_config);
            (void)tf_set_sms_report_config(sm, src, dst, ev);
            do_transition = true;
        }
        else
        {
            //no configuration change -> nothing to do
            do_transition = false;
        }
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetSmsReportConfig\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
        do_transition = false;
    }

    return do_transition;
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


bool tf_wait_user_command(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.set_error_state(MdmErrorState::NONE);
    sm.kick_cmd_timeout(timer_status_query);
    return true;
}

bool tf_init_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //Kritischer, unerwarteter Fehler. Die Modem-Schnittstelle ist nicht verwendbar.
    (void)src; (void)dst; (void)ev; 
    mdmd_Log(MD_LOG_ERR, "%s: modem initialization failed\n", sm.get_state().c_str());
    sm.reset_service_states();
    sm.set_error_state(MdmErrorState::INIT_FAILED);
    sm.deactivate_cmd_timeout();
    return true;
}

bool tf_void_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; (void)ev; 
    sm.deactivate_cmd_timeout();

    return true;
}

bool tf_disable_status_reports(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+creg=0;+cgreg=0;+qindcfg=\"csq\",0");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_get_sms_format(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+cmgf?");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_set_sms_format(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    const MessageServiceConfig message_service_config = sm.get_message_service_config();

    auto at_cmd{"at+cmgf="s};
    at_cmd.append(std::to_string(message_service_config.get_sms_format()));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_get_simstate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 

    sm.reset_sim_states();
    sm.write("at+cpin?");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}


/*
 * Specific transaction functions
 */
/*s01*/
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

/*s02*/
bool tf_modem_port_not_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    sm.set_port_wait_count(1);
    sm.kick_cmd_timeout(timer_wait_port);
    mdmd_Log(MD_LOG_WRN, "%s: modem port not ready\n", sm.get_state().c_str());
    return true;
}

/*s03*/
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

bool tf_get_modeminfo(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.write("ati");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}


/*s03*/
bool tf_modeminfo_revision(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto revision = rev->named_match("revision");
    sm.set_modem_revision(revision);

    return true;
}

/*s03*/
bool tf_modeminfo_manufacturer(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto manufacturer = rev->named_match("line");
    sm.set_modem_manufacturer( manufacturer );

    return true;
}

/*s03*/
bool tf_modeminfo_model(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto model = rev->named_match("line");
    sm.set_modem_model(model);

    return true;
}

bool tf_get_modemidentity(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.write("at+gsn");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

/*s03*/
bool tf_modemidentity(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto imei = rev->named_match("line");
    sm.set_modem_identity( imei );

    return true;
}
/*i01_05*/
bool tf_wait_cfun(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //The access to cfun state may be not yet allowed (e.g. after modem reset). We have to wait for CFUN indication here.
    (void)src; (void)dst; (void)ev; 
    sm.inc_cfun_wait_count();
    sm.kick_cmd_timeout(timer_wait_cfun);
    mdmd_Log(MD_LOG_DBG, "%s: modem not ready (%d)\n", sm.get_state().c_str(), sm.get_cfun_wait_count());

    return true;
}

bool tf_get_modem_function_state(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.write("at+cfun?");
    sm.kick_cmd_timeout(timer_at_cmd_cfun);

    return true;
}

bool tf_set_modem_full_function(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.write("at+cfun=1");
    sm.kick_cmd_timeout(timer_at_cmd_cfun);

    return true;
}

/*i02_01*/
bool tf_setsimpin_from_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    const auto pin = sm.get_stored_sim_pin();
    auto at_cmd{"at+cpin="s};
    at_cmd += pin;
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_waitsim_pin(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.reset_service_states();
    sm.set_error_state(MdmErrorState::SIM_PIN_NEEDED);
    //stop command timer, wait for user set pin
    sm.deactivate_cmd_timeout();
    return true;
}

bool tf_waitsim_puk(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.reset_service_states();
    sm.set_error_state(MdmErrorState::SIM_PUK_NEEDED);
    //stop command timer, wait for user set pin
    sm.deactivate_cmd_timeout();
    return true;
}

bool tf_waitsim_removed(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.reset_service_states();
    sm.set_error_state(MdmErrorState::SIM_REMOVED);
    //start timer to get sim state again
    sm.kick_cmd_timeout(timer_poll_simstate);
    return true;
}

bool tf_waitsim_busy(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.reset_service_states();
    sm.kick_cmd_timeout(timer_sim_busy);
    return true;
}

bool tf_waitsim_busy_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //Set error not before timeout. SIM is for some seconds after modem restart not ready and this is no failure.
    sm.set_error_state(MdmErrorState::SIM_NOT_READY);
    (void)tf_get_simstate(sm, src, dst, ev);
    return true;
}

bool tf_sim_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.reset_service_states();
    sm.set_error_state(MdmErrorState::SIM_INVALID);
    //stop command timer, user has to switch off and change the SIM card
    sm.deactivate_cmd_timeout();
    return true;
}

bool tf_activatesim_get_iccid(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+qccid");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool tf_waitsim_init_complete(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    //start timer for sim activation completed (CPIN READY & SMS DONE & PHB DONE)
    sm.kick_cmd_timeout(timer_wait_pb_done);
    //set flag to avoid second wait loop
    sm.set_siminit_wait_flag();
    return true;
}

bool tf_qinistat_sms_enable(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.set_sim_sms_support_flag();
    return true;
}

bool tf_siminit_abort(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; (void)ev;
    //just print warning here and continue without full SIM functionality
    mdmd_Log(MD_LOG_WRN, "%s: SIM initialization incomplete\n", sm.get_state().c_str());
    return true;
}


bool tf_get_sim_initialization_state(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+qinistat");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    return true;
}

bool extract_dbus_setsimpin(GVariant *gvar, std::string &pin_str, std::string &error_message)
{
    bool result = false;
    gchar* pin = nullptr;
    g_variant_get(gvar, "(s)", &pin);
    if (pin != nullptr)
    {
        pin_str = pin;
        g_free(pin);
        result = true;
    }
    else
    {
        error_message = "INTERNAL FAILURE: var nullptr";
    }

    return result;
}

bool tf_dbus_setpin(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    bool do_transition;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    std::string pin;
    std::string error_message;
    if (extract_dbus_setsimpin(dev->invocation().parameters(), pin, error_message))
    {
        auto at_cmd("at+cpin="s);
        at_cmd.append(pin);
        sm.write(at_cmd);
        sm.kick_cmd_timeout(timer_at_cmd_short);

        //store pin here, iccid will be stored later when SIM activation is successful
        sm.store_sim_pin("", pin);
        sm.set_current_invocation(dev->invocation());
        do_transition = true;
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetSimPin\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
        do_transition = false;
    }

    return do_transition;
}

bool extract_dbus_setsimpuk(GVariant *gvar, std::string &puk_str, std::string &pin_str, std::string error_message)
{
    bool result = false;
    gchar* puk = nullptr;
    gchar* pin = nullptr;
    g_variant_get(gvar, "(ss)", &puk, &pin);
    if ((puk != nullptr) && (pin != nullptr))
    {
        puk_str = puk;
        pin_str = pin;
        result = true;
    }
    else
    {
        error_message = "INTERNAL FAILURE: var nullptr";
    }
    if (puk != nullptr)
    {
        g_free(puk);
    }
    if (pin != nullptr)
    {
        g_free(pin);
    }

    return result;
}

bool tf_dbus_setpuk(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    bool do_transition;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    std::string puk;
    std::string pin;
    std::string error_message;
    if (extract_dbus_setsimpuk(dev->invocation().parameters(), puk, pin, error_message))
    {
        auto at_cmd("at+cpin="s);
        at_cmd.append(puk + ',' + pin);
        sm.write(at_cmd);
        sm.kick_cmd_timeout(timer_at_cmd_short);

        //store pin already here and iccid when SIM activation is successful
        sm.store_sim_pin("", pin);
        sm.set_current_invocation(dev->invocation());
        do_transition = true;
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetSimPuk\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
        do_transition = false;
    }

    return do_transition;
}

bool tf_dbus_setpin_cme_sim_not_inserted(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"MODEM COMMAND FAILURE: "s};
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    error_message.append(cme_to_string(std::stoi(rev->named_match("err"))));
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    (void)tf_waitsim_removed(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setpin_cme_sim_busy(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"MODEM COMMAND FAILURE: "s};
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    error_message.append(cme_to_string(std::stoi(rev->named_match("err"))));
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    (void)tf_waitsim_busy(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setpin_cme_sim_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"MODEM COMMAND FAILURE: "s};
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    error_message.append(cme_to_string(std::stoi(rev->named_match("err"))));
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    (void)tf_sim_failure(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setpin_cme_wrong_password(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"MODEM COMMAND FAILURE: "s};
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    error_message.append(cme_to_string(std::stoi(rev->named_match("err"))));
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    //sim state could change, get state again
    (void)tf_get_simstate(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setpin_err(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_error("de.wago.mdmdError", "MODEM COMMAND FAILURE");
    (void)tf_get_simstate(sm, src, dst, ev);
    return true;
}

bool tf_set_iccid_to_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string iccid_modem = rev->named_match("iccid");
    std::string simpin_stored = sm.get_stored_sim_pin();

    //SIM activation is successful here, store iccid together with already stored pin
    sm.store_sim_pin(iccid_modem, simpin_stored);
    return true;
}

bool tf_dbus_setpin_ok_not_persistent(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value(nullptr);
    mdmd_Log(MD_LOG_WRN, "%s: Set PIN successful but no ICCID for automatic activation\n", sm.get_state().c_str());
    (void)tf_get_sim_initialization_state(sm, src, dst, ev);
    return true;
}

bool tf_dbus_setpin_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.current_invocation().return_value(nullptr);
    (void)tf_get_sim_initialization_state(sm, src, dst, ev);
    return true;
}

bool tf_init_sim_complete_get_opernames(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.set_error_state(MdmErrorState::NONE);
    sm.write("at+copn");
    sm.kick_cmd_timeout(timer_at_cmd_copn);

    return true;
}

bool tf_get_operselection(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+cops?");
    sm.kick_cmd_timeout(timer_at_cmd_cops);

    return true;
}

bool tf_init_sim_complete_get_operselection(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_error_state(MdmErrorState::NONE);
    (void)tf_get_operselection(sm, src, dst, ev);

    return true;
}

bool tf_sim_activation_required(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    //SIM needs activation again after previous SIM initialization completed

    sm.reset_service_states();
    //start SIM initialization again
    (void)tf_get_simstate(sm, src, dst, ev);

    return true;
}

bool tf_get_sms_report_config(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+cnmi?");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

/*i03_01*/
bool tf_readoperatorname(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto id = std::stoi(rev->named_match("id"));
    const auto name = rev->named_match("name");
    (void)sm.insert_operator_name(id, name);
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

    sm.clear_smslist();
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(i)", &state);
    at_cmd.append(std::to_string(state));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

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
      sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
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

bool tf_dbus_readsms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto at_cmd{"at+cmgr="s};
    int sms_index = -1;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    
    sm.clear_smslist();
    sm.set_current_invocation(dev->invocation());
    g_variant_get(sm.current_invocation().parameters(), "(i)", &sms_index);
    at_cmd.append(std::to_string(sms_index));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);
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
      sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
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

bool tf_dbus_sendsms_result(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; 
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    const auto msgref = std::stoi(rev->named_match("msgref"));
    auto*  gvar = g_variant_new("(i)", msgref);
    if (gvar != nullptr)
    {
      sm.current_invocation().return_value( gvar );
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }

    return true;
}

bool tf_get_smsstorage_default(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; 
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    SmsStorageConfig default_sms_storage = sm.get_sms_storage_config();
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
      dev->invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
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

bool tf_dbus_getsmsstorage_result(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
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
      sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
    
    return true;
}


bool tf_dbus_reject_modem_not_available(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: Modem not available");
    return true;
}

bool tf_dbus_reject_modem_init_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: Modem initialization failed");
    return true;
}

bool tf_dbus_reject_modem_not_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: Modem not ready");
    return true;
}

bool tf_dbus_reject_sim_locked(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: SIM locked");
    return true;
}

bool tf_dbus_reject_sim_not_inserted(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: SIM not inserted");
    return true;
}

bool tf_dbus_reject_sim_not_ready(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: SIM not ready");
    return true;
}

bool tf_dbus_reject_sim_failure(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: SIM failure");
    return true;
}

bool tf_dbus_reject_sim_without_sms(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: SIM without SMS support");
    return true;
}

bool tf_dbus_reject_net_no_service(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    dev->invocation().return_error("de.wago.mdmdError", "OPERATION REJECTED: Network service not available");
    return true;
}


bool tf_renew_wwan_address(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_INF, "%s: IP address change detected, trigger renew\n", sm.get_state().c_str());
    //renew WWAN IP address
    sm.wwan_renew();
    sm.kick_cmd_timeout(timer_status_query);

    return true;
}

bool tf_get_rmnet_profileid(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+qcfg=\"rmnet/profileid\"");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_set_rmnet_profileid(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    GprsAccessConfig gprs_access_config = sm.get_gprs_access_config();
    auto at_cmd{R"(at+qcfg="rmnet/profileid",)"s};
    at_cmd.append(std::to_string(gprs_access_config.get_profile()));
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_get_rmnet_autoconnect(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+qcfg=\"rmnet/autoconnect\"");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_get_rmnet_autoconnect_with_dtrset(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    //remember autoconnect format e.g. for operator detection
    sm.set_rmnet_autoconnect_with_dtrset(true);

    return true;
}

bool tf_get_rmnet_autoconnect_no_dtrset(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    //remember autoconnect format e.g. for operator detection
    sm.set_rmnet_autoconnect_with_dtrset(false);

    return true;
}

bool tf_get_sms_storage_config(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write("at+cpms?");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_set_sms_storage_config(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; 
    auto at_cmd{"at+cpms="s};
    SmsStorageConfig sms_storage_config = sm.get_sms_storage_config();
    at_cmd.append(R"(")" + sms_storage_config.get_mem1() + R"(",)");
    at_cmd.append(R"(")" + sms_storage_config.get_mem2() + R"(",)");
    at_cmd.append(R"(")" + sms_storage_config.get_mem3() + R"(")");
    sm.write(at_cmd);
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool extract_dbus_setsmsstorage(GVariant *gvar, SmsStorageConfig& sms_storage_config, const std::vector<std::string> allowed_cpms_parameter, std::string &error_message)
{

    bool result = false;
    gchar* c_mem1 = nullptr;
    gchar* c_mem2 = nullptr;
    gchar* c_mem3 = nullptr;
    g_variant_get(gvar, "(sss)", &c_mem1, &c_mem2, &c_mem3);

    if ((c_mem1 != nullptr) && (c_mem2 != nullptr) && (c_mem3 != nullptr))
    {
        std::string mem1{c_mem1};
        std::string mem2{c_mem2};
        std::string mem3{c_mem3};
        if ( (std::find(allowed_cpms_parameter.begin(), allowed_cpms_parameter.end(), mem1) != allowed_cpms_parameter.end()) &&
             (std::find(allowed_cpms_parameter.begin(), allowed_cpms_parameter.end(), mem2) != allowed_cpms_parameter.end()) &&
             (std::find(allowed_cpms_parameter.begin(), allowed_cpms_parameter.end(), mem3) != allowed_cpms_parameter.end()) )
        {
            sms_storage_config.set_mem1(mem1);
            sms_storage_config.set_mem2(mem2);
            sms_storage_config.set_mem3(mem3);
            result = true;
        }
    }
    else
    {
        error_message = "INTERNAL FAILURE: var nullptr";
    }
    
    if (c_mem1 != nullptr)
    {
        g_free(c_mem1);
    }
    if (c_mem2 != nullptr)
    {
        g_free(c_mem2);
    }
    if (c_mem3 != nullptr)
    {
        g_free(c_mem3);
    }

    return result;
}

bool tf_dbus_setsmsstorage_to_parameter_storage(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    SmsStorageConfig sms_storage_config = sm.get_sms_storage_config();
    std::string error_message;
    if (extract_dbus_setsmsstorage(dev->invocation().parameters(), sms_storage_config, {"SM", "ME", "MT", ""}, error_message))
    {
        sm.set_sms_storage_config(sms_storage_config);
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetSmsStorage\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
    }

    return true;
}

bool tf_dbus_setsmsstorage_to_modem(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    bool do_transition;
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    SmsStorageConfig sms_storage_config = sm.get_sms_storage_config();
    std::string error_message;
    if (extract_dbus_setsmsstorage(dev->invocation().parameters(), sms_storage_config, {"SM", "ME", "MT"}, error_message))
    {
        //compare new and actual configuration to avoid unnecessary actions
        if (0 != sms_storage_config.compare(sm.get_sms_storage_config()))
        {
            //set parameter to storage
            sm.set_sms_storage_config(sms_storage_config);
            (void)tf_set_sms_storage_config(sm, src, dst, ev);
            do_transition = true;
        }
        else
        {
            //no configuration change -> nothing to do
            do_transition = false;
        }
        dev->invocation().return_value(nullptr);
    }
    else
    {
        mdmd_Log(MD_LOG_DBG, "%s: DBUS method \"SetSmsStorage\" with invalid parameter\n", sm.get_state().c_str());
        dev->invocation().return_error("de.wago.mdmdError", error_message.c_str());
        do_transition = false;
    }
    return do_transition;
}

bool tf_get_modeminfo_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Get modem information failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge ist lediglich, dass Informationen nicht angezeigt werden.
    //-> weiter mit Modem-Initialisierung
    (void)tf_get_modemidentity(sm, src, dst, ev);

    return true;
}

bool tf_get_modemidentity_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Get modem identity failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge ist lediglich, dass IMEI nicht angezeigt wird.
    //-> weiter mit Modem-Initialisierung
    (void)tf_get_modem_function_state(sm, src, dst, ev);

    return true;
}

bool tf_init_modem_function_state_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_ERR, "%s: Init modem functionality failed -> trigger modem restart\n", sm.get_state().c_str());
    sm.set_error_state(MdmErrorState::INIT_FAILED);
    //Kritischer Fehler. Das Modem muss in einen voll funktionsfähigen Zustand gebracht werden.
    //-> weiter mit vollständigem Reset
    (void)tf_trigger_modem_shutdown(sm, src, dst, ev);

    return true;
}


bool tf_disable_status_reports_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Disable modem status reports failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge können unerwartete Events vom Modem sein, die aber verworfen werden.
    //-> weiter mit SIM-Initialisierung
    (void)tf_get_simstate(sm, src, dst, ev);

    return true;
}

bool tf_setsimpin_from_storage_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Set SIM PIN from storage failed\n", sm.get_state().c_str());
    //Die SIM-Karte kann mit dem gespeicherten PIN nicht automatisch aktiviert werden.
    //Eventuell wurde die Karte getauscht oder ist defekt.
    //-> Lösche gespeicherte ICCID und PIN, dann weiter mit erneuter SIM-Initialisierung
    sm.store_sim_pin("","");
    (void)tf_get_simstate(sm, src, dst, ev);

    return true;
}

bool tf_get_opernames_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Get operator names failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge ist lediglich, dass die Providernamen nicht angezeigt werden.
    //-> Weiter mit Provider-Initialisierung
    (void)tf_get_operselection(sm, src, dst, ev);

    return true;
}

bool tf_operselection_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_WRN, "%s: Operator selection failed\n", sm.get_state().c_str());
    sm.set_error_state(MdmErrorState::NET_NO_SERVICE);
    sm.clear_current_oper();
    //Die Provider-Auswahl und deren Status wird für weitere Dienste des Modems benötigt.
    //-> Weiter nach kurzer Wartezeit mit erneutem Versuch
    sm.kick_cmd_timeout(timer_retry_network_access);

    return true;
}

bool tf_set_nwscan_config_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Set network scan config failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge ist, dass Vorgaben für die automatische Providerauswahl nicht beachtet werden.
    //-> Weiter mit Provider-Initialisierung
    (void)tf_get_oper_regstate(sm, src, dst, ev);

    return true;
}

bool tf_get_oper_regstate_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Get operator registration status failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge ist, dass der Status hier nicht korrekt angezeigt wird.
    //Aktualisierung erfolgt später aber periodisch
    //-> Weiter mit Datendienst-Initialisierung
    (void)tf_get_rmnet_profileid(sm, src, dst, ev);

    return true;
}

bool tf_get_rmnet_profileid_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_INF, "%s: Get data service profile id not possible\n", sm.get_state().c_str());
    //Kein Fehler. Ältere Firmware-Versionen des Modems lassen einen Zugriff auf die Profile-ID nicht zu.
    //In diesem Fall entspricht die ID dem konstanten Default-Wert (1)
    //-> Weiter mit Datendienst-Initialisierung
    (void)tf_get_rmnet_autoconnect(sm, src, dst, ev);

    return true;
}

bool tf_set_rmnet_profileid_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Set data service profile id failed\n", sm.get_state().c_str());
    //Das sollte nicht passieren, Datendienst-Konfiguration nicht möglich
    //-> Weiter mit Datendienst-Initialisierung
    (void)tf_get_rmnet_autoconnect(sm, src, dst, ev);

    return true;
}

bool tf_set_rmnet_profileid_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_INF, "%s: Data service profile changed -> trigger modem restart\n", sm.get_state().c_str());
    //Eine Änderung der Profile ID wird nur mit Neustart des Modems wirksam
    (void)tf_trigger_modem_shutdown(sm, src, dst, ev);

    return true;
}

bool tf_rmnet_autoconnect_config_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_INF, "%s: Data service autoconnect changed -> trigger modem restart\n", sm.get_state().c_str());
    //Eine Änderung der Autoconnect-Einstellung wird nur mit Neustart des Modems wirksam
    (void)tf_trigger_modem_shutdown(sm, src, dst, ev);

    return true;
}

bool tf_gprsprofile_config_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_INF, "%s: Data service configuration failed\n", sm.get_state().c_str());
    //Das sollte nicht passieren, Datendienst eventuell nicht möglich mit aktuellen Einstellungen im Modem
    //-> Weiter mit Datendienst-Initialisierung
    (void)tf_get_gprs_regstate(sm, src, dst, ev);

    return true;
}

bool tf_gprsprofile_config_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_INF, "%s: Data service configuration changed -> trigger modem restart\n", sm.get_state().c_str());
    //Eine Änderung der Profile-Parameter wird nur mit Neustart des Modems wirksam
    (void)tf_trigger_modem_shutdown(sm, src, dst, ev);

    return true;
}

bool tf_get_gprs_regstate_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Get data service registration status failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge ist, dass der Status kurzzeitig nicht korrekt angezeigt wird.
    //Aktualisierung erfolgt später aber periodisch
    //-> Weiter mit SMS-Initialisierung
    (void)tf_get_sms_storage_config(sm, src, dst, ev);

    return true;
}

bool tf_get_gprs_regstate_fail_skip_smsinit(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Get data service registration status failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge ist, dass der Status kurzzeitig nicht korrekt angezeigt wird.
    //Aktualisierung erfolgt nachfolgend periodisch mit allen Status-Werten
    //-> Überspringen der SMS-Initialisierung
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_sms_storage_config_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_WRN, "%s: SMS storage configuration failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge können falsch oder nicht gespeicherte Nachrichten sein
    //-> Weiter mit SMS-Initialisierung
    (void)tf_get_sms_format(sm, src, dst, ev);

    return true;
}

bool tf_sms_format_config_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_WRN, "%s: SMS format configuration failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge können unerwartete Events vom Modem sein, die aber verworfen werden.
    //-> Weiter mit SMS-Initialisierung
    (void)tf_get_sms_report_config(sm, src, dst, ev);

    return true;
}


bool tf_sms_report_config_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    mdmd_Log(MD_LOG_WRN, "%s: SMS report configuration failed\n", sm.get_state().c_str());
    //Unkritischer Fehler. Folge können unerwartete Events vom Modem sein, die aber verworfen werden.
    //-> Weiter mit SMS-Initialisierung
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_get_registration_status_failed(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Could not fetch modem status\n", sm.get_state().c_str());
    (void)tf_wait_user_command(sm, src, dst, ev);

    return true;
}


bool tf_gprs_autoconnect_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    mdmd_Log(MD_LOG_WRN, "%s: Data service autoconnect timeout ->  trigger modem restart\n", sm.get_state().c_str());
    //In seltenen Fällen kann sich die Autoconnect-Dienst im Modem aufhängen.
    //Über einen Neustart des Modems wird der Dienst wieder gestartet.
    //Hinweis: Dies ist lediglich ein Workaround für einen Fehler im Modem, der mit neuer Modem-Firmware gelöst sein kann.
    (void)tf_trigger_modem_shutdown(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_autoconnect_disable_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"INTERNAL FAILURE: Modem autoconnect deactivation failed"s};
    mdmd_Log(MD_LOG_DBG, "%s: Could not scan provider: %s\n", sm.get_state().c_str(), error_message.c_str());
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    (void)tf_query_reg_state_full(sm, src, dst, ev);

    return true;
}

bool tf_autoconnect_enable(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    sm.write(sm.get_rmnet_autoconnect_with_dtrset() ? "at+qcfg=\"rmnet/autoconnect\",1,0,1"
                                                   : "at+qcfg=\"rmnet/autoconnect\",1,0");
    sm.kick_cmd_timeout(timer_at_cmd_short);

    return true;
}

bool tf_getoperlist_request(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; //unused parameter
    auto* dev = dynamic_cast<DBusEvent *>(&ev);
    /* Disable autoconnect function in modem (fix WAT21951)
    *  The problem is that at+cops=? sometimes returns CME ERROR 3 (Operation not allowed) when autoconnect is active.
    *  But the command "at+qndisop=suspendauto=1" to disable autoconnect is not supported on UC20G devices anymore.
    *  Note: This change requires a modem reset
    */
    sm.write(sm.get_rmnet_autoconnect_with_dtrset() ? "at+qcfg=\"rmnet/autoconnect\",0,0,1"
                                                   : "at+qcfg=\"rmnet/autoconnect\",0,0");
    sm.kick_cmd_timeout(timer_at_cmd_short);
    sm.set_current_invocation(dev->invocation());

    return true;
}

bool tf_getoperlist_from_modem(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev; //unused parameter
    sm.clear_opermap();
    sm.write("at+cops=?");
    sm.kick_cmd_timeout(timer_at_cmd_cops);

    return true;
}

bool tf_getoperlist_nextentry(MdmStatemachine &sm, State &src, State &dst, Event &ev)
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

bool tf_getoperlist_ok(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto* gvar = sm.get_var_opermap();
    if (gvar != nullptr)
    {
      sm.current_invocation().return_value(gvar);
    }
    else
    {
      sm.current_invocation().return_error("de.wago.mdmdError", "INTERNAL FAILURE: var nullptr");
    }
    //-> zum Abschluss erneute Aktivierung von autoconnect
    tf_autoconnect_enable(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_wrong_simstate(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)sm; (void)src; (void)dst; //unused parameter
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    std::string state = rev->named_match("state");
    mdmd_Log(MD_LOG_WRN, "%s: Unexpected sim state '%s'\n", sm.get_state().c_str(), state.c_str());

    return true;
}

bool tf_getoperlist_shutdown_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"INTERNAL FAILURE: Modem restart not possible"s};
    mdmd_Log(MD_LOG_DBG, "%s: Could not scan provider: %s\n", sm.get_state().c_str(), error_message.c_str());
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    //-> zum Abschluss erneute Aktivierung von autoconnect
    tf_autoconnect_enable(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_fail_cme(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"MODEM COMMAND FAILURE: "s};
    auto* rev = dynamic_cast<ModemEvent *>(&ev);
    error_message.append(cme_to_string(std::stoi(rev->named_match("err"))));
    mdmd_Log(MD_LOG_DBG, "%s: Could not scan provider: %s\n", sm.get_state().c_str(), error_message.c_str());
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    //-> zum Abschluss erneute Aktivierung von autoconnect
    tf_autoconnect_enable(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"MODEM COMMAND FAILURE"s};
    mdmd_Log(MD_LOG_DBG, "%s: Could not scan provider: %s\n", sm.get_state().c_str(), error_message.c_str());
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    //-> zum Abschluss erneute Aktivierung von autoconnect
    tf_autoconnect_enable(sm, src, dst, ev);

    return true;
}

bool tf_continue_getoperlist(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    sm.set_continue_getoperlist(false);
    (void)tf_startinit(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_init_fail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"INTERNAL FAILURE: Modem initialization failed"s};
    mdmd_Log(MD_LOG_DBG, "%s: Could not scan provider: %s\n", sm.get_state().c_str(), error_message.c_str());
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    //-> zum Abschluss erneute Aktivierung von autoconnect
    tf_autoconnect_enable(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_simfail(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"INTERNAL FAILURE: SIM initialization failed"s};
    mdmd_Log(MD_LOG_DBG, "%s: Could not scan provider: %s\n", sm.get_state().c_str(), error_message.c_str());
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    //-> zum Abschluss erneute Aktivierung von autoconnect
    tf_autoconnect_enable(sm, src, dst, ev);

    return true;
}

bool tf_getoperlist_waitsim(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    (void)src; (void)dst; (void)ev;
    //start timer to get sim state again
    sm.kick_cmd_timeout(timer_poll_simstate);

    return true;
}

bool tf_getoperlist_siminit_timeout(MdmStatemachine &sm, State &src, State &dst, Event &ev)
{
    auto error_message{"INTERNAL FAILURE: SIM initialization timeout"s};
    mdmd_Log(MD_LOG_DBG, "%s: Could not scan provider: %s\n", sm.get_state().c_str(), error_message.c_str());
    sm.current_invocation().return_error("de.wago.mdmdError", error_message.c_str());
    //-> zum Abschluss erneute Aktivierung von autoconnect
    tf_autoconnect_enable(sm, src, dst, ev);

    return true;
}


} //namespace

/****************************/

using NT = no_op_transition<MdmStatemachine>;
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

    /*Start states*/
    State s01("s01", "wait dbus");
    State s02("s02", "open modem port");
    State s03("s03", "wait modem port", true);
    State s04("s04", "wait modem port enable", true);

    /*Error states*/
    State i00("i00", "init failed", true);

    /*Modem initialisation states*/
    State i01_01("i01_01", "init modem, deactivate echo mode");
    State i01_02("i01_02", "init modem, set error format");
    State i01_03("i01_03", "init modem, get info");
    State i01_04("i01_04", "init modem, get identity");
    State i01_05("i01_05", "init modem, get functionality state");
    State i01_06("i01_06", "init modem, get functionality state, is full");
    State i01_07("i01_07", "wait modem", true);
    State i01_08("i01_08", "init modem, set full functional");
    State i01_09("i01_09", "init modem, disable status reports");

    State i02_01("i02_01", "init SIM, get state");
    State i02_02("i02_02", "init SIM, got state, ready");
    State i02_03("i02_03", "init SIM, got state, PIN required");
    State i02_04("i02_04", "init SIM, got state, PUK required");
    State i02_05("i02_05", "init SIM, got state, not inserted");
    State i02_06("i02_06", "init SIM, got state, busy");
    State i02_10("i02_10", "wait SIM, PIN required", true);
    State i02_11("i02_11", "wait SIM, get PIN count");
    State i02_12("i02_12", "wait SIM, PUK required", true);
    State i02_13("i02_13", "wait SIM, get PUK count");
    State i02_14("i02_14", "wait SIM, not inserted", true);
    State i02_15("i02_15", "wait SIM, not ready", true);
    State i02_16("i02_16", "SIM failure", true);
    State i02_20("i02_20", "activate SIM, get iccid to compare");
    State i02_21("i02_21", "activate SIM, got iccid, match storage");
    State i02_22("i02_22", "activate SIM, set PIN from storage");
    State i02_23("i02_23", "activate SIM, set PIN/PUK from user");
    State i02_24("i02_24", "activate SIM, get iccid to persist");
    State i02_25("i02_25", "activate SIM, got iccid");
    State i02_26("i02_26", "activate SIM, check initialization completed");
    State i02_27("i02_27", "activate SIM, wait initialization completed", true);
    State i02_28("i02_28", "activate SIM, initialization completed");

    State i03_01("i03_01", "init network access, get operator names");
    State i03_02("i03_02", "init network access, get operator selection");
    State i03_03("i03_03", "init network access, set operator selection");
    State i03_04("i03_04", "wait network access", true);
    State i03_05("i03_05", "init network access, set scan configuration");
    State i03_06("i03_06", "init network access, get registration status");


    State i04_01("i04_01", "init data service, get profile id");
    State i04_02("i04_02", "init data service, get profile id, set required");
    State i04_03("i04_03", "init data service, set profile id");
    State i04_04("i04_04", "init data service, get autoconnect state");
    State i04_05("i04_05", "init data service, get autoconnect state, set required");
    State i04_06("i04_06", "init data service, set autoconnect state");
    State i04_07("i04_07", "init data service, get gprs profile");
    State i04_08("i04_08", "init data service, get gprs profile, set required");
    State i04_09("i04_09", "init data service, set gprs profile");
    State i04_10("i04_10", "init data service, get registration status");

    State i05_01("i05_01", "init sms service, get storage config");
    State i05_02("i05_02", "init sms service, get storage config, set required");
    State i05_03("i05_03", "init sms service, set storage config");
    State i05_04("i05_04", "init sms service, get format");
    State i05_05("i05_05", "init sms service, get format, set required");
    State i05_06("i05_06", "init sms service, set format");
    State i05_07("i05_07", "init sms service, get event report config");
    State i05_08("i05_08", "init sms service, get event report config, set required");
    State i05_09("i05_09", "init sms service, set event report config");

    /*Operational states*/
    State o01("o01",       "wait user command", true);
    State o02("o02",       "get network registration details (periodic)");

    State o06_01("o06_01", "detect network access, deactivate autoconnect");
    State o06_02("o06_02", "detect network access, trigger shutdown");
    State o06_03("o06_03", "detect network access, wait shutdown complete");
    State o06_04("o06_04", "detect network access, deactivate echo mode");
    State o06_05("o06_05", "detect network access, get modem functionality");
    State o06_06("o06_06", "detect network access, get modem functionality, is full");
    State o06_07("o06_06", "detect network access, wait modem functionality");
    State o06_10("o06_10", "detect network access, get sim state");
    State o06_11("o06_11", "detect network access, sim ready");
    State o06_12("o06_12", "detect network access, sim pin required");
    State o06_13("o06_13", "detect network access, check iccid for automatic sim activation");
    State o06_14("o06_14", "detect network access, got matching iccid");
    State o06_15("o06_15", "detect network access, wait sim state");
    State o06_16("o06_16", "detect network access, activate SIM with stored PIN");
    State o06_17("o06_17", "detect network access, check SIM initialization completed");
    State o06_18("o06_18", "detect network access, wait SIM initialization completed");
    State o06_19("o06_19", "detect network access, SIM initialization completed");
    State o06_20("o06_20", "detect network access, list operators");

    State o10("o10",       "get SMS list");
    State o11("o11",       "read SMS");
    State o12_01("o12_01", "send SMS");
    State o12_02("o12_02", "send SMS, result returned");
    State o13("o13",       "delete sms");
    State o14_01("o14_01", "get SMS storage");
    State o14_02("o14_02", "get SMS storage, result returned");

    /*Shutdown states*/
    State x01_01("x01_01",  "trigger shutdown (user)");
    State x01_02("x01_02",  "wait shutdown complete");
    State x02_01("x02_01",  "trigger shutdown (internal)");
    State x02_02("x02_02",  "wait shutdown complete (internal)");

  MdmStatemachine::transition_list_t tl {

    /*s01: wait dbus*/
    { s01, new RegexEvent("DBus_Connection_up"), s01, new TF(tf_dbus_up), new PG },
    { s01, new RegexEvent("DBus_Connection_down"), s01, new TF(tf_wait_dbus), new PG },
    { s01, new TimeoutEvent(), s01, new TF(tf_wait_dbus), new GF(gf_dbus_offline) },
    { s01, new TimeoutEvent(), s04, new TF(tf_port_disabled), new GF(gf_modem_port_disabled) },
    { s01, new TimeoutEvent(), o06_04, new TF(tf_continue_getoperlist), new GF(gf_modem_port_open_continue_getoperlist) },
    { s01, new TimeoutEvent(), i01_01, new TF(tf_startinit), new GF(gf_modem_port_open) },
    { s01, new TimeoutEvent(), s02, new TF(tf_openport), new PG },
    /*s02: open modem port*/
    { s02, new TimeoutEvent(), o06_04, new TF(tf_continue_getoperlist), new GF(gf_modem_port_open_continue_getoperlist) },
    { s02, new TimeoutEvent(), i01_01, new TF(tf_startinit), new GF(gf_modem_port_open) },
    { s02, new TimeoutEvent(), s03, new TF(tf_modem_port_not_ready), new PG },
    /*s03: wait modem port*/
    { s03, new TimeoutEvent(), o06_04, new TF(tf_continue_getoperlist), new GF(gf_modem_port_open_continue_getoperlist) },
    { s03, new TimeoutEvent(), i01_01, new TF(tf_startinit), new GF(gf_modem_port_open) },
    { s03, new TimeoutEvent(), s01, new TF(tf_wait_modem_port_timeout), new GF(gf_wait_modem_port_abort) },
    { s03, new TimeoutEvent(), s03, new TF(tf_openport_retry), new PG },
    { s03, new DBusEvent("GetSmsStorage"), s03, new TF(tf_get_smsstorage_default), new PG },
    { s03, new DBusEvent("SetSmsStorage"), s03, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { s03, new DBusEvent("SetSmsReportConfig"), s03, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { s03, new DBusEvent("SetGprsAccess2"), s03, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { s03, new DBusEvent("ListSms"), s03, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s03, new DBusEvent("ReadSms"), s03, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s03, new DBusEvent("SendSms"), s03, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s03, new DBusEvent("DeleteSms"), s03, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s03, new DBusEvent("GetOperList"), s03, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s03, new DBusEvent("SetOper"), s03, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { s03, new DBusEvent("GetSimState"), s03, new TF(tf_dbus_getsimstate_unknown), new PG },
    { s03, new DBusEvent("ModemReset"), s01, new TF(tf_dbus_modem_reset_immediate), new PG },
    { s03, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    /*s04: wait modem port enable*/
    { s04, new DBusEvent("GetSmsStorage"), s04, new TF(tf_get_smsstorage_default), new PG },
    { s04, new DBusEvent("SetSmsStorage"), s04, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { s04, new DBusEvent("SetSmsReportConfig"), s04, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { s04, new DBusEvent("SetGprsAccess2"), s04, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { s04, new DBusEvent("ListSms"), s04, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s04, new DBusEvent("ReadSms"), s04, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s04, new DBusEvent("SendSms"), s04, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s04, new DBusEvent("DeleteSms"), s04, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s04, new DBusEvent("GetOperList"), s04, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s04, new DBusEvent("SetOper"), s04, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { s04, new DBusEvent("GetSimState"), s04, new TF(tf_dbus_getsimstate_unknown), new PG },
    { s04, new DBusEvent("ModemReset"), s04, new TF(tf_dbus_reject_modem_not_available), new PG },
    { s04, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { s04, new TimeoutEvent(), s04, new TF(tf_void_timeout), new PG },
    /*i00: unrecoverable initialization error*/
    { i00, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i00, new DBusEvent("GetSmsStorage"), i00, new TF(tf_get_smsstorage_default), new PG },
    { i00, new DBusEvent("SetSmsStorage"), i00, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i00, new DBusEvent("SetSmsReportConfig"), i00, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i00, new DBusEvent("SetGprsAccess2"), i00, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i00, new DBusEvent("ListSms"), i00, new TF(tf_dbus_reject_modem_init_fail), new PG },
    { i00, new DBusEvent("ReadSms"), i00, new TF(tf_dbus_reject_modem_init_fail), new PG },
    { i00, new DBusEvent("SendSms"), i00, new TF(tf_dbus_reject_modem_init_fail), new PG },
    { i00, new DBusEvent("DeleteSms"), i00, new TF(tf_dbus_reject_modem_init_fail), new PG },
    { i00, new DBusEvent("GetOperList"), i00, new TF(tf_dbus_reject_modem_init_fail), new PG },
    { i00, new DBusEvent("SetOper"), i00, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { i00, new DBusEvent("GetSimState"), i00, new TF(tf_dbus_getsimstate_unknown), new PG },
    { i00, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i00, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i00, new TimeoutEvent(), i00, new TF(tf_void_timeout), new PG },
    /*i01_01: init modem, deactivate echo mode*/
    { i01_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_01, new ModemEvent("^ATE0$"), i01_01, new NT, new PG },
    { i01_01, new ModemEvent("ERROR"), i00, new TF(tf_init_err), new PG },
    { i01_01, new ModemEvent("^OK$"), i01_02, new swrite("at+cmee=1", timer_at_cmd_short), new PG },
    { i01_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i01_02: init modem, set error format*/
    { i01_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_02, new ModemEvent("ERROR"), i00, new TF(tf_init_err), new PG },
    { i01_02, new ModemEvent("^OK$"), i01_03, new TF(tf_get_modeminfo), new PG },
    { i01_02, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i01_03: init modem, get info*/
    { i01_03, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_03, new ModemEvent("ERROR"), i01_04, new TF(tf_get_modeminfo_fail), new PG },
    { i01_03, new ModemEvent("^OK$"), i01_04, new TF(tf_get_modemidentity), new PG },
    { i01_03, new ModemEvent("^Revision: (?<revision>.*)"), i01_03, new TF(tf_modeminfo_revision), new PG },
    { i01_03, new ModemEvent("(?<line>.*)"), i01_03, new TF(tf_modeminfo_manufacturer), new GF(gf_have_no_manufacturer) },
    { i01_03, new ModemEvent("(?<line>.*)"), i01_03, new TF(tf_modeminfo_model), new PG },
    { i01_03, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i01_04: init modem, get identity*/
    { i01_04, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_04, new ModemEvent("ERROR"), i01_05, new TF(tf_get_modemidentity_fail), new PG },
    { i01_04, new ModemEvent("^OK$"), i01_05, new TF(tf_get_modem_function_state), new PG },
    { i01_04, new ModemEvent("(?<line>.*)"), i01_04, new TF(tf_modemidentity), new PG },
    { i01_04, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i01_05: init modem, get functionality state*/
    { i01_05, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_05, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), i01_06, new NT, new GF(gf_is_cfun_full) },
    { i01_05, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), i01_05, new NT, new PG },
    { i01_05, new ModemEvent("ERROR"), x02_01, new TF(tf_init_modem_function_state_fail), new GF(gf_wait_cfun_abort) },
    { i01_05, new ModemEvent("ERROR"), i01_07, new TF(tf_wait_cfun), new PG },
    { i01_05, new ModemEvent("^OK$"), i01_08, new TF(tf_set_modem_full_function), new PG },
    { i01_05, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i01_06: init modem, get functionality state, is full*/
    { i01_06, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_06, new ModemEvent("ERROR"), i01_09, new TF(tf_disable_status_reports), new PG },
    { i01_06, new ModemEvent("^OK$"), i01_09, new TF(tf_disable_status_reports), new PG },
    { i01_06, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i01_07: wait modem*/
    { i01_07, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_07, new DBusEvent("GetSmsStorage"), i01_07, new TF(tf_get_smsstorage_default), new PG },
    { i01_07, new DBusEvent("SetSmsStorage"), i01_07, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i01_07, new DBusEvent("SetSmsReportConfig"), i01_07, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i01_07, new DBusEvent("SetGprsAccess2"), i01_07, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i01_07, new DBusEvent("ListSms"), i01_07, new TF(tf_dbus_reject_modem_not_ready), new PG },
    { i01_07, new DBusEvent("ReadSms"), i01_07, new TF(tf_dbus_reject_modem_not_ready), new PG },
    { i01_07, new DBusEvent("SendSms"), i01_07, new TF(tf_dbus_reject_modem_not_ready), new PG },
    { i01_07, new DBusEvent("DeleteSms"), i01_07, new TF(tf_dbus_reject_modem_not_ready), new PG },
    { i01_07, new DBusEvent("GetOperList"), i01_07, new TF(tf_dbus_reject_modem_not_ready), new PG },
    { i01_07, new DBusEvent("SetOper"), i01_07, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { i01_07, new DBusEvent("GetSimState"), i01_07, new TF(tf_dbus_getsimstate_unknown), new PG },
    { i01_07, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i01_07, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i01_07, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), i01_09, new TF(tf_disable_status_reports), new GF(gf_is_cfun_full) },
    { i01_07, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), i01_08, new TF(tf_set_modem_full_function), new PG },
    { i01_07, new TimeoutEvent(), i01_05, new TF(tf_get_modem_function_state), new PG },
    /*i01_08: init modem, set full functional*/
    { i01_08, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_08, new ModemEvent("ERROR"), i01_07, new TF(tf_wait_cfun), new PG },
    { i01_08, new ModemEvent("^OK$"), i01_09, new TF(tf_disable_status_reports), new PG },
    { i01_08, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i01_09: init modem, disable status reports*/
    { i01_09, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i01_09, new ModemEvent("ERROR"), i02_01, new TF(tf_disable_status_reports_fail), new PG },
    { i01_09, new ModemEvent("^OK$"), i02_01, new TF(tf_get_simstate), new PG },
    { i01_09, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_01: init SIM, get state*/
    { i02_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_01, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_02, new NT, new GF(gf_cpin_ready) },
    { i02_01, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_03, new NT, new GF(gf_cpin_sim_pin) },
    { i02_01, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_04, new NT, new GF(gf_cpin_sim_puk) },
    { i02_01, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_05, new NT, new GF(gf_cpin_not_inserted) },
    { i02_01, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_06, new NT, new GF(gf_cpin_not_ready) },
    { i02_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_20, new TF(tf_activatesim_get_iccid), new GF(gf_cme_sim_pin_required) },
    { i02_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_12, new TF(tf_waitsim_puk), new GF(gf_cme_sim_puk_required) },
    { i02_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i02_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i02_01, new ModemEvent("ERROR"), i02_16, new TF(tf_sim_failure), new PG },
    { i02_01, new ModemEvent("^OK$"), i02_16, new TF(tf_sim_failure), new PG }, //unknown SIM state ?
    { i02_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_02: init SIM, got state, ready*/
    { i02_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_02, new ModemEvent("ERROR"), i02_26, new TF(tf_get_sim_initialization_state), new PG }, //got state, continue anyway
    { i02_02, new ModemEvent("^OK$"), i02_26, new TF(tf_get_sim_initialization_state), new PG },
    { i02_02, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_03: init SIM, got state, PIN required*/
    { i02_03, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_03, new ModemEvent("ERROR"), i02_20, new TF(tf_activatesim_get_iccid), new PG }, //got state, continue anyway
    { i02_03, new ModemEvent("^OK$"), i02_20, new TF(tf_activatesim_get_iccid), new PG },
    { i02_03, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_04: init SIM, got state, PUK required*/
    { i02_04, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_04, new ModemEvent("ERROR"), i02_12, new TF(tf_waitsim_puk), new PG }, //got state, continue anyway
    { i02_04, new ModemEvent("^OK$"), i02_12, new TF(tf_waitsim_puk), new PG },
    { i02_04, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_05: init SIM, got state, not inserted*/
    { i02_05, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_05, new ModemEvent("ERROR"), i02_14, new TF(tf_waitsim_removed), new PG }, //got state, continue anyway
    { i02_05, new ModemEvent("^OK$"), i02_14, new TF(tf_waitsim_removed), new PG },
    { i02_05, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_06: init SIM, got state, busy*/
    { i02_06, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_06, new ModemEvent("ERROR"), i02_15, new TF(tf_waitsim_busy), new PG }, //got state, continue anyway
    { i02_06, new ModemEvent("^OK$"), i02_15, new TF(tf_waitsim_busy), new PG },
    { i02_06, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_10: wait SIM, PIN required*/
    { i02_10, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_10, new DBusEvent("GetSmsStorage"), i02_10, new TF(tf_get_smsstorage_default), new PG },
    { i02_10, new DBusEvent("SetSmsStorage"), i02_10, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i02_10, new DBusEvent("SetSmsReportConfig"), i02_10, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i02_10, new DBusEvent("SetGprsAccess2"), i02_10, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i02_10, new DBusEvent("ListSms"), i02_10, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_10, new DBusEvent("ReadSms"), i02_10, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_10, new DBusEvent("SendSms"), i02_10, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_10, new DBusEvent("DeleteSms"), i02_10, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_10, new DBusEvent("GetOperList"), i02_10, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_10, new DBusEvent("SetOper"), i02_10, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { i02_10, new DBusEvent("GetSimState"), i02_11, new TF(tf_dbus_getsimstate_activation_required), new PG },
    { i02_10, new DBusEvent("SetSimPin"), i02_23, new TF(tf_dbus_setpin), new PG },
    { i02_10, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i02_10, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i02_10, new ModemEvent("^OK$"), i02_10, new TF(tf_void_timeout), new PG }, //OK for QPINC, see i02_11
    { i02_10, new TimeoutEvent(), i02_10, new TF(tf_void_timeout), new PG },
    /*i02_11: wait SIM, get PIN count*/
    { i02_11, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_11, new ModemEvent("^\\+QPINC: \"SC\",(?<s1>[0-9]+),(?<s2>[0-9]+)"), i02_10, new TF(tf_getsim_pincount), new PG },
    { i02_11, new ModemEvent("ERROR"), i02_10, new TF(tf_getsim_nopincount), new PG },
    { i02_11, new ModemEvent("^OK$"), i02_10, new TF(tf_getsim_nopincount), new PG },
    { i02_11, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_12: wait SIM, PUK required*/
    { i02_12, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_12, new DBusEvent("GetSmsStorage"), i02_12, new TF(tf_get_smsstorage_default), new PG },
    { i02_12, new DBusEvent("SetSmsStorage"), i02_12, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i02_12, new DBusEvent("SetSmsReportConfig"), i02_12, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i02_12, new DBusEvent("SetGprsAccess2"), i02_12, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i02_12, new DBusEvent("ListSms"), i02_12, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_12, new DBusEvent("ReadSms"), i02_12, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_12, new DBusEvent("SendSms"), i02_12, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_12, new DBusEvent("DeleteSms"), i02_12, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_12, new DBusEvent("GetOperList"), i02_12, new TF(tf_dbus_reject_sim_locked), new PG },
    { i02_12, new DBusEvent("SetOper"), i02_12, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { i02_12, new DBusEvent("GetSimState"), i02_13, new TF(tf_dbus_getsimstate_activation_required), new PG },
    { i02_12, new DBusEvent("SetSimPuk"), i02_23, new TF(tf_dbus_setpuk), new PG },
    { i02_12, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i02_12, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i02_12, new ModemEvent("^OK$"), i02_12, new TF(tf_void_timeout), new PG }, //OK for QPINC, see i02_13
    { i02_12, new TimeoutEvent(), i02_12, new TF(tf_void_timeout), new PG },
    /*i02_13: wait SIM, get PUK count*/
    { i02_13, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_13, new ModemEvent("^\\+QPINC: \"SC\",(?<s1>[0-9]+),(?<s2>[0-9]+)"), i02_12, new TF(tf_getsim_pukcount), new PG },
    { i02_13, new ModemEvent("ERROR"), i02_12, new TF(tf_getsim_nopukcount), new PG },
    { i02_13, new ModemEvent("^OK$"), i02_12, new TF(tf_getsim_nopukcount), new PG },
    { i02_13, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_14: wait SIM, not inserted*/
    { i02_14, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_14, new DBusEvent("GetSmsStorage"), i02_14, new TF(tf_get_smsstorage_default), new PG },
    { i02_14, new DBusEvent("SetSmsStorage"), i02_14, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i02_14, new DBusEvent("SetSmsReportConfig"), i02_14, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i02_14, new DBusEvent("SetGprsAccess2"), i02_14, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i02_14, new DBusEvent("ListSms"), i02_14, new TF(tf_dbus_reject_sim_not_inserted), new PG },
    { i02_14, new DBusEvent("ReadSms"), i02_14, new TF(tf_dbus_reject_sim_not_inserted), new PG },
    { i02_14, new DBusEvent("SendSms"), i02_14, new TF(tf_dbus_reject_sim_not_inserted), new PG },
    { i02_14, new DBusEvent("DeleteSms"), i02_14, new TF(tf_dbus_reject_sim_not_inserted), new PG },
    { i02_14, new DBusEvent("GetOperList"), i02_14, new TF(tf_dbus_reject_sim_not_inserted), new PG },
    { i02_14, new DBusEvent("SetOper"), i02_14, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { i02_14, new DBusEvent("GetSimState"), i02_14, new TF(tf_dbus_getsimstate_not_inserted), new PG },
    { i02_14, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i02_14, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i02_14, new TimeoutEvent(), i02_01, new TF(tf_get_simstate), new PG },
    /*i02_15: wait SIM, not ready*/
    { i02_15, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    /*ignore unexpected CFUN indication here (WAT31977)*/
    { i02_15, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), i02_15, new NT, new GF(gf_is_cfun_full) },
    { i02_15, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_27, new TF(tf_waitsim_init_complete), new GF(gf_cpin_ready) },
    { i02_15, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_20, new TF(tf_activatesim_get_iccid), new GF(gf_cpin_sim_pin) },
    { i02_15, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_12, new TF(tf_waitsim_puk), new GF(gf_cpin_sim_puk) },
    { i02_15, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cpin_not_inserted) },
    { i02_15, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_15, new NT, new GF(gf_cpin_not_ready) },
    { i02_15, new DBusEvent("GetSmsStorage"), i02_15, new TF(tf_get_smsstorage_default), new PG },
    { i02_15, new DBusEvent("SetSmsStorage"), i02_15, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i02_15, new DBusEvent("SetSmsReportConfig"), i02_15, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i02_15, new DBusEvent("SetGprsAccess2"), i02_15, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i02_15, new DBusEvent("ListSms"), i02_15, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_15, new DBusEvent("ReadSms"), i02_15, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_15, new DBusEvent("SendSms"), i02_15, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_15, new DBusEvent("DeleteSms"), i02_15, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_15, new DBusEvent("GetOperList"), i02_15, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_15, new DBusEvent("SetOper"), i02_15, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { i02_15, new DBusEvent("GetSimState"), i02_15, new TF(tf_dbus_getsimstate_not_ready), new PG },
    { i02_15, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i02_15, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i02_15, new TimeoutEvent(), i02_01, new TF(tf_waitsim_busy_timeout), new PG },
    /*i02_16: SIM failure*/
    { i02_16, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_16, new DBusEvent("GetSmsStorage"), i02_16, new TF(tf_get_smsstorage_default), new PG },
    { i02_16, new DBusEvent("SetSmsStorage"), i02_16, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i02_16, new DBusEvent("SetSmsReportConfig"), i02_16, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i02_16, new DBusEvent("SetGprsAccess2"), i02_16, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i02_16, new DBusEvent("ListSms"), i02_16, new TF(tf_dbus_reject_sim_failure), new PG },
    { i02_16, new DBusEvent("ReadSms"), i02_16, new TF(tf_dbus_reject_sim_failure), new PG },
    { i02_16, new DBusEvent("SendSms"), i02_16, new TF(tf_dbus_reject_sim_failure), new PG },
    { i02_16, new DBusEvent("DeleteSms"), i02_16, new TF(tf_dbus_reject_sim_failure), new PG },
    { i02_16, new DBusEvent("GetOperList"), i02_16, new TF(tf_dbus_reject_sim_failure), new PG },
    { i02_16, new DBusEvent("SetOper"), i02_16, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { i02_16, new DBusEvent("GetSimState"), i02_16, new TF(tf_dbus_getsimstate_failure), new PG },
    { i02_16, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i02_16, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i02_16, new ModemEvent("^OK$"), i02_16, new TF(tf_void_timeout), new PG }, //OK for previous command, e.g. COPS or QINISTAT
    { i02_16, new TimeoutEvent(), i02_16, new TF(tf_void_timeout), new PG },
    /*i02_20: activate SIM, get iccid to compare*/
    { i02_20, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_20, new ModemEvent("^\\+QCCID: (?<iccid>.*)"), i02_21, new NT, new GF(gf_qccid_match_storage)},
    { i02_20, new ModemEvent("^\\+QCCID: (?<iccid>.*)"), i02_20, new NT, new PG},
    { i02_20, new ModemEvent("ERROR"), i02_10, new TF(tf_waitsim_pin), new PG }, //ICCID not available
    { i02_20, new ModemEvent("^OK$"), i02_10, new TF(tf_waitsim_pin), new PG }, //ICCID not matching
    { i02_20, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_21: activate SIM, got iccid, match storage*/
    { i02_21, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_21, new ModemEvent("ERROR"), i02_22, new TF(tf_setsimpin_from_storage), new PG },
    { i02_21, new ModemEvent("^OK$"), i02_22, new TF(tf_setsimpin_from_storage), new PG },
    { i02_21, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_22: activate SIM, set PIN from storage*/
    { i02_22, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_22, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i02_22, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i02_22, new ModemEvent("ERROR"), i02_01, new TF(tf_setsimpin_from_storage_fail), new PG },
    { i02_22, new ModemEvent("^OK$"), i02_27, new TF(tf_waitsim_init_complete), new PG },
    { i02_22, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_23: activate SIM, set PIN/PUK from user*/
    { i02_23, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { i02_23, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_dbus_setpin_cme_sim_not_inserted), new GF(gf_cme_sim_not_inserted) },
    { i02_23, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_dbus_setpin_cme_sim_busy), new GF(gf_cme_sim_busy) },
    { i02_23, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_dbus_setpin_cme_sim_failure), new GF(gf_cme_sim_failure) },
    { i02_23, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_dbus_setpin_cme_wrong_password), new GF(gf_cme_sim_pin_required) },
    { i02_23, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_dbus_setpin_cme_wrong_password), new GF(gf_cme_sim_puk_required) },
    { i02_23, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_dbus_setpin_cme_wrong_password), new GF(gf_cme_incorrect_password) },
    { i02_23, new ModemEvent("ERROR"), i02_01, new TF(tf_dbus_setpin_err), new PG },
    { i02_23, new ModemEvent("^OK$"), i02_24, new TF(tf_activatesim_get_iccid), new PG },
    { i02_23, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*i02_24: activate SIM, get iccid to persist*/
    { i02_24, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { i02_24, new ModemEvent("^\\+QCCID: (?<iccid>.*)"), i02_25, new TF(tf_set_iccid_to_storage), new PG },
    { i02_24, new ModemEvent("ERROR"), i02_26, new TF(tf_dbus_setpin_ok_not_persistent), new PG }, //ICCID not available
    { i02_24, new ModemEvent("^OK$"), i02_26, new TF(tf_dbus_setpin_ok_not_persistent), new PG }, //ICCID not detected?
    { i02_24, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*i02_25: activate SIM, got iccid*/
    { i02_25, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { i02_25, new ModemEvent("ERROR"), i02_26, new TF(tf_dbus_setpin_ok), new PG },
    { i02_25, new ModemEvent("^OK$"), i02_26, new TF(tf_dbus_setpin_ok), new PG },
    { i02_25, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*i02_26: activate SIM, check initialization completed*/
    { i02_26, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_26, new ModemEvent("ERROR"), i02_16, new TF(tf_sim_failure), new PG },
    { i02_26, new ModemEvent("^\\+QINISTAT: (?<state>.*)"), i02_28, new TF(tf_qinistat_sms_enable), new GF(gf_qinistat_complete) },
    { i02_26, new ModemEvent("^\\+QINISTAT: (?<state>.*)"), i02_28, new TF(tf_siminit_abort), new GF(gf_qinistat_abort_siminit) },
    { i02_26, new ModemEvent("^\\+QINISTAT: (?<state>.*)"), i02_26, new NT, new PG },
    { i02_26, new ModemEvent("^OK$"), i00, new TF(tf_init_err), new GF(gf_siminit_timeout) },
    { i02_26, new ModemEvent("^OK$"), i02_27, new TF(tf_waitsim_init_complete), new PG },
    { i02_26, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i02_27: activate SIM, wait initialization completed*/
    { i02_27, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_27, new ModemEvent("^OK$"), i02_27, new NT, new PG }, //OK e.g. for QINISTAT, see i02_26
    { i02_27, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_27, new NT, new GF(gf_cpin_ready) },
    { i02_27, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_16, new TF(tf_sim_failure), new PG },
      //PB DONE indicates SIM initialization completed -> check again now, don't wait for timeout 
    { i02_27, new ModemEvent("^\\+QIND: PB DONE"), i02_26, new TF(tf_get_sim_initialization_state), new PG },
    { i02_27, new DBusEvent("GetSmsStorage"), i02_27, new TF(tf_get_smsstorage_default), new PG },
    { i02_27, new DBusEvent("SetSmsStorage"), i02_27, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i02_27, new DBusEvent("SetSmsReportConfig"), i02_27, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i02_27, new DBusEvent("SetGprsAccess2"), i02_27, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i02_27, new DBusEvent("ListSms"), i02_27, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_27, new DBusEvent("ReadSms"), i02_27, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_27, new DBusEvent("SendSms"), i02_27, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_27, new DBusEvent("DeleteSms"), i02_27, new TF(tf_dbus_reject_sim_not_ready), new PG },
    { i02_27, new DBusEvent("GetOperList"), o06_01, new TF(tf_getoperlist_request), new PG },
    { i02_27, new DBusEvent("SetOper"), i02_27, new TF(tf_dbus_setoper_to_parameter_storage), new PG },
    { i02_27, new DBusEvent("GetSimState"), i02_27, new TF(tf_dbus_getsimstate_ready), new PG },
    { i02_27, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i02_27, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i02_27, new TimeoutEvent(), i02_26, new TF(tf_get_sim_initialization_state), new PG },
    /*i02_28: activate SIM, initialization completed*/
    { i02_28, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i02_28, new ModemEvent("ERROR"), i03_01, new TF(tf_init_sim_complete_get_opernames), new GF(gf_get_opernames_required) },
    { i02_28, new ModemEvent("ERROR"), i03_02, new TF(tf_init_sim_complete_get_operselection), new PG },
    { i02_28, new ModemEvent("^OK$"), i03_01, new TF(tf_init_sim_complete_get_opernames), new GF(gf_get_opernames_required) },
    { i02_28, new ModemEvent("^OK$"), i03_02, new TF(tf_init_sim_complete_get_operselection), new PG },
    { i02_28, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i03_01: init network access, get operator names*/
    { i03_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i03_01, new ModemEvent("^\\+COPN: \"(?<id>[0-9]+)\",\"(?<name>.*)\""), i03_01, new TF(tf_readoperatorname), new PG },
    { i03_01, new ModemEvent("ERROR"), i03_02, new TF(tf_get_opernames_fail), new PG },
    { i03_01, new ModemEvent("^OK$"), i03_02, new TF(tf_get_operselection), new PG },
    { i03_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i03_02: init network access, get operator selection*/
    { i03_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i03_02, new ModemEvent("^\\+COPS: 1"), i03_02, new NT, new PG }, //manual network access, initialization should set correct network
    { i03_02, new ModemEvent("^\\+COPS: 2"), i03_02, new NT, new PG }, //network access blocked, initialization should fail with SIM_FAILURE
    { i03_02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i03_02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i03_02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i03_02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i03_02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i03_02, new ModemEvent("ERROR"), i03_04, new TF(tf_operselection_fail), new PG },
    { i03_02, new ModemEvent("^OK$"), i03_03, new TF(tf_set_operselection), new GF(set_operselection_required) },
    { i03_02, new ModemEvent("^OK$"), i03_06, new TF(tf_get_oper_regstate), new GF(gf_is_oper_selection_manual) },
    { i03_02, new ModemEvent("^OK$"), i03_05, new TF(tf_set_nwscan_config), new PG },
    { i03_02, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i03_03: init network access, set operator selection*/
    { i03_03, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i03_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i03_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i03_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i03_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i03_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i03_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { i03_03, new ModemEvent("ERROR"), i03_04, new TF(tf_operselection_fail), new PG },
    { i03_03, new ModemEvent("^OK$"), i03_06, new TF(tf_set_operator_selection_manual_ok), new GF(gf_is_oper_selection_manual) },
    { i03_03, new ModemEvent("^OK$"), i03_05, new TF(tf_set_operator_selection_automatic_ok), new PG },
    { i03_03, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i03_04: wait network access*/
    { i03_04, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i03_04, new ModemEvent("^\\+CPIN: (?<state>.*)"), i03_04, new NT, new GF(gf_cpin_ready) },
    { i03_04, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_16, new TF(tf_sim_failure), new PG },
    { i03_04, new DBusEvent("GetSmsStorage"), i03_04, new TF(tf_get_smsstorage_default), new PG },
    { i03_04, new DBusEvent("SetSmsStorage"), i03_04, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new PG },
    { i03_04, new DBusEvent("SetSmsReportConfig"), i03_04, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new PG },
    { i03_04, new DBusEvent("SetGprsAccess2"), i03_04, new TF(tf_dbus_setgprsaccess_to_parameter_storage), new PG },
    { i03_04, new DBusEvent("ListSms"), i03_04, new TF(tf_dbus_reject_sim_without_sms), new GF(gf_sim_without_sms_support) },
    { i03_04, new DBusEvent("ListSms"), o10, new TF(tf_dbus_listsms), new PG },
    { i03_04, new DBusEvent("ReadSms"), i03_04, new TF(tf_dbus_reject_sim_without_sms), new GF(gf_sim_without_sms_support) },
    { i03_04, new DBusEvent("ReadSms"), o11, new TF(tf_dbus_readsms), new PG },
    { i03_04, new DBusEvent("SendSms"), i03_04, new TF(tf_dbus_reject_sim_without_sms), new GF(gf_sim_without_sms_support) },
    { i03_04, new DBusEvent("SendSms"), i03_04, new TF(tf_dbus_reject_net_no_service), new PG },
    { i03_04, new DBusEvent("DeleteSms"), i03_04, new TF(tf_dbus_reject_sim_without_sms), new GF(gf_sim_without_sms_support) },
    { i03_04, new DBusEvent("DeleteSms"), o13, new TF(tf_dbus_deletesms), new PG },
    { i03_04, new DBusEvent("GetOperList"), o06_01, new TF(tf_getoperlist_request), new PG },
    { i03_04, new DBusEvent("SetOper"), i03_03, new TF(tf_dbus_setoper_to_modem), new PG },
    { i03_04, new DBusEvent("GetSimState"), i03_04, new TF(tf_dbus_getsimstate_ready), new PG },
    { i03_04, new DBusEvent("SetSimPin"), i03_04, new TF(tf_dbus_setpin_ready), new PG },
    { i03_04, new DBusEvent("SetSimPuk"), i03_04, new TF(tf_dbus_setpin_ready), new PG },
    { i03_04, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { i03_04, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { i03_04, new TimeoutEvent(), i03_03, new TF(tf_set_operselection), new PG },
    /*i03_05: init network access, set scan configuration*/
    { i03_05, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i03_05, new ModemEvent("ERROR"), i03_06, new TF(tf_set_nwscan_config_fail), new PG },
      //INFO: network scan configuration takes effect immediately -> continue with data service initialization
    { i03_05, new ModemEvent("^OK$"), i03_06, new TF(tf_get_oper_regstate), new PG },
    { i03_05, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i03_06: init network access, get registration status*/
    { i03_06, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i03_06, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i03_06, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i03_06, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i03_06, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i03_06, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i03_06, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { i03_06, new ModemEvent("ERROR"), i04_01, new TF(tf_get_oper_regstate_fail), new PG },
    { i03_06, new ModemEvent("^OK$"), i04_01, new TF(tf_get_rmnet_profileid), new PG },
    { i03_06, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_01: init data service, get profile id*/
    { i04_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_01, new ModemEvent("^\\+QCFG: \"rmnet/profileid\",(?<id>[0-9])"), i04_02, new NT, new GF(gf_set_rmnet_profileid_required) },
    { i04_01, new ModemEvent("^\\+QCFG: \"rmnet/profileid\",(?<id>[0-9])"), i04_01, new NT, new PG },
    { i04_01, new ModemEvent("ERROR"), i04_04, new TF(tf_get_rmnet_profileid_fail), new PG },
    { i04_01, new ModemEvent("^OK$"), i04_04, new TF(tf_get_rmnet_autoconnect), new PG },
    { i04_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_02: init data service, get profile id, set required*/
    { i04_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_02, new ModemEvent("ERROR"), i04_03, new TF(tf_set_rmnet_profileid), new PG },
    { i04_02, new ModemEvent("^OK$"), i04_03, new TF(tf_set_rmnet_profileid), new PG },
    { i04_02, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_03: init data service, set profile id*/
    { i04_03, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_03, new ModemEvent("ERROR"), i04_04, new TF(tf_set_rmnet_profileid_fail), new PG },
    { i04_03, new ModemEvent("^OK$"), x02_01, new TF(tf_set_rmnet_profileid_ok), new PG },
    { i04_03, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_04: init data service, get autoconnect state*/
    { i04_04, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_04, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",(?<status>[0-9]),(?<linkprot>[0-9]),(?<dtrset>[0-9])"), i04_05, new TF(tf_get_rmnet_autoconnect_with_dtrset), new GF(gf_set_rmnet_autoconnect_required2) },
    { i04_04, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",(?<status>[0-9]),(?<linkprot>[0-9]),(?<dtrset>[0-9])"), i04_04, new TF(tf_get_rmnet_autoconnect_with_dtrset), new PG },
    { i04_04, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",(?<status>[0-9]),(?<linkprot>[0-9])"), i04_05, new TF(tf_get_rmnet_autoconnect_no_dtrset), new GF(gf_set_rmnet_autoconnect_required) },
    { i04_04, new ModemEvent("^\\+QCFG: \"rmnet/autoconnect\",(?<status>[0-9]),(?<linkprot>[0-9])"), i04_04, new TF(tf_get_rmnet_autoconnect_no_dtrset), new PG },
    { i04_04, new ModemEvent("ERROR"), i00, new TF(tf_init_err), new PG },
    { i04_04, new ModemEvent("^OK$"), i04_07, new TF(tf_get_gprsprofile), new PG },
    { i04_04, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_05: init data service, get autoconnect state, set required*/
    { i04_05, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_05, new ModemEvent("ERROR"), i04_06, new TF(tf_autoconnect_enable), new PG },
    { i04_05, new ModemEvent("^OK$"), i04_06, new TF(tf_autoconnect_enable), new PG },
    { i04_05, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_06: init data service, set autoconnect state*/
    { i04_06, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_06, new ModemEvent("ERROR"), i00, new TF(tf_init_err), new PG },
    { i04_06, new ModemEvent("^OK$"), x02_01, new TF(tf_rmnet_autoconnect_config_ok), new PG },
    { i04_06, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_07: init data service, get gprs profile*/
    { i04_07, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_07, new ModemEvent("^\\+QICSGP: (?<type>[0-9]+),\"(?<apn>.*)\",\"(?<user>.*)\",\"(?<pass>.*)\",(?<auth>[0-9]+)"), i04_08, new NT, new GF(gf_set_gprs_profile_required) },
    { i04_07, new ModemEvent("^\\+QICSGP: (?<type>[0-9]+),\"(?<apn>.*)\",\"(?<user>.*)\",\"(?<pass>.*)\",(?<auth>[0-9]+)"), i04_07, new NT, new PG },
    { i04_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i04_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i04_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i04_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i04_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i04_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { i04_07, new ModemEvent("ERROR"), i04_10, new TF(tf_gprsprofile_config_fail), new PG },
    { i04_07, new ModemEvent("^OK$"), i04_10, new TF(tf_get_gprs_regstate), new PG },
    { i04_07, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_08: init data service, get gprs profile, set required*/
    { i04_08, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_08, new ModemEvent("ERROR"), i04_09, new TF(tf_set_gprsprofile), new PG },
    { i04_08, new ModemEvent("^OK$"), i04_09, new TF(tf_set_gprsprofile), new PG },
    { i04_08, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_09: init data service, set gprs profile*/
    { i04_09, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i04_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i04_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i04_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i04_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i04_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { i04_09, new ModemEvent("ERROR"), i04_10, new TF(tf_gprsprofile_config_fail), new PG },
    { i04_09, new ModemEvent("^OK$"), x02_01, new TF(tf_gprsprofile_config_ok), new PG },
    { i04_09, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i04_10: init data service, get registration status*/
    { i04_10, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i04_10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i04_10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i04_10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i04_10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i04_10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i04_10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
      //skip SMS initialisation when no SMS support
    { i04_10, new ModemEvent("ERROR"), o02, new TF(tf_get_gprs_regstate_fail_skip_smsinit), new GF(gf_sim_without_sms_support) },
    { i04_10, new ModemEvent("ERROR"), i05_01, new TF(tf_get_gprs_regstate_fail), new PG },
    { i04_10, new ModemEvent("^OK$"), o02, new TF(tf_query_reg_state_full), new GF(gf_sim_without_sms_support) },
    { i04_10, new ModemEvent("^OK$"), i05_01, new TF(tf_get_sms_storage_config), new PG },
    { i04_10, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_01: init SMS service, get storage config*/
    { i05_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_01, new ModemEvent("^\\+CPMS: \"(?<mem1>.*)\",(?<used1>[0-9]+),(?<total1>[0-9]+),\"(?<mem2>.*)\",(?<used2>[0-9]+),(?<total2>[0-9]+),\"(?<mem3>.*)\",(?<used3>[0-9]+),(?<total3>[0-9]+)"), i05_03, new NT, new GF(gf_set_sms_storage_config_required) },
    { i05_01, new ModemEvent("^\\+CPMS: \"(?<mem1>.*)\",(?<used1>[0-9]+),(?<total1>[0-9]+),\"(?<mem2>.*)\",(?<used2>[0-9]+),(?<total2>[0-9]+),\"(?<mem3>.*)\",(?<used3>[0-9]+),(?<total3>[0-9]+)"), i05_01, new NT, new PG },
    { i05_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i05_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i05_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i05_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i05_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i05_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { i05_01, new ModemEvent("ERROR"), i05_04, new TF(tf_sms_storage_config_fail), new PG },
    { i05_01, new ModemEvent("^OK$"), i05_04, new TF(tf_get_sms_format), new PG },
    { i05_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_02: init SMS service, get storage config, set required*/
    { i05_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_02, new ModemEvent("ERROR"), i05_03, new TF(tf_set_sms_storage_config), new PG },
    { i05_02, new ModemEvent("^OK$"), i05_03, new TF(tf_set_sms_storage_config), new PG },
    { i05_02, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_03: init SMS service, set storage config*/
    { i05_03, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_03, new ModemEvent("^\\+CPMS: (?<used1>[0-9]+),(?<total1>[0-9]+),(?<used2>[0-9]+),(?<total2>[0-9]+),(?<used3>[0-9]+),(?<total3>[0-9]+)"), i05_03, new NT, new PG },
    { i05_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i05_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i05_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i05_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i05_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i05_03, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { i05_03, new ModemEvent("ERROR"), i05_04, new TF(tf_sms_storage_config_fail), new PG },
    { i05_03, new ModemEvent("^OK$"), i05_04, new TF(tf_get_sms_format), new PG },
    { i05_03, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_04: init SMS service, get format*/
    { i05_04, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_04, new ModemEvent("^\\+CMGF: (?<mode>[0-9]+)"), i05_05, new NT, new GF(gf_set_sms_format_required) },
    { i05_04, new ModemEvent("^\\+CMGF: (?<mode>[0-9]+)"), i05_04, new NT, new PG },
    { i05_04, new ModemEvent("ERROR"), i05_07, new TF(tf_sms_format_config_fail), new PG },
    { i05_04, new ModemEvent("^OK$"), i05_07, new TF(tf_get_sms_report_config), new PG },
    { i05_04, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_05: init SMS service, get format, set required*/
    { i05_05, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_05, new ModemEvent("ERROR"), i05_06, new TF(tf_set_sms_format), new PG },
    { i05_05, new ModemEvent("^OK$"), i05_06, new TF(tf_set_sms_format), new PG },
    { i05_05, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_06: init SMS service, set format*/
    { i05_06, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_06, new ModemEvent("ERROR"), i05_07, new TF(tf_sms_format_config_fail), new PG },
    { i05_06, new ModemEvent("^OK$"), i05_07, new TF(tf_get_sms_report_config), new PG },
    { i05_06, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_07: init SMS service, get event report config*/
    { i05_07, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_07, new ModemEvent("^\\+CNMI: (?<mode>[0-9]+),(?<mt>[0-9]+),(?<bm>[0-9]+),(?<ds>[0-9]+),(?<bfr>[0-9]+)"), i05_08, new NT, new GF(gf_set_sms_report_config_required) },
    { i05_07, new ModemEvent("^\\+CNMI: (?<mode>[0-9]+),(?<mt>[0-9]+),(?<bm>[0-9]+),(?<ds>[0-9]+),(?<bfr>[0-9]+)"), i05_07, new NT, new PG },
    { i05_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i05_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i05_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i05_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i05_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i05_07, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { i05_07, new ModemEvent("ERROR"), o02, new TF(tf_sms_report_config_fail), new PG },
    { i05_07, new ModemEvent("^OK$"), o02, new TF(tf_query_reg_state_full), new PG },
    { i05_07, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_08: init SMS service, get event report config, set required*/
    { i05_08, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_08, new ModemEvent("ERROR"), i05_09, new TF(tf_set_sms_report_config), new PG },
    { i05_08, new ModemEvent("^OK$"), i05_09, new TF(tf_set_sms_report_config), new PG },
    { i05_08, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*i05_09: init SMS service, set event report config*/
    { i05_09, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { i05_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { i05_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { i05_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { i05_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { i05_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { i05_09, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { i05_09, new ModemEvent("ERROR"), o02, new TF(tf_sms_report_config_fail), new PG },
    { i05_09, new ModemEvent("^OK$"), o02, new TF(tf_query_reg_state_full), new PG },
    { i05_09, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },

    /*o01: wait user command*/
    { o01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { o01, new ModemEvent("^\\+CPIN: (?<state>.*)"), o01, new NT, new GF(gf_cpin_ready) },
    { o01, new ModemEvent("^\\+CPIN: (?<state>.*)"), i02_16, new TF(tf_sim_failure), new PG },
    { o01, new DBusEvent("ListSms"), o01, new TF(tf_dbus_reject_sim_without_sms), new GF(gf_sim_without_sms_support) },
    { o01, new DBusEvent("ListSms"), o10, new TF(tf_dbus_listsms), new PG },
    { o01, new DBusEvent("ReadSms"), o01, new TF(tf_dbus_reject_sim_without_sms), new GF(gf_sim_without_sms_support) },
    { o01, new DBusEvent("ReadSms"), o11, new TF(tf_dbus_readsms), new PG },
    { o01, new DBusEvent("SendSms"), o01, new TF(tf_dbus_reject_sim_without_sms), new GF(gf_sim_without_sms_support) },
    { o01, new DBusEvent("SendSms"), o12_01, new TF(tf_dbus_sendsms_head), new PG },
    { o01, new DBusEvent("DeleteSms"), o01, new TF(tf_dbus_reject_sim_without_sms), new GF(gf_sim_without_sms_support) },
    { o01, new DBusEvent("DeleteSms"), o13, new TF(tf_dbus_deletesms), new PG },
    { o01, new DBusEvent("GetSmsStorage"), o01, new TF(tf_get_smsstorage_default), new GF(gf_sim_without_sms_support) },
    { o01, new DBusEvent("GetSmsStorage"), o14_01, new TF(tf_dbus_getsmsstorage), new PG },
    { o01, new DBusEvent("SetSmsStorage"), o01, new TF(tf_dbus_setsmsstorage_to_parameter_storage), new GF(gf_sim_without_sms_support) },
    { o01, new DBusEvent("SetSmsStorage"), i05_03, new TF(tf_dbus_setsmsstorage_to_modem), new PG },
    { o01, new DBusEvent("SetSmsReportConfig"), o01, new TF(tf_dbus_setsmsreportconfig_to_parameter_storage), new GF(gf_sim_without_sms_support) },
    { o01, new DBusEvent("SetSmsReportConfig"), i05_09, new TF(tf_dbus_setsmsreportconfig_to_modem), new PG },
    { o01, new DBusEvent("SetGprsAccess2"), i04_09, new TF(tf_dbus_setgprsaccess_to_modem), new PG },
    { o01, new DBusEvent("GetOperList"), o06_01, new TF(tf_getoperlist_request), new PG },
    { o01, new DBusEvent("SetOper"), i03_03, new TF(tf_dbus_setoper_to_modem), new PG },
    { o01, new DBusEvent("GetSimState"), o01, new TF(tf_dbus_getsimstate_ready), new PG },
    { o01, new DBusEvent("SetSimPin"), o01, new TF(tf_dbus_setpin_ready), new PG },
    { o01, new DBusEvent("SetSimPuk"), o01, new TF(tf_dbus_setpin_ready), new PG },
    { o01, new DBusEvent("ModemReset"), x01_01, new TF(tf_dbus_modem_reset), new PG },
    { o01, new DBusEvent("SetPortState"), s01, new TF(tf_dbus_setportstate), new PG },
    { o01, new TimeoutEvent(), o02, new TF(tf_query_reg_state_full), new PG },
    /*o02: get network registration details (periodic)*/
    { o02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { o02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_14, new TF(tf_waitsim_removed), new GF(gf_cme_sim_not_inserted) },
    { o02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_15, new TF(tf_waitsim_busy), new GF(gf_cme_sim_busy) },
    { o02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_16, new TF(tf_sim_failure), new GF(gf_cme_sim_failure) },
    { o02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_pin_required) },
    { o02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i02_01, new TF(tf_sim_activation_required), new GF(gf_cme_sim_puk_required) },
    { o02, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i03_04, new TF(tf_no_network_service), new GF(gf_cme_no_network_service) },
    { o02, new ModemEvent("ERROR"), o01, new TF(tf_renew_wwan_address), new GF(gf_renew_wwan_address) },
    { o02, new ModemEvent("ERROR"), x02_01, new TF(tf_gprs_autoconnect_timeout), new GF(gf_gprs_autoconnect_abort) },
    { o02, new ModemEvent("ERROR"), i03_04, new TF(tf_no_network_service), new GF(gf_err_no_network_service) },
    { o02, new ModemEvent("ERROR"), o01, new TF(tf_get_registration_status_failed), new PG },
    { o02, new ModemEvent("^OK$"), o01, new TF(tf_renew_wwan_address), new GF(gf_renew_wwan_address) },
    { o02, new ModemEvent("^OK$"), x02_01, new TF(tf_gprs_autoconnect_timeout), new GF(gf_gprs_autoconnect_abort) },
    { o02, new ModemEvent("^OK$"), i03_04, new TF(tf_no_network_service), new GF(gf_err_no_network_service) },
    { o02, new ModemEvent("^OK$"), o01, new TF(tf_wait_user_command), new PG },
    { o02, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*o06_01: detect network access, deactivate autoconnect*/
    { o06_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_01, new ModemEvent("ERROR"), o02, new TF(tf_getoperlist_autoconnect_disable_fail), new PG },
    { o06_01, new ModemEvent("^OK$"), o06_02, new TF(tf_trigger_modem_shutdown), new PG },
    { o06_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_02: detect network access, trigger shutdown*/
    { o06_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_02, new ModemEvent("ERROR"), i04_06, new TF(tf_getoperlist_shutdown_fail), new PG },
    { o06_02, new ModemEvent("^OK$"), o06_03, new TF(tf_shutdown_ok), new PG },
    { o06_02, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_03: detect network access, wait shutdown complete*/
    { o06_03, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_03, new ModemEvent("^POWERED DOWN$"), s01, new TF(tf_getoperlist_shutdown_finish), new PG },
    { o06_03, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_04: detect network access, deactivate echo mode*/
    { o06_04, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_04, new ModemEvent("^ATE0$"), o06_04, new NT, new PG },
    { o06_04, new ModemEvent("ERROR"), i04_06, new TF(tf_getoperlist_init_fail), new PG },
    { o06_04, new ModemEvent("^OK$"), o06_05, new TF(tf_get_modem_function_state), new PG },
    { o06_04, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_05: detect network access, get modem functionality*/
    { o06_05, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_05, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), o06_06, new NT, new GF(gf_is_cfun_full) },
    { o06_05, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), o06_05, new NT, new PG },
    { o06_05, new ModemEvent("ERROR"), o06_07, new TF(tf_wait_cfun), new PG },
    { o06_05, new ModemEvent("^OK$"), o06_07, new TF(tf_wait_cfun), new PG },
    { o06_05, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_06: detect network access, get modem functionality, is full*/
    { o06_06, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_06, new ModemEvent("ERROR"), o06_10, new TF(tf_get_simstate), new PG },
    { o06_06, new ModemEvent("^OK$"), o06_10, new TF(tf_get_simstate), new PG },
    { o06_06, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_07: detect network access, wait modem functionality*/
    { o06_07, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_07, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), o06_10, new TF(tf_get_simstate), new GF(gf_is_cfun_full) },
    { o06_07, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), o06_06, new NT, new PG },
    { o06_07, new TimeoutEvent(), i04_06, new TF(tf_getoperlist_init_fail), new PG },
    /*o06_10: detect network access, get sim state*/
    { o06_10, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_10, new ModemEvent("^\\+CPIN: (?<state>.*)"), o06_11, new NT, new GF(gf_cpin_ready) },
    { o06_10, new ModemEvent("^\\+CPIN: (?<state>.*)"), o06_12, new NT, new GF(gf_cpin_sim_pin) },
    { o06_10, new ModemEvent("^\\+CPIN: (?<state>.*)"), o06_10, new TF(tf_getoperlist_wrong_simstate), new GF(gf_cpin_sim_puk) },
    { o06_10, new ModemEvent("^\\+CPIN: (?<state>.*)"), o06_10, new TF(tf_getoperlist_wrong_simstate), new GF(gf_cpin_not_inserted) },
    { o06_10, new ModemEvent("^\\+CPIN: (?<state>.*)"), o06_15, new NT, new GF(gf_cpin_not_ready) },
    { o06_10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), o06_16, new TF(tf_setsimpin_from_storage), new GF(gf_cme_sim_pin_required) },
    { o06_10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), o06_15, new TF(tf_getoperlist_waitsim), new GF(gf_cme_sim_busy) },
    { o06_10, new ModemEvent("ERROR"), i04_06, new TF(tf_getoperlist_simfail), new PG },
    { o06_10, new ModemEvent("^OK$"), i04_06, new TF(tf_getoperlist_simfail), new PG }, //unhandled event, to be fixed
    { o06_10, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_11: detect network access, sim ready*/
    { o06_11, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_11, new ModemEvent("ERROR"), o06_17, new TF(tf_get_sim_initialization_state), new PG }, //got state, continue anyway
    { o06_11, new ModemEvent("^OK$"), o06_17, new TF(tf_get_sim_initialization_state), new PG },
    { o06_11, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_12: detect network access, sim pin required*/
    { o06_12, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_12, new ModemEvent("ERROR"), o06_13, new TF(tf_activatesim_get_iccid), new PG },
    { o06_12, new ModemEvent("^OK$"), o06_13, new TF(tf_activatesim_get_iccid), new PG },
    { o06_12, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_13: detect network access, check iccid for automatic sim activation*/
    { o06_13, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_13, new ModemEvent("^\\+QCCID: (?<iccid>.*)"), o06_14, new NT, new GF(gf_qccid_match_storage)},
    { o06_13, new ModemEvent("^\\+QCCID: (?<iccid>.*)"), o06_13, new NT, new PG},
    { o06_13, new ModemEvent("ERROR"), i04_06, new TF(tf_getoperlist_simfail), new PG }, //ICCID not available
    { o06_13, new ModemEvent("^OK$"), i04_06, new TF(tf_getoperlist_simfail), new PG }, //ICCID not matching
    { o06_13, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_14: detect network access, got matching iccid*/
    { o06_14, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_14, new ModemEvent("ERROR"), o06_16, new TF(tf_setsimpin_from_storage), new PG },
    { o06_14, new ModemEvent("^OK$"), o06_16, new TF(tf_setsimpin_from_storage), new PG },
    { o06_14, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_15: detect network access, wait sim state*/
    { o06_15, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    /*ignore unexpected CFUN indication here (WAT31977)*/
    { o06_15, new ModemEvent("^\\+CFUN: (?<state>[0-9]+)"), o06_15, new NT, new GF(gf_is_cfun_full) },
    { o06_15, new ModemEvent("^\\+CPIN: (?<state>.*)"), o06_10, new TF(tf_get_simstate), new PG },
    { o06_15, new TimeoutEvent(), i04_06, new TF(tf_getoperlist_siminit_timeout), new PG },
    /*o06_16: detect network access, activate SIM with stored PIN*/
    { o06_16, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_16, new ModemEvent("ERROR"), i04_06, new TF(tf_getoperlist_simfail), new PG },
    { o06_16, new ModemEvent("^OK$"), o06_18, new TF(tf_waitsim_init_complete), new PG },
    { o06_16, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_17: detect network access, check SIM initialization completed*/
    { o06_17, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_17, new ModemEvent("ERROR"), i04_06, new TF(tf_getoperlist_simfail), new PG },
    { o06_17, new ModemEvent("^\\+QINISTAT: (?<state>.*)"), o06_19, new NT, new GF(gf_qinistat_complete) },
    { o06_17, new ModemEvent("^\\+QINISTAT: (?<state>.*)"), o06_19, new NT, new GF(gf_qinistat_abort_siminit) },
    { o06_17, new ModemEvent("^\\+QINISTAT: (?<state>.*)"), o06_17, new NT, new PG },
    { o06_17, new ModemEvent("^OK$"), i04_06, new TF(tf_getoperlist_siminit_timeout), new GF(gf_siminit_timeout) },
    { o06_17, new ModemEvent("^OK$"), o06_18, new TF(tf_waitsim_init_complete), new PG },
    { o06_17, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_18: detect network access, wait SIM initialization completed*/
    { o06_18, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_18, new ModemEvent("^\\+CPIN: (?<state>.*)"), o06_18, new NT, new GF(gf_cpin_ready) },
    { o06_18, new ModemEvent("^\\+CPIN: (?<state>.*)"), i04_06, new TF(tf_getoperlist_simfail), new PG },
    { o06_18, new ModemEvent("^\\+QIND: PB DONE"), o06_17, new TF(tf_get_sim_initialization_state), new PG },
    { o06_18, new TimeoutEvent(), o06_17, new TF(tf_get_sim_initialization_state), new PG },
    /*o06_19: detect network access, SIM initialization completed*/
    { o06_19, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_19, new ModemEvent("ERROR"), o06_20, new TF(tf_getoperlist_from_modem), new PG },
    { o06_19, new ModemEvent("^OK$"), o06_20, new TF(tf_getoperlist_from_modem), new PG },
    { o06_19, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o06_20: detect network access, list operators*/
    { o06_20, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o06_20, new ModemEvent("^\\+COPS: (?<cops>\\([0-9],\".*\",\".*\",\"[0-9]+\",[0-9]\\),)"), o06_20, new TF(tf_getoperlist_nextentry), new PG },
    { o06_20, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), i04_06, new TF(tf_getoperlist_fail_cme), new PG },
    { o06_20, new ModemEvent("ERROR"), i04_06, new TF(tf_getoperlist_fail), new PG },
    { o06_20, new ModemEvent("^OK$"), i04_06, new TF(tf_getoperlist_ok), new PG },
    { o06_20, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o10: get SMS list*/
    { o10, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o10, new ModemEvent("^\\+CMGL: (?<index>[0-9]+),(?<state>[0-9]+)"), o10, new TF(tf_listsms_next), new PG },
    { o10, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cme_dbus_return), new PG },
    { o10, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cms_dbus_return), new PG },
    { o10, new ModemEvent("ERROR"), o02, new TF(tf_operation_failure_dbus_return), new PG },
    { o10, new ModemEvent("^OK$"), o02, new TF(tf_dbus_listsms_ok), new PG },
    { o10, new ModemEvent("(?<line>.*)"), o10, new TF(tf_sms_get_body), new PG },
    { o10, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o11: read SMS*/
    { o11, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o11, new ModemEvent("^\\+CMGR: (?<state>[0-9]+)"), o11, new TF(tf_readsms), new PG },
    { o11, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cme_dbus_return), new PG },
    { o11, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cms_dbus_return), new PG },
    { o11, new ModemEvent("ERROR"), o02, new TF(tf_operation_failure_dbus_return), new PG },
    { o11, new ModemEvent("^OK$"), o02, new TF(tf_dbus_readsms_ok), new PG },
    { o11, new ModemEvent("(?<line>.*)"), o11, new TF(tf_sms_get_body), new PG },
    { o11, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o12_01: send SMS*/
    { o12_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o12_01, new ModemEvent("^(?<prompt>[>])"), o12_01, new TF(tf_sendsms_body), new PG },
    { o12_01, new ModemEvent("^\\+CMGS: (?<msgref>[0-9]+)"), o12_02, new TF(tf_dbus_sendsms_result), new PG },
    { o12_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cme_dbus_return), new PG },
    { o12_01, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cms_dbus_return), new PG },
    { o12_01, new ModemEvent("ERROR"), o02, new TF(tf_operation_failure_dbus_return), new PG },
    { o12_01, new ModemEvent("^OK$"), o02, new TF(tf_operation_failure_dbus_return), new PG },
    { o12_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o12_02: send SMS, result returned*/
    { o12_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { o12_02, new ModemEvent("ERROR"), o02, new TF(tf_query_reg_state_full), new PG },
    { o12_02, new ModemEvent("^OK$"), o02, new TF(tf_query_reg_state_full), new PG },
    { o12_02, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*o13: delete sms*/
    { o13, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o13, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cme_dbus_return), new PG },
    { o13, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cms_dbus_return), new PG },
    { o13, new ModemEvent("ERROR"), o02, new TF(tf_operation_failure_dbus_return), new PG },
    { o13, new ModemEvent("^OK$"), o02, new TF(tf_dbus_deletesms_ok), new PG },
    { o13, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o14_01: get SMS storage*/
    { o14_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { o14_01, new ModemEvent("^\\+CPMS: \"(?<mem1>.*)\",(?<used1>[0-9]+),(?<total1>[0-9]+),\"(?<mem2>.*)\",(?<used2>[0-9]+),(?<total2>[0-9]+),\"(?<mem3>.*)\",(?<used3>[0-9]+),(?<total3>[0-9]+)"), o14_02, new TF(tf_dbus_getsmsstorage_result), new PG },
    { o14_01, new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cme_dbus_return), new PG },
    { o14_01, new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), o02, new TF(tf_operation_failure_cms_dbus_return), new PG },
    { o14_01, new ModemEvent("ERROR"), o02, new TF(tf_operation_failure_dbus_return), new PG },
    { o14_01, new ModemEvent("^OK$"), o02, new TF(tf_operation_failure_dbus_return), new PG },
    { o14_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*o14_02: get SMS storage, result returned*/
    { o14_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { o14_02, new ModemEvent("ERROR"), o02, new TF(tf_query_reg_state_full), new PG },
    { o14_02, new ModemEvent("^OK$"), o02, new TF(tf_query_reg_state_full), new PG },
    { o14_02, new TimeoutEvent(), s01, new TF(tf_modem_timeout), new PG },
    /*x01_01: trigger shutdown (dbus)*/
    { x01_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { x01_01, new ModemEvent("ERROR"), s01, new TF(tf_shutdown_err_dbus_return), new PG },
    { x01_01, new ModemEvent("^OK$"), x01_02, new TF(tf_shutdown_ok), new PG },
    { x01_01, new TimeoutEvent(), s01, new TF(tf_modem_timeout_dbus_return), new PG },
    /*x01_02: wait shutdown complete*/
    { x01_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error_dbus_return), new PG },
    { x01_02, new ModemEvent("^POWERED DOWN$"), s01, new TF(tf_shutdown_finish_dbus_return), new PG },
    { x01_02, new TimeoutEvent(), s01, new TF(tf_shutdown_timeout_dbus_return), new PG },
    /*x02_01: trigger shutdown (internal)*/
    { x02_01, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { x02_01, new ModemEvent("ERROR"), s01, new TF(tf_shutdown_err), new PG },
    { x02_01, new ModemEvent("^OK$"), x02_02, new TF(tf_shutdown_ok), new PG },
    { x02_01, new TimeoutEvent(), s01, new TF(tf_shutdown_err), new PG },
    /*x02_02: wait shutdown complete (internal)*/
    { x02_02, new RegexEvent("ModemPort_IO_error"), s01, new TF(tf_io_error), new PG },
    { x02_02, new ModemEvent("^POWERED DOWN$"), s01, new TF(tf_shutdown_finish), new PG },
    { x02_02, new TimeoutEvent(), s01, new TF(tf_shutdown_timeout), new PG },
  };

  MdmStatemachine::action_list_t al {
    { new ModemEvent("^\\+CSQ: (?<rssi>[0-9]+),(?<ber>[0-9]+)"), new AF(af_read_csq) },
    { new ModemEvent("^\\+COPS: (?<mode>[0-9]+),2,\"(?<id>[0-9]+)\",(?<act>[0-9]+)"), new AF(af_read_cops)},
    { new ModemEvent("^\\+COPS: 0"), new AF(af_read_cops_null)},
    { new ModemEvent("^\\+CREG: (?<mode>[0-9]+),(?<state>[0-9]+),\"(?<lac>.*)\",\"(?<cid>.*)\",(?<act>[0-9]+)"), new AF(af_read_creg_full)},
    { new ModemEvent("^\\+CREG: (?<mode>[0-9]+),(?<state>[0-9]+)"), new AF(af_read_creg)},
    { new ModemEvent("^\\+CGREG: (?<mode>[0-9]+),(?<state>[0-9]+)"), new AF(af_read_cgreg)},
    { new ModemEvent("^\\+CGPADDR: (?<cid>[0-9]+),\"(?<pdp_addr>.*)\""), new AF(af_read_pdp_address) },
    { new ModemEvent("^\\+CME ERROR: (?<err>[0-9]+)"), new AF(af_cme_error) },
    { new ModemEvent("^\\+CMS ERROR: (?<err>[0-9]+)"), new AF(af_cms_error) },
    { new ModemEvent("^\\+CMTI: "), new AF(af_sms_event_report) },
    { new ModemEvent("^\\+QIND: SMS DONE"), new AF(af_sms_done) },
    { new ModemEvent("^\\+QUSIM: (?<simtype>[0-9]+)"), new AF(af_do_nothing) },
    { new DBusEvent("GetOperState"), new AF(af_dbus_getoperstate) },
    { new DBusEvent("GetPortState2"), new AF(af_dbus_getportstate2) },
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
    { new DBusEvent("GetSimAutoActivation"), new AF(af_dbus_getsimautoactivation) },
    { new DBusEvent("SetSimAutoActivation"), new AF(af_dbus_setsimautoactivation) },
  };

    GThread* worker_thread = g_thread_new("mdm_cuse_worker",
                                          static_cast<GThreadFunc>(mdm_cuse_worker_main),
                                          nullptr);

    GKeyFileStorage gKeyFileStorage(MDMD_CONFIG_FILE_PATH, MDMD_CONFIG_FILE_GROUP);
    MdmStatemachine sm(tl, s01, al, MDM_GPIO__PWRKEY_PIN, MDM_MUSB_DRIVER_PORT, gKeyFileStorage );
    //trigger initial timeout for statemachine entry
    sm.do_timeout(0,false);

    ml = g_main_loop_new(nullptr, TRUE);
    g_main_loop_run(ml);

    g_thread_join(worker_thread);

    exit(EXIT_SUCCESS);
}

