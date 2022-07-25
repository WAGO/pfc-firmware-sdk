//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file main.c
///
/// \version $Revision: 65689 $
///
/// \brief Main-Function of the LED-Error-Servers
///
/// \author Hans Florian Scholz : WAGO
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <diagnostic/diagnostic_API.h>
#include <signal.h>
#include "config.h"
#include "common.h"
#include "diagnostic_xml.h"
#include <diagnostic/diagnostic_xml_API.h>
#include <execinfo.h>
#include <syslog.h>
#include <ucontext.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <glib.h>
#include "led_info_json.h"

#include <mcheck.h>

#include <ledserver_API.h>


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define WAGO_DIAGNOSE_ID_INTERFACE      "wago.diagnose"
#define LEDCONFIG_XML_DOCUMENT          "/tmp/led.xml"



//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
//error handling
typedef enum eEventError{
  EVENT_ERR_LOGLEVEL,
  EVENT_ERR_STATE,
  EVENT_ERR_LED_NO,
  EVENT_ERR_VAR_1,
  EVENT_ERR_VAR_2,
  EVENT_ERR_E_CODE,
  EVENT_ERR_E_ARG,
  EVENT_ERR_FLAGS,
  EVENT_ERR_NAME,
  EVENT_ERR_TEXT
}tEventError;

typedef struct _sig_ucontext {
 unsigned long     uc_flags;
 struct ucontext   *uc_link;
 stack_t           uc_stack;
 struct sigcontext uc_mcontext;
 sigset_t          uc_sigmask;
} sig_ucontext_t;



typedef union {
    uint8_t  byte;
    uint16_t word;
    uint32_t integer;
}tUDataType;

typedef struct {
    int        type;
    tUDataType value;
}tIdAdditionalParam;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
tLedEventList * ledEvents = NULL;



static const tIdInfo defaultIdInfo = {
                                      .id = 0x00010017,
                                      .info = NULL
};
static bool isShutdown = false;



//-- Function: SignalHandler ------------------------------------------------
///
///  Handles with incoming signals
///
///  \param connection connection to the DBus
///  \param message The receifed message
///  \param user_data additional Data
///
//------------------------------------------------------------------------------
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void _IdInfoToDbus(tIdAdditionalParam *src,DBusMessageIter * pIter)
{
  DBGFUNC();
  DBG2("src: %X",(uint32_t) src);
  switch(src->type)
  {
    case DBUS_TYPE_BYTE:
      DBG("byte");
      dbus_message_iter_append_basic(pIter,src->type,&src->value.byte);
      break;
    case DBUS_TYPE_BOOLEAN:
    case DBUS_TYPE_INT32:
    case DBUS_TYPE_UINT32:
      DBG("int");
      dbus_message_iter_append_basic(pIter,src->type,&src->value.integer);
      break;
    case DBUS_TYPE_INT16:
    case DBUS_TYPE_UINT16:
      DBG("word");
      dbus_message_iter_append_basic(pIter,src->type,&src->value.word);
      break;
    default:
      DBG2("ERROR MESSAGE PARAMETER OF TYPE %d NOT SUPPORTED",src->type);
      break;
  }
  DBG("end");
}




static int GetLedInfo(tLedNr ledNr, tLedInfo * ledInfo)
{
  tLedData * ledData =NULL;
  if(LED_RETURN_OK == ledserver_LEDCTRL_GetLed(ledNr,&ledInfo->state,(void*)&(ledInfo->vars), sizeof(ledInfo->vars),(void**)&ledData))
  {
    if(ledData !=NULL)
    {
      size_t szVar=0;
      GetSizeOfState(ledInfo->state, &szVar);
      ledInfo->flags = ledData->flags;
      ledInfo->idInfo = ledData->idInfo;
      ledInfo->pWarteschlange = ledData->pWarteschlange;
      ledInfo->defaultState = ledData->defaultState;
      ledInfo->defaultIdInfo = ledData->defaultIdInfo;
      VarCopy(&(ledInfo->defaultVars),&(ledData->defaultVars),ledInfo->defaultState);
      ledInfo->setTime.tv_sec = ledData->setTime.tv_sec;
      ledInfo->setTime.tv_usec = ledData->setTime.tv_usec;
    }
    else
    {
      InitLedInfo(ledInfo);
    }
    return 0;
  }
  else
  {
    return -1;
  }
}

