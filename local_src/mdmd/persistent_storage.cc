// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


#include "persistent_storage.h"
#include <exception>
#include <stdio.h>
#include <unistd.h>

#include "mdmd_log.h"
#include <errno.h>

//TODO: _gkeyfile make unique_ptr
PersistentStorage::PersistentStorage()
{
    GError *err = nullptr;

    _gkeyfile = g_key_file_new();

    if (!_gkeyfile)
	throw std::exception();

    if( access( MDMD_CONFIG_FILE, F_OK ) == -1 )
    {
      mdmd_Log(MD_LOG_WRN, "Configuration file %s doesn't exist\n", MDMD_CONFIG_FILE);
    }
    else
    {
      g_key_file_load_from_file( _gkeyfile, MDMD_CONFIG_FILE, G_KEY_FILE_KEEP_COMMENTS, &err );
      if (err)
      {
        g_error_free(err);
        err = nullptr;
        mdmd_Log(MD_LOG_WRN, "Could not read configuration file %s\n", MDMD_CONFIG_FILE);
      }
    }
}

PersistentStorage::~PersistentStorage()
{
    g_key_file_free(_gkeyfile);
}


int PersistentStorage::get_loglevel(void) const
{
    return mdmd_GetLogLevel();
}

bool PersistentStorage::set_loglevel(const int loglevel)
{
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "log_level", loglevel);
    return mdmd_SetLogLevel( loglevel );
}

void PersistentStorage::init_loglevel(void)
{
    GError *err = nullptr;
    int value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "log_level", &err);
    if (err)
    {
      value = mdmd_GetLogLevel();
      g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "log_level", value);
      g_error_free(err);
      err = nullptr;
    }
    else
    {
      if (!mdmd_SetLogLevel( value ))
      {
        value = mdmd_GetLogLevel();
        g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "log_level", value);
      }
    }
}

bool PersistentStorage::get_sim(std::string &iccid, std::string &pin) const
{
    GError *err = 0;
    char *value;
    value = g_key_file_get_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sim_iccid", &err);
    if (err)
    {
      g_error_free(err);
      err = nullptr;
      value = 0;
    }
    if (0 == value)
    {
    	return false;
    }
    iccid = value;
    g_free(value);

    value = g_key_file_get_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sim_pin", &err);
    if (err)
    {
      g_error_free(err);
      err = nullptr;
      value = 0;
    }
    if (0 == value)
    {
    	return false;
    }
    pin = value;
    g_free(value);
    return true;
}

void PersistentStorage::set_sim(const std::string &iccid, const std::string &pin)
{
    g_key_file_set_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sim_iccid", iccid.c_str());
    g_key_file_set_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sim_pin", pin.c_str());
}

bool PersistentStorage::get_selection(int &mode) const
{
  GError *err = nullptr;
  int value;

  value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_selection_mode", &err);
  if (err)
  {
    g_error_free(err);
    return false;
  }
  mode = value;
  return true;
}

void PersistentStorage::set_selection(const int mode)
{
  g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_selection_mode", mode);
}

bool PersistentStorage::get_port_enabled() const
{
    GError *err = nullptr;
    int value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "port_state", &err);
    if (err)
    {
      value = 1;
      g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "port_state", value);
      g_error_free(err);
      err = nullptr;
    }
    return (value != 0);
}

void PersistentStorage::set_port_enabled(bool enabled)
{
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "port_state", (enabled ? 1 : 0));
}


bool PersistentStorage::get_oper(int &id, int &act) const
{
    GError *err = nullptr;
    int value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_manual_oper", &err);
    if (err)
    {
      g_error_free(err);
    	return false;
    }
    id = value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_manual_act", &err);
    if (err)
    {
      g_error_free(err);
    	return false;
    }
    act = value;
    return true;
}

void PersistentStorage::set_oper(int id, int act)
{
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_manual_oper", id);
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_manual_act", act);
}

