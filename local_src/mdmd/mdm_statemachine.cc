// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


#include <glib.h>
#include <sys/stat.h>

#include "mdm_cuse_worker.h"
#include "mdm_statemachine.h"
#include "mdmd_log.h"
#include <sstream>
#include <string>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define MDM_DEFAULT_TIMEOUT_MILLISECONDS  10000

static void h_strip_newline(char *pChar)
{
  if (pChar != nullptr)
  {
    while (*pChar != 0)
    {
      switch (*pChar)
      {
        case '\n':
          *pChar = 0;
          break;
        case '\r':
          *pChar = 0;
          break;
        default:
          pChar = pChar + 1;
      }
    }
  }
}

MdmStatemachine::MdmStatemachine (transition_list_t &tl, State &s1, action_list_t &al,
                                  int gpioPowerControl, int musbDriverPort,
                                  ParameterStorage& storage)
    : StateMachine(tl,s1,al),
      _gpioPowerKey(gpioPowerControl),
      _musbDriverPort(musbDriverPort),
      _parameter_storage(storage)
{
    _dbus_server = new MdmDBusServer(this);

    _port = 0;
    this->try_open_modem();

    _timeout = new MdmTimeout(this, MDM_DEFAULT_TIMEOUT_MILLISECONDS, 0);

    //init statemachine member variables
    _modem_manufacturer = "";
    _modem_model = "";
    _modem_revision = "";
    _modem_identity = "";
    _rmnet_autoconnect_with_dtrset = false;
    _gprs_reg_state = static_cast<int>(SERVICE_REG_STATE::UNKNOWN);
    _gprs_wwan_state_change = false;
    _gprs_pdp_addr_change = false;
    _oper_reg_state = static_cast<int>(SERVICE_REG_STATE::UNKNOWN);
    _oper_reg_lac = "";
    _oper_reg_cid = "";
    _gprs_autoconnect_count = 0;
    _sms_list.clear();
    _port_wait_count = 0;
    _cfun_wait_count = 0;
    _siminit_wait_flag = false;
    _sim_sms_support_flag = false;
    _pdp_addr = "";
    _oper_map.clear();
    _continue_getoperlist = false;

    //init logging
    ModemManagementConfig mdmdConfig = _parameter_storage.get_modem_management_config();
    mdmd_SetLogLevel( mdmdConfig.get_log_level() );

    mdmd_Log(MD_LOG_DBG, "initial state: %s \"%s\"\n", s1.id().c_str(), s1.name().c_str());

    //init GPIO
    if (!_gpioPowerKey.gpio_exported())
    {
      if (_gpioPowerKey.gpio_export() == -1)
      {
          mdmd_Log(MD_LOG_WRN, "GPIO export failed\n");
      }
    }
    if (_gpioPowerKey.gpio_exported())
    {
      if (_gpioPowerKey.gpio_set_direction_out() == -1)
      {
          mdmd_Log(MD_LOG_WRN, "GPIO set direction failed\n");
      }
    }

    //init WWAN interface, make sure that it is initially stopped
    wwan_stop();
}

constexpr auto _GPRS_CONNECTIVITY_DISABLED = 0;
//#define _GPRS_CONNECTIVITY_ENABLED   int(1)
//#define _GPRS_CONNECTIVITY_DEFAULT   _GPRS_CONNECTIVITY_ENABLED

MdmStatemachine::~MdmStatemachine()
{
    delete _port;
    if (_timeout != 0)
    {
        delete _timeout;
    }
    if (_dbus_server != 0)
    {
        delete _dbus_server;
    }
    _sms_list.clear();

    if (_gpioPowerKey.gpio_unexport() == -1)
    {
        mdmd_Log(MD_LOG_WRN, "GPIO unexport failed\n");
    }
}

void
MdmStatemachine::write( const std::string &text )
{
    if (_port != nullptr)
    {
      try
      {
        _port->write(text);
      }
      catch(...) {
        mdmd_Log(MD_LOG_ERR, "%s: modem port write error\n", get_state().c_str() );
      }
    }
}

void
MdmStatemachine::deactivate_cmd_timeout()
{
    if (_timeout != 0)
    {
        _timeout->deactivate();
    }
}