void AppendToDbusByState(DBusMessageIter * iter,tLedInfo *ledInfo)
{
  switch(ledInfo->state)
  {
    case LED_STATE_STATIC:
      dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.staticVar));
      break;
    case LED_STATE_BLINK:
      dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.blinkVar.color1));
      dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.blinkVar.color2));
      dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.blinkVar.time1));
      dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.blinkVar.time2));
      break;
    case LED_STATE_FLASH:
      dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.flashVar.flashColor));
      dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.flashVar.flashTime));
      dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&(ledInfo->vars.flashVar.staticColor));
      break;
    case LED_STATE_750_ERR:
      dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.err750Var.errorCode));
      dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.err750Var.errorArg));
      break;
    case LED_STATE_CAN:
      dbus_message_iter_append_basic  (iter,COM_TYPE_UINT16,&(ledInfo->vars.canVar.canCode));
      break;
    default:
      break;
  }
}

void AddInfoToDbus(DBusMessageIter * iter,tLedNr ledNr,tLedInfo * ledInfo,uint32_t * listLength)
{
  char nameString[16];
  char * pNameString = nameString;
  //uint32_t listLength;
  int i;

  ledserver_LEDCTRL_GetLedName(ledNr, nameString,sizeof(nameString));
  for(i=0;nameString[i]!=0;i++)
  {
    nameString[i]=toupper((int)nameString[i]);
  }

  dbus_message_iter_append_basic  (iter,COM_TYPE_STRING,&pNameString);
  dbus_message_iter_append_basic  (iter,COM_TYPE_INT32,&ledInfo->state);
  dbus_message_iter_append_basic  (iter,COM_TYPE_UINT32,&(ledInfo->setTime.tv_sec));
  dbus_message_iter_append_basic  (iter,COM_TYPE_UINT32,&(ledInfo->setTime.tv_usec));

  AppendToDbusByState(iter,ledInfo);

  dbus_message_iter_append_basic  (iter,COM_TYPE_UINT32,&(ledInfo->idInfo.id));

  *listLength = g_list_length((GList*)ledInfo->idInfo.info);
  dbus_message_iter_append_basic  (iter,COM_TYPE_UINT32,listLength);
  g_list_foreach((GList*)ledInfo->idInfo.info,(GFunc)_IdInfoToDbus,iter);
}

void GetAllLedHandler(com_tConnection * con, com_tComMessage * msg,tLedNames * names)
{
  //static tLedCtlState * state = NULL;
  static tLedInfo * ledInfo = NULL;
  static uint32_t * listLength = NULL;
  int noOfLeds = (int) ledserver_LEDCTRL_GetLedCount();
  DBusMessageIter iter;
  DBusMessage * reply;
  int i;
  if(ledInfo == NULL)
  {
    ledInfo = malloc(sizeof(tLedInfo)*noOfLeds);
    listLength = malloc(sizeof(uint32_t)*noOfLeds);
  }
  reply = dbus_message_new_method_return(msg->msg);
  dbus_message_iter_init_append (reply, &iter);

  //TEST
  //noOfLeds = 1;
  //TEST
  for(i=0;i<noOfLeds; i++)
  {
    GetLedInfo(i,  &ledInfo[i]);
    AddInfoToDbus(&iter,i,&(ledInfo[i]),&(listLength[i]));
  }

  dbus_connection_send(con->bus, reply, NULL);
  dbus_connection_flush(con->bus);
  dbus_message_unref(reply);
}

void GetLedHandler(com_tConnection * con, com_tComMessage * msg, int * ledNr)
{
  DBusMessageIter iter;
  DBusMessage * reply;
  tLedInfo  ledInfo;
  uint32_t length;


  GetLedInfo(*ledNr,  &ledInfo);
  reply = dbus_message_new_method_return(msg->msg);
  dbus_message_iter_init_append (reply, &iter);
  AddInfoToDbus(&iter,*ledNr,&ledInfo,&length);
  dbus_connection_send(con->bus, reply, NULL);
  dbus_connection_flush(con->bus);
  dbus_message_unref(reply);
}

uint32_t GetArgFromMessage(int argno, DBusMessage *message)
{
  DBusMessageIter iter;
  int i;
  int type;
  uint32_t ret = 0xFFFFFFFF;

  dbus_message_iter_init(message, &iter);
  for(i = 0; i < 3; i++)
  {
    if(dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_INVALID)
    {
      break;
    }
    dbus_message_iter_next(&iter);
  }
  for(i = 0; i < argno; i++)
  {
    if(dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_INVALID)
    {
      break;
    }
    dbus_message_iter_next(&iter);
  }
  type = dbus_message_iter_get_arg_type (&iter);
  if(type != DBUS_TYPE_INVALID)
  {
    ret = 0;
    dbus_message_iter_get_basic (&iter, &ret);
  }
  return  ret;
}

