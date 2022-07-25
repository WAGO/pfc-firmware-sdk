//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     $File:     $:
///
///  \version  $Revision: 65689 $:
///
///  \brief    <Part of the config-tools package: Manage ethernet port settings>
///
///  \author   <AGa> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "config_tool_lib.h"

#include <glib.h>

// Small WAGO lib to read/write via MIIM (MDIO).
// It is placed to the sysroot by ptxdist.
#include <libpac-mdio.h>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

#define SIZEOF_TABLE(t) ( sizeof(t) / sizeof(t[0]) )

#define CONFIG_FILE_NAME SETTINGS_CONF_FILE 
#define NR_PORTS 2

// State/Transmission mode settings and registers
#define KSZ8863_PHY_ID           -1

#define KSZ8863_TM_REG_PORT0     28 
#define KSZ8863_TM_REG_PORT1     44

#define KSZ8863_STATE_REG_PORT0  29
#define KSZ8863_STATE_REG_PORT1  45

#define KSZ8863_STATE_ENABLED    0x0
#define KSZ8863_STATE_DISABLED   0x8

#define KSZ8863_AUTONEG          0xff 

#define KSZ8863_100M_FULL_DUPLEX 0x7f
#define KSZ8863_100M_HALF_DUPLEX 0x5f

#define KSZ8863_10M_FULL_DUPLEX  0x3f
#define KSZ8863_10M_HALF_DUPLEX  0x1f


//------------------------------------------------------------------------------
// Static variables
//------------------------------------------------------------------------------

enum {
  PARAM_AUTONEG = 0,
  PARAM_DUPLEX,
  PARAM_SPEED,
  PARAM_STATE
};

// expected by getsubopts
static char * const g_settingKeys[] = {
  [PARAM_AUTONEG] = "autoneg",  // on/off
  [PARAM_DUPLEX] = "duplex",    // full/half
  [PARAM_SPEED] = "speed",      // 10/100M
  [PARAM_STATE] = "state",      // on/off
  NULL
};

static const char *g_settingValues[][3] = {
  [PARAM_AUTONEG] = { "on", "off" , NULL},
  [PARAM_DUPLEX]  = { "half", "full", NULL },
  [PARAM_SPEED]   = { "10M", "100M", NULL },
  [PARAM_STATE]   = { "on", "off", NULL }
};

static int g_tmRegAddr[NR_PORTS] = {KSZ8863_TM_REG_PORT0, KSZ8863_TM_REG_PORT1};
static int g_stateRegAddr[NR_PORTS] = {KSZ8863_STATE_REG_PORT0, KSZ8863_STATE_REG_PORT1};

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

//FIXME: find a better name (this is no part of "libconf API")
static int ConfigWriteSettings(config_t config, int port, char *settings);

static int GetPortSetting(config_t portConfig, int port, const char *key, char **value); 
static int SetPortSetting(config_t portConfig, int port, const char *key, char *value); 

static int MdioWriteSettings(config_t config, const char *interface);

static int SettingValueIsValid(int index, const char *value);

/**
 * @brief Checks if the provided value is among the allowed values for a setting defined
 *      by the index nr.
 *
 * @param index Defines a setting (PARAM_AUTONEG, PARAM_DUPLEX etc.) 
 * @param value The value to be checked.
 *
 * @return TRUE is the value is valid; FALSE otherwise.
 */
static int SettingValueIsValid(int index, const char *value)
{
  int ret = FALSE;
  int val = 0;
  while(NULL != g_settingValues[index][val])
  {
    if( 0 == strcmp(g_settingValues[index][val], value))
    {
      ret = TRUE;
      break;
    }
    ++val;
  }
  
  return ret;
}

/**
 * @brief Enables or disables an ethernet port of a given number.
 *
 * @param port  Port number
 * @param state Port state ("on"/"off")
 *
 * @return SUCCESS if the state could be changed.
 *         INVALID_PARAMETER if the state was neither "on" nor "off".
 *         TODO: mdio_write error code
 */
