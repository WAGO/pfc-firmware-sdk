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
///  \file     diagnostic.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <diagnostic/diagnostic_API.h>
#include <LedCtl_API.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <sys/time.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


#define LOG_ID_NAME_FORMAT    "ID%.8X"
#define LOG_ID_NAME_PREALLOC  "ID00000000"

typedef struct {
    log_tEventId      id;
    log_tEventHandler callback;
    void *        user_data;
}tEvent;

#if 1
static char   programNameArray[64];
static char   *programName;
static bool   libInitDone = false;
com_tConnection   diagnosticConnection = { .bus = NULL };

void log_EVENT_Init(          const char       * name)
{
  if(libInitDone == false)
  {
    libInitDone = true;
    strncpy(programNameArray, name, sizeof(programNameArray));
    programNameArray[sizeof(programNameArray)-1]='\0';
    programName = (char*)programNameArray;
    //diag_ConnectToLog(programName, LOG_ODELAY, LOG_USER);
    openlog(programName, LOG_ODELAY, LOG_USER);
    com_GEN_Init(&diagnosticConnection);
  }
}

void log_EVENT_LogId(         log_tEventId           id,
                              bool               set)
{
  log_EVENT_LogIdParam(id,set,LOG_TYPE_INVALID);
}

void log_EVENT_LogIdParam(    log_tEventId           id,
                                   bool          set,
                                   int           first_arg_type, ...)
{
  struct timeval tv;
  static char strId[16] = LOG_ID_NAME_PREALLOC;
  static uint32_t lastId = 0;
  static const char * pId = strId ;
  com_tComBool dbSet = TRUE;
  DBusMessage *message;
  DBusMessageIter iter;
  int type;
  va_list var_args;

  gettimeofday(&tv, NULL);
  if(set == false)
  {
    dbSet = FALSE;
  }

  if(id != lastId)
  {
    sprintf(strId, LOG_ID_NAME_FORMAT, id);
    lastId = id;
  }
  message = dbus_message_new_signal(LOG_PATH,LOG_INTERFACE, pId);



  dbus_message_iter_init_append (message, &iter);

  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &(tv.tv_sec));
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &(tv.tv_usec));
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &(programName));
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_BOOLEAN, &(dbSet));
  if(first_arg_type != LOG_TYPE_INVALID)
  {
    va_start (var_args, first_arg_type);
    type = first_arg_type;
    while (type != LOG_TYPE_INVALID)
    {
      if(type == LOG_TYPE_INT32)
      {
        const int32_t *value = va_arg(var_args, int32_t*);
        dbus_message_iter_append_basic (&iter, type, value);
      }
      else if(type == LOG_TYPE_UINT16)
      {
        const uint16_t *value = va_arg(var_args, uint16_t*);
        dbus_message_iter_append_basic (&iter, type, value);
      }
      else if(type == LOG_TYPE_UINT32)
      {
        const uint32_t *value = va_arg(var_args, uint32_t*);
        dbus_message_iter_append_basic (&iter, type, value);
      }
      else if(type == LOG_TYPE_INT16)
      {
        const int16_t *value = va_arg(var_args, int16_t*);
        dbus_message_iter_append_basic (&iter, type, value);
      }
      else if(type == LOG_TYPE_BYTE)
      {
        const uint8_t *byte = va_arg(var_args, uint8_t*);
        dbus_message_iter_append_basic (&iter, type, byte);
      }
      else
      {
        break;
      }
      type = va_arg (var_args, int);
    }
    va_end(var_args);
  }
  (void)com_GEN_BlockThreadCancelling();
  dbus_connection_send(
      (DBusConnection*)com_GEN_GetDBusVar((com_tConnection*) &diagnosticConnection,
                                          DBUSVAR_CONNECTION),
      message, NULL);
  dbus_connection_flush((DBusConnection*)com_GEN_GetDBusVar((com_tConnection*) &diagnosticConnection,
                                                            DBUSVAR_CONNECTION));
  (void)com_GEN_UnblockThreadCancelling();
  dbus_message_unref(message);
}

/*void log_EVENT_LogDev(        int priority,
                              const char         format, ...);*/

static void log_EVENT_EventHandler(com_tConnection * con, com_tComMessage * msg, tEvent * event)
{
  event->callback(con, msg, event->id, event->user_data);
}

