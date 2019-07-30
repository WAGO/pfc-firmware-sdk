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
///  \brief    BACnet config-tool to backup and restore BACnet specific parameters.
///
///  \author   Nico Baade : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <getopt.h>

#include <bacnet_config_api.h>      // interface to BACnet configuration

#include "config_tool_lib.h"        // status codes
#include "liblog/ct_liblog.h"       // write to log file

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

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

#define BIT_SAVE        0
#define BIT_HELP        1
#define BIT_RESTORE     2
#define BIT_PARAM_COUNT 3

struct option g_longopts[] = {
   { "save",         REQUIRED_ARGUMENT,  NULL,   's' },
   { "help",         NO_ARGUMENT,        NULL,   'h' },
   { "restore",      REQUIRED_ARGUMENT,  NULL,   'r' },
   { "param-count",  NO_ARGUMENT,        NULL,   'p' },
   // last line
   { 0,             0,                  0,      0   }
};

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

// show help text
static void ShowHelpText(void)
{
  printf("\n* Backup and restore BACnet settings *\n\n");
  printf("Usage: bacnet_backup_restore <options> \n");
  printf("options: --save         || -s  <backup-file>  - backup BACnet settings \n");
  printf("         --restore      || -r  <backup-file>  - restore BACnet settings \n");
  printf("         --param-count  || -p                 - get number of BACnet settings to save \n");
  printf("\n");
}

// argc =   1           2           3             4
// argv =   [0]         [1]         [2]           [3]
//          <program>   <command>   <parameter>   <value>
int main(int    argc,
         char** argv)
{
  int status = SUCCESS;
  BAC_CONFIG_STATUS libConfigStatus = BAC_CONFIG_STATUS_OK;
  int flag = 0;
  char* pBackupRestoreFile = NULL;

  int oc; // option character
  while ((oc = getopt_long(argc, argv, ":s:hr:p", g_longopts, NULL)) != -1)
  {
    switch (oc)
    {
      // valid options
      case 's':
        pBackupRestoreFile = optarg;
        flag |= 1 << BIT_SAVE;
        break;
      case 'h':
        flag |= 1 << BIT_HELP;
        break;
      case 'r':
        pBackupRestoreFile = optarg;
        flag |= 1 << BIT_RESTORE;
        break;
      case 'p':
        flag |= 1 << BIT_PARAM_COUNT;
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
    // help
    if(flag & (1 << BIT_HELP))
    {
      ShowHelpText();
    }

    // parameter count
    else if(flag & (1 << BIT_PARAM_COUNT))
    {
      unsigned int paramCount = bac_CONFIG_getParameterCount();
      printf("%i\n", paramCount);
    }

    // save
    else if( (flag & (1 << BIT_SAVE)) &&
             (pBackupRestoreFile != NULL) )
    {

      libConfigStatus = bac_CONFIG_backupParameter(pBackupRestoreFile);
      if(libConfigStatus != BAC_CONFIG_STATUS_OK)
      {
        ct_liblog_setLastError("Internal error while backup BACnet parameters.");
        status = ERROR;
      }
    }

    // restore
    else if( (flag & (1 << BIT_RESTORE)) &&
             (pBackupRestoreFile != NULL) )
    {
      libConfigStatus = bac_CONFIG_restoreParameter(pBackupRestoreFile);
      if(libConfigStatus != BAC_CONFIG_STATUS_OK)
      {
        ct_liblog_setLastError("Internal error while restore BACnet parameters.");
        status = ERROR;
      }
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
    // save error
    ct_liblog_reportError(status, "Error in bacnet_backup_restore.");
  }

  return status;
}

//---- End of source file ------------------------------------------------------