static int MdioSetPortState(const char *interface, int port, char *state)
{
  int ret = SUCCESS;
  int value;

  if(port >= NR_PORTS)
  {
   ret = INVALID_PARAMETER;
  }
 
  if( 0 == strcmp(state, "on"))
  {
    value = KSZ8863_STATE_ENABLED;
  }
  else if ( 0 == strcmp(state, "off"))
  {
    value = KSZ8863_STATE_DISABLED;
  }
  else
  {
    ret = INVALID_PARAMETER;
  }

 
  if(SUCCESS == ret)
  {
    ret = mdio_write(interface, KSZ8863_PHY_ID, g_stateRegAddr[port], value);
  }

  return ret;

}

/**
 * @brief Sets the transmission mode settings (autoneg/speed/duplex) for a given port.
 *
 * @param port    Port number.
 * @param autoneg Autonegatiation: "on"/"off" (When autoneg is enabled, speed/duplex
 *            values are ignored).
 * @param speed   Speed: "10M"/"100M".
 * @param duplex  Duplex mode: "full"/"half".
 *
 * @return SUCCESS if the settings could be set.
 *         INVALID_PARAMETER if one of the parameters was not among the allowed values.
 *         TODO: mdio_write error codes.
 */
static int MdioSetTransmissionMode(const char *interface, int port, char *autoneg, char *speed, char *duplex)
{
  int ret = SUCCESS;
  int value;

  if(port >= NR_PORTS)
  {
   ret = INVALID_PARAMETER;
  }
 
  if ( 0 == strcmp(autoneg, "on") )
  {
    value = KSZ8863_AUTONEG;
  } 
  else if ( 0 == strcmp(autoneg, "off") )
  {
    if ( 0 == strcmp(speed, "10M") && 0 == strcmp(duplex, "half" ))
    {
      value = KSZ8863_10M_HALF_DUPLEX;
    }
    else if (0 == strcmp(speed, "10M") && 0 == strcmp(duplex, "full" ))
    {
      value = KSZ8863_10M_FULL_DUPLEX;
    }
    else if (0 == strcmp(speed, "100M") && 0 == strcmp(duplex, "half" ))
    {
      value = KSZ8863_100M_HALF_DUPLEX;
    }
    else if (0 == strcmp(speed, "100M") && 0 == strcmp(duplex, "full" ))
    {
      value = KSZ8863_100M_FULL_DUPLEX;
    }
    else
    {
      ret = INVALID_PARAMETER;
    }

  }
  else
  {
    ret = INVALID_PARAMETER;
  }

  if(SUCCESS == ret)
  {
    ret = mdio_write(interface, KSZ8863_PHY_ID, g_tmRegAddr[port], value);
  }

  return ret;
}

/**
 * @brief Sets the port parameters according to the configuration. 
 *
 * @param config A representation of the configuration settings.
 *
 * @return SUCCESS if the configuration could be set.
 *         INVALID_PARAMETER if one of the configuration values was invalid.
 *         TODO: mdio_write error codes.
 */
static int MdioWriteSettings(config_t config, const char *interface)
{
  int ret = SUCCESS;
  char *state = NULL;
  char *autoneg = NULL;
  char *speed = NULL;
  char *duplex = NULL;

  int port;
  for(port=0; port < NR_PORTS && SUCCESS == ret; ++port)
  {

    if(SUCCESS != (ret = GetPortSetting(config, port, g_settingKeys[PARAM_STATE], &state)))
    {
      break;
    }

    if(SUCCESS != (ret = GetPortSetting(config, port, g_settingKeys[PARAM_AUTONEG], &autoneg)))
    {
      break;
    }

    if(SUCCESS != (ret = GetPortSetting(config, port, g_settingKeys[PARAM_SPEED], &speed)))
    {
      break;
    }

    if(SUCCESS != (ret = GetPortSetting(config, port, g_settingKeys[PARAM_DUPLEX], &duplex)))
    {
      break;
    }

    if(SUCCESS != (ret = MdioSetPortState(interface, port, state)))
    {
      break;
    }

    ret = MdioSetTransmissionMode(interface, port, autoneg, speed, duplex);

  }

  return ret;
}


