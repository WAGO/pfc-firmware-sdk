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
///  \file     getledstate.c
///
///  \version  $Revision: 1 $
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
#include <errno.h>
#include <glib.h>
#include <dbus/dbus.h>
#include "parse_log.h"

#include "eactingbox.h"
#include "parse_log.h"
#include "diagnostic_xml.h"

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

/*
 *-- Function: ShowHelp ---------------------------------------------------
 *
 *  Zeigt den Hilfe-Text für das Programm an
 *
 *------------------------------------------------------------------------------
 */
static void ShowHelp(void)
{
  printf("\n* Get State of LED *\n\n");
  printf("Usage: getledstate <LED-Name> \n\n");
}
#if 0
static void PrintLedState(tDiagXml ** doc, tLedStatus * ledStatus)
{
  int i;
  for(i = 0; states[i].symbol != LEDCTL_S_ERROR; i++)
  {
    if(states[i].symbol == ledStatus->state)
    {
      break;
    }
  }
  if(states[i].symbol == ledStatus->state)
  {
    if(ledStatus->state != LEDCTL_S_ERROR)
    {
      char  timeString[64];
      char * valString;
      tLogData logData;
      size_t len;
      int docnum;
      //sprintf(timeString, (const char*)ctime(&ledStatus->timestamp.tv_sec));
      strcpy(timeString, ctime(&ledStatus->timestamp.tv_sec));
      len = strlen(timeString)-1;
      timeString[len]= 0;
      for(docnum=0;docnum<2;docnum++)
      {
        valString = (char*)GetStringOfId(ledStatus->idInfo.id, doc[docnum], NULL);
        if(valString != NULL)
        {
          break;
        }
      }
      logData.id = ledStatus->idInfo.id;
      logData.progName = NULL;
      logData.set = TRUE;
      logData.timestamp.tv_sec = ledStatus->timestamp.tv_sec;
      logData.timestamp.tv_usec = ledStatus->timestamp.tv_usec;
      logData.variables = ledStatus->idInfo.info;

      if(valString == NULL)
      {
        printf("%s: %s %u %u %s: \"Undefined ID: %.8X\"\n",
               ledStatus->ledName,
               states[i].string,
               ledStatus->var1,
               ledStatus->var2,
               timeString,
               ledStatus->idInfo.id);
      }
      else
      {
        char   completeString[4096];
        ReplaceVariables(completeString,valString, &logData, sizeof(completeString));
        printf("%s: %s %u %u %s: \"%s\"\n",
               ledStatus->ledName,
               states[i].string,
               ledStatus->var1,
               ledStatus->var2,
               timeString,
               completeString);
      }
    }
    else
    {
      printf("%s: %s\n", ledStatus->ledName, states[i].string);
    }
  }
  else
  {
    printf("unknown LED-State: %s 0x%X %u %d 0x%.8X\n",
           ledStatus->ledName,
           ledStatus->state,
           ledStatus->var1,
           ledStatus->var2,
           ledStatus->idInfo.id);
  }
}
#else
static const char * ColorToString(tLedStaticColor color)
{
  const char * ret = NULL;
  /*
  {.color = LED_COLOR_RED,      .string = "red"},
  {.color = LED_COLOR_RED,      .string = "RED"},
  {.color = LED_COLOR_GREEN,    .string = "green"},
  {.color = LED_COLOR_GREEN,    .string = "GRN"},
  {.color = LED_COLOR_YELLOW,   .string = "yellow"},
  {.color = LED_COLOR_YELLOW,   .string = "orange"},
  {.color = LED_COLOR_YELLOW,   .string = "YLW"},
  {.color = LED_COLOR_BLUE,     .string = "blue"},
  {.color = LED_COLOR_BLUE,     .string = "BLU"},
  {.color = LED_COLOR_CYAN,     .string = "cyan"},
  {.color = LED_COLOR_CYAN,     .string = "CYN"},
  {.color = LED_COLOR_MAGENTA,  .string = "magenta"},
  {.color = LED_COLOR_MAGENTA,  .string = "MAG"},
  {.color = LED_COLOR_WHITE,    .string = "white"},
  {.color = LED_COLOR_WHITE,    .string = "WHT"},
  */
  switch(color)
  {
    case LED_COLOR_RED:
      ret = "RED";
      break;
    case LED_COLOR_GREEN:
      ret = "GRN";
      break;
    case LED_COLOR_YELLOW:
      ret = "YLW";
      break;
    case LED_COLOR_BLUE:
      ret = "BLU";
      break;
    case LED_COLOR_CYAN:
      ret = "CYN";
      break;
    case LED_COLOR_MAGENTA:
      ret = "MAG";
      break;
    case LED_COLOR_WHITE:
      ret = "WHT";
      break;
    case LED_COLOR_OFF:
      ret = "OFF";
      break;
  }
  return ret;
}
static int StateToDepricated(tLedStatus * ledStatus,char * statestring,uint32_t * var1,uint32_t * var2)
{
  int ret =1;
   switch(ledStatus->state)
   {
     case LED_STATE_STATIC:
       sprintf(statestring,"%s",
               ColorToString(ledStatus->vars.staticVar));
       *var1=0;
       *var2=0;
       break;
     case LED_STATE_BLINK:
       if(ledStatus->vars.blinkVar.color2 == LED_COLOR_OFF)
       {
         sprintf(statestring,"BLINK_%s",
                 ColorToString(ledStatus->vars.blinkVar.color1));
       }
       else
       {
         sprintf(statestring,"BLINK_%s_%s",
                 ColorToString(ledStatus->vars.blinkVar.color1),
                 ColorToString(ledStatus->vars.blinkVar.color2));
       }
       *var1=ledStatus->vars.blinkVar.time1;
       *var2=ledStatus->vars.blinkVar.time2;
      break;
     case LED_STATE_FLASH:
       sprintf(statestring,"FLASH_%s_%s",
               ColorToString(ledStatus->vars.flashVar.flashColor),
               ColorToString(ledStatus->vars.flashVar.staticColor));
       *var1=ledStatus->vars.flashVar.flashTime;
       *var2=0;
       break;
     case LED_STATE_750_ERR:
       sprintf(statestring,"ERR_SEQ_750");
       *var1=ledStatus->vars.err750Var.errorCode;
       *var2=ledStatus->vars.err750Var.errorArg;
       break;
     case LED_STATE_CAN:
       sprintf(statestring,"SPECIAL_BLINK");
       *var1=1;
       *var2=ledStatus->vars.canVar.canCode;
       break;
     default:
       ret =0;
       break;
   }

   return ret;
}
static void PrintLedState(tDiagXml ** doc, tLedStatus * ledStatus)
{
  char statestring[255];
  uint32_t var1=0;
  uint32_t var2=0;

  if(StateToDepricated(ledStatus,statestring,&var1,&var2))
  {
    char  timeString[64];
    char * valString;
    tLogData logData;
    size_t len;
    int docnum;
    //sprintf(timeString, (const char*)ctime(&ledStatus->timestamp.tv_sec));
    strcpy(timeString, ctime(&ledStatus->timestamp.tv_sec));
    len = strlen(timeString)-1;
    timeString[len]= 0;
    for(docnum=0;docnum<2;docnum++)
    {
      valString = (char*)GetStringOfId(ledStatus->info.id, doc[docnum], NULL);
      if(valString != NULL)
      {
        break;
      }
    }
    logData.id = ledStatus->info.id;
    logData.progName = NULL;
    logData.set = TRUE;
    logData.timestamp.tv_sec = ledStatus->timestamp.tv_sec;
    logData.timestamp.tv_usec = ledStatus->timestamp.tv_usec;
    logData.variables = ledStatus->info.info;

    if(valString == NULL)
    {
      printf("%s: %s %u %u %s: \"Undefined ID: %.8X\"\n",
             ledStatus->name,
             statestring,
             var1,
             var2,
             timeString,
             ledStatus->info.id);
    }
    else
    {
      char   completeString[4096];
      ReplaceVariables(completeString,valString, &logData, sizeof(completeString));
      printf("%s: %s %u %u %s: \"%s\"\n",
             ledStatus->name,
             statestring,
             var1,
             var2,
             timeString,
             completeString);
    }
  }
  else
  {
    printf("unknown LED-State: %s 0x%X %u %d 0x%.8X\n",
           ledStatus->name,
           ledStatus->state,
           var1,
           var2,
           ledStatus->info.id);
  }
}
#endif