void
MdmStatemachine::kick_cmd_timeout(unsigned int milliseconds)
{
    if (_timeout != 0)
    {
      _timeout->kick_timeout(milliseconds);
    }
}

namespace {

bool is_modem_available(const std::string& deviceName) noexcept
{
    struct stat statBuffer{};
    return stat(deviceName.c_str(), &statBuffer) == 0 ? true : false; 
}

bool get_modem_command_device(std::string &command_device)
{
  /*
   * Serial USB modem devices:
   * ttyModem0 = Diagnose and Monitoring
   * ttyModem1 = GPS NMEA output
   * ttyModem2 = AT commands
   * ttyModem3 = PPP connection or AT commands
   */

  // TODO: Maybe use named symbolic links instead of ttyModem0...
  // Suggestion: /dev/ttyModemDiag, /dev/ttyModemNmea, /dev/ttyModemCmd, /dev/ttyModemCon

  /* WAT12155
   * after modem reset the DeviceIDs could be discontinuous (e.g. ttyModem0, ttyModem1, ttyModem3, ttyModem4)
   * but in any case the third device is for AT commands
   */
  for(int i = 0, num_modem_devices = 0; ((i < 1000) && (num_modem_devices < 3)); ++i)
  {
    std::string device("/dev/ttyModem");
    device.append(std::to_string(i));
    
    if(is_modem_available(device))
    {
      ++num_modem_devices;
      if (num_modem_devices == 3)
      {
          command_device = device;
          return true;
      }
    }
  }
  return false;
}

} // namespace

void MdmStatemachine::open_modem_port(const std::string& command_device)
{
    _port = new MdmSMPort( this, command_device, &_port_read_buffer );
    if (_port != nullptr)
    {
        _port->open();
    }
}

void MdmStatemachine::try_open_modem()
{
    try
    {
      std::string command_device;
      if (get_modem_command_device(command_device))
      {
        open_modem_port(command_device);
      }
    }
    catch(...) {
      if (_port != nullptr)
      {
        delete _port;
        _port = nullptr;
      }
    }
}

void
MdmStatemachine::clear_current_oper()
{
    if (_current_oper.is_valid())
    {
      mdmd_Log(MD_LOG_INF, "%s: Mobile Network: operator=0(invalid)\n", get_state().c_str());
      _current_oper.clear();
    }
    _diagnostic.set_access_class(MdmAccessClass::NONE);
}

void
MdmStatemachine::clear_modem_port()
{
    if (_port != nullptr)
    {
      delete _port;
      _port = nullptr;
    }
    _port_read_buffer.clear();

    reset_service_states();
    reset_statemachine();

    kick_cmd_timeout(MDM_DEFAULT_TIMEOUT_MILLISECONDS);

    mdmd_Log(MD_LOG_INF, "%s: modem port cleared\n", get_state().c_str() );
}

bool
MdmStatemachine::is_port_enabled() const
{
  ModemManagementConfig mdmdConfig = _parameter_storage.get_modem_management_config();
  return (mdmdConfig.get_port_state() != 0) ? true : false;
}

void
MdmStatemachine::set_port_enabled(bool enabled)
{
  if (!enabled)
  {
    clear_modem_port();
  }
  ModemManagementConfig mdmdConfig = _parameter_storage.get_modem_management_config();
  mdmdConfig.set_port_state(enabled);
  _parameter_storage.set_modem_management_config(mdmdConfig);
}

void
MdmStatemachine::modem_reset(ModemResetMode reset_mode)
{
  clear_modem_port();
  if (reset_mode == ModemResetMode::MUSB_RESET)
  {
    //unbind USB device
    if (_musbDriverPort.musb_bound())
    {
      if (_musbDriverPort.musb_unbind() == -1)
      {
        mdmd_Log(MD_LOG_WRN, "%s: modem USB driver unbind failed\n", get_state().c_str());
      }
      else
      {
        mdmd_Log(MD_LOG_INF, "%s: modem USB driver unbound\n", get_state().c_str());
        g_usleep(1000*1000); //wait a second after unbind
      }
    }
  }

  bool modem_powered_off = (_gpioPowerKey.gpio_write_value(1) == 0);
  if (!modem_powered_off)
  {
    mdmd_Log(MD_LOG_WRN, "%s: modem power off failed\n", get_state().c_str());
  }
  else
  {
    mdmd_Log(MD_LOG_INF, "%s: modem power off\n", get_state().c_str());
    g_usleep(1000*1000); //wait a second after power off
  }

  if (!_musbDriverPort.musb_bound())
  {
    if (_musbDriverPort.musb_bind() == -1)
    {
      mdmd_Log(MD_LOG_ERR, "%s: modem USB driver bind failed!\n", get_state().c_str());
    }
    else
    {
      mdmd_Log(MD_LOG_INF, "%s: modem USB driver bound\n", get_state().c_str());
      g_usleep(1000*1000); //wait a second after bind
    }
  }
  if (modem_powered_off)
  {
    if (_gpioPowerKey.gpio_write_value(0) != 0)
    {
      mdmd_Log(MD_LOG_ERR, "%s: modem power on failed!\n", get_state().c_str());
    }
    else
    {
      mdmd_Log(MD_LOG_INF, "%s: modem power on\n", get_state().c_str());
    }
  }
}