tIdAdditionalParam * _GetNewInfoEl(int type,DBusMessageIter *iter)
{
  tIdAdditionalParam * ret = NULL;

  ret = malloc(sizeof(tIdAdditionalParam));
  if(ret != NULL)
  {
    ret->type = type;
    switch(type)
    {
      case DBUS_TYPE_BYTE:
        dbus_message_iter_get_basic (iter, &ret->value.byte);
        break;
      case DBUS_TYPE_BOOLEAN:
      case DBUS_TYPE_INT32:
      case DBUS_TYPE_UINT32:
        dbus_message_iter_get_basic (iter, &ret->value.integer);
        break;
      case DBUS_TYPE_INT16:
      case DBUS_TYPE_UINT16:
        dbus_message_iter_get_basic (iter, &ret->value.integer);
        break;
      default:
        DBG2("ERROR MESSAGE PARAMETER OF TYPE %d NOT SUPPORTED",type);
        break;
    }
  }

  return ret;
}

GList * _GetIdInfoFromMessage(com_tComMessage * msg)
{
 GList * infoList = NULL;
 DBusMessage *message = msg->msg;
 DBusMessageIter iter;
 int type;
 int i;

 dbus_message_iter_init(message, &iter);
 for(i = 0; i <= 3; i++)
 {
   if(dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_INVALID)
   {
     break;
   }
   dbus_message_iter_next(&iter);
 }
 type = dbus_message_iter_get_arg_type (&iter);
 while(DBUS_TYPE_INVALID != type)
 {
   tIdAdditionalParam * element = _GetNewInfoEl(type,&iter);
   infoList = g_list_append(infoList, element);


   dbus_message_iter_next(&iter);
   type = dbus_message_iter_get_arg_type (&iter);
 }

 return infoList;
}


static void _FreeIdInfo(gpointer element)
{
  DBG2("free element: %X", (uint32_t)element);
  free(element);
}
static void _CopyIdInfo(tIdAdditionalParam *src,tIdInfo * pIdInfo)
{
  tIdAdditionalParam * newEl = NULL;

  newEl = malloc(sizeof(tIdAdditionalParam));
  memcpy(newEl,src,sizeof(tIdAdditionalParam));

  pIdInfo->info = g_list_append(pIdInfo->info, newEl);

}


int SetLedDefaultState(tLedNr ledNr,tLedInfo * ledInfo, tLedInfo * ledDefaultInfo)
{
  int ret =0;


  DBGFUNC();

  ledInfo->defaultState = ledDefaultInfo->state;
  VarCopy(&(ledInfo->defaultVars),&(ledDefaultInfo->vars),ledDefaultInfo->state);


  if(ledInfo->defaultIdInfo.info != NULL)
  {
    g_list_free_full(ledInfo->defaultIdInfo.info,_FreeIdInfo);
    ledInfo->defaultIdInfo.info = NULL;
  }
  ledInfo->defaultIdInfo.id = ledDefaultInfo->idInfo.id;
  DBG("copy default info");
  g_list_foreach(ledDefaultInfo->idInfo.info,(GFunc)_CopyIdInfo,&ledInfo->defaultIdInfo);


  return ret;
}
tLedData * LedInfoToLedData(tLedInfo * ledInfo)
{
  tLedData  * ledData = malloc(sizeof(tLedData));
  if(ledData != NULL)
  {
    memcpy(&ledData->defaultIdInfo,&ledInfo->defaultIdInfo,sizeof(tIdInfo));
    ledData->defaultState = ledInfo->defaultState;
    VarCopy(&(ledData->defaultVars),&(ledInfo->defaultVars),ledInfo->defaultState);
    ledData->flags=ledInfo->flags;
    ledData->idInfo.id = ledInfo->idInfo.id;
    ledData->idInfo.info = NULL;
    ledData->idInfo.refCount = 0;
    g_list_foreach(ledInfo->idInfo.info,(GFunc)_CopyIdInfo,&ledData->idInfo);
    ledData->pWarteschlange = ledInfo->pWarteschlange;
    ledData->setTime.tv_sec = ledInfo->setTime.tv_sec;
    ledData->setTime.tv_usec = ledInfo->setTime.tv_usec;
  }
  return ledData;
}

void _CleanIdInfo(tIdInfo * idInfo)
{
  g_list_free_full(idInfo->info,_FreeIdInfo);
  idInfo->info = NULL;
}