com_tSignalHandle log_EVENT_RegisterForId(  log_tEventId           eventId,
                              log_tEventHandler      callback,
                              void             * user_data)
{
  char signame[16];
  tEvent * event = malloc(sizeof(tEvent));
  if(event == NULL)
  {
    return -1;
  }
  event->callback  = callback;
  event->id        = eventId;
  event->user_data = user_data;
  sprintf(signame, LOG_ID_NAME_FORMAT, eventId);
  return com_MSG_RegisterSignal(&diagnosticConnection,
                                LOG_INTERFACE,
                                signame,
                                (com_tHandlerFunction)log_EVENT_EventHandler,
                                (void*)event);
}

bool log_EVENT_HandlerIdSet(com_tConnection * con, com_tComMessage * msg)
{
  int dummy;
  char * dummyString;
  com_tComBool dbRet;

  com_MSG_GetParams(con, msg, COM_TYPE_INT32, &dummy,
                                                COM_TYPE_INT32, &dummy,
                                                COM_TYPE_STRING,&dummyString,
                                                COM_TYPE_BOOLEAN, &dbRet,
                                                COM_TYPE_INVALID);

  if(dbRet == TRUE)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void FreeEventHandler(tEvent * event)
{
  free(event);
}

int log_EVENT_DeregisterId(com_tSignalHandle  handle)
{
  return com_MSG_DeregisterSignal(&diagnosticConnection,
                                  handle,
                                 (void(*)(void *user_data)) FreeEventHandler);
}

bool log_EVENT_GetState(     log_tEventId           eventId,
                             struct timeval   * timestamp,
                             int                first_arg_type, ...)
{
  DBusMessage * reply;
  va_list       var_args;
  //bool ret = false;
  com_tComBool dbSet = FALSE;
  DBusMessageIter iter;
  int type;
  /*com_MSG_MethodCall(&diagnosticConnection,LOG_PARSER_NAME,
                         LOG_PATH, LOG_GET_INFO,
                         COM_TYPE_INT32, &timestamp->tv_sec,
                         COM_TYPE_INT32, &timestamp->tv_usec,
                         COM_TYPE_INVALID, COM_TYPE_INVALID);*/
  com_MSG_MethodCall(&diagnosticConnection,LOG_PARSER_NAME,
                           LOG_DIAG_PATH, LOG_GET_INFO,
                           COM_TYPE_UINT32, &eventId,
                           COM_TYPE_INVALID, COM_TYPE_INVALID);

  reply = (DBusMessage*) com_GEN_GetDBusVar(&diagnosticConnection, DBUSVAR_LAST_REPLY);

  dbus_message_iter_init (reply, &iter);
  if(COM_TYPE_INT32 == dbus_message_iter_get_arg_type (&iter))
  {
    dbus_message_iter_get_basic(&iter, &timestamp->tv_sec);
    dbus_message_iter_next(&iter);
  }
  if(COM_TYPE_INT32 == dbus_message_iter_get_arg_type (&iter))
  {
    dbus_message_iter_get_basic(&iter, &timestamp->tv_usec);
    dbus_message_iter_next(&iter);
  }
  if(COM_TYPE_BOOLEAN == dbus_message_iter_get_arg_type (&iter))
  {
    dbus_message_iter_get_basic(&iter, &dbSet);
  }
  va_start(var_args, first_arg_type);
  type = first_arg_type;
  while (type != LOG_TYPE_INVALID)
  {
    int inType = dbus_message_iter_get_arg_type (&iter);
    if(FALSE == dbus_message_iter_next(&iter))
    {
      break;
    }

    if(inType != type)
    {
      break;
    }

    if(type == LOG_TYPE_INT32)
    {
      const int32_t *value = va_arg(var_args, int32_t*);
      dbus_message_iter_get_basic(&iter,(void*)value);
    }
    else if(type == LOG_TYPE_UINT16)
    {
      const uint16_t *value = va_arg(var_args, uint16_t*);
      dbus_message_iter_get_basic(&iter, (void*)value);
    }
    else if(type == LOG_TYPE_UINT32)
    {
      const uint32_t *value = va_arg(var_args, uint32_t*);
      dbus_message_iter_get_basic(&iter, (void*)value);
    }
    else if(type == LOG_TYPE_INT16)
    {
      const int16_t *value = va_arg(var_args, int16_t*);
      dbus_message_iter_get_basic(&iter, (void*)value);
    }
    else if(type == LOG_TYPE_BYTE)
    {
      const uint8_t *byte = va_arg(var_args, uint8_t*);
      dbus_message_iter_get_basic(&iter, (void*)byte);
    }
    else
    {
      break;
    }

    type = va_arg (var_args, int);
  }
  va_end(var_args);
  if(dbSet == FALSE)
  {
    return false;
  }
  else
  {
    return true;
  }
}
#if 0
int led_GetLedState(        const char       * ledName,
                             uint8_t          * state,
                             uint16_t         * var1,
                             uint16_t         * var2)
{
  char path[4096];
  int ret;
  sprintf(path, LED_DBUS_PATH "%s", ledName);
  ret = com_MSG_MethodCall(&diagnosticConnection,LED_DBUS_NAME,
                         path, LED_DBUS_GET_STATE,
                         COM_TYPE_INVALID,
                         COM_TYPE_BYTE,   state,
                         COM_TYPE_UINT16, var1,
                         COM_TYPE_UINT16, var2,
                         COM_TYPE_INVALID);
  return ret;
}
#endif
#if 0
int led_GetLedStateWithId(   const char       * ledName,
                             uint8_t          * state,
                             uint16_t         * var1,
                             uint16_t         * var2,
                             uint32_t         * id,
                             struct timeval   * timestamp)
{
  char path[4096];
  int ret;
  sprintf(path, LED_DBUS_PATH "%s", ledName);
  ret = com_MSG_MethodCall(&diagnosticConnection,LED_DBUS_NAME,
                         path, LED_DBUS_GET_STATE,
                         COM_TYPE_INVALID,
                         COM_TYPE_BYTE,   state,
                         COM_TYPE_UINT16, var1,
                         COM_TYPE_UINT16, var2,
                         COM_TYPE_UINT32, id,
                         COM_TYPE_UINT32, &timestamp->tv_sec,
                         COM_TYPE_UINT32, &timestamp->tv_usec,
                         COM_TYPE_INVALID);
  return ret;
}
#endif
#if 0
static void _GetIdCtrlInfo(DBusMessage * message, tIdCtrlInfo * idInfo)
{
  DBusMessageIter iter;
   int i;
   int type;
   static char string[4096];

   dbus_message_iter_init  (message,&iter);//iterator initialisieren

   for(i = 0; i < 6; i++)
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
   if(strlen(string) > 0)
   {
     idInfo->info = strdup(string);
   }
}
#else
static void _GetIdCtrlInfo(DBusMessageIter * iter, tIdCtrlInfo * idInfo)
{
  uint32_t i;
  int type;
  uint32_t length;
  static char string[4096];

  string[0] = 0;

  dbus_message_iter_get_basic (iter, &length);

  if(length > 0)
  {

    string[0] = 0;
    for(i = 0;i < length;i++)
    {
      char buffer[32];
      uint8_t  byte;
      uint16_t uWord;
      int16_t  word;
      uint32_t uInteger;
      int32_t  integer;
      dbus_bool_t boolean;
      if(FALSE == dbus_message_iter_has_next(iter))break;
      dbus_message_iter_next(iter);
      type = dbus_message_iter_get_arg_type (iter);
      switch(type)
      {
        case DBUS_TYPE_BYTE:
          dbus_message_iter_get_basic (iter, &byte);
          sprintf(buffer, "%u", (uint32_t) byte);
          break;
        case DBUS_TYPE_INT16:
          dbus_message_iter_get_basic (iter, &word);
          sprintf(buffer, "%d", (int) word);
          break;
        case DBUS_TYPE_UINT16:
          dbus_message_iter_get_basic (iter, &uWord);
          sprintf(buffer, "%u", (uint32_t) uWord);
          break;
        case DBUS_TYPE_INT32:
          dbus_message_iter_get_basic (iter, &integer);
          sprintf(buffer, "%d", (int) integer);
          break;
        case DBUS_TYPE_UINT32:
          dbus_message_iter_get_basic (iter, &uInteger);
          sprintf(buffer, "%u", (uint32_t) uInteger);
          break;
        case DBUS_TYPE_BOOLEAN:
          dbus_message_iter_get_basic (iter, &boolean);
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
    }
    idInfo->info = strdup(string);
  }
}
#endif
#if 0
int led_GetLedStateWithIdInfo(   const char       * ledName,
                             uint8_t          * state,
                             uint16_t         * var1,
                             uint16_t         * var2,
                             tIdCtrlInfo      * id,
                             struct timeval   * timestamp)
{
  char path[4096];
  int ret;
  //int i;
  sprintf(path, LED_DBUS_PATH "%s", ledName);
  DBusMessage * lastmsg;
  ret = com_MSG_MethodCall(&diagnosticConnection,LED_DBUS_NAME,
                         path, LED_DBUS_GET_STATE,
                         COM_TYPE_INVALID,
                         COM_TYPE_BYTE,   state,
                         COM_TYPE_UINT16, var1,
                         COM_TYPE_UINT16, var2,
                         COM_TYPE_UINT32, &id->id,
                         COM_TYPE_UINT32, &timestamp->tv_sec,
                         COM_TYPE_UINT32, &timestamp->tv_usec,
                         COM_TYPE_INVALID);
  lastmsg = (DBusMessage *)com_GEN_GetDBusVar(&diagnosticConnection, DBUSVAR_LAST_REPLY);
  _GetIdCtrlInfo(lastmsg, id);

  return ret;
}
#endif
void led_GetDbusInfoByState(DBusMessageIter * iter,tLedStatus * status)
{

   switch(status->state)
   {
     case LED_STATE_STATIC:
       //dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.staticVar));
       dbus_message_iter_get_basic (iter, &(status->vars.staticVar));
       dbus_message_iter_next(iter);
       break;
     case LED_STATE_BLINK:
//       dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.blinkVar.color1));
//       dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.blinkVar.color2));
//       dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.blinkVar.time1));
//       dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.blinkVar.time2));
       dbus_message_iter_get_basic (iter, &(status->vars.blinkVar.color1));
       dbus_message_iter_next(iter);
       dbus_message_iter_get_basic (iter, &(status->vars.blinkVar.color2));
       dbus_message_iter_next(iter);
       dbus_message_iter_get_basic (iter, &(status->vars.blinkVar.time1));
       dbus_message_iter_next(iter);
       dbus_message_iter_get_basic (iter, &(status->vars.blinkVar.time2));
       dbus_message_iter_next(iter);
      break;
     case LED_STATE_FLASH:
//       dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.flashVar.flashColor));
//       dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.flashVar.flashTime));
//       dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.flashVar.staticColor));
       dbus_message_iter_get_basic (iter, &(status->vars.flashVar.flashColor));
       dbus_message_iter_next(iter);
       dbus_message_iter_get_basic (iter, &(status->vars.flashVar.flashTime));
       dbus_message_iter_next(iter);
       dbus_message_iter_get_basic (iter, &(status->vars.flashVar.staticColor));
       dbus_message_iter_next(iter);
       break;
     case LED_STATE_750_ERR:
//       dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.err750Var.errorCode));
//       dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.err750Var.errorArg));
       dbus_message_iter_get_basic (iter, &(status->vars.err750Var.errorCode));
       dbus_message_iter_next(iter);
       dbus_message_iter_get_basic (iter, &(status->vars.err750Var.errorArg));
       dbus_message_iter_next(iter);
       break;
     case LED_STATE_CAN:
       //dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.canVar.canCode));
       dbus_message_iter_get_basic (iter, &(status->vars.canVar.canCode));
       dbus_message_iter_next(iter);
       break;
     default:
       break;
   }
   dbus_message_iter_get_basic (iter, &(status->info.id));
   dbus_message_iter_next(iter);
  _GetIdCtrlInfo(iter, &status->info);
}
int led_GetLedStatus(const char * led_name,
                     tLedStatus * status)
{
  char path[4096];
  int ret;
  int i;
  char *ledName=NULL;
  sprintf(path, LED_DBUS_PATH "%s", led_name);
  DBusMessage * lastmsg;
  DBusMessageIter iter;
  ret = com_MSG_MethodCall(&diagnosticConnection,LED_DBUS_NAME,
                         path, LED_DBUS_GET_STATE,
                         COM_TYPE_INVALID,
                         COM_TYPE_STRING,  &ledName,
                         COM_TYPE_INT32,   &status->state,
                         COM_TYPE_UINT32,   &status->timestamp.tv_sec,
                         COM_TYPE_UINT32,   &status->timestamp.tv_usec,
                         COM_TYPE_INVALID);
  if(!ret)
  {
    strcpy(status->name,ledName);
    lastmsg = (DBusMessage *)com_GEN_GetDBusVar(&diagnosticConnection, DBUSVAR_LAST_REPLY);

    dbus_message_iter_init  (lastmsg,&iter);//iterator initialisieren
    for(i = 0; i < 4; i++)
    {
      dbus_message_iter_next(&iter);
    }

    led_GetDbusInfoByState(&iter,status);
  }
  return ret;
}

#endif
//---- End of source file ------------------------------------------------------