/*
 * Function execute_command:
 * - calls cmd string directly (redirects and pipes are not allowed)
 * - returns status >= 0 on successful execution, return value is the exit code of the program
 * - returns status < 0 if the command failed to run or was aborted due to a signal
 * - returns cmd_output with optional stdout on successful execution
 */
int
MdmStatemachine::execute_command(const std::string &cmd, std::string &cmd_output, std::string &cmd_error)
{
  int status = -1;
  gchar * std_output = nullptr;
  gchar * std_error = nullptr;
  gint exit_status = -1;
  const gboolean cmd_ok =
      g_spawn_command_line_sync(cmd.c_str(), &std_output, &std_error, &exit_status, nullptr);
  if (cmd_ok && WIFEXITED(exit_status))
  {
    status = WEXITSTATUS(exit_status);
  }
  h_strip_newline(std_output);
  h_strip_newline(std_error);
  cmd_output = (std_output != nullptr) ? std_output : "";
  cmd_error = (std_error != nullptr) ? std_error : "";
  mdmd_Log(MD_LOG_DBG, "%s: executed command '%s', status: %d, std_out: '%s', err_out: '%s'\n", get_state().c_str(),
           cmd.c_str(), status, cmd_output.c_str(), cmd_error.c_str());
  //Note: nullptr pointer is tolerated by g_free()
  g_free(std_output);
  g_free(std_error);
  return status;
}

bool
MdmStatemachine::wwan_disable()
{
  std::string cmd_output;
  std::string cmd_error;
  const int cmd_result = execute_command("/etc/config-tools/config_wwan --disable", cmd_output, cmd_error);
  bool disabled = (cmd_result == 0);
  if (!disabled)
  {
    mdmd_Log(MD_LOG_ERR, "%s: wwan disable failed: %d %s\n", get_state().c_str(),
             cmd_result, cmd_error.c_str());
  }
  else
  {
    mdmd_Log(MD_LOG_INF, "%s: wwan disable success\n", get_state().c_str());
  }
  return disabled;
}

bool
MdmStatemachine::wwan_enable()
{
  std::string cmd_output;
  std::string cmd_error;
  const int cmd_result = execute_command("/etc/config-tools/config_wwan --enable", cmd_output, cmd_error);
  bool enabled = (cmd_result == 0);
  if (!enabled)
  {
    mdmd_Log(MD_LOG_ERR, "%s: wwan enable failed %d %s\n", get_state().c_str(),
             cmd_result, cmd_error.c_str());
  }
  else
  {
    mdmd_Log(MD_LOG_INF, "%s: wwan enable success\n", get_state().c_str());
  }
  return enabled;
}


bool
MdmStatemachine::wwan_get_status(bool &is_enabled, bool &is_configured)
{
  bool status_valid = true;
  std::string cmd_output;
  std::string cmd_error;
  const int cmd_result = execute_command("/etc/config-tools/config_wwan --status", cmd_output, cmd_error);
  if (cmd_result != 0)
  {
    mdmd_Log(MD_LOG_WRN, "%s: get wwan status failed: %d %s\n", get_state().c_str(),
             cmd_result, cmd_error.c_str());
    status_valid = false;
  }
  else if (0 == cmd_output.compare("UP"))
  {
    is_enabled = true;
    is_configured = true;
  }
  else if (0 == cmd_output.compare("DOWN"))
  {
    is_enabled = false;
    is_configured = true;
  }
  else if (0 == cmd_output.compare("NOT_CONFIGURED"))
  {
    is_enabled = false;
    is_configured = false;
  }
  else
  {
    mdmd_Log(MD_LOG_WRN, "%s: unknown wwan status '%s'\n", get_state().c_str(),
             cmd_output.c_str());
    status_valid = false;
  }
  return status_valid;
}