void SetInfoToState(tLedVariables * setLedVariables, tLedInfo * ledInfo)
{
  switch(ledInfo->state)
  {
    case LED_STATE_STATIC:
      setLedVariables->staticVar = ledInfo->vars.staticVar;
      break;
    case LED_STATE_BLINK:
      setLedVariables->blinkVar.color1 = ledInfo->vars.blinkVar.color1;
      setLedVariables->blinkVar.color2 = ledInfo->vars.blinkVar.color2;
      setLedVariables->blinkVar.time1 = ledInfo->vars.blinkVar.time1;
      setLedVariables->blinkVar.time2 = ledInfo->vars.blinkVar.time2;
      break;
    case LED_STATE_FLASH:
      setLedVariables->flashVar.flashColor = ledInfo->vars.flashVar.flashColor;
      setLedVariables->flashVar.flashTime = ledInfo->vars.flashVar.flashTime;
      setLedVariables->flashVar.staticColor = ledInfo->vars.flashVar.staticColor;
      break;
    case LED_STATE_750_ERR:
      setLedVariables->err750Var.errorCode = ledInfo->vars.err750Var.errorCode;
      setLedVariables->err750Var.errorArg = ledInfo->vars.err750Var.errorArg;
      break;
    case LED_STATE_CAN:
      setLedVariables->canVar.canCode = ledInfo->vars.canVar.canCode;
      break;
    default:
      break;
  }
}

void SetLedState(tLedNr ledNr,tLedStateClass status, tLedInfo * ledInfo)
{
  tLedData * ledData = LedInfoToLedData(ledInfo);
  tLedVariables setData;
  if( ledData != NULL )
  {
    gettimeofday(&ledData->setTime,NULL);
    ledData->flags &= ~LED_FLAG_DEFAULT_VAL;
  }
  SetInfoToState(&setData, ledInfo);

  ledserver_LEDCTRL_SetLed(ledNr, status,&setData,ledData);
}



void SetDefaultState(tLedNr ledNr,tLedInfo * ledInfo)
{
  tLedData  * ledData = NULL;
  tLedVariables getData;
  tLedStateClass dummy;

  if(ledInfo->idInfo.info != NULL)
  {
    g_list_free_full(ledInfo->idInfo.info,_FreeIdInfo);
    ledInfo->idInfo.info = NULL;
  }
  ledInfo->idInfo.id = ledInfo->defaultIdInfo.id;
  DBG("copy default info");
  g_list_foreach(ledInfo->defaultIdInfo.info,(GFunc)_CopyIdInfo,&ledInfo->idInfo);
  ledInfo->state = ledInfo->defaultState;
  VarCopy(&(ledInfo->vars),&(ledInfo->defaultVars),ledInfo->defaultState);

  SetLedState(ledNr, ledInfo->state,ledInfo);

  ledserver_LEDCTRL_GetLed(ledNr,&dummy,(void*)&getData,sizeof(getData),(void**)&ledData);
  if(ledData != NULL)
  {
    ledData->flags |= LED_FLAG_DEFAULT_VAL;
    _CleanIdInfo(&ledData->idInfo);
  }
}

void SetIdInfo(tLedInfo * ledInfo,tIdInfo * idInfo)
{
  if(idInfo != NULL)
  {
    ledInfo->idInfo.id          = idInfo->id;
    if(    (ledInfo->idInfo.info != NULL)
        && (idInfo->info != ledInfo->idInfo.info))
    {
      _CleanIdInfo(&ledInfo->idInfo);
    }
    ledInfo->idInfo.info        = idInfo->info;
    idInfo->refCount++;
  }
  else
  {
    _CleanIdInfo(&ledInfo->idInfo);
    ledInfo->idInfo.id = 0x00010018;
    ledInfo->idInfo.info = NULL;
  }
}

