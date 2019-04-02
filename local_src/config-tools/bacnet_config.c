//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     bacnet_config.c
///
///  \brief    BACnet config-tool to get and set BACnet specific parameters.
///
///  \author   Nico Baade : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <bacnet_config_api.h>      // interface to BACnet configuration

#include "config_tool_lib.h"        // status codes
#include "liblog/ct_liblog.h"       // write to log file

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

#define INVALID_INDEX           -1
#define INVALID_VALUE           -2
#define NO_ACCESS               -3

#define NO_ARGUMENT             0
#define REQUIRED_ARGUMENT       1
#define OPTIONAL_ARGUMENT       2

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

#define BIT_GET         0
#define BIT_HELP        1
#define BIT_JSON        2
#define BIT_SET         3
#define BIT_VALUE       4
#define BIT_INFO        5

struct option g_longopts[] = {
   { "get",         REQUIRED_ARGUMENT,  NULL,   'g' },
   { "help",        NO_ARGUMENT,        NULL,   'h' },
   { "info",        NO_ARGUMENT,        NULL,   'i' },
   { "json",        NO_ARGUMENT,        NULL,   'j' },
   { "set",         REQUIRED_ARGUMENT,  NULL,   's' },
   { "value",       REQUIRED_ARGUMENT,  NULL,   'v' },
   // last line
   { 0,             0,                  0,      0   }
};

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

static int GetRequest(char* pParameter)
{
  int status = ERROR;
  int parameterIndex = INVALID_INDEX;
  int libConfigStatus = BAC_CONFIG_STATUS_ERROR;

  parameterIndex = bac_CONFIG_getIndexByName(pParameter);
  if(parameterIndex != INVALID_INDEX)
  {
    BAC_CONFIG_PARAMETER_VALUE getParamValue = {0,0,NULL};

    getParamValue.paramId = bac_CONFIG_getParameterIdByIndex(parameterIndex);
    getParamValue.dataType = BAC_CONFIG_DATA_TYPE_STRING;
    // call get function
    libConfigStatus = bac_CONFIG_getParameterValue(&getParamValue);
    if(libConfigStatus == BAC_CONFIG_STATUS_OK)
    {
      printf("%s",getParamValue.pValue);
      bac_CONFIG_freeParameterValue(&getParamValue);
      status = SUCCESS;
    }
  }

  return status;
}


static int JsonRequest(void)
{
  int status = ERROR;
  int libConfigStatus = BAC_CONFIG_STATUS_OK;
  int paramId = 0;
  char *pParamName = NULL;

  printf("[ ");
  while(paramId < BAC_CONFIG_PARAM_END && libConfigStatus == BAC_CONFIG_STATUS_OK)
  {
    BAC_CONFIG_PARAMETER_VALUE getParamValue = {0,0,NULL};
    int parameterIndex = bac_CONFIG_getIndexbyParameterId(paramId);

    if(parameterIndex != INVALID_INDEX)
    {
      getParamValue.paramId = bac_CONFIG_getParameterIdByIndex(parameterIndex);
      getParamValue.dataType = BAC_CONFIG_DATA_TYPE_STRING;
      // call get function
      libConfigStatus = bac_CONFIG_getParameterValue(&getParamValue);
      pParamName = (char*) bac_CONFIG_getNameByIndex(parameterIndex);
      if(libConfigStatus == BAC_CONFIG_STATUS_OK)
      {
        if(0 != paramId)
        {
          printf(", ");
        }
        printf("\"%s\":\"%s\"", pParamName, getParamValue.pValue);
        status = SUCCESS;
        bac_CONFIG_freeParameterValue(&getParamValue);
      }
    }

    paramId++;
  }
  printf(" ]");

  return status;
}


static int SetRequest(char* pParameter, char* pValue)
{
  int status = ERROR;
  int parameterIndex = INVALID_INDEX;
  int libConfigStatus = BAC_CONFIG_STATUS_ERROR;

  parameterIndex = bac_CONFIG_getIndexByName(pParameter);
  if(parameterIndex != INVALID_INDEX)
  {
    if(bac_CONFIG_getAccessByIndex(parameterIndex) == BAC_CONIG_ACCESS_READ_WRITE)
    {
      BAC_CONFIG_PARAMETER_VALUE setParamValue = {0,0,NULL};

      setParamValue.paramId = bac_CONFIG_getParameterIdByIndex(parameterIndex);
      setParamValue.dataType = BAC_CONFIG_DATA_TYPE_STRING;
      setParamValue.pValue = pValue;
      // call set function
      libConfigStatus = bac_CONFIG_setParameterValue(&setParamValue);
      if(libConfigStatus == BAC_CONFIG_STATUS_OK)
      {
        status = SUCCESS;
      }
      else if(libConfigStatus == BAC_CONFIG_STATUS_INVALID_VALUE)
      {
        status = INVALID_VALUE;
      }
    }
    else
    {
      status = NO_ACCESS;
    }
  }

  return status;
}


