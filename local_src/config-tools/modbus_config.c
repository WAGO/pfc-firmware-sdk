//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     modbus_config.c
///
///  \version  $Revision: 18092 $1
///
///  \brief    <short description of the file contents>
///
///  \author   <u015614> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <communication_API.h>
#include "ct_dbus.h"
#include "MbConfig.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
static void print_help();
static void print_status(mbcfg_Status_type status);
static void get_service(mbcfg_Context_type *pCtx, mbcfg_ServiceProtocol_type serviceProtocol,
                        const char* szParameterName);
static void set_service(mbcfg_Context_type *pCtx, mbcfg_ServiceProtocol_type serviceProtocol,
                        const char* szParameterName, const char* szValue);
static void get_alternative_watchdog(mbcfg_Context_type *pCtx, const char* szParameterName);
static void set_alternative_watchdog(mbcfg_Context_type *pCtx, const char* szParameterName, const char* szValue);
static void get_statistics(const uint8_t fResetFlag);
static void postwrite_trigger(const char * path);
static int decode_flag_value(const char *szValue, mbcfg_Flag_type *pResult);
static int decode_uint16_value(const char *szValue, uint16_t *pResult);

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables’ and constants’ definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
  mbcfg_Context_type *pCtx = NULL;
  mbcfg_Status_type status = mbcfg_CreateContext(&pCtx);
  if (MBCFG_STATUS_OK != status)
  {
    print_status(status);
  }
  else
  {
    if ((3 == argc) && (0 == strcmp("get", argv[1])))
    {
      if (0 == strcmp("stat", argv[2])) {
        get_statistics(false);
      }
      else
      { //unknown group
        print_help();
      }
    }
    else if ((4 == argc) && (0 == strcmp("get", argv[1])))
    {
      if (0 == strcmp("alternative-watchdog", argv[2])) {
        get_alternative_watchdog(pCtx, argv[3]);
      }
      else if (0 == strcmp("tcp", argv[2]))
      {
        get_service(pCtx, MBCFG_SERVICE_TCP, argv[3]);
      }
      else if (0 == strcmp("udp", argv[2]))
      {
        get_service(pCtx, MBCFG_SERVICE_UDP, argv[3]);
      }
      else if (0 == strcmp("stat", argv[2]))
      {
        if (0 == strcmp("reset", argv[3])) {
          get_statistics(true);
        }
        else
        { //unknown group
          print_help();
        }
      }
      else
      { //unknown group
        print_help();
      }
    }
    else if ((5 == argc) && (0 == strcmp("set", argv[1])))
    {
      if (0 == strcmp("alternative-watchdog", argv[2])) {
        set_alternative_watchdog(pCtx, argv[3], argv[4]);
      }
      else if (0 == strcmp("tcp", argv[2]))
      {
        set_service(pCtx, MBCFG_SERVICE_TCP, argv[3], argv[4]);
      }
      else if (0 == strcmp("udp", argv[2]))
      {
        set_service(pCtx, MBCFG_SERVICE_UDP, argv[3], argv[4]);
      }
      else
      { //unknown group
        print_help();
      }
    }
    else
    { //unknown command
      print_help();
    }
    status = mbcfg_DestroyContext(pCtx);
    (void)status; //ignore failure
  }
  return 0;
}


static void print_help()
{
  printf( "\n*** Get and set Modbus service parameter ***\n\n");
  printf( "Usage: modbus_config <command> <group> <parameter> [value]\n");
  printf( "  commands:  get - read value\n");
  printf( "             set - write value\n");
  printf( "  groups:    tcp - Modbus TCP service\n");
  printf( "             udp - Modbus UDP service\n");
  printf( "             alternative-watchdog - Modbus alternative watchdog\n");
  printf( "             stat - Modbus Error Counter (Statistics)\n");
  printf( "  parameter: enabled - Modbus service or watchdog state {true | false}\n");
  printf( "             port    - Modbus service port {0..65535}\n");
  printf( "             timeout - Modbus watchdog timeout {0..65535}\n");
  printf( "             options - Modbus watchdog options mask {0..65535}\n");
  printf( "             reset   - Modbus Error Counter (Statistics) Read & Reset\n\n");
  printf( "Examples:\n");
  printf( "  modbus_config get tcp enabled\n");
  printf( "  modbus_config set tcp enabled false\n");
  printf( "  modbus_config get tcp port\n");
  printf( "  modbus_config set tcp port 502\n");
  printf( "  modbus_config get alternative-watchdog enabled\n");
  printf( "  modbus_config set alternative-watchdog enabled true\n");
  printf( "  modbus_config get stat\n");
  printf( "  modbus_config get stat reset\n");
}