/////////////////////////////////////////////////////////////////

/**
 * @brief FIXME
 *
 * @param portConfig
 * @param port
 * @param key
 * @param value
 *
 * @return 
 */
static int GetPortSetting(config_t portConfig, 
                          int port, 
                          const char *key, 
                          char **value) 
{
  const char *groupId;

  // FIXME: would break for a switch with more than 2 ports
  // FIXME: breaks abstraction towards glib parser (group id belongs into ConfigSetString?)
  switch(port)
  {
  case 0:
    groupId = SETTINGS_CONF_GROUP_PORT0;
    break;
  case 1:
    groupId = SETTINGS_CONF_GROUP_PORT1;
    break;
  default:
    return INVALID_PARAMETER;
    break;
  }

  // FIXME: state > autoneg > {speed,duplex}

  return ctlib_ConfigGetString(portConfig, key, value, (void*) groupId);

}

/**
 * @brief FIXME
 *
 * @param portConfig
 * @param port
 * @param key
 * @param value
 *
 * @return 
 */
static int SetPortSetting(config_t portConfig, 
                          int port, 
                          const char *key, 
                          char *value) 
{
  const char *groupId;

  // FIXME: would break for a switch with more than 2 ports
  // FIXME: breaks abstraction towards glib parser (group id belongs into ConfigSetString?)
  switch(port)
  {
  case 0:
    groupId = SETTINGS_CONF_GROUP_PORT0;
    break;
  case 1:
    groupId = SETTINGS_CONF_GROUP_PORT1;
    break;
  default:
    return INVALID_PARAMETER;
    break;
  }

  ctlib_ConfigSetString(portConfig, key, value, (void*) groupId);
  return SUCCESS;
}

      
/**
 * @brief FIXME
 *
 * @param config
 * @param port
 * @param settings
 *
 * @return 
 */
static int ConfigWriteSettings(config_t config, int port, char *settings)
{
  int ret = SUCCESS;

  int settingIndex;
  char *value   = NULL;

  // If settings == NULL: no -s parameter provided => 'write all'
  
  while( NULL != settings && '\0' != *settings && SUCCESS == ret )
  {
    settingIndex = getsubopt(&settings, g_settingKeys, &value);

    if(-1 != settingIndex &&
        settingIndex < (int) SIZEOF_TABLE(g_settingKeys))
    {
      if(SettingValueIsValid(settingIndex, value))
      {
        ret = SetPortSetting(config, port, g_settingKeys[settingIndex], value);
      }
      else
      {
        ret = INVALID_PARAMETER;
      }
    }
  }

  return ret;
}

/**
 * @brief FIXME
 */
static void usage(void)
{
  int keyIndex = 0;
  int valueIndex = 0;

  printf(
"\npfc200_ethtool: manage ethernet port settings.\n"
"\n"
"Read a single setting:\n"
" pfc200_ethtool -p port_nr [-c config_file] [-i eth_interface] -s setting\n"
"\n"
"Set ports according to the settings in the config file:\n"
" pfc200_ethtool -W [-c config_file] [-i eth_interface]\n"
"\n"
"Change settings in config file and reconfigure ports:\n"
" pfc200_ethtool -W -p port_nr [-c config_file] [-i eth_interface] -s setting_list\n"
"\n"
"\n"
" --write/-W:\t\tWrite mode.\n"
" --setting/-s:\t\tSetting to read or write (setting lists are accepted for write mode).\n"
" --interface/-i:\tWhich interface to use (default: eth0).\n"
" --port/-p:\t\tWhich port to use (range 0..%d)\n"
" --config/-c:\t\tWhich config file to use (default: " CONFIG_FILE_NAME  ").\n"
"\n"
"\n"
  , (NR_PORTS - 1));

  printf("Accepted settings and values:\n");

  while(g_settingKeys[keyIndex] != NULL)
  {
    printf(" %s [", g_settingKeys[keyIndex]);

    valueIndex = 0;
    while(g_settingValues[keyIndex][valueIndex] != NULL)
    {
      printf(g_settingValues[keyIndex][valueIndex]);
      ++valueIndex;

      if(g_settingValues[keyIndex][valueIndex] != NULL)
      {
        printf(", ");
      }
    }

    printf("].\n");
    ++keyIndex;
  }

  printf("\n");
  printf("Example for a setting list (write mode only):\n ");
 
  keyIndex = 0;
  while(g_settingKeys[keyIndex] != NULL)
  {
    printf("%s=%s", g_settingKeys[keyIndex], g_settingValues[keyIndex][0]);

    ++keyIndex;

    if(g_settingKeys[keyIndex] != NULL)
    {
      printf(",");
    }
  }
  printf("\n");

}