bool
MdmStatemachine::wwan_stop()
{

  bool result;
  std::string cmd_output;
  std::string cmd_error;
  int cmd_result = execute_command("/etc/config-tools/config_wwan --stop", cmd_output, cmd_error);
  if (cmd_result != 0)
  {
    //could not stop wwan interface, try to disable (Note: first version of config_wwan supports only enable/disable)
    result = wwan_disable();
  }
  else
  {
    mdmd_Log(MD_LOG_INF, "%s: wwan stop success\n", get_state().c_str());
    result = true;
  }
  return result;
}

bool
MdmStatemachine::wwan_start()
{
  bool result;
  bool is_enabled = true;
  bool is_configured = true;

  if (!wwan_get_status(is_enabled, is_configured))
  { //could not get wwan status, try to enable (Note: first version of config_wwan supports only enable/disable)
    result = wwan_enable();
  }
  else
  {
    if (!is_configured)
    {
      mdmd_Log(MD_LOG_ERR, "%s: wwan interface not configured\n", get_state().c_str());
      result = false;
    }
    else if (!is_enabled)
    { //wwan has to be enabled (e.g. after system start)
      result = wwan_enable();
    }
    else
    {
      std::string cmd_output;
      std::string cmd_error;
      const int cmd_result = execute_command("/etc/config-tools/config_wwan --start", cmd_output, cmd_error);
      if (cmd_result != 0)
      {
        mdmd_Log(MD_LOG_ERR, "%s: wwan start failed %d %s\n", get_state().c_str(),
                 cmd_result, cmd_error.c_str());
        result = false;
      }
      else
      {
        mdmd_Log(MD_LOG_INF, "%s: wwan start success\n", get_state().c_str());
        result = true;
      }
    }
  }
  return result;
}

void
MdmStatemachine::wwan_renew()
{
  std::string cmd_output;
  std::string cmd_error;
  const int cmd_result = execute_command("/etc/config-tools/config_wwan --renew", cmd_output, cmd_error);
  if (cmd_result != 0)
  {
    mdmd_Log(MD_LOG_ERR, "%s: wwan IP address renew failed: %d %s\n", get_state().c_str(),
             cmd_result, cmd_error.c_str());
  }
  else
  {
    mdmd_Log(MD_LOG_INF, "%s: wwan IP address renew successfully triggered\n", get_state().c_str());
  }
}


void
MdmStatemachine::set_current_oper_csq(int ber, int rssi)
{
  const MdmSignalQualityLevel previous_signal_quality = _diagnostic.get_signal_quality_level();
  _current_oper.set_csq(ber, rssi);
  switch(_current_oper.get_quality_step())
  {
    default:
      //no break;
    case 0:
      _diagnostic.set_signal_quality_level((MdmSignalQualityLevel::NONE));
      break;
    case 1:
      _diagnostic.set_signal_quality_level((MdmSignalQualityLevel::MARGINAL));
      break;
    case 2:
      _diagnostic.set_signal_quality_level((MdmSignalQualityLevel::BAD));
      break;
    case 3:
      _diagnostic.set_signal_quality_level((MdmSignalQualityLevel::SUFFICIENT));
      break;
    case 4:
      _diagnostic.set_signal_quality_level((MdmSignalQualityLevel::STABLE));
      break;
    case 5:
      _diagnostic.set_signal_quality_level((MdmSignalQualityLevel::GOOD));
      break;
    case 6:
      _diagnostic.set_signal_quality_level((MdmSignalQualityLevel::EXCELLENT));
      break;
  }
  const MdmSignalQualityLevel new_signal_quality = _diagnostic.get_signal_quality_level();
  if (previous_signal_quality != new_signal_quality)
  {
    mdmd_Log(MD_LOG_INF, "%s: Signal Quality: level=%d\n", get_state().c_str(), static_cast<int>(new_signal_quality));
  }
}