static void print_status(mbcfg_Status_type status)
{
  switch(status)
  {
    default: //no break;
    case MBCFG_STATUS_FAILED:              printf("ERROR: internal failure"); break;
    case MBCFG_STATUS_NOT_IMPLEMENTED:     printf("ERROR: function not implemented"); break;
    case MBCFG_STATUS_INVALID_PARAMETER:   printf("ERROR: invalid parameter"); break;
    case MBCFG_STATUS_NO_MEMORY_RESOURCES: printf("ERROR: not enough memory"); break;
    case MBCFG_STATUS_NO_THREAD_RESOURCES: printf("ERROR: not enough thread resources"); break;
    case MBCFG_STATUS_NOT_STARTED:         printf("ERROR: processing not started"); break;
    case MBCFG_STATUS_OK:                  printf("OK"); break;
  }
}


static void get_alternative_watchdog(mbcfg_Context_type *pCtx, const char* szParameterName)
{
  mbcfg_AlternativeWatchdogParameter_type watchdogParameter;
  mbcfg_Status_type const status = mbcfg_ReadAlternativeWatchdogParameter(pCtx, &watchdogParameter);
  if (MBCFG_STATUS_OK != status)
  {
    print_status(status);
  }
  else if (0 == strcmp("enabled", szParameterName))
  {
    printf("%s", (watchdogParameter.enabled == MBCFG_FLAG_DISABLED) ? "false" : "true");
  }
  else if (0 == strcmp("timeout", szParameterName))
  {
    printf("%d", watchdogParameter.timeout);
  }
  else if (0 == strcmp("options", szParameterName))
  {
    printf("%d", watchdogParameter.options);
  }
  else
  { //unknown parameter
    print_help();
  }
}


static void set_alternative_watchdog(mbcfg_Context_type *pCtx, const char* szParameterName, const char* szValue)
{
  mbcfg_Flag_type fEnabled;
  uint16_t nTimeout;
  uint16_t nOptions;

  //Note: don't call postwrite_trigger, watchdog online reconfiguration is not specified
  if ((0 == strcmp("enabled", szParameterName)) && (0 == decode_flag_value(szValue, &fEnabled)))
  {
    mbcfg_Status_type const status = mbcfg_WriteAlternativeWatchdogEnabled(pCtx, MBCFG_FLAG_DISABLED, NULL, NULL,
                                                                           fEnabled);
    if (MBCFG_STATUS_OK != status)
    {
      print_status(status);
    }
  }
  else if ((0 == strcmp("timeout", szParameterName)) && (0 == decode_uint16_value(szValue, &nTimeout)))
  {
    mbcfg_Status_type const status = mbcfg_WriteAlternativeWatchdogTimeout(pCtx, MBCFG_FLAG_DISABLED, NULL, NULL,
                                                                           nTimeout);
    if (MBCFG_STATUS_OK != status)
    {
      print_status(status);
    }
  }
  else if ((0 == strcmp("options", szParameterName)) && (0 == decode_uint16_value(szValue, &nOptions)))
  {
    mbcfg_Status_type const status = mbcfg_WriteAlternativeWatchdogOptions(pCtx, MBCFG_FLAG_DISABLED, NULL, NULL,
                                                                           nOptions);
    if (MBCFG_STATUS_OK != status)
    {
      print_status(status);
    }
  }
  else
  { //unknown parameter or invalid value
    print_help();
  }
}


