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
///  \file     getledstate.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include "parse_log.h"

#include "eactingbox.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
int run;
static const char * persistentLog    =   "/home/log/wagolog.log";
static const char * nonPersistentLog =   "/var/log/wago/wagolog.log";


tLogData * GetLastOccurOfId(uint32_t id)
{
  tLogData * ret = NULL;
  FILE * fpLog = NULL;
  char * logfile;
  char buffer[4096];
  char buffer2[4096];
  bool found = false;;

  if(id & 0x80000000)
  {
    logfile = (char*)persistentLog;
    id = (id & (~(0x80000000)));
  }
  else
  {
    logfile = (char*)nonPersistentLog;
  }

  fpLog = fopen(logfile, "r");
  if(fpLog != NULL)
  {
    while(NULL != fgets(buffer, 4096, fpLog))
    {
      if(true == CheckLineForId(id, buffer))
      {
        strcpy(buffer2, buffer);
        found = true;
      }
    }
    fclose(fpLog);
  }
  if(found == true)
  {
    ret = ParseLogLine(buffer2);
  }

  return ret;
}

void GetParameterFromString(char * pParams, char **pEnd,int *type, void **value)
{
  static int32_t  integer;
  static uint32_t uInteger;
  static int16_t  word;
  static uint16_t uWord;
  static uint8_t  byte;
  static dbus_bool_t boolean;

  *type = strtol(pParams, &pParams, 16);
  switch(*type)
  {
    case DBUS_TYPE_BYTE:
      byte = strtol(pParams, pEnd, 10);
      *value = &byte;
      break;
    case DBUS_TYPE_INT16:
      word = (int16_t) strtol(pParams, pEnd, 10);
      *value = &word;
      break;
    case DBUS_TYPE_UINT16:
      uWord = (uint16_t) strtol(pParams, pEnd, 10);
      *value = &uWord;
      break;
    case DBUS_TYPE_INT32:
      integer = strtol(pParams, pEnd, 10);
      *value = &integer;
      break;
    case DBUS_TYPE_UINT32:
      uInteger = (uint32_t)strtoll(pParams, pEnd, 10);
      *value = &uInteger;
      break;
    case DBUS_TYPE_BOOLEAN:
      boolean = strtol(pParams, pEnd, 10);
      *value = &boolean;
      break;
    default:
      integer = 0;
      *value = (void*)integer;
      break;
  }
  if(   (**pEnd == '\n')
     || (**pEnd == '\0'))
  {
    *pEnd = NULL;
  }
}

DBusHandlerResult MethodHandler (DBusConnection *connection,
                                DBusMessage    *message,
                                void           *user_data)
{
 DBusHandlerResult ret = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
 DBusMessage * reply = NULL;
 (void)user_data;

 if((dbus_message_has_member     (message,"getInfo") == TRUE) )
 {
   run = 10;
   DBusError      error;
   uint32_t id;
   DBusMessageIter iter;
   tLogData * idLog;
   dbus_error_init (&error);
   dbus_bool_t dbSet = FALSE;

   dbus_message_get_args(message, &error,
                         DBUS_TYPE_UINT32, &id,
                         DBUS_TYPE_INVALID);

   idLog = GetLastOccurOfId(id);

   reply = dbus_message_new_method_return  ( message) ;
   dbus_message_iter_init_append(reply, &iter);
   if(idLog != NULL)
   {
     if(idLog->set == true)
     {
       dbSet = TRUE;
     }
     dbus_message_iter_append_basic(&iter,DBUS_TYPE_INT32,(const void *) &idLog->timestamp.tv_sec);
     dbus_message_iter_append_basic(&iter,DBUS_TYPE_INT32,(const void *) &idLog->timestamp.tv_usec);
     dbus_message_iter_append_basic(&iter,DBUS_TYPE_BOOLEAN,(const void *) &dbSet);
     if(idLog->variables != NULL)
     {
       char * pParams = idLog->variables;
       while(pParams != NULL)
       {
         int    type;
         void * value;
         GetParameterFromString(pParams, &pParams, &type, &value);
         dbus_message_iter_append_basic(&iter,type,(const void *) value);
       }
     }
   }
   else
   {
     int trash  =0;
     dbus_message_iter_append_basic(&iter,DBUS_TYPE_INT32,(const void *) &trash);
     dbus_message_iter_append_basic(&iter,DBUS_TYPE_INT32,(const void *) &trash);
     dbus_message_iter_append_basic(&iter,DBUS_TYPE_BOOLEAN,(const void *) &dbSet);
   }
 }

 if(reply != NULL)
 {
   dbus_connection_send(connection, reply, 0);
   dbus_connection_flush(connection);
   dbus_message_unref(reply);
   //dbus_message_unref(message);
   ret = DBUS_HANDLER_RESULT_HANDLED;
 }

 return ret;
}

/*
 *-- Function: getledstate_main ---------------------------------------------------
 * 
 *  Main-Funktion für getledstate
 * 
 *  \param argc
 *  \param argv
 * 
 *  \return <description of the return values of the function>
 *------------------------------------------------------------------------------
 */
int getidstate_main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  DBusConnection *connection;
  DBusError error;
  DBusObjectPathVTable vtable;

  dbus_error_init (&error);
  connection = dbus_bus_get (DBUS_BUS_STARTER, &error);
  dbus_bus_request_name(connection, "wago.diagnostic",0, &error);

  vtable.message_function = MethodHandler;
  dbus_connection_try_register_object_path  ( connection,"/wago/diagnose",
                                              &vtable,NULL,&error);

  //dbus_connection_add_filter (connection, (DBusHandleMessageFunction) MethodHandler, NULL, NULL);
  run = 10;
  //while (TRUE == dbus_connection_read_write_dispatch(connection,1000));
  while(run > 0)
  {
    run--;
    dbus_connection_read_write_dispatch(connection,1000);
  }
  dbus_connection_unref (connection);
  return 0;
}
//---- End of source file ------------------------------------------------------