void
MdmStatemachine::set_oper_reg_state(int state, const std::string &lac, const std::string &cid, int act)
{
  (void)act; //unused parameter
  const bool statusChange = (_oper_reg_state != state) ||
                            (_oper_reg_cid.compare(cid) != 0) ||
                            (_oper_reg_lac.compare(lac) != 0);
  _oper_reg_cid = cid;
  _oper_reg_lac = lac;
  _oper_reg_state = state;
  if (statusChange)
  {
    mdmd_Log(MD_LOG_INF, "%s: Mobile Network: state=%d(%s), area=%s, cell=%s\n", get_state().c_str(),
             state, (is_oper_registered()) ? "registered" : "not registered", lac.c_str(), cid.c_str());
  }
  if ((is_oper_registered()) && (_current_oper.is_valid()))
  {
    if (_current_oper.is_type_3g())
    {
      _diagnostic.set_access_class(MdmAccessClass::UMTS);
    }
    else
    {
      _diagnostic.set_access_class(MdmAccessClass::GSM);
    }
  }
  else
  {
    _diagnostic.set_access_class(MdmAccessClass::NONE);
  }
}

void
MdmStatemachine::set_gprs_reg_state(int state)
{
  if (_gprs_reg_state != state)
  {
    const bool gprs_disabled = is_gprs_disabled();
    mdmd_Log(MD_LOG_INF, "%s: Data Service: state=%d(%s), %s\n", get_state().c_str(),
             state, ((state==1)||(state==5)) ? "registered" : "not registered",
             gprs_disabled ? "disabled" : "enabled");
    switch (_gprs_reg_state)
    {
      case 1:
      case 5:
        if ((state!=1) && (state!=5))
        {
          _gprs_wwan_state_change = wwan_stop();
        }
        break;
      default:
        if ((state==1) || (state==5))
        {
          if (!gprs_disabled)
          {
            _gprs_wwan_state_change = wwan_start();
          }
        }
        break;
    }
    _gprs_reg_state = state;
  }
}

std::string
MdmStatemachine::get_stored_sim_iccid() const
{
  SimAutoActivation simAutoActivation = _parameter_storage.get_sim_autoactivation();
  return simAutoActivation.get_iccid();
}

std::string
MdmStatemachine::get_stored_sim_pin() const
{
  SimAutoActivation simAutoActivation = _parameter_storage.get_sim_autoactivation();
  return simAutoActivation.get_pin();
}

void
MdmStatemachine::store_sim_pin(const std::string &iccid, const std::string &pin)
{
  SimAutoActivation simAutoActivation = _parameter_storage.get_sim_autoactivation();
  if ((0 != iccid.compare(simAutoActivation.get_iccid())) || (0 != pin.compare(simAutoActivation.get_pin())))
  {
    simAutoActivation.set_iccid(iccid);
    simAutoActivation.set_pin(pin);
    _parameter_storage.set_sim_autoactivation(simAutoActivation);
  }
}

void
MdmStatemachine::set_current_oper(int mode, int id, int act)
{
  if ((id != _current_oper.get_id()) || (act != _current_oper.get_act()))
  {
    mdmd_Log(MD_LOG_INF, "%s: Mobile Network: operator=%d, act=%d\n", get_state().c_str(), id, act);
    _current_oper.set_selection_mode(mode);
    _current_oper.set_id(id);
    _current_oper.set_act(act);
    _current_oper.set_current();

    //workaround: set actual signal strength here again
    //this is necessary since get_quality_step method returns no value as long as current_oper is invalid
    //otherwise signal LEDs and diagnostics are not set before next signal strength change
    set_current_oper_csq(_current_oper.get_ber(), _current_oper.get_rssi());

  }
  if ((is_oper_registered()) && (_current_oper.is_valid()))
  {
    if (_current_oper.is_type_3g())
    {
      _diagnostic.set_access_class(MdmAccessClass::UMTS);
    }
    else
    {
      _diagnostic.set_access_class(MdmAccessClass::GSM);
    }
  }
  else
  {
    _diagnostic.set_access_class(MdmAccessClass::NONE);
  }
}

void
MdmStatemachine::add_opermap(int id, int act, int state)
{
  Oper oper(state);
  oper.set_id(id);
  oper.set_act(act);
  int key= id * 100 + act % 100;
  _oper_map[key] = oper;
}