static void get_service(mbcfg_Context_type *pCtx, mbcfg_ServiceProtocol_type serviceProtocol,
                        const char* szParameterName)
{
  mbcfg_ServiceParameter_type serviceParameter;
  mbcfg_Status_type const status = mbcfg_ReadServiceParameter(pCtx, serviceProtocol, &serviceParameter);
  if (MBCFG_STATUS_OK != status)
  {
    print_status(status);
  }
  else if (0 == strcmp("enabled", szParameterName))
  {
    printf("%s", (serviceParameter.enable == MBCFG_FLAG_DISABLED) ? "false" : "true");
  }
  else if (0 == strcmp("port", szParameterName))
  {
    printf("%d", serviceParameter.port);
  }
  else
  { //unknown parameter
    print_help();
  }
}


static void set_service(mbcfg_Context_type *pCtx, mbcfg_ServiceProtocol_type serviceProtocol,
                        const char* szParameterName, const char* szValue)
{
  mbcfg_Flag_type fEnabled;
  uint16_t nPort;
  if ((0 == strcmp("enabled", szParameterName)) && (0 == decode_flag_value(szValue, &fEnabled)))
  {
    mbcfg_Status_type const status = mbcfg_WriteServiceEnable(pCtx, MBCFG_FLAG_DISABLED, NULL, NULL, serviceProtocol,
                                                              fEnabled);
    if (MBCFG_STATUS_OK != status)
    {
      print_status(status);
    }
    else
    {
      postwrite_trigger((serviceProtocol == MBCFG_SERVICE_TCP) ? CT_DBUS_PATH_MODBUS_TCP_ENABLED :
                                                                 CT_DBUS_PATH_MODBUS_UDP_ENABLED);
    }
  }
  else if ((0 == strcmp("port", szParameterName)) && (0 == decode_uint16_value(szValue, &nPort)))
  {
    mbcfg_Status_type const status = mbcfg_WriteServicePort(pCtx, MBCFG_FLAG_DISABLED, NULL, NULL, serviceProtocol,
                                                            nPort);
    if (MBCFG_STATUS_OK != status)
    {
      print_status(status);
    }
    else
    {
      postwrite_trigger((serviceProtocol == MBCFG_SERVICE_TCP) ? CT_DBUS_PATH_MODBUS_TCP_PORT :
                                                                 CT_DBUS_PATH_MODBUS_UDP_PORT);
    }
  }
  else
  { //unknown parameter or invalid value
    print_help();
  }
}


static void get_statistics(const uint8_t fResetFlag)
{
  com_tConnection con;
  if (0 == com_GEN_Init(&con))
  {
    int callResult;
    com_MSG_SetMethodCallTimeout(&con, 5000);
    callResult = com_MSG_MethodCall(&con, CT_DBUS_NAME_MODBUS, CT_DBUS_PATH_MODBUS_STAT, CT_DBUS_METHOD_STATISTICS, COM_TYPE_INVALID);
    (void)callResult; //ignore failure
    com_GEN_Close(&con);
  }
}

static void postwrite_trigger(const char *szPath)
{
  com_tConnection con;
  if (0 == com_GEN_Init(&con))
  {
    int callResult;
    com_MSG_SetMethodCallTimeout(&con, 5000);
    callResult = com_MSG_MethodCall(&con, CT_DBUS_NAME_MODBUS, szPath, CT_DBUS_METHOD_POSTWRITE, COM_TYPE_INVALID);
    (void)callResult; //ignore failure
    com_GEN_Close(&con);
  }
}


static int decode_flag_value(const char *szValue, mbcfg_Flag_type *pResult)
{
  if (0 == strcmp("true", szValue))
  {
    *pResult = MBCFG_FLAG_ENABLED;
    return 0;
  }
  if (0 == strcmp("false", szValue))
  {
    *pResult = MBCFG_FLAG_DISABLED;
    return 0;
  }
  return -1;
}


static int decode_uint16_value(const char *szValue, uint16_t *pResult)
{
  long int nValue = strtol(szValue, NULL, 10);
  if ((nValue < 0) || (nValue > UINT16_MAX))
  {
    return -1;
  }
  *pResult = (uint16_t)nValue;
  return 0;
}