#if 0
static GList * CreateLedStateList(void)
{
  GList * list = NULL;
  DBusMessage * message;
  DBusMessage * reply;
  DBusMessageIter iter;

  int type;
  com_tConnection   dBusCon = { .bus = NULL };
  com_GEN_Init(&dBusCon);
  message = dbus_message_new_method_call(LED_DBUS_NAME, LED_DBUS_PATH_ALL,
                                         NULL, LED_DBUS_GET_STATE_ALL);
  reply = dbus_connection_send_with_reply_and_block   (dBusCon.bus,message,-1,NULL);
  dbus_message_iter_init(reply, &iter);
  type = dbus_message_iter_get_arg_type (&iter);
  while(DBUS_TYPE_INVALID != type)
  {
    tLedCtlState * ledState = malloc(sizeof(tLedCtlState));
    char * ledName = NULL;
    char string[4096];
    uint32_t length;
    uint32_t i;

    memset(ledState,0,sizeof(tLedCtlState));

    dbus_message_iter_get_basic (&iter, &ledName);
    strcpy(ledState->ledName,ledName);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->state);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->var1);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->var2);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->idInfo.id);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->timestamp.tv_sec);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->timestamp.tv_usec);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);


    dbus_message_iter_get_basic (&iter, &length);

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
        if(FALSE == dbus_message_iter_has_next(&iter))break;
        dbus_message_iter_next(&iter);
        type = dbus_message_iter_get_arg_type (&iter);
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
      }
      ledState->idInfo.info = strdup(string);
    }
    list = g_list_append(list, ledState);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);
    type = dbus_message_iter_get_arg_type (&iter);
  }

  return list;
}
#else
static GList * CreateLedStateList(void)
{
  GList * list = NULL;
  DBusMessage * message;
  DBusMessage * reply;
  DBusMessageIter iter;

  int type;
  com_tConnection   dBusCon = { .bus = NULL };
  com_GEN_Init(&dBusCon);
  message = dbus_message_new_method_call(LED_DBUS_NAME, LED_DBUS_PATH_ALL,
                                         NULL, LED_DBUS_GET_STATE_ALL);
  reply = dbus_connection_send_with_reply_and_block   (dBusCon.bus,message,-1,NULL);
  dbus_message_iter_init(reply, &iter);
  type = dbus_message_iter_get_arg_type (&iter);
  while(DBUS_TYPE_INVALID != type)
  {
    tLedStatus * ledState = malloc(sizeof(tLedStatus));
    char * ledName = NULL;

    memset(ledState,0,sizeof(tLedStatus));

    dbus_message_iter_get_basic (&iter, &ledName);
    strcpy(ledState->name,ledName);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->state);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->timestamp.tv_sec);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->timestamp.tv_usec);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    led_GetDbusInfoByState(&iter,ledState);

    list = g_list_append(list, ledState);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);
    type = dbus_message_iter_get_arg_type (&iter);
  }

  return list;
}
#endif