// get storage option (sd-card or internal-flash)
static int GetStorageOption()
{
  int status = ERROR;
  int libConfigStatus = BAC_CONFIG_STATUS_ERROR;

  libConfigStatus = bac_CONFIG_getStorageOption();

  if(BAC_CONFIG_STORAGE_OPTION_ERROR != libConfigStatus)
  {
    status = SUCCESS;
    printf("%i",libConfigStatus);
  }

  return status;
}


// show help text
static void ShowHelpText(void)
{
  const BAC_CONFIG_PARAMETER_LIST * pParamList = bac_CONFIG_getParameterList();

  printf("\n* Get or set BACnet settings *\n\n");
  printf("Usage: bacnet_configs <options> \n");
  printf("options: --help  || -h              - show help text \n");
  printf("         --info  || -i              - show storage options(all=1, internal only=0) \n");
  printf("         --get   || -g <parameter>  - read parameter value \n");
  printf("         --json  || -j              - read all parameter values in json format \n");
  printf("         --set   || -s <parameter>  - write parameter value (option -v must be set also)\n");
  printf("         --value || -v <value>      - value to set \n");

  if(pParamList != NULL)
  {
    int i;

    printf("\naccess, parameter and value: \n");
    for(i = 0; i < pParamList->listSize; i++)
    {
      switch(pParamList->paramContents[i].paramAccess)
      {
        case BAC_CONIG_ACCESS_READ_ONLY:
          printf("\t(get/---)");
          break;
        case BAC_CONIG_ACCESS_READ_WRITE:
          printf("\t(get/set)");
          break;
        default:
          printf("\t(---/---)");
          break;
      }
      printf("<%s>;\t", pParamList->paramContents[i].pName);
      switch(pParamList->paramContents[i].paramVal.dataType)
      {
        case BAC_CONFIG_DATA_TYPE_STRING:
          switch(pParamList->paramContents[i].paramVal.paramId)
          {
            case BAC_CONFIG_PARAM_PERSISTENCE_STORAGE:
            case BAC_CONFIG_PARAM_TRENDLOG_STORAGE:
            case BAC_CONFIG_PARAM_EVENTLOG_STORAGE:
              printf("<%s> || <%s>", BAC_CONFIG_STORAGE_VALUE_FLASH, BAC_CONFIG_STORAGE_VALUE_CARD);
              break;
            default:
              printf("<string>");
              break;
          }
          break;
        case BAC_CONFIG_DATA_TYPE_BOOL:
          printf("<true> || <false>");
          break;
        case BAC_CONFIG_DATA_TYPE_INT:
          printf("<integer>");
          break;
      }
      printf("\n");
    }
  }
  printf("\n");
}

// argc =   1           2           3             4
// argv =   [0]         [1]         [2]           [3]
//          <program>   <command>   <parameter>   <value>
int main(int    argc,
         char** argv)
{
  int status = SUCCESS;
  int flag = 0;
  char* pParameter = NULL;
  char* pValue = NULL;

  int oc; // option character
  while ((oc = getopt_long(argc, argv, ":g:hijs:v:", g_longopts, NULL)) != -1)
  {
    switch (oc)
    {
      // valid options
      case 'g':
        pParameter = optarg;
        flag |= 1 << BIT_GET;
        break;
      case 'h':
        flag |= 1 << BIT_HELP;
        break;
      case 'i':
        flag |= 1 << BIT_INFO;
        break;
      case 'j':
        flag |= 1 << BIT_JSON;
        break;
      case 's':
        flag |= 1 << BIT_SET;
        pParameter = optarg;
        break;
      case 'v':
        flag |= 1 << BIT_VALUE;
        pValue = optarg;
        break;
      // invalid options or missing option argument
      case '?':
        status = INVALID_PARAMETER;
        break;
      case ':':
        status = MISSING_PARAMETER;
        break;
      default:
        status = ERROR;
    }
  }

  if(status == SUCCESS)
  {
    int parameterIndex = INVALID_INDEX;
    int libConfigStatus = BAC_CONFIG_STATUS_ERROR;

    // help
    if(flag & (1 << BIT_HELP))
    {
      ShowHelpText();
    }

    // info
    else if(flag & (1 << BIT_INFO))
    {
      status = GetStorageOption();
    }

    // get
    else if( (flag & (1 << BIT_GET)) &&
             (pParameter != NULL) )
    {
      status = GetRequest(pParameter);
    }

    // json
    else if(flag & (1 << BIT_JSON))
    {
      status = JsonRequest();
    }

    // set
    else if( (flag & (1 << BIT_SET)) &&
             (flag & (1 << BIT_VALUE)) &&
             (pParameter != NULL) &&
             (pValue != NULL))
    {
      status = SetRequest(pParameter, pValue);
    }

    // unknown -> error
    else
    {
      status = ERROR;
    }
  }

  // data output
  if(status != SUCCESS)
  {
    if(status == INVALID_VALUE)
    { // invalid value (out of range, etc.)
      ct_liblog_setLastError("Invalid value");
    }
    else if(NO_ACCESS)
    { // parameter is only readable
      ct_liblog_setLastError("No write access");
    }
    else
    { // other internal error
      ct_liblog_setLastError("Internal error while configure BACnet parameters.");
    }
    // save error
    ct_liblog_reportError(status, "Error in bacnet_config.");
  }

  return status;
}

//---- End of source file ------------------------------------------------------