GVariant*
MdmStatemachine::get_var_opermap() const
{
  GVariantBuilder * const builder = g_variant_builder_new( G_VARIANT_TYPE("a(iiiis)" ) );
  for(std::map<int,Oper>::const_iterator it = _oper_map.begin(); it!=_oper_map.end(); ++it)
  {
    if (it->second.is_valid())
    {
      const std::string operator_name = get_operator_name(it->second.get_id());
      g_variant_builder_add_value(builder, g_variant_new( "(iiiis)",
                                                          it->second.get_state(),
                                                          it->second.get_id(),
                                                          it->second.get_act(),
                                                          it->second.get_quality_percent(),
                                                          operator_name.c_str()));
    }
  }  
  GVariant *retVal = g_variant_new("(@a(iiiis))", g_variant_builder_end(builder));
  g_variant_builder_unref(builder);
  return retVal;
}

void
MdmStatemachine::add_smslist(int index)
{
  Sms sms(index);
  _sms_list.push_back(sms);
}

void
MdmStatemachine::add_smslist(int index, int state, int length)
{
  Sms sms(index, state, length);
  _sms_list.push_back(sms);
}

void
MdmStatemachine::set_sms_body_last_read()
{
  if (!_sms_list.empty())
  {
    _sms_list.back().set_body(get_last_read());
  }
}

void
MdmStatemachine::set_last_sms(int state, int length)
{
  if (!_sms_list.empty())
  {
    _sms_list.back().set_state(state);
    _sms_list.back().set_length(length);
  }
}

GVariant*
MdmStatemachine::get_var_smslist() const
{
  GVariantBuilder * const builder = g_variant_builder_new( G_VARIANT_TYPE("a(iiis)" ) );
  for(std::list<Sms>::const_iterator it = _sms_list.begin(); it!=_sms_list.end(); ++it)
  {
    g_variant_builder_add_value(builder, g_variant_new( "(iiis)",
                                                        it->get_index(),
                                                        it->get_state(),
                                                        it->get_length(),
                                                        it->get_body().c_str()));
  }
  GVariant *retVal = g_variant_new("(@a(iiis))", g_variant_builder_end(builder));
  g_variant_builder_unref(builder);
  return retVal;
}

GVariant*
MdmStatemachine::get_var_smsread() const
{
  GVariant* retVal;
  if (_sms_list.empty())
  {
    retVal = nullptr;
  }
  else
  {
    if (_sms_list.front().is_valid())
    {
      retVal = g_variant_new("(iis)",
                             _sms_list.front().get_state(),
                             _sms_list.front().get_length(),
                             _sms_list.front().get_body().c_str());

    }
    else
    {
      retVal = nullptr;
    }
  }
  return retVal;
}

void
MdmStatemachine::event_report_last_read()
{
  const char* pipe_report = MDM_CUSE_FIFO_EVENT_REPORT;
  const int fd = ::open(pipe_report, O_WRONLY|O_NONBLOCK);
  if (fd < 0)
  {
    mdmd_Log(MD_LOG_ERR, "%s: open %s for non-blocking write failed: %s\n", get_state().c_str(),
             pipe_report, strerror(errno));
  }
  else
  {
    int written = ::write(fd, "\r\n", 2); //write first a <CR><LF> for possible previous incomplete report
    if (written == 2)
    {
      written = ::write(fd, get_last_read().c_str(), get_last_read().length());
      if ((written >= 0) && ((size_t)written == get_last_read().length()))
      { //add <CR><LF> when completed
        written = ::write(fd, "\r\n", 2);
      }
    }
    if (written < 0)
    {
      mdmd_Log(MD_LOG_ERR, "%s: write event report failed: %s\n", get_state().c_str(), strerror(errno));
    }
    if (::close(fd) == -1)
    {
      mdmd_Log(MD_LOG_WRN, "%s: close fd %d failed\n", get_state().c_str(), fd );
    }
  }
}
void
MdmSMPort::read_line_cb(const std::string &line)
{
    SerialPort::read_line_cb(line);
    _sm->do_read(line);
}

void
MdmSMPort::io_error()
{
    _sm->do_string("ModemPort_IO_error");
}