void _PrintLedStateWrapper(tLedCtlState * status,tDiagXml ** doc)
{
  PrintLedState(doc, (tLedStatus*)status);
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
int getledstate_main(int argc, char **argv)
{

  //char *   ledName  = NULL;
  //uint8_t  ledState = 0;
  //uint16_t ledVar1  = 0;
  //uint16_t ledVar2  = 0;
  //uint32_t stateId  = 0;
  //struct timeval tv;

  tDiagXml * doc[2];
  int exitval = 0;

  if(argc == 2 && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0))
  {
    ShowHelp();
    return -1;
  }
  if(argc != 2)
  {
    printf("WRONG NUMBER OF ELEMENTS\n\n");
    ShowHelp();
    return -1;
  }

  log_EVENT_Init("getledstate");

  doc[0] = ParseDoc(DIAGNOSTIC_XML_DOCUMENT_CUSTOM);
  doc[1] = ParseDoc(DIAGNOSTIC_XML_DOCUMENT);
  if(strcmp(argv[1], "--all") == 0 || strcmp(argv[1], "-a") == 0)
  {
    GList * states = NULL;
    states=CreateLedStateList();
    g_list_foreach(states,(GFunc)_PrintLedStateWrapper,doc);

  }
  else
  {
    char name[255];
   /* tLedCtlState ledState = { .ledName = {0},
                              .state     = 0,
                              .var1      = 0,
                              .var2      = 0,
                              .idInfo = {.id     = 0,
                                         .info = NULL},
                              .timestamp = {.tv_sec = 0,
                                            .tv_usec = 0}
                             };*/
    tLedStatus ledState;
    strcpy(name, argv[1]);

    if( 0 != led_GetLedStatus(name,&ledState))
    {
      int errr = errno;
      puts("Error: check correct spelling of name.");
      printf("Error: %d: %s\n",errr, strerror(errr));
      exitval = -1;
    }
    else
    {
        PrintLedState(doc, &ledState);
    }
  }
  return exitval;
}
//---- End of source file ------------------------------------------------------