/**
 * @brief FIXME
 *
 * @param argc
 * @param argv
 *
 * @return 
 */
int main(int argc, char **argv)
{

  int ret = SUCCESS;
  
  char *configuration = NULL;
  char *interface = NULL;
  int port = -1;
  
  char *settings = NULL;

  char *resultString = NULL; // managed by glib
  int write = 0;

  config_t portConfig;
 
  for (;;) {
    int option_index = 0;
    static const char *short_options = "i:p:c:s:W";
    static const struct option long_options[] = {
      {"write",            required_argument,  0, 'W'},
      {"setting",          required_argument,  0, 's'},
      {"interface",        required_argument,  0, 'i'},
      {"port",             required_argument,  0, 'p'},
      {"config",           required_argument,  0, 'c'},
      {0,0,0,0},
    };

   int c = getopt_long(argc, argv, short_options,
        long_options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
    case 'W':
      write = 1;
      break;
    case 'c':
      configuration = g_strdup(optarg);
      break;
    case 'i':
      interface = g_strdup(optarg);
      break;
    case 'p':
      port = strtol(optarg, NULL, 10);
      break;
    case 's':
      settings = g_strdup(optarg);
      break;
    case '?':
    default:
      fprintf(stderr, "ERROR: Unknown parameter: %c\n.", c);
      usage();
      exit(EXIT_FAILURE);
    }
  }

  if(NULL == configuration)
  {
    configuration = g_strdup(CONFIG_FILE_NAME);
  }
  if(NULL == interface)
  {
    interface = g_strdup("eth0");
  }

 do {

    if( -1 == port )
    {

      // Need no port for a write operation without changing settings
      if( settings != NULL || !write)
      {
        fprintf(stderr, "ERROR: a valid port number between 0 and %d has to be provided (i.e. -p 0).\n", (NR_PORTS - 1) );
        usage();
        ret = MISSING_PARAMETER;
        break;
      }
    }

    if(SUCCESS != ctlib_ConfigOpen(configuration, &portConfig, NULL))
    {
      ret = SYSTEM_CALL_ERROR;
      break;
    }

    if(write)
    {

      // Change settings memory representation if settings were provided.
      // The changed settings are used to change switch registers.
      // The changes are saved to file only if the operation was successful.
      // (Does nothing if settings == NULL)
      ret = ConfigWriteSettings(portConfig, port, settings); 
      
      if(SUCCESS == ret)
      {
        ret = MdioWriteSettings(portConfig, interface);  
      }

      if(SUCCESS == ret)
      {
        ret = ctlib_ConfigSaveChanges(portConfig, configuration);
      }

    }
    else // Read
    {
      if(NULL == settings)
      {
        fprintf(stderr, "Error: settings have to be provided (i.e. -s \"duplex\").\n");
        ret = MISSING_PARAMETER;
        break;
      }

      ret = GetPortSetting(portConfig, port, settings, &resultString);
      if(SUCCESS == ret)
      {
        printf("%s", resultString);
      }
    }

    if(NULL != portConfig)
    {
      ctlib_ConfigClose(portConfig);
    }

  } while(0);

  g_free(configuration);
  g_free(interface); 
  g_free(settings);

  return ret;
}   

//---- End of source file ------------------------------------------------------
