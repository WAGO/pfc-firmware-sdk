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


PersistentStorage::PersistentStorage()
{
    GError *err = NULL;

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
        err = NULL;
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
    GError *err = NULL;
    int value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "log_level", &err);
    if (err)
    {
      value = mdmd_GetLogLevel();
      g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "log_level", value);
      g_error_free(err);
      err = NULL;
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
      err = NULL;
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
      err = NULL;
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
  GError *err = NULL;
  int value;

  value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_selection", &err);
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
  g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "net_selection", mode);
}

bool PersistentStorage::get_port_enabled() const
{
    GError *err = NULL;
    int value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "port_enabled", &err);
    if (err)
    {
      value = 1;
      g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "port_enabled", value);
      g_error_free(err);
      err = NULL;
    }
    return (value != 0);
}

void PersistentStorage::set_port_enabled(bool enabled)
{
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "port_enabled", (enabled ? 1 : 0));
}


bool PersistentStorage::get_oper(int &id, int &act) const
{
    GError *err = NULL;
    int value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "oper_id", &err);
    if (err)
    {
      g_error_free(err);
    	return false;
    }
    id = value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "oper_act", &err);
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
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "oper_id", id);
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "oper_act", act);
}

void PersistentStorage::remove_oper()
{
    GError *err = 0;
    g_key_file_remove_key(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "oper_id", &err);
    if (err)
    {
      g_error_free(err);
      err = NULL;
    }
    g_key_file_remove_key(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "oper_act", &err);
    if (err)
    {
      g_error_free(err);
      err = NULL;
    }
}

bool PersistentStorage::get_gprs_connectivity(int &state) const
{
    GError *err = NULL;
    int value;

    value = g_key_file_get_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_connectivity", &err);
    if (err)
    {
      g_error_free(err);
      return false;
    }
    state = value;
    return true;
}

void PersistentStorage::set_gprs_connectivity(const int value)
{
    g_key_file_set_integer(_gkeyfile, MDMD_CONFIG_FILE_SECTION, "gprs_connectivity", value);
}


void PersistentStorage::save()
{
    gchar *dat = g_key_file_to_data(_gkeyfile, NULL, NULL);
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