void SetArgsFromMessage(tLedEventList * event, com_tComMessage * msg)
{
  tLedNr ledNr;
  if(LED_RETURN_OK != ledserver_LEDCTRL_GetLedNumber(event->ledName,&ledNr))
  {
    ledNr=event->ledNr;
  }

  switch(event->info.state)
  {
    case LED_STATE_OFF:
        //NOT allowed
      break;
    case LED_STATE_STATIC:
      {
        //nothing to do
      }
      break;
    case LED_STATE_BLINK:
      {
        tLedBlink * blinkArg = (tLedBlink*) &(event->args);
        tLedBlink * blinkVal = (tLedBlink*) &(event->info.vars);
        if(blinkArg->color1 != 0)
        {
          blinkVal->color1 = GetArgFromMessage(blinkArg->color1, msg->msg);
        }
        if(blinkArg->color2 != 0)
        {
          blinkVal->color2 = GetArgFromMessage(blinkArg->color2, msg->msg);
        }
        if(blinkArg->time1 != 0)
        {
          blinkVal->time1 = GetArgFromMessage(blinkArg->time1, msg->msg);
        }
        if(blinkArg->time2 != 0)
        {
          blinkVal->time2 = GetArgFromMessage(blinkArg->time2, msg->msg);
        }
      }
      break;
    case LED_STATE_FLASH:
      {
        tLedFlash * flashArg = (tLedFlash*) &(event->args);
        tLedFlash * flashVal = (tLedFlash*) &(event->info.vars);
        if(flashArg->flashColor != 0)
        {
          flashVal->flashColor = GetArgFromMessage(flashArg->flashColor, msg->msg);
        }
        if(flashArg->flashTime != 0)
        {
          flashVal->flashTime = GetArgFromMessage(flashArg->flashTime, msg->msg);
        }
        if(flashArg->staticColor != 0)
        {
          flashVal->staticColor = GetArgFromMessage(flashArg->staticColor, msg->msg);
        }
      }
      break;
    case LED_STATE_CAN:
      {
        tLedCan * canArg = (tLedCan*) &(event->args);
        tLedCan * canVal = (tLedCan*) &(event->info.vars);
        if(canArg->canCode != 0)
        {
          canVal->canCode = GetArgFromMessage(canArg->canCode, msg->msg);
        }
      }
      break;
    case LED_STATE_750_ERR:
      {
        tLed750 * e750Arg = (tLed750*) &(event->args);
        tLed750 * e750Val = (tLed750*) &(event->info.vars);
        if(e750Arg->errorArg != 0)
        {
          e750Val->errorArg = GetArgFromMessage(e750Arg->errorArg, msg->msg);
        }
        if(e750Arg->errorCode != 0)
        {
          e750Val->errorCode = GetArgFromMessage(e750Arg->errorCode, msg->msg);
        }
      }
      break;
    default:
      //empty
      break;
  }
}

void IdHandler(com_tConnection * con, com_tComMessage * msg, log_tEventId id, tLedEventList * event)
{
  struct timeval setTime;
  char * dummyname;
  int ledRemapped;
  com_tComBool set;
  tIdInfo idInfo;
  tLedReturnCode result;


  idInfo.id = id;
  idInfo.info = NULL;
  idInfo.refCount = 0;

  com_MSG_GetParams(con, msg,  COM_TYPE_INT32,   &setTime.tv_sec,
                               COM_TYPE_INT32,   &setTime.tv_usec,
                               COM_TYPE_STRING,  &dummyname,
                               COM_TYPE_BOOLEAN, &set,
                               COM_TYPE_INVALID);


  SetArgsFromMessage(event,msg);

  idInfo.info = _GetIdInfoFromMessage(msg);

  result =  ledserver_LEDCTRL_GetLedNumber(event->ledName,&ledRemapped);
  if(LED_RETURN_ERROR_NO_NAME_AVAILABLE == result)
  {
    ledRemapped=event->ledNr;
    result=LED_RETURN_OK;
  }
  if(result != LED_RETURN_OK && event->alias != NULL)
  {
    tLedAlias * alias = event->alias;
    do{
      result =  ledserver_LEDCTRL_GetLedNumber(alias->alias,&ledRemapped);
      alias=alias->pNext;
    }while(alias != NULL && result != LED_RETURN_OK);

  }

  if(set == TRUE)
  {
    tLedInfo ledInfo;
    DBG("LED SHOULD BE SET");
    GetLedInfo(ledRemapped, &ledInfo);
    if(event->info.flags & LED_FLAG_SET_TO_DEFAULT)
    {
      SetLedDefaultState(ledRemapped,&ledInfo,&event->info);
      SetLedState(ledRemapped,ledInfo.state,&ledInfo);

      if(    (!(event->info.flags & LED_FLAG_DEFAULT_NOT_ADOPT))
          || (ledInfo.flags & LED_FLAG_DEFAULT_VAL))
      {
        DBG("Set val to default");
        SetDefaultState(ledRemapped,&ledInfo);
      }
    }
    else
    {
      ledInfo.state = event->info.state;
      VarCopy(&ledInfo.vars,&(event->info.vars),ledInfo.state);
      SetIdInfo(&ledInfo, &idInfo);
      SetLedState(ledRemapped,ledInfo.state,&ledInfo);
    }
  }
  else
  {
    tLedInfo ledInfo;
    GetLedInfo(ledRemapped, &ledInfo);

    SetDefaultState(ledRemapped,&ledInfo);
  }
  DBG("Clean ID Info");
  _CleanIdInfo(&idInfo);
}
#pragma GCC diagnostic warning "-Wunused-parameter"

