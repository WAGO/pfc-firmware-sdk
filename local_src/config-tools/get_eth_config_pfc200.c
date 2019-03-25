//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2006 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_eth_config_pfc200.c
///
///  \version  $Revision: 6731 $1
///
///  \brief    
///
///  \author   AGa : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//#if 0

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
/*
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
*/

#define PORT0_LABEL "X2"
#define PORT1_LABEL "X1"

#include "config_tool_lib.h"

static int _GetEthPortParameter(int port, const char *param, char *value);
static int GetTcpipState(char *pOutputString);
static void MapOutputToCanonic(char *pOutputString);

void MapOutputToCanonic(char *pOutputString)
{
  if(0 == strcmp(pOutputString, "on"))
  {
    g_strlcpy(pOutputString, "enabled", MAX_LENGTH_ETH_CONFIG_STRING);
  }
  else if(0 == strcmp(pOutputString, "off"))
  {
    g_strlcpy(pOutputString, "disabled", MAX_LENGTH_ETH_CONFIG_STRING);
  }
}

/**
 * @brief Get get the device-id of a specified interface.

 *
 * @param pPortString     String with the requested port (eth0/eth1)
 * @param pSearchString   not used
 * @param searchLineNr    not used
 * @param pOutputString   String with the device label (X1,X2 etc.)
 *                       Must be allocated by calling function!
 *
 * @return 
 */
int GetDeviceId(char* pPortString, 
                char* pSearchString,
                int   searchLineNr,
                char* pOutputString)
{
  char* pProcessorType = NULL;

  // check input-parameter
  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  if ( 0 == strcmp(pPortString, ETH0) )
  {
    strncpy(pOutputString, PORT0_LABEL, MAX_LENGTH_ETH_CONFIG_STRING);
  }
  else if ( 0 == strcmp(pPortString, ETH1) )
  {
    strncpy(pOutputString, PORT1_LABEL, MAX_LENGTH_ETH_CONFIG_STRING);
  }

  return(SUCCESS);
}

/**
 * @brief 
 *
 * @param port
 * @param param
 * @param value
 *
 * @return 
 */
static int _GetEthPortParameter(int port, const char *param, char *value)
{
  int ret = INVALID_PARAMETER;
  char *tmpVal = NULL;

  char *strippedParam = g_strdup(param);
  g_strchomp(strippedParam); // remove trailing whitespaces

  const char *groupId;

  switch(port)
  {
  case 0:
    groupId = SETTINGS_CONF_GROUP_PORT0;
    break;
  case 1:
    groupId = SETTINGS_CONF_GROUP_PORT1;
    break;
  default:
    groupId = "";
    break;
  }

  do
  {
    config_t config = NULL;
    
    if(SUCCESS != (ret = ctlib_ConfigOpen(SETTINGS_CONF_FILE, &config, NULL)))
    {
      break;
    }

    if(SUCCESS != (ret = ctlib_ConfigGetString(config, strippedParam, &tmpVal, (void*) groupId)))
    {
      break;
    }

    // tmpVal gets freed when the config is closed below
    g_strlcpy(value, tmpVal, MAX_LENGTH_ETH_CONFIG_STRING);

    ctlib_ConfigClose(config);
  } while(0);

  g_free(strippedParam);

  return ret;
}

/**
 * @brief 
 *
 * @param pPortString
 * @param pSearchString
 * @param searchLineOffset
 * @param pOutputString
 *
 * @return 
 */
