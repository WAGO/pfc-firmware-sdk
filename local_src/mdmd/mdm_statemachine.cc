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
///  \brief    Specific statemachine implementation
///
///  \author   KNu
//------------------------------------------------------------------------------
#include <glib.h>
#include <sys/stat.h>

#include "mdm_cuse_worker.h"
#include "mdm_statemachine.h"
#include <sstream>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define MDM_DEFAULT_TIMEOUT_MILLISECONDS  10000

static void h_strip_newline(char *pChar)
{
  if (pChar != NULL)
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
                                  int gpioPowerControl, int musbDriverPort)
    : StateMachine(tl,s1,al),
      _gpioPowerKey(gpioPowerControl),
      _musbDriverPort(musbDriverPort)
{
    _dbus_server = new MdmDBusServer(this);
    if (_dbus_server == 0)
        throw std::exception();

    _port = 0;
    this->try_open_modem();

    _timeout = new MdmTimeout(this, MDM_DEFAULT_TIMEOUT_MILLISECONDS, 0);
    if (_timeout == 0) throw std::exception();

    init();
    _net_led_blink_code_output = false;
    _storage.init_loglevel();

    mdmd_Log(MD_LOG_DBG, "initial state: %s \"%s\"\n", s1.id().c_str(), s1.name().c_str());
}

#define _GPRS_REG_INIT_STATE   -1
#define _RMNET_PROFILEID_DEFAULT 1

#define _GPRS_CONNECTIVITY_DISABLED  int(0)
#define _GPRS_CONNECTIVITY_ENABLED   int(1)
#define _GPRS_CONNECTIVITY_DEFAULT   _GPRS_CONNECTIVITY_ENABLED
void
MdmStatemachine::init()
{
    int gprs_connectivity;

    _sim_state=MdmSimState::UNKNOWN;
    _cfun_state=-1;
    _pin_count=-1;
    _puk_count=-1;
    _modem_manufacturer = "";
    _modem_model = "";
    _modem_revision = "";
    _modem_identity = "";
    _iccid = "";
    _pin = "";
    _puk = "";
    _gprs_reg_state = _GPRS_REG_INIT_STATE;
    _oper_reg_state = -1;
    _oper_reg_lac = "";
    _oper_reg_cid = "";
    _oper_scan_mode = -1;
    _oper_scan_seq = -1;
    _rmnet_profileid = _RMNET_PROFILEID_DEFAULT;
    _gprsaccess.clear();
    _gprsaccess_set_oninit = false;
    _gprs_temporary_disable = false;
    _current_signal_step = 0;
    _last_cme_error = -1;
    _last_cms_error = -1;
    _sms_list.clear();
    _last_sms_msg_ref = 0;
    _port_wait_count = 0;
    _cfun_wait_count = 0;
    _pdp_addr = "";

    if (!_storage.get_gprs_connectivity(gprs_connectivity)) {
      gprs_connectivity = _GPRS_CONNECTIVITY_DEFAULT;
      _storage.set_gprs_connectivity(gprs_connectivity);
      _storage.save();
    }
}

MdmStatemachine::~MdmStatemachine()
{
    delete _port;
    delete _timeout;
    delete _dbus_server;
    _sms_list.clear();
}

void
MdmStatemachine::write( const std::string &text )
{
    if(_port)
    {
      try {
        _port->write(text);
      } catch (SerialPortException &e) {
        mdmd_Log(MD_LOG_ERR, "%s: modem port write error\n", get_state().c_str() );
      }
    }
}

void
MdmStatemachine::deactivate_cmd_timeout()
{
    _timeout->deactivate();
}

void
MdmStatemachine::kick_cmd_timeout(unsigned int milliseconds)
{
    _timeout->kick_timeout(milliseconds);
}

bool
MdmStatemachine::get_modem_command_device(std::string &commandDevice) const
{
  unsigned int deviceIndex = 0;
  unsigned int devicesFound = 0;
  bool retVal = false;
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
  while((deviceIndex < 1000) && (retVal == false))
  {
    struct stat statBuffer;
    char deviceName[16];
    sprintf(deviceName, "/dev/ttyModem%d", deviceIndex);
    if(stat(deviceName, &statBuffer) == 0)
    {
      devicesFound++;
      if (devicesFound == 3)
      {
        commandDevice = deviceName;
        retVal = true;
      }
    }
    deviceIndex++;
  }
  return retVal;
}