void SetDefaults(tLedDefaults * ledDefaults)
{
  tLedNr ledNr;
  while(ledDefaults != NULL)
  {
    tLedInfo ledInfo;
    tLedReturnCode ret;

    ret = ledserver_LEDCTRL_GetLedNumber(ledDefaults->ledName,&ledNr);
    if(LED_RETURN_ERROR_NO_NAME_AVAILABLE == ret)
    {
      ledNr=ledDefaults->ledNr;
      ret = LED_RETURN_OK;
    }

    if(ret == LED_RETURN_OK)
    {
      DBG("SCHEDULE_SetLedDefaultState");
      GetLedInfo(ledNr, &ledInfo);
      SetLedDefaultState(ledNr,&ledInfo,&(ledDefaults->info));
      SetDefaultState(ledNr,&ledInfo);
    }

    DBG("ledDefaults");
    ledDefaults = ledDefaults->pNext;
  }

  for(ledNr = 0; ledNr < ledserver_LEDCTRL_GetLedCount();ledNr++)
  {
    tLedInfo ledInfo;
    GetLedInfo(ledNr, &ledInfo);
    if(ledInfo.state == LED_STATE_OFF)
    {
      tLedInfo defaultInfo;
      defaultInfo.idInfo.id = defaultIdInfo.id;
      defaultInfo.idInfo.info = defaultIdInfo.info;
      defaultInfo.idInfo.refCount = 0;
      defaultInfo.state = LED_STATE_STATIC;
      defaultInfo.vars.staticVar = LED_COLOR_OFF;
      SetLedDefaultState(ledNr,&ledInfo,&(defaultInfo));
      SetDefaultState(ledNr,&ledInfo);
    }
  }

}






void SigTerm(int sigNum, siginfo_t * info, void * ucontext)
{
  void *             array[50];
  void *             caller_address;
  sig_ucontext_t *   uc;
  int size;


  uc = (sig_ucontext_t *)ucontext;

  /* Get the address at the time the signal was raised */
#if defined(__arm__)
  caller_address = (void *) uc->uc_mcontext.arm_pc; // RIP: arm specific
#elif defined(__x86_64__) // gcc specific
  caller_address = (void *) uc->uc_mcontext.rip; // RIP: x86_64 specific
#elif defined(__i386__) // gcc specific
  caller_address = (void *) uc->uc_mcontext.eip; // EIP: x86 specific
#else
#error Unsupported architecture. // TODO: Add support for other arch.
#endif
  if(sigNum != 2)
  {
    printf("signal %d (%s), address is %p from %p\n",
              sigNum, strsignal(sigNum), info->si_addr,
        (void *)caller_address);
    syslog(LOG_ERR, "signal %d (%s), address is %p from %p\n",
              sigNum, strsignal(sigNum), info->si_addr,
        (void *)caller_address);

    size = backtrace(array, 50);

    /* overwrite sigaction with caller's address */
    array[1] = caller_address;
    backtrace_symbols_fd(array, size, STDOUT_FILENO);
  }
  isShutdown=true;
  ledserver_LEDCTRL_Deinit();
  DBG("Generate Coredump");
  //set rlimit to create coredump
  //always create a backtrace is maybe a littlebit hard so dont do it and
  //in case of led-server Crashes ask the user to type "ulimit -c unlimited" on the target
#ifdef DEBUG
  {
    struct rlimit limit;
    limit.rlim_cur = RLIM_INFINITY;
    limit.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &limit);
  }
#endif
  signal(sigNum,SIG_DFL);
  kill(getpid(), sigNum);
}



void SigHup(int sigNum)
{
  syslog(LOG_ERR,"Ignore Signal %d",sigNum);
}

//-- Function: mainloop ------------------------------------------------------------
///
/// The MainLoop Function
///
//------------------------------------------------------------------------------
void mainloop(tLedBehavior * ledBehavior, tLedFiles * ledFiles)
{
  //mtrace();
  while(1)
  {
    sleep(5);
    if(   (UpdateLedEventList(ledBehavior,ledFiles,&ledEvents))
       &&(ledEvents != NULL))
    {
      tLedEventList * pAct=ledEvents;
      tLedEventList * pLast = NULL;
      while(pAct != NULL)
      {
        tLedEventList * pDel = NULL;
        if(pAct->file->flags & LED_FILE_FLAG_REREAD)
        {
          if(pAct->info.flags & LED_FLAG_EVENT_NEW)
          {
            pAct->handle = log_EVENT_RegisterForId(pAct->info.idInfo.id, (log_tEventHandler) IdHandler, pAct);
            DBG2("Registered ID:0x%.8X", pAct->info.idInfo.id);
            pAct->info.flags &= ~LED_FLAG_EVENT_NEW;
          }
          else if(!(pAct->info.flags & LED_FLAG_EVENT_MODIFIED))
          {
            log_EVENT_DeregisterId(pAct->handle);
            DBG2("Deregistered ID:0x%.8X", pAct->info.idInfo.id);
            pDel = pAct;
          }
          else
          {
            pAct->info.flags &= ~LED_FLAG_EVENT_MODIFIED;
          }
        }
        pAct = pAct->pNext;
        if(pDel != NULL)
        {
          if(pLast == NULL)
          {
            ledEvents = pAct;
          }
          else
          {
            pLast->pNext = pAct;
          }
          free(pDel);
        }
        else
        {
          if(pLast == NULL)
          {
            pLast = ledEvents;
          }
          else
          {
            pLast = pLast->pNext;
          }
        }
      }
    }
  }
}