int GetEthPortParameter(char* pPortString,
                        char* pSearchString,
                        int   searchLineOffset,
                        char* pOutputString)
{
  int   status                 = SUCCESS;
  char* pInterfacesFileContent = NULL;

  // check input-parameter
  if((pPortString == NULL) || (pSearchString == NULL) || (pOutputString == NULL))
  {
    return INVALID_PARAMETER;
  }

  if( 0 != strcmp(pSearchString, "autoneg ") &&
      0 != strcmp(pSearchString, "speed ")   &&
      0 != strcmp(pSearchString, "duplex ")  &&
      0 != strcmp(pSearchString, "state ") // port state only => GetTcpipState is used for eth0 state
    )
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  *pOutputString = '\0';

  // spaces at the end seem to be needed for compatibility with old IPC code.

  int port = -1;

  if(0 == strcmp(pPortString, PORT0_LABEL))
  {
    port = 0;
  }
  else if(0 == strcmp(pPortString, PORT1_LABEL))
  {
    port = 1;
  }
  else
  {
    return INVALID_PARAMETER;
  }

  status = _GetEthPortParameter(port, pSearchString, pOutputString);

  if(SUCCESS == status)
  {
    MapOutputToCanonic(pOutputString);
  }


  return status;
}

/**
 * @brief 
 *
 * @param pOutputString
 *
 * @return 
 */
static int GetTcpipState(char *pOutputString)
{

  int status = SUCCESS;

// old implementation: tcpip state based on port states
// May lead to inconsistencies: when ports are off, tcpip state is
// "disabled" => commented out in /etc/network/interfaces.
// Then until new ip address is entered the comments will remain.
//
// Simply returning "enabled" is enough here because the network
// is enabled/disabled via ethernet port settings.

  g_strlcpy(pOutputString, "enabled", MAX_LENGTH_ETH_CONFIG_STRING);
#if 0
  int tcpipState = 1; // on

  if(SUCCESS == GetEthPortParameter(0, "state", pOutputString))
  {
    if(0 == strcmp("off", pOutputString))
    {
      if(SUCCESS == GetEthPortParameter(1, "state", pOutputString))
      {
        if(0 == strcmp("off", pOutputString))
        {
          tcpipState = 0; //off
        }
      }
    }

    if (SUCCESS == status)
    {
      if(tcpipState)
      {
        g_strlcpy(pOutputString, "enabled", MAX_LENGTH_ETH_CONFIG_STRING);
      }
      else
      {
        g_strlcpy(pOutputString, "disabled", MAX_LENGTH_ETH_CONFIG_STRING);
      }
    }
  }
#endif

  return status;
}

/**
 * @brief 
 *
 * @param pPortString
 * @param pSearchString
 * @param searchLineNr
 * @param pOutputString
 *
 * @return 
 */
int GetState(char* pPortString, 
             char* pSearchString,
             int   searchLineNr,
             char* pOutputString)
{
  char configTypeString[MAX_LENGTH_ETH_CONFIG_STRING] = "";
  int status = SUCCESS;

  // check input-parameter
  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }
 
  *pOutputString = '\0';

  int port = -1;

  if(0 == strcmp(pPortString, ETH0))
  {
    return GetTcpipState(pOutputString);
  }
  else if (0 == strcmp(pPortString, PORT0_LABEL))
  {
    port = 0;
  }
  else if(0 == strcmp(pPortString, PORT1_LABEL))
  {
    port = 1;
  }
  else
  {
    return INVALID_PARAMETER;
  }

  status = _GetEthPortParameter(port, "state", pOutputString);

  // map on/off to expected enabled/disabled  
  if(SUCCESS == status)
  {
    MapOutputToCanonic(pOutputString);
  }

  return status;
}

// dumbed-down check -> parameter will be a small non-negative number
static int isNumber(const char *arg)
{
  int i;
  for(i=0;'\0'!= arg[i];++i)
  {
    if(!isdigit(arg[i]))
    {
      return false;
    }
  }
  return true;
}

int CheckValidInterfaceArg(const char *arg)
{
  //ETH1 is currently still needed to get device ids (i.e. labels)
  return !strcmp(arg, ETH0) || !strcmp(arg, ETH1) || !strcmp(arg, PORT0_LABEL) || !strcmp(arg, PORT1_LABEL) ||
         isNumber(arg);
}
//#endif