void PersistentStorage::remove_oper()
{
    GError *err = 0;
    g_key_file_remove_key(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_manual_oper", &err);
    if (err)
    {
      g_error_free(err);
      err = nullptr;
    }
    g_key_file_remove_key(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_manual_act", &err);
    if (err)
    {
      g_error_free(err);
      err = nullptr;
    }
}

bool PersistentStorage::get_gprs_access(GprsAccessConfig& gprs_access) const
{
	GError *gprs_state_err = nullptr;
    GError *gprs_auth_err = nullptr;
	int   gprs_state = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_state", &gprs_state_err);
    char* gprs_apn  = g_key_file_get_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_apn", nullptr);
    int   gprs_auth = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_auth_type", &gprs_auth_err);
    char* gprs_user = g_key_file_get_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_username", nullptr);
    char* gprs_pass = g_key_file_get_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_password", nullptr);
    bool result = ((!gprs_state_err) && (gprs_apn) && (!gprs_auth_err) && (gprs_user) && (gprs_pass));

    if (result)
    {
		gprs_access.set_state(gprs_state);
        gprs_access.set_apn(gprs_apn);
        gprs_access.set_auth(gprs_auth);
        gprs_access.set_user(gprs_user);
        gprs_access.set_pass(gprs_pass);
    }
    if (gprs_state_err)
    {
        g_error_free(gprs_state_err);
    }
    if (gprs_apn)
    {
        g_free(gprs_apn);
    }
    if (gprs_auth_err)
    {
        g_error_free(gprs_auth_err);
    }
    if (gprs_user)
    {
        g_free(gprs_user);
    }
    if (gprs_pass)
    {
        g_free(gprs_pass);
    }
    return result;
}

void PersistentStorage::set_gprs_access(const GprsAccessConfig& gprs_access)
{
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_state", gprs_access.get_state());
    g_key_file_set_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_apn", gprs_access.get_apn().c_str());
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_auth_type", gprs_access.get_auth());
    g_key_file_set_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_username", gprs_access.get_user().c_str());
    g_key_file_set_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_password", gprs_access.get_pass().c_str());
}


bool PersistentStorage::get_sms_reporting_config(SmsEventReportingConfig& sms_config) const
{
    GError *err = nullptr;
    int mode;
    int mt;
    int bm;
    int ds;
    int bfr;

    mode = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_mode", &err);
    if (err)
    {
      g_error_free(err);
      return false;
    }

    mt = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_mt", &err);
    if (err)
    {
      g_error_free(err);
      return false;
    }

    bm = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_bm", &err);
    if (err)
    {
      g_error_free(err);
      return false;
    }

    ds = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_ds", &err);
    if (err)
    {
      g_error_free(err);
      return false;
    }

    bfr = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_bfr", &err);
    if (err)
    {
      g_error_free(err);
      return false;
    }
    sms_config.set_mode(mode);
    sms_config.set_mt(mt);
    sms_config.set_bm(bm);
    sms_config.set_ds(ds);
    sms_config.set_bfr(bfr);

    return true;
}

void PersistentStorage::set_sms_reporting_config(const SmsEventReportingConfig& sms_config)
{
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_mode", sms_config.get_mode());
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_mt", sms_config.get_mt());
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_bm", sms_config.get_bm());
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_ds", sms_config.get_ds());
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cnmi_bfr", sms_config.get_bfr());
}



bool PersistentStorage::get_sms_storage_config(SmsStorageConfig& sms_storage_config) const
{
    char* mem1;
    char* mem2;
    char* mem3;

    mem1 = g_key_file_get_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cpms_mem1", nullptr);
    if(mem1 == nullptr)
    {
      return false;
    }

    mem2 = g_key_file_get_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cpms_mem2", nullptr);
    if(mem2 == nullptr)
    {
      g_free(mem1);
      return false;
    }
 
    mem3 = g_key_file_get_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cpms_mem3", nullptr);
    if(mem3 == nullptr)
    {
      g_free(mem1);
      g_free(mem2);
      return false;
    }
 
    sms_storage_config.set_mem1(mem1);
    sms_storage_config.set_mem2(mem2);
    sms_storage_config.set_mem3(mem3);

    g_free(mem1);
    g_free(mem2);
    g_free(mem3);

    return true;
}

void PersistentStorage::set_sms_storage_config(const SmsStorageConfig& sms_storage_config)
{
    g_key_file_set_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cpms_mem1", sms_storage_config.get_mem1().c_str());
    g_key_file_set_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cpms_mem2", sms_storage_config.get_mem2().c_str());
    g_key_file_set_string(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "sms_cpms_mem3", sms_storage_config.get_mem3().c_str());
}

void PersistentStorage::save()
{
    gchar *dat = g_key_file_to_data(_gkeyfile, nullptr, nullptr);
    if (dat==0)
      throw std::exception();

    FILE *fp = fopen(MDMD_CONFIG_FILE,"w");
    if (fp==0)
    {
      mdmd_Log(MD_LOG_WRN, "Could not open configuration file %s for writing.\n", MDMD_CONFIG_FILE);
    }
    else
    {
      fprintf(fp, "%s", dat);
      fclose(fp);
    }
    g_free(dat);
}