void FreeLedData(tLedData * ledData)
{
  free(ledData);
}

int FreeLedDataHandler(tLedNr ledNr, void* data,void*user_data)
{
  (void)ledNr;
  (void)data;
  tLedData * ledData = (tLedData*)user_data;
  if(ledData != NULL)
  {
    if(isShutdown)
    {
      _CleanIdInfo(&(ledData->defaultIdInfo));
      _CleanIdInfo(&(ledData->idInfo));
    }
    FreeLedData(ledData);
  }
  return 0;
}

static void _SetupLedAliasFromInfoList(const char * alias,tLedNames * newName)
{
  tLedAlias * newAlias = malloc(sizeof(tLedAlias));
  newAlias->alias=strdup(alias);
  newAlias->pNext = newName->alias;
  newName->alias = newAlias;
}

static void _SetupLedFromInfoList(tLEDInfo * info,tLedBehavior * ledBehavior)
{
  tLedNr ledNr;

  if(LED_RETURN_OK == ledserver_LEDCTRL_GetLedNumber(info->name,&ledNr))
  {
    tLedNames     * newName = malloc(sizeof(tLedNames));

    newName->ledNr = ledNr;
    newName->ledName = strdup((const char*)info->name);
    newName->alias =  NULL;
    g_list_foreach(info->alias,(GFunc)_SetupLedAliasFromInfoList,newName);
    newName->pNext = ledBehavior->names;
    ledBehavior->names = newName;
  }
}

static gint _LedFinder(gconstpointer pLedInfo, gconstpointer pLedName)
{
  tLEDInfo * ledInfo = (tLEDInfo*)pLedInfo;
  char * ledName = (char*)pLedName;
  return strcmp(ledInfo->name,ledName);
}

static void _SetupLedDefaultsFromInfoList(tLEDInfo * info)
{
  tLedNr ledNr;

  if(LED_RETURN_OK == ledserver_LEDCTRL_GetLedNumber(info->name,&ledNr))
  {
    if(ledEvents != NULL)
    {
      tLedEventList * pAct=ledEvents;
      tLedInfo ledInfo;
      while(pAct != NULL)
      {
        if(    (pAct->info.idInfo.id & 0x3fffffff)
            == (info->startid        & 0x3fffffff))
        {
          break;
        }
        pAct = pAct->pNext;
      }
      if(pAct != NULL)
      {
        GetLedInfo(ledNr, &ledInfo);
        SetLedDefaultState(ledNr,&ledInfo,&(pAct->info));
        SetDefaultState(ledNr,&ledInfo);
      }
    }
  }
}