void
MdmStatemachine::try_open_modem()
{
  if (_port == 0)
  {
    std::string commandDevice;
    if (get_modem_command_device(commandDevice))
    { //device found
      _port = new MdmSMPort( this, commandDevice, &_port_read_buffer );
      if (_port == 0)
      throw std::exception();

      try {
        _port->open();
      } catch (SerialPortException &e) {
        delete _port;
        _port = 0;
        //mdmd_Log(MD_LOG_ERR, "%s: Open modem port %s failed", get_state().c_str(), commandDevice.c_str() );
      }
    }
    else
    {
      mdmd_Log(MD_LOG_ERR, "%s: AT interface device not found", get_state().c_str() );
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
      update_status_leds();
    }
}

void
MdmStatemachine::clear_modem_port()
{
    if (_port != 0)
    {
      delete _port;
      _port = 0;
    }
    _port_read_buffer.clear();

    set_gprs_reg_state(0); //necessary for "if down" trigger
    clear_current_oper();

    reset_statemachine();

    init();

    kick_cmd_timeout(MDM_DEFAULT_TIMEOUT_MILLISECONDS);

    mdmd_Log(MD_LOG_INF, "%s: modem port cleared\n", get_state().c_str() );
}

void
MdmStatemachine::set_port_enabled(bool enabled)
{
  if (!enabled)
  {
    clear_modem_port();
  }
  _storage.set_port_enabled(enabled);
  _storage.save();
}

int MdmStatemachine::set_modem_powerkey(int value) const
{
  int result;
  if (_gpioPowerKey.gpio_export() == -1)
  {
    mdmd_Log(MD_LOG_WRN, "%s: modem power %s: GPIO export failed\n", get_state().c_str(), (value==0)?"on":"off");
  }
  if (_gpioPowerKey.gpio_set_direction_out() == -1)
  {
    mdmd_Log(MD_LOG_WRN, "%s: modem power %s: GPIO set direction failed\n", get_state().c_str(), (value==0)?"on":"off");
  }
  result = _gpioPowerKey.gpio_write_value(1);
  if (_gpioPowerKey.gpio_unexport() == -1)
  {
    mdmd_Log(MD_LOG_WRN, "%s: modem power %s: GPIO unexport failed\n", get_state().c_str(), (value==0)?"on":"off");
  }
  return result;
}

void
MdmStatemachine::modem_hard_reset()
{
  bool restart_ok = true;
  mdmd_Log(MD_LOG_INF, "%s: modem restart...\n", get_state().c_str());

  //step 1: clear modem port
  clear_modem_port();
  //step 2: unbind USB device
  if (_musbDriverPort.musb_unbind() == -1)
  {
    mdmd_Log(MD_LOG_ERR, "%s: unbind modem driver failed\n", get_state().c_str());
    restart_ok = false;
  }
  //wait a second
  g_usleep(1000*1000);
  //step 3: modem power off
  if (set_modem_powerkey(1) == -1)
  {
    mdmd_Log(MD_LOG_ERR, "%s: modem power off failed\n", get_state().c_str());
    restart_ok = false;
  }
  //step 4: bind USB device
  if (_musbDriverPort.musb_bind() == -1)
  {
    mdmd_Log(MD_LOG_ERR, "%s: bind modem driver failed\n", get_state().c_str());
    restart_ok = false;
  }
  //wait a second
  g_usleep(1000*1000);
  //step 5: modem power on
  if (set_modem_powerkey(0) == -1)
  {
    mdmd_Log(MD_LOG_ERR, "%s: modem power on failed\n", get_state().c_str());
    restart_ok = false;
  }
  if (restart_ok)
  {
    mdmd_Log(MD_LOG_INF, "%s: modem restart successful\n", get_state().c_str());
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
  gchar * std_output = NULL;
  gchar * std_error = NULL;
  gint exit_status = -1;
  const gboolean cmd_ok =
      g_spawn_command_line_sync(cmd.c_str(), &std_output, &std_error, &exit_status, NULL);
  if (cmd_ok && WIFEXITED(exit_status))
  {
    status = WEXITSTATUS(exit_status);
  }
  h_strip_newline(std_output);
  h_strip_newline(std_error);
  cmd_output = (std_output != NULL) ? std_output : "";
  cmd_error = (std_error != NULL) ? std_error : "";
  mdmd_Log(MD_LOG_DBG, "%s: executed command '%s', status: %d, std_out: '%s', err_out: '%s'\n", get_state().c_str(),
           cmd.c_str(), status, cmd_output.c_str(), cmd_error.c_str());
  //Note: NULL pointer is tolerated by g_free()
  g_free(std_output);
  g_free(std_error);
  return status;
}

void
MdmStatemachine::wwan_disable()
{
  std::string cmd_output;
  std::string cmd_error;
  const int cmd_result = execute_command("/etc/config-tools/config_wwan --disable", cmd_output, cmd_error);
  if (cmd_result != 0)
  {
    mdmd_Log(MD_LOG_ERR, "%s: wwan disable failed: %d %s\n", get_state().c_str(),
             cmd_result, cmd_error.c_str());
  }
  else
  {
    mdmd_Log(MD_LOG_INF, "%s: wwan disable success\n", get_state().c_str());
  }
}

void
MdmStatemachine::wwan_enable()
{
  std::string cmd_output;
  std::string cmd_error;
  const int cmd_result = execute_command("/etc/config-tools/config_wwan --enable", cmd_output, cmd_error);
  if (cmd_result != 0)
  {
    mdmd_Log(MD_LOG_ERR, "%s: wwan enable failed %d %s\n", get_state().c_str(),
             cmd_result, cmd_error.c_str());
  }
  else
  {
    mdmd_Log(MD_LOG_INF, "%s: wwan enable success\n", get_state().c_str());
  }
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

void
MdmStatemachine::wwan_stop()
{
  bool is_enabled = true;
  bool is_configured = true;

  if (!wwan_get_status(is_enabled, is_configured))
  { //could not get wwan status, try to disable (Note: first version of config_wwan supports only enable/disable)
    wwan_disable();
  }
  else
  {
    if (!is_configured)
    {
      mdmd_Log(MD_LOG_ERR, "%s: wwan not configured\n", get_state().c_str());
    }
    else if (!is_enabled)
    {
      mdmd_Log(MD_LOG_ERR, "%s: wwan disabled\n", get_state().c_str());
    }
    else
    {
      std::string cmd_output;
      std::string cmd_error;
      const int cmd_result = execute_command("/etc/config-tools/config_wwan --stop", cmd_output, cmd_error);
      if (cmd_result != 0)
      {
        mdmd_Log(MD_LOG_ERR, "%s: wwan stop failed: %d %s\n", get_state().c_str(),
                 cmd_result, cmd_error.c_str());
      }
      else
      {
        mdmd_Log(MD_LOG_INF, "%s: wwan stop success\n", get_state().c_str());
      }
    }
  }
}

void
MdmStatemachine::wwan_start()
{
  bool is_enabled = true;
  bool is_configured = true;

  if (!wwan_get_status(is_enabled, is_configured))
  { //could not get wwan status, try to enable (Note: first version of config_wwan supports only enable/disable)
    wwan_enable();
  }
  else
  {
    if (!is_configured)
    {
      mdmd_Log(MD_LOG_ERR, "%s: wwan interface not configured\n", get_state().c_str());
    }
    else if (!is_enabled)
    { //wwan has to be enabled (e.g. after system start)
      wwan_enable();
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
      }
      else
      {
        mdmd_Log(MD_LOG_INF, "%s: wwan start success\n", get_state().c_str());
      }
    }
  }
}

void
MdmStatemachine::log_event(unsigned int id)
{
  switch(id)
  {
    case DIAG_3GMM_ERR_NOSIM:
    case DIAG_3GMM_ERR_SIMAUTH:
    case DIAG_3GMM_ERR_PORT_NOT_READY:
    case DIAG_3GMM_ERR_INIT_FAIL:
    case DIAG_3GMM_ERR_RESET_FAIL:
      _net_led_blink_code_output = true;
      _dbus_server->emit_diag_signal(DIAG_3GMM_OPER_SIGNAL_6_OFF, true, 0);
      _dbus_server->emit_diag_signal(DIAG_3GMM_OPER_SIGNAL_5_OFF, true, 0);
      _dbus_server->emit_diag_signal(DIAG_3GMM_OPER_SIGNAL_4_OFF, true, 0);
      _dbus_server->emit_diag_signal(DIAG_3GMM_OPER_SIGNAL_3_OFF, true, 0);
      _dbus_server->emit_diag_signal(DIAG_3GMM_OPER_SIGNAL_2_OFF, true, 0);
      _dbus_server->emit_diag_signal(DIAG_3GMM_OPER_SIGNAL_1_OFF, true, 0);
      break;
    default:
      break;
  }
  _dbus_server->emit_diag_signal(id, true, 0);
}

void
MdmStatemachine::update_status_leds()
{
  if (!_net_led_blink_code_output)
  {
    if (_current_oper.is_valid())
    {
      mdmd_Log(MD_LOG_DBG, "%s: operator valid", get_state().c_str());
      int i;

      if (is_oper_registered())
      {
        mdmd_Log(MD_LOG_DBG, "%s: operator registered", get_state().c_str());
        if (_current_oper.is_type_3g())
        {
          log_event( DIAG_3GMM_OPER_NET_3G);
        }
        else
        {
          log_event( DIAG_3GMM_OPER_NET_2G);
        }
      }
      else
      {
        mdmd_Log(MD_LOG_DBG, "%s: operator not registered", get_state().c_str());
        log_event( DIAG_3GMM_OPER_NO_NET );
      }

      for (i = 0; i < _current_signal_step; i++)
      {
        switch (i)
        {
        case 0:
          log_event( DIAG_3GMM_OPER_SIGNAL_1_ON);
          break;
        case 1:
          log_event( DIAG_3GMM_OPER_SIGNAL_2_ON);
          break;
        case 2:
          log_event( DIAG_3GMM_OPER_SIGNAL_3_ON);
          break;
        case 3:
          log_event( DIAG_3GMM_OPER_SIGNAL_4_ON);
          break;
        case 4:
          log_event( DIAG_3GMM_OPER_SIGNAL_5_ON);
          break;
        case 5:
          log_event( DIAG_3GMM_OPER_SIGNAL_6_ON);
          break;
        default:
          break;
        }
      }
      for (i = 6; i > _current_signal_step; i--)
      {
        switch (i)
        {
          case 6:
            log_event( DIAG_3GMM_OPER_SIGNAL_6_OFF);
            break;
          case 5:
            log_event( DIAG_3GMM_OPER_SIGNAL_5_OFF);
            break;
          case 4:
            log_event( DIAG_3GMM_OPER_SIGNAL_4_OFF);
            break;
          case 3:
            log_event( DIAG_3GMM_OPER_SIGNAL_3_OFF);
            break;
          case 2:
            log_event( DIAG_3GMM_OPER_SIGNAL_2_OFF);
            break;
          case 1:
            log_event( DIAG_3GMM_OPER_SIGNAL_1_OFF);
            break;
          default:
            break;
        }
      }
    }
    else
    {
      mdmd_Log(MD_LOG_DBG, "%s: operator invalid", get_state().c_str());
      log_event( DIAG_3GMM_OPER_SIGNAL_6_OFF);
      log_event( DIAG_3GMM_OPER_SIGNAL_5_OFF);
      log_event( DIAG_3GMM_OPER_SIGNAL_4_OFF);
      log_event( DIAG_3GMM_OPER_SIGNAL_3_OFF);
      log_event( DIAG_3GMM_OPER_SIGNAL_2_OFF);
      log_event( DIAG_3GMM_OPER_SIGNAL_1_OFF);
      log_event( DIAG_3GMM_OPER_NO_NET );
    }
  }
}

void
MdmStatemachine::stop_net_led_blink_code()
{
  _net_led_blink_code_output = false;
  update_status_leds();
}

void
MdmStatemachine::set_current_oper_csq(int ber, int rssi)
{
  int new_signal_step;
  _current_oper.set_csq(ber, rssi);
  new_signal_step = _current_oper.get_quality_step();
  if (_current_signal_step != new_signal_step)
  {
    mdmd_Log(MD_LOG_INF, "%s: Signal Quality: level=%d\n", get_state().c_str(), new_signal_step);
    _current_signal_step = new_signal_step;
    update_status_leds();
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
  if (_oper_reg_state != state)
  {
    _oper_reg_state = state;
    update_status_leds();
  }
  if (statusChange)
  {
    mdmd_Log(MD_LOG_INF, "%s: Mobile Network: state=%d(%s), area=%s, cell=%s\n", get_state().c_str(),
             state, ((state==1)||(state==5)) ? "registered" : "not registered", lac.c_str(), cid.c_str());
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
      case _GPRS_REG_INIT_STATE:
        if ((state==1) || (state==5))
        {
          if (!gprs_disabled)
          {
            wwan_start();
          }
        }
        else
        {
          wwan_stop();
        }
        break;
      case 1:
      case 5:
        if ((state!=1) && (state!=5))
        {
          wwan_stop();
        }
        break;
      default:
        if ((state==1) || (state==5))
        {
          if (!gprs_disabled)
          {
            wwan_start();
          }
        }
        break;
    }
    _gprs_reg_state = state;
  }
}

bool
MdmStatemachine::is_new_sim_iccid() const
{
	if (0<_iccid.length())
	{
		std::string iccid;
		std::string pin;
		if(_storage.get_sim(iccid, pin))
		{
			return (0!=_iccid.compare(iccid));
		}
	}
	return true; //assume new SIM when information is not available
}

std::string
MdmStatemachine::get_stored_sim_pin() const
{
	std::string iccid;
	std::string pin;
	_storage.get_sim(iccid, pin);
	return pin;
}

void
MdmStatemachine::set_current_oper(int id, int act)
{
  if ((id != _current_oper.get_id()) || (act != _current_oper.get_act()))
  {
    mdmd_Log(MD_LOG_INF, "%s: Mobile Network: operator=%d, act=%d\n", get_state().c_str(), id, act);
    _current_oper.set_id(id);
    _current_oper.set_act(act);
    _current_oper.set_current();
    update_status_leds();
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
    retVal = NULL;
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
      retVal = NULL;
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
MdmStatemachine::set_gprs_connectivity(const int new_value)
{
  int current_value;
  const bool gprs_disabled_old = is_gprs_disabled();
  const int state = _gprs_reg_state;
  if (!_storage.get_gprs_connectivity(current_value) || (new_value != current_value)) {
    _storage.set_gprs_connectivity(new_value);
    _storage.save();
    if (gprs_disabled_old != is_gprs_disabled())
    {
      mdmd_Log(MD_LOG_INF, "%s: Data Service: state=%d(%s), %s\n", get_state().c_str(),
               state, ((state==1)||(state==5)) ? "registered" : "not registered",
               is_gprs_disabled() ? "disabled" : "enabled");
    }
  }
}

int
MdmStatemachine::get_gprs_connectivity() const
{
  int value;

  if (!_storage.get_gprs_connectivity(value)) {
    mdmd_Log(MD_LOG_ERR, "%s: GprsConnectivity not initialized, assume default value\n",
             get_state().c_str());
    value = _GPRS_CONNECTIVITY_DEFAULT;
  }
  return value;
}

bool
MdmStatemachine::is_gprs_disabled() const
{
  const int gprs_connectivity = get_gprs_connectivity();
  return ((_gprs_temporary_disable) || (gprs_connectivity == _GPRS_CONNECTIVITY_DISABLED));
}

bool
MdmStatemachine::is_gprs_temporary_disabled() const
{
  return _gprs_temporary_disable;
}

void
MdmStatemachine::set_gprs_temporary_disable()
{
  const bool gprs_disabled_old = is_gprs_disabled();
  const int state = _gprs_reg_state;
  _gprs_temporary_disable = true;
  if (!gprs_disabled_old)
  {
    mdmd_Log(MD_LOG_INF, "%s: Data Service: state=%d(%s), temp. disabled\n", get_state().c_str(),
             state, ((state==1)||(state==5)) ? "registered" : "not registered");
  }
}

void
MdmStatemachine::set_gprs_temporary_enable()
{
  const int state = _gprs_reg_state;
  _gprs_temporary_disable = false;
  if (!is_gprs_disabled())
  {
    mdmd_Log(MD_LOG_INF, "%s: Data Service: state=%d(%s), enabled\n", get_state().c_str(),
             state, ((state==1)||(state==5)) ? "registered" : "not registered");
  }
}

bool
MdmStatemachine::set_loglevel(const int loglevel)
{
  bool retVal;
  if (loglevel != _storage.get_loglevel()) {
    retVal = _storage.set_loglevel(loglevel);
    _storage.save();
  }
  else {
    retVal = true;
  }
  return retVal;
}

int
MdmStatemachine::get_loglevel() const
{
  return _storage.get_loglevel();
}

void
MdmStatemachine::set_pdp_address( const std::string &pdp_addr )
{
  if (_pdp_addr.compare(pdp_addr) != 0)
  {
    mdmd_Log(MD_LOG_INF, "%s: Data Service: PDP address=\'%s\'\n", get_state().c_str(), pdp_addr.c_str());
    _pdp_addr = pdp_addr;
  }
}

bool MdmStatemachine::insert_operator_name(int id, const std::string& name)
{
  std::pair<std::map<int,std::string>::iterator,bool> ret;
  ret = _operator_names.insert(std::pair<int,std::string>(id, name));
  return ret.second;
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

