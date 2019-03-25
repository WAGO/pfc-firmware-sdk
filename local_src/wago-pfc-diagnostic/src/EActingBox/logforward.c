//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     clearLog.c
///
///  \version  $Revision: 13867 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib-lowlevel.h>
#include "config.h"
#include <diagnostic/diagnostic_API.h>
#ifdef CMD_TOOLS
#include "eactingbox.h"
#endif

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define WAGO_DIAGNOSE_ID_INTERFACE "wago.diagnose"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

GMainContext* com_GEN_GetServerContext(void);

/*
 *-- Function: ShowHelp ---------------------------------------------------
 *
 *  Zeigt den Hilfe-Text für das Programm an
 *
 *------------------------------------------------------------------------------
 */
static void ShowHelp(void)
{
  printf("\n* forward logmessages to syslog-ng *\n\n");
  printf("Usage: logforward\n\n");
}

char * GetParameterAsStr(DBusMessage * message)
{
  DBusMessageIter iter;
  int i;
  int type;
  static char string[4096];

  dbus_message_iter_init  (message,&iter);//iterator initialisieren

  for(i = 0; i < 4; i++)
  {
    dbus_message_iter_next(&iter);
  }
  string[0] = 0;
  while((type = dbus_message_iter_get_arg_type (&iter)) != DBUS_TYPE_INVALID)
  {
    char buffer[32];
    uint8_t  byte;
    uint16_t uWord;
    int16_t  word;
    uint32_t uInteger;
    int32_t  integer;
    dbus_bool_t boolean;
    switch(type)
    {
      case DBUS_TYPE_BYTE:
        dbus_message_iter_get_basic (&iter, &byte);
        sprintf(buffer, "%u", (uint32_t) byte);
        break;
      case DBUS_TYPE_INT16:
        dbus_message_iter_get_basic (&iter, &word);
        sprintf(buffer, "%d", (int) word);
        break;
      case DBUS_TYPE_UINT16:
        dbus_message_iter_get_basic (&iter, &uWord);
        sprintf(buffer, "%u", (uint32_t) uWord);
        break;
      case DBUS_TYPE_INT32:
        dbus_message_iter_get_basic (&iter, &integer);
        sprintf(buffer, "%d", (int) integer);
        break;
      case DBUS_TYPE_UINT32:
        dbus_message_iter_get_basic (&iter, &uInteger);
        sprintf(buffer, "%u", (uint32_t) uInteger);
        break;
      case DBUS_TYPE_BOOLEAN:
        dbus_message_iter_get_basic (&iter, &boolean);
        sprintf(buffer, "%d", (int) boolean);
        break;
      default:
        printf("error unknown argument type");
        type = 0;
        break;
    }
    if(type != 0)
    {
      sprintf(string, "%s %X %s", string, type, buffer);
    }
    dbus_message_iter_next(&iter);
  }

  return string;
}

void SyslogForwarder (com_tConnection* con, com_tComMessage* msg)
{
  struct timeval tv;
  char *programname;
  dbus_bool_t set;
  struct tm *timestamp;
  char timeStr[64];
  char cSet;
  uint32_t id;
  char * member;
  char * parameterStr;
  int perm;


  com_MSG_GetParams(con, msg,  COM_TYPE_INT32,   &tv.tv_sec,
                               COM_TYPE_INT32,   &tv.tv_usec,
                               COM_TYPE_STRING,  &programname,
                               COM_TYPE_BOOLEAN, &set,
                               COM_TYPE_INVALID);

  //timestamp = gmtime(&tv.tv_sec);
  timestamp = localtime(&tv.tv_sec);

  member = (char*) dbus_message_get_member(msg->msg);

  id =  strtoll(member+2, NULL, 16);

  if(!(id & 0x40000000))
  {
    if(id & 0x80000000)
    {
      perm = 1;
    }
    else
    {
      perm = 0;
    }

    if(set == TRUE)
    {
      cSet = 'S';
    }
    else
    {
      cSet = 'R';
    }

    strftime(timeStr, 63, "%a %b %d %Y %T", timestamp);

    parameterStr = GetParameterAsStr(msg->msg);
    log_EVENT_LogDev(LOG_NOTICE, "%s\t%d\t%s.%.6d\t0x%.8X\t%c\t%s", programname, perm, timeStr, (int)tv.tv_usec, id, cSet, parameterStr);
  }
}



/*
 *-- Function: eventmsg_main ---------------------------------------------------
 * 
 *  Main-Funktion für eventmsg
 * 
 *  \param argc
 *  \param argv
 * 
 *  \return <description of the return values of the function>
 *------------------------------------------------------------------------------
 */
int logforward_main(int argc, char **argv)
{
    com_tConnection     con;
    GMainLoop       * loop =  NULL;


    if((argc == 2) && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0))
    {
      ShowHelp();
      return -1;
    }

    if(argc != 1)
    {
      printf("WRONG NUMBER OF ELEMENTS\n\n");
      ShowHelp();
      return -1;
    }

    com_GEN_Init(&con);
    log_EVENT_Init("logforward");
    com_SERV_Stop();
    com_MSG_RegisterSignal(&con, WAGO_DIAGNOSE_ID_INTERFACE, NULL, (com_tHandlerFunction)SyslogForwarder, NULL);
    loop = g_main_loop_new (com_GEN_GetServerContext(),FALSE);
    g_main_loop_run (loop);

    return 0;
}
//---- End of source file ------------------------------------------------------