void
MdmTimeout::callback()
{
    _sm->do_timeout(_id, false);
}

void
MdmStatemachine::set_gprs_access_config(const GprsAccessConfig &config)
{
  const bool gprs_disabled_with_old_config = is_gprs_disabled();
  _parameter_storage.set_gprs_access_config(config);
  const bool gprs_disabled_now = is_gprs_disabled();
  if (gprs_disabled_with_old_config != gprs_disabled_now)
  {
    const int state = _gprs_reg_state;
    mdmd_Log(MD_LOG_INF, "%s: Data Service: state=%d(%s), %s\n", get_state().c_str(),
             state, ((state==1)||(state==5)) ? "registered" : "not registered",
             gprs_disabled_now ? "disabled" : "enabled");
  }
}

bool
MdmStatemachine::is_gprs_disabled() const
{
  const GprsAccessConfig gprsAccessConfig = _parameter_storage.get_gprs_access_config();
  return (gprsAccessConfig.get_state() == _GPRS_CONNECTIVITY_DISABLED);
}

bool
MdmStatemachine::set_loglevel(const int loglevel)
{
  bool retVal = true;
  ModemManagementConfig mdmdConfig = _parameter_storage.get_modem_management_config();
  if (loglevel != mdmdConfig.get_log_level())
  {
    //change logging
    if (mdmd_SetLogLevel( loglevel ))
    {
      //save new configuration
      mdmdConfig.set_log_level(loglevel);
      _parameter_storage.set_modem_management_config(mdmdConfig);
    }
    else
    {
      retVal = false;
    }
  }
  return retVal;
}

int
MdmStatemachine::get_loglevel() const
{
  ModemManagementConfig mdmdConfig = _parameter_storage.get_modem_management_config();
  return mdmdConfig.get_log_level();
}

void
MdmStatemachine::set_pdp_address( const std::string &pdp_addr )
{
  if (_pdp_addr.compare(pdp_addr) != 0)
  {
    _gprs_pdp_addr_change = true;
    mdmd_Log(MD_LOG_INF, "%s: Data Service: PDP address=\'%s\'\n", get_state().c_str(), pdp_addr.c_str());
    _pdp_addr = pdp_addr;
  }
}

bool MdmStatemachine::insert_operator_name(int id, const std::string& name)
{
  std::pair<std::map<int,std::string>::iterator,bool> ret;
  ret = _operator_names.insert(std::pair<int,std::string>(id, name));
  //The pair::second element is set to true if a new element was inserted or false if an equivalent key already existed.
  bool element_inserted = ret.second;
  return element_inserted;
}

std::string MdmStatemachine::get_operator_name(int id) const
{
  std::stringstream tmpstream;
  std::map<int,std::string>::const_iterator it = _operator_names.find(id);
  if (it == _operator_names.end())
  {
    tmpstream << "Operator " << id;
  }
  return (it == _operator_names.end()) ? tmpstream.str() : it->second;
}

void MdmStatemachine::update_gprs_autoconnect_count()
{
  if ((is_gprs_registered() == false) && (is_oper_registered() == true) && (is_gprs_disabled() == false))
  {
    _gprs_autoconnect_count++;
  }
  else
  {
    _gprs_autoconnect_count = 0;
  }
}

void MdmStatemachine::reset_gprs_autoconnect_count()
{
  _gprs_autoconnect_count = 0;
}

void MdmStatemachine::reset_service_states()
{
  //short message service
  _sms_list.clear();

  //packet data service
  set_gprs_reg_state(static_cast<int>(SERVICE_REG_STATE::UNKNOWN));
  _gprs_wwan_state_change = false;
  _gprs_pdp_addr_change = false;
  _rmnet_autoconnect_with_dtrset = false;
  _gprs_autoconnect_count = 0;
  _pdp_addr = "";

  //network access
  _oper_map.clear();
  clear_current_oper();
  std::string location_area_code;
  std::string cell_id;
  int access_technology = -1;
  set_oper_reg_state(static_cast<int>(SERVICE_REG_STATE::UNKNOWN), location_area_code, cell_id, access_technology);

  //common
  _cfun_wait_count = 0;
  _port_wait_count = 0;
}

void MdmStatemachine::reset_sim_states()
{
  _siminit_wait_flag = false;
  _sim_sms_support_flag = false;
}