//-- Function: main ------------------------------------------------------------
///
/// The Main Function
///
/// \return return for the linux system
//------------------------------------------------------------------------------
int main(void)
{
  com_tConnection      con;
  tLedBehavior    ledBehavior;
  GList          *ledInfoList;
  tLedFiles       ledFiles[2] = {
                                 {
                                  .path   = DIAGNOSTIC_XML_DOCUMENT,
                                  .flags  = 0,
                                  .mtim   = {0,0},
                                  .pNext  = &ledFiles[1]
                                 },
                                 {
                                  .path   = DIAGNOSTIC_XML_DOCUMENT_CUSTOM,
                                  .flags  = 0,
                                  .mtim   = {0,0},
                                  .pNext  = NULL
                                 }};
  struct sigaction sigactionTerm, sigactionHup;
  int i;

  sigactionTerm.sa_sigaction = (void(*)(int, siginfo_t *, void *))SigTerm;
  sigemptyset(&sigactionTerm.sa_mask);
  sigactionTerm.sa_flags = SA_RESTART | SA_SIGINFO;

  sigactionHup.sa_sigaction = (void(*)(int, siginfo_t *, void *))SigHup;
  sigemptyset(&sigactionHup.sa_mask);
  sigactionHup.sa_flags = 0;

  for(i = 1; i < SIGRTMAX; i++)
  {
    if(   (i != 9)
       && (i != 19)
       && (i != 20)
       && (i != 32)
       && (i != 33))
    if(sigaction(i, &sigactionTerm, NULL))
    {
      DBG2("could not init signal %d \n", i);
      perror("msg");
    }
  }
  if(sigaction(SIGHUP, &sigactionHup, NULL))
  {
    DBG2("could not init signal %d (SIGHUP) \n", SIGHUP);
    perror("msg");
  }

  com_GEN_Init(&con);

  log_EVENT_Init("ledserver");
  com_MSG_RegisterName(&con, "wago.ledserver");
  ledserver_LEDCTRL_Init();
  ledserver_LEDCTRL_SetResetUserDataHandler(LED_STATE_STATIC,FreeLedDataHandler);
  ledserver_LEDCTRL_SetResetUserDataHandler(LED_STATE_BLINK,FreeLedDataHandler);
  ledserver_LEDCTRL_SetResetUserDataHandler(LED_STATE_FLASH,FreeLedDataHandler);
  ledserver_LEDCTRL_SetResetUserDataHandler(LED_STATE_750_ERR,FreeLedDataHandler);
  ledserver_LEDCTRL_SetResetUserDataHandler(LED_STATE_CAN,FreeLedDataHandler);
  ledBehavior.defaults = NULL;
  ledBehavior.names = NULL;

  ledInfoList = getInfoLedListFromFile(LED_INFO_FILE);
  g_list_foreach(ledInfoList,(GFunc)_SetupLedFromInfoList,&ledBehavior);

  CreateLedEventList(&ledBehavior,ledFiles,&ledEvents);
  DBG2("ledEvents: %.8X",(uint32_t)(intptr_t)ledEvents);

  //g_list_foreach(ledInfoList,(GFunc)_SetupLedDefaultsFromInfoList,&ledBehavior);
  {
    tLedNames * pAct = ledBehavior.names;
    while(pAct!=NULL)
    {
      GList * llink = g_list_find_custom(ledInfoList,pAct->ledName,_LedFinder);
      if(llink != NULL)
      {
        _SetupLedDefaultsFromInfoList(llink->data);
      }
      pAct=pAct->pNext;
    }
  }


  if(ledEvents != NULL)
  {
    tLedEventList * pAct=ledEvents;
    while(pAct != NULL)
    {
      pAct->handle = log_EVENT_RegisterForId(pAct->info.idInfo.id, (log_tEventHandler) IdHandler, pAct);
      DBG2("Registered ID:0x%.8X", pAct->info.idInfo.id);
      DBG2("flags1:0x%.2X", pAct->info.flags);
      pAct->info.flags &= ~LED_FLAG_EVENT_NEW;
      pAct->info.flags &= ~LED_FLAG_EVENT_MODIFIED;
      DBG2("flags2:0x%.2X", pAct->info.flags);
      pAct = pAct->pNext;
    }
  }
  if(ledBehavior.names != NULL)
  {
    tLedNames * pAct = ledBehavior.names;

    while(pAct != NULL)
    {
      int * ledNr = malloc(sizeof(int));
      char path[4096];
      tLedReturnCode result;

      result =  ledserver_LEDCTRL_GetLedNumber(pAct->ledName,ledNr);
      if(LED_RETURN_ERROR_NO_NAME_AVAILABLE == result)
      {
        *ledNr = pAct->ledNr;
      }
      if(result != LED_RETURN_ERROR_NAME)
      {
        sprintf(path, "/wago/led/%s", pAct->ledName);
        com_MSG_RegisterObject(&con, path, (com_tHandlerFunction) GetLedHandler, (void*) ledNr);
      }
      if(pAct->alias != NULL)
      {
        tLedAlias * act = pAct->alias;
        do{
          result =  ledserver_LEDCTRL_GetLedNumber(act->alias,ledNr);
          if(result == LED_RETURN_ERROR_NAME)
          {
            sprintf(path, "/wago/led/%s", act->alias);
            com_MSG_RegisterObject(&con, path, (com_tHandlerFunction) GetLedHandler, (void*) ledNr);
          }
          act = act->pNext;
        }while(act != NULL);
      }

      pAct = pAct->pNext;
    }
  }
  com_MSG_RegisterObject(&con, LED_DBUS_PATH_ALL, (com_tHandlerFunction) GetAllLedHandler, (void*) ledBehavior.names);
  cleanUpInfoLedList(ledInfoList);
  mainloop(&ledBehavior,ledFiles);
  closelog();

  return 0;
}
