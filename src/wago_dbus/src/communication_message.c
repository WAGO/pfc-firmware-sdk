//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file wago_dbus.c
///
/// \version 0.1
///
/// \brief see wago_dbus_API.h
///
/// \author Hans Florian Scholz : WAGO
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <syslog.h>

#include "communication_API.h"
#include "communication_internal.h"


tInterfaceList  * rootSignalHandler = NULL;
static int32_t    signalHandle = 0;

//int method_call_timeout= -1;

//-- Function: NewCallback ---------------------------------------------------
///
///  Create a New Container fo a Callback-Handler
///
///  \param tHandlerFunction functionpointer to the callback
///  \param user_data       pointer to additional user data
///
///  \return the newly created callback, or NULL on memory error;
///
//------------------------------------------------------------------------------
tCallbackList * NewCallback( com_tHandlerFunction callback, void *user_data)
{
  tCallbackList * cbl     = malloc(sizeof(tCallbackList));
  if(cbl != NULL)
  {
    cbl->pNext              = NULL;
    cbl->callback           = callback;
    cbl->user_data          = user_data;
  }
  return cbl;
}

//-- Function: NewMember -------------------------------------------------------
///
///  Create a New Container for a Signal-Member (name)
///
///  \param member string containing the member
///
///  \return the newly created tMember, or NULL on memory error;
///
//------------------------------------------------------------------------------
tMemberList * NewMember(const char * member)
{
  tMemberList * mbr       = malloc(sizeof(tMemberList));
  if(mbr != NULL)
  {
    mbr->pNext              = NULL;
    mbr->callbackList       = NULL;
    mbr->registered         = false;
    if(member != NULL)
    {
      mbr->member           = malloc((sizeof(char) * strlen(member))+1);
      strcpy(mbr->member, member);
    }
    else
    {
      mbr->member           = NULL;
    }
  }
  return mbr;
}


//-- Function: NewInterface ----------------------------------------------------
///
///  Create a New Container for an Interface
///
///  \param interface string containing the member
///
///  \return the newly created tInterface, or NULL on memory error;
///
//------------------------------------------------------------------------------
tInterfaceList * NewInterface(const char * interface)
{
  tInterfaceList * iFace  = malloc(sizeof(tInterfaceList));
  if(iFace != NULL)
  {
    iFace->pNext            = NULL;
    iFace->memberList       = NULL;
    iFace->registered       = false;
    iFace->interface        = malloc((sizeof(char) * strlen(interface))+1);
    strcpy(iFace->interface, interface);
  }
  return iFace;
}

//-- Function: NewObject ----------------------------------------------------
///
///  Create a New Container for an Object-Handler
///
///  \param tHandlerFunction functionpointer to the callback
///  \param user_data       pointer to additional user data
///
///  \return the newly created Object, or NULL on memory error
///
//------------------------------------------------------------------------------
tObject * MSG_NewObject(com_tHandlerFunction callback, void * user_data)
{
  tObject * obj  = malloc(sizeof(tObject));
  if(obj != NULL)
  {
    obj->callback  = callback;
    obj->user_data = user_data;
    obj->worker = NULL;
  }
  return obj;
}

//-- Function: FreeCallback ----------------------------------------------------
///
///  Frees the Memory of a CallbackHandler
///
///  \param tCallbackList Callback-To be freed
///
///
//------------------------------------------------------------------------------
void FreeCallback(tCallbackList * del)
{
  free(del);
}

void DeleteCallbackFromList(tCallbackList ** root, tCallbackList * prev, tCallbackList * del)
{
  if(prev == NULL)
  {
    *root = del->pNext;
  }
  else
  {
    prev->pNext = del->pNext;
  }
  FreeCallback(del);
}

//-- Function: FreeMember ----------------------------------------------------
///
///  Frees the Memory of a tMember
///
///  \param tMemberList tMember to be freed
///
///
//------------------------------------------------------------------------------
void FreeMember(tMemberList * del)
{
    free(del->member);
    free(del);
}

void DeleteMemberFromList(tMemberList ** root,tMemberList * prev, tMemberList * del)
{
  if(prev == NULL)
  {
    *root = del->pNext;
  }
  else
  {
    prev->pNext = del->pNext;
  }
  FreeMember(del);
}

//-- Function: FreeInterface ----------------------------------------------------
///
///  Frees the Memory of a tMember
///
///  \param tInterfaceList Interface to be freed
///
///
//------------------------------------------------------------------------------
void FreeInterface(tInterfaceList * del)
{
    free(del->interface);
    free(del);
}

void DeleteInterfaceFromList(tInterfaceList ** root,tInterfaceList * prev, tInterfaceList * del)
{
  if(prev == NULL)
  {
    *root = del->pNext;
  }
  else
  {
    prev->pNext = del->pNext;
  }
  FreeInterface(del);
}

//-- Function: FreeInterface ----------------------------------------------------
///
///  Frees the Memory of a tMember
///
///  \param tInterfaceList Interface to be freed
///
///
//------------------------------------------------------------------------------
void FreeObject(tObject * del)
{
  free(del);
}

//-- Function: SetCallBack ----------------------------------------------------
///
///  Set A Callback to a handlerlist of a Member
///
///  \param tMemberList       Member to be extended
///  \param tHandlerFunction  The Callback-Function
///  \param user_data         additional User-Data-Pointer
///
///  \return handle ( >= 0)  if everything is ok; otherwise -1
///
//-----------------------------------------------------------------------------
com_tSignalHandle SetCallBack(tMemberList     * pMember,
                              com_tHandlerFunction   callback,
                 void            * user_data)
{
  tCallbackList * list = pMember->callbackList;
  com_tSignalHandle ret = 0;

  if(list == NULL)
  {
    pMember->callbackList = NewCallback(callback,user_data);
    list = pMember->callbackList;
  }
  else
  {
    while(   (list->callback  != callback)
          || (list->user_data != user_data))
    {
      if(list->pNext == NULL)
      {
        list->pNext      = NewCallback(callback,user_data);
        list             = list->pNext;
        break;
      }
      list = list->pNext;
    }
  }
  if(list == NULL)
  {
    ret = -1;
  }
  else
  {
    list->handle = signalHandle++;
    ret = list->handle;
  }
  return ret;
}

//-- Function: CompareMember ----------------------------------------------------
///
///  Compare to pointer  if they are both NULL or cpoint to an equal string
///
///  \param member1           compare-member 1
///  \param member2           compare-member 2
///
///  \return 0 if they are equal; otherwise 1
///
//-----------------------------------------------------------------------------
int CompareMember(const char *member1,const char *member2)
{
  //if both are NULL
  if(   (member1 == NULL)
     || (member2 == NULL))
  {
    if(member1 == member2)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return strcmp(member1, member2);
  }
}

//-- Function: MSG_GetMember ----------------------------------------------------
///
///  Go throw a tMemberList and look for a given Member-String
///
///  \param pInterface        Interface-Pointer
///  \param memebr            the member-string
///
///  \return NULL if not found otherwise the tMember-Pointer
///
//-----------------------------------------------------------------------------
tMemberList * MSG_GetMember(tInterfaceList * pInterface, const char * member)
{
  tMemberList * list = pInterface->memberList;

  if(member == NULL)
  {
    while(list != NULL)
    {
      if(list->member == NULL)
      {
        break;
      }
      list = list->pNext;
    }
  }
  else
  {
    while(list != NULL)
    {
      if(   (list->member != NULL)
         && (!strcmp(member, list->member)))
      {
        break;
      }
      list = list->pNext;
    }
  }
  return list;
}


//-- Function: MSG_GetMember ----------------------------------------------------
///
///  Go throw a tMemberList and look for a given Member-String if not found
///  create a ne element at the end of the list
///
///  \param pInterface        Interface-Pointer
///  \param memebr            the member-string
///
///  \return NULL on Error otherwise the tMember-Pointer
///
//-----------------------------------------------------------------------------
tMemberList * GetMemberNew(tInterfaceList * pInterface, const char * member)
{
  tMemberList * list = pInterface->memberList;
  if(list == NULL)
  {
    pInterface->memberList = NewMember(member);
    list = pInterface->memberList;
  }
  else
  {
    while(CompareMember(member, list->member))
    {
      if(list->pNext == NULL)
      {
        list->pNext      = NewMember(member);
        list             = list->pNext;
        break;
      }
      list = list->pNext;
    }
  }
  return list;
}

//-- Function: MSG_GetInterface ----------------------------------------------------
///
///  Go throw a tInterfaceList and look for a given interface-String
///
///  \param inetrafce            the interface-string
///
///  \return NULL if not found otherwise the tInterface-Pointer
///
//-----------------------------------------------------------------------------
tInterfaceList * MSG_GetInterface(const char * interface)
{
  tInterfaceList * list = rootSignalHandler;
  while(list != NULL)
  {
    if(!strcmp(interface, list->interface))
    {
      break;
    }
    list = list->pNext;
  }
  return list;
}

//-- Function: GetInterfaceNew ----------------------------------------------------
///
///  Go throw a tInterfaceList and look for a given Member-String if not found
///  create a ne element at the end of the list
///
///  \param interface            the interface-string
///
///  \return NULL on Error otherwise the tMember-Pointer
///
//-----------------------------------------------------------------------------
tInterfaceList * GetInterfaceNew(const char * interface)
{
  tInterfaceList * list = MSG_GetInterface(interface);

  if(list == NULL)
  {
    list = NewInterface(interface);
    if(list != NULL)
    {
      list->pNext = rootSignalHandler;
      rootSignalHandler = list;
    }
  }

  return list;
}

//-- Function: RegisterMatch ----------------------------------------------------
///
///  Register a Matchrule for an Interface and a Member at the DBus
///
///  \param con connection-pointer
///  \param pInterface The given Interface
///  \param pMember the given Member
///
///  \return 0 on success; otherwise -1;
///
//-----------------------------------------------------------------------------
int RegisterMatch(com_tConnection *con, tInterfaceList * pInterface, tMemberList * pMember)
{
  char regStr[4096];
  const char * pRegStr = regStr;
  sprintf(regStr, "type='signal',interface='%s'", pInterface->interface);
  if(pMember->member != NULL)
  {
    sprintf(regStr, "%s,member='%s'", regStr, pMember->member);
  }
  //dbus_error_init(&con->error);
  dbus_error_free(&con->error);
  //dbus_bus_add_match (con->bus, regStr,&con->error);
  com_MSG_MethodCall(con, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, "AddMatch",
                     DBUS_TYPE_STRING, &pRegStr,
                     COM_TYPE_INVALID,COM_TYPE_INVALID);
  if(dbus_error_is_set(&con->error))
  {
    return -1;
  }
  else
  {
    pMember->registered = true;
    pInterface->registered = true;
    return 0;
  }
}

//-- Function: CallObject ----------------------------------------------------
///
///  Default-Object-Handler for calling the registered Object
///
///  \param con connection-pointer
///  \param the DBus-Message
///  \param Pointer to an Object-List
///
///  \return DBUS_HANDLER_RESULT_HANDLED
///
//-----------------------------------------------------------------------------
DBusHandlerResult MSG_CallObject(DBusConnection *connection,
                             DBusMessage *message,
                             tObject * object)
{

  static int r = 0;
  r++;

  //struct timespec start, end;
  //clock_gettime(CLOCK_MONOTONIC, &start);
  if(object->worker == NULL)
  {
    com_tComMessage msg;
    memset(&msg, 0, sizeof msg);
    com_tConnection con;
    memset(&con, 0, sizeof con);

	  con.bus = connection;
	  con.type = COM_CONNECTION_PROXY;
	  con.method_call_timeout = -1;
	  dbus_error_init(&con.error);
	  msg.msg = message;
	  object->callback(&con, &msg, object->user_data);
  }
  else
  {
	  dbus_message_ref(message);
	  SERV_AppentToWorkingQueue(object,connection,message);
  }
  //clock_gettime(CLOCK_MONOTONIC, &end);
  //printf("got msg at      %ld.%.9ld %d\n", start.tv_sec, start.tv_nsec, dbus_message_get_serial   (  message ) );
  //printf("release func at %ld.%.9ld %d\n", end.tv_sec, end.tv_nsec, dbus_message_get_serial   (  message ));

  return DBUS_HANDLER_RESULT_HANDLED;
}

//-- Function: UnregisterObject ------------------------------------------------
///
///  Default-Object-Handler for calling the registered Object
///
///  \param the Object that has bo be unregistered
///
///  \return DBUS_HANDLER_RESULT_HANDLED
///
//-----------------------------------------------------------------------------
DBusHandlerResult MSG_UnregisterObject(DBusConnection *con,tObject * object)
{
  (void)con; //unused
  FreeObject(object);
  return DBUS_HANDLER_RESULT_HANDLED;
}

//-- Function: PendingWait ------------------------------------------------
///
///  Handler for wating on an DBus Pending Call
///
///  \param pending needed by dbus definition
///  \param cond
///
///
//-----------------------------------------------------------------------------
void PendingWait(DBusPendingCall *pending, tPendingConditions *cond)
{
  (void)pending; //unused
  pthread_mutex_lock(&cond->mutex);
  pthread_cond_signal(&cond->condition);
  pthread_mutex_unlock(&cond->mutex);
}

void com_MSG_SignalVaArg(com_tConnection* con,
                         const char * path,
                         const char * interface,
                         const char * name,
                         int          first_arg_type,
                         va_list      var_args)
{
  DBusMessage *message = dbus_message_new_signal(path, interface, name);

  dbus_message_append_args_valist (message,
                                   first_arg_type,
                                   var_args);

  dbus_connection_send(con->bus, message, NULL);
  //dbus_connection_flush(con->bus);
}

//-- Function: com_MSG_Signal ---------------------------------------------------
///
///  Send a Signal to a given interface
///
///  \param tConnection* pointer to a connection variable
///  \param path         object path
///  \param interface    the interface
///  \param name         the member-name
///  \param additional parameters
///
//------------------------------------------------------------------------------
void com_MSG_Signal(com_tConnection* con,
                     const char * path,
                     const char * interface,
                     const char * name,
                     int          first_arg_type,
                     ...)
{
  va_list var_args;
  va_start (var_args, first_arg_type);
  (void)com_GEN_BlockThreadCancelling();
  com_MSG_SignalVaArg(con,path,interface,name,first_arg_type,var_args);
  (void)com_GEN_UnblockThreadCancelling();
  va_end (var_args);
}

int _MutexInit(pthread_mutex_t * mutex)
{
  int result;
  pthread_mutexattr_t mta;
  do
    {
      result = pthread_mutexattr_init(&mta);
      if(result)break;

      result = pthread_mutexattr_setprotocol(&mta, PTHREAD_PRIO_INHERIT);
      if(result)break;

      result = pthread_mutexattr_setrobust(&mta, PTHREAD_MUTEX_ROBUST);
      if(result)break;

      result = pthread_mutex_init(mutex, &mta);
      if(result)break;

      result = pthread_mutexattr_destroy(&mta);
      if(result)break;
    }while(0);
  return result;
}

int _ConditionInit(pthread_cond_t * condition)
{
  pthread_condattr_t attr;
  int result;

  do{
    result = pthread_condattr_init(&attr);
    if(result)break;

    result = pthread_condattr_setpshared(&attr,PTHREAD_PROCESS_PRIVATE);
    if(result)break;

    result = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    if(result)break;

    result =pthread_cond_init(condition,&attr);
    if(result)break;

    result =pthread_condattr_destroy(&attr);
    if(result)break;
  }while(0);

  return result;

}
typedef enum  {
  PCS_UNINTIALIZED,
  PCS_FREE,
  PCS_USED
}tPendingConditionState;
typedef struct {
    tPendingConditions cond;
    tPendingConditionState state;
}tPendingPool;
pthread_mutex_t protectPendingCondition;
static tPendingPool ppool[] = {
                                { .state = PCS_UNINTIALIZED },
                                { .state = PCS_UNINTIALIZED },
                                { .state = PCS_UNINTIALIZED }
};
static const size_t szPendingPool= sizeof (ppool)/sizeof (tPendingPool);

static int _InitPendingCondition(tPendingConditions * cond)
{
  int result;

    do{
      result = _MutexInit(&cond->mutex);
      if(result)break;
      result = _ConditionInit(&cond->condition);
      if(result)break;

    }while(0);
    if(result)
    {
      pthread_mutex_destroy(&cond->mutex);
      pthread_cond_destroy(&cond->condition);
    }
    return result;
}

tPendingConditions * _getPendingCondition(void)
{
  tPendingConditions * cond = NULL;


  size_t i;
  static int once = 0;
  if(!once)
  {
    _MutexInit(&protectPendingCondition);
    once=1;
  }

  pthread_mutex_lock(&protectPendingCondition);

  for(i = 0; i < szPendingPool; i++)
  {
    if(ppool[i].state == PCS_UNINTIALIZED)
    {

      if(_InitPendingCondition(&(ppool[i].cond)))
      {
        //errorhandling??
        break;
      }
      ppool[i].state=PCS_FREE;
    }
    if(ppool[i].state == PCS_FREE)
    {
      ppool[i].state=PCS_USED;
      cond = &(ppool[i].cond);
      break;
    }
  }
  if(cond == NULL)
  {
    cond = malloc(sizeof (tPendingConditions));
    memset(cond,0,sizeof (tPendingConditions));
    if(_InitPendingCondition(cond))
    {
      free(cond);
      cond=NULL;
    }
  }
  pthread_mutex_unlock(&protectPendingCondition);
  return cond;
}


void _releasePendingCondition(tPendingConditions * cond)
{
  if(cond == NULL)
  {
    return;
  }
  size_t i;
  int heapMutex=1;
  pthread_mutex_lock(&protectPendingCondition);
  for(i = 0; i < szPendingPool; i++)
  {
    if(cond == &(ppool[i].cond))
    {
      heapMutex=0;
      ppool[i].state=PCS_FREE;
      break;
    }
  }
  if(heapMutex)
  {
    pthread_mutex_destroy(&cond->mutex);
    pthread_cond_destroy(&cond->condition);
    free(cond);
  }
  pthread_mutex_unlock(&protectPendingCondition);
}

/*TODO: Break down to several methods for not using the goto's*/
int com_MSG_MethodCallVaArgs(com_tConnection* con,
                              const char * destination,
                              const char * path,
                              const char * name,
                              int          first_arg_type,
                              va_list      var_args)
{
  DBusMessage * message = dbus_message_new_method_call(destination, path,
                                                       NULL, name);
  DBusMessage * reply;
  DBusPendingCall * pending_return = NULL;
  tPendingConditions * cond = NULL;

  int ret = 0;
  dbus_message_append_args_valist (message,
                                   first_arg_type,
                                   var_args);

  dbus_error_free(&con->error);

  if(con->type == COM_CONNECTION_CONTEXT)
  {
    reply = DIRECT_HandleContextMessage(con,message);
    goto HandleReply;
  }

  if(SERV_TryBlockServer())
  {

    dbus_bool_t returns;
    cond = _getPendingCondition();

    returns = dbus_connection_send_with_reply(con->bus,message,&pending_return, con->method_call_timeout);
    if( (!returns) || (pending_return == NULL) )
    {
      ret = -ECOMM;
      SERV_UnblockServer();
      goto EndOfFunction;
    }
    pthread_mutex_lock(&cond->mutex);
    //first ask if pendig comleted in the case that we don't need to set notify
    if(FALSE == dbus_pending_call_get_completed   (   pending_return ))
    {
      int rc=0;
      struct timespec pendingTimeout;
      unsigned int waitRetry=0;
      if(FALSE == dbus_pending_call_set_notify(pending_return,
                                               (DBusPendingCallNotifyFunction) PendingWait,
                                               cond, NULL))
      {
        ret = -ECOMM;
        pthread_mutex_unlock(&(cond->mutex));
        SERV_UnblockServer();
        goto EndOfFunction;
      }

      dbus_connection_flush(con->bus);
      //set first timeout to ashort time: 20 ms
      clock_gettime(CLOCK_MONOTONIC, &pendingTimeout);
      pendingTimeout.tv_nsec += 20000000;//20ms
      if(pendingTimeout.tv_nsec >= 1000000000)
      {
        pendingTimeout.tv_sec ++;
        pendingTimeout.tv_nsec -= 1000000000;
      }
      while(   (FALSE == dbus_pending_call_get_completed(   pending_return ))
            && (rc == 0))
      {
        rc = pthread_cond_timedwait(&(cond->condition),&(cond->mutex),
                                  &pendingTimeout);
        if( rc == ETIMEDOUT)
        {
          syslog(LOG_ERR, "DBus PendingCall error: Condition-Timeout (completed %d) ",
                 dbus_pending_call_get_completed   (   pending_return ));
          //wait max 10s (DBus std timeout) for 50ms when Pending call not completed
          if(waitRetry++ < 200)
          {
            rc=0;
            clock_gettime(CLOCK_MONOTONIC, &pendingTimeout);
            pendingTimeout.tv_nsec += 50000000;//50ms
            if(pendingTimeout.tv_nsec >= 1000000000)
            {
              pendingTimeout.tv_sec ++;
              pendingTimeout.tv_nsec -= 1000000000;
            }
          }
        }
      }
    }
    pthread_mutex_unlock(&(cond->mutex));
  }
  else
  {
    dbus_connection_send_with_reply   (con->bus,message,&pending_return, con->method_call_timeout);
    if(pending_return == NULL)
    {
      ret = -ECOMM;
      SERV_UnblockServer();
      goto EndOfFunction;
    }
    dbus_connection_flush(con->bus);
    dbus_pending_call_block(pending_return);
  }
  SERV_UnblockServer();
  reply =dbus_pending_call_steal_reply (pending_return);
  dbus_pending_call_unref(pending_return);
HandleReply:
  if(reply == NULL)
  {
    if(ret == 0)ret = -ENOMEM;
  }
  else
  {
    //dbus_pending_call_unref(pending_return);
    if(DBUS_MESSAGE_TYPE_ERROR == dbus_message_get_type  (  reply) )
    {
      dbus_error_free(&con->error);
      dbus_set_error_from_message (&con->error, reply);
      if(ret == 0)ret = -ENOMSG;
    }
    else
    {
      while(first_arg_type != COM_TYPE_INVALID)
      {
        first_arg_type = va_arg (var_args, int);
      }
      first_arg_type = va_arg (var_args, int);
      if(FALSE == dbus_message_get_args_valist  ( reply,&con->error,first_arg_type,var_args))
      {
        if(ret == 0)ret = -EMEDIUMTYPE;
      }
    }
    if(con->lastReply != NULL)
    {
      dbus_message_unref(con->lastReply);
    }
    con->lastReply = reply;
  }
  if(con->lastMessage != NULL)
  {
    dbus_message_unref(con->lastMessage);
  }
  con->lastMessage = message;
EndOfFunction:
  _releasePendingCondition(cond);
  errno = -ret;
  if(ret != 0)
  {
    ret = -1;
  }
  return ret;
}

//-- Function: com_MSG_MethodCall ---------------------------------------------------
///
///  Send a MethodCall to a given object
///
///  \param tConnection* pointer to a connection variable
///  \param dest         the destination name
///  \param path         object path
///  \param name         the member-name
///  \param additional parameters
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_MethodCall(com_tConnection* con,
                       const char * destination,
                       const char * path,
                       const char * name,
                       int          first_arg_type,
                       ...)
{
  va_list var_args;
  int ret;
  va_start (var_args, first_arg_type);
  (void)com_GEN_BlockThreadCancelling();
  ret = com_MSG_MethodCallVaArgs(con,destination,path,name,first_arg_type,var_args);
  (void)com_GEN_UnblockThreadCancelling();
  va_end (var_args);
  return ret;
}

//-- Function: com_MSG_RegisterSignal ---------------------------------------------------
///
///  Registers a Signal-Handler for a given interface
///
///  \param interface    the interface
///  \param member         the signal member
///  \param callback     the callback-function
///
///  \return -1 on error or an Handle ( >= 0) for deregistering the signal
//------------------------------------------------------------------------------
com_tSignalHandle com_MSG_RegisterSignal(com_tConnection *con,
                                     const char * interface,
                                     const char * member,
                                     com_tHandlerFunction callback,
                                     void * user_data)
{
  tInterfaceList * pInterface;
  tMemberList    * pMember;
  com_tSignalHandle ret =0;
  pInterface = GetInterfaceNew(interface);
  if(pInterface == NULL)
  {
    ret = -1;

  }
  if(   (ret == 0)
     &&  (NULL == (pMember = GetMemberNew(pInterface, member))))
  {
    ret = -1;
  }
  if((ret == 0))
  {
    ret = SetCallBack(pMember, callback, user_data);
  }

  if(ret >= 0)
  {
    if(   (pInterface->registered == false)
       || (pMember->registered == false))
    {
      RegisterMatch( con, pInterface, pMember);
    }
    SERV_RecallServerThread();
  }
  else
  {
    //if an error occurs, it can only be a memory error
    dbus_set_error  (   &con->error, DBUS_ERROR_NO_MEMORY , NULL );
  }
  return ret;
}

//-- Function: com_MSG_RegisterName ---------------------------------------------------
///
///  Registers a unique name on the bus
///
///  \param name    the name
///
///  \return Resultcode (>0) on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_RegisterName(com_tConnection * con, const char * name)
{
  DIRECT_RequestName(name);
  dbus_error_free(&con->error);

  (void)com_GEN_BlockThreadCancelling();
  if(0 <= dbus_bus_request_name(con->bus, name, DBUS_NAME_FLAG_DO_NOT_QUEUE , &con->error))
  {
    (void)com_GEN_UnblockThreadCancelling();
    return 0;
  }
  else
  {
    (void)com_GEN_UnblockThreadCancelling();
    return -1;
  }
}

//-- Function: com_MSG_RegisterObject ---------------------------------------------------
///
///  Registers an Object-Handler for a given path
///
///  \param path         the path
///  \param callback     the callback-function
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
static int _RegisterObject(com_tConnection * con,
                           const char * path,
                           tObject * newObject)
{
  static DBusObjectPathVTable vtable;
  int ret = 0;
  vtable.message_function = (DBusObjectPathMessageFunction)MSG_CallObject;
  vtable.unregister_function = (DBusObjectPathUnregisterFunction)MSG_UnregisterObject;

  (void)com_GEN_BlockThreadCancelling();
  if(    con->type != COM_CONNECTION_CONTEXT
      && FALSE == dbus_connection_try_register_object_path  (con->bus,
                                                             path,
                                                             &vtable,
                                                             (void*)newObject,
                                                             &con->error))
  {
    ret = -1;
  }

  (void)com_GEN_UnblockThreadCancelling();
  if(ret == 0)
  {
    objectReferenceCounter++;
    SERV_RecallServerThread();
    DIRECT_AddObject(&vtable,path,newObject);
  }

  return ret;
}

int com_MSG_RegisterObject(com_tConnection * con,
                           const char * path,
                           com_tHandlerFunction callback,
                           void * user_data)
{
	tObject * newObject = MSG_NewObject(callback, user_data);
	return _RegisterObject(con,path,newObject);
}

int com_MSG_RegisterObjectForWorker(com_tConnection * con,
                           const char * path,
                           com_tHandlerFunction callback,
                           void * user_data,
                           tDbusWorker * handle)
{
	tObject * newObject = MSG_NewObject(callback, user_data);
	newObject->worker = handle;
	return _RegisterObject(con,path,newObject);
}

//-- Function: com_MSG_GetMethodName ---------------------------------------------------
///
///  Returnes the name of a given message
///
///  \param tComMessage*   the message
///
///  \return the method name
//------------------------------------------------------------------------------
const char * com_MSG_GetMethodName(com_tComMessage * msg)
{
  return dbus_message_get_member(msg->msg);
}

//-- Function: com_MSG_GetParams ---------------------------------------------------
///
///  Get Parameter of a Message
///
///  \param tConnection*        pointer to a connection variable
///  \param tComMessage*        the message
///  \param additional parameter
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_GetParams(com_tConnection* con,
                      com_tComMessage * msg,
                      int first_arg_type, ...)
{
  va_list var_args;
  int ret = 0;
  va_start (var_args, first_arg_type);
  if(FALSE == dbus_message_get_args_valist  ( msg->msg,&(con->error),first_arg_type,var_args))
  {
    ret = -1;
  }
  va_end (var_args);

  return ret;
}

//-- Function: com_MSG_Return ---------------------------------------------------
///
///  Create Return to a given Message
///
///  \param tConnection*        pointer to a connection variable
///  \param tComMessage*        the message
///  \param additional parameter
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_Return(com_tConnection* con,
                   com_tComMessage * msg,
                   int first_arg_type, ...)
{
  DBusMessage * reply = dbus_message_new_method_return(msg->msg);
  va_list var_args;
  va_start (var_args, first_arg_type);
  dbus_message_append_args_valist (reply,
                                   first_arg_type,
                                   var_args);
  va_end (var_args);
  //struct timespec  end;
  //clock_gettime(CLOCK_MONOTONIC, &end);
  if(con->type != COM_CONNECTION_CONTEXT)
  {
    (void)com_GEN_BlockThreadCancelling();
    dbus_connection_send(con->bus, reply, NULL);
    dbus_connection_flush(con->bus);
    dbus_message_unref(reply);
    (void)com_GEN_UnblockThreadCancelling();
  }
  else
  {
    con->directReply = reply;
  }
  return 0;
}

//-- Function: com_MSG_ReturnError ---------------------------------------------------
///
///  Create error-message as Return to a given Message
///
///  \param tConnection*        pointer to a connection variable
///  \param tComMessage*        the message
///  \param name                error name
///  \param format              format of the message String or NULL
///  \param additional Params   used if Format is not NULL
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_ReturnError(com_tConnection * con,
                        com_tComMessage * msg,
                        const char  * name,
                        const char  * format,
                        ...)
{
  DBusMessage * reply;
  char errorMsg[4096];
  va_list ap;

  if(format != NULL)
  {
    va_start(ap, format);
    vsprintf(errorMsg, format, ap);
    va_end(ap);
    reply = dbus_message_new_error(msg->msg,name,errorMsg);
  }
  else
  {
    reply = dbus_message_new_error(msg->msg,name,NULL);
  }
  if(con->type != COM_CONNECTION_CONTEXT)
  {
    (void)com_GEN_BlockThreadCancelling();
    dbus_connection_send(con->bus, reply, NULL);
    dbus_connection_flush(con->bus);
    (void)com_GEN_UnblockThreadCancelling();
  }
  else
  {
    con->directReply = reply;
  }
  return 0;
}

#if 0
//-- Function: com_MSG_DeregisterSignal ---------------------------------------------------
///
///  Removes a Signal-Handler for a given interface
///
///  \param interface    the interface
///  \param name         the signal member
///  \param callback     the callback-function
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_DeregisterSignal(tConnection     * con,
                             const char      * interface,
                             const char      * name,
                             tSignalHandle     handle)
{
  tInterfaceList * iFace = MSG_GetInterface(interface);
  tMemberList    * member;
  tCallbackList  * cbList;

  dbus_error_free(&con->error);
  if(iFace == NULL)
  {
    return 0;
  }
  member = MSG_GetMember(iFace, name);
  if(member == NULL)
  {
    return 0;
  }
  cbList = member->callbackList;
  if(cbList->handle == handle)
  {
    member->callbackList = cbList->pNext;
    FreeCallback(cbList);
  }
  else
  {
    while(cbList->pNext != NULL)
    {
      tCallbackList  * pCheck = cbList->pNext;
      if(cbList->handle == handle)
      {
        cbList->pNext = pCheck->pNext;
        FreeCallback(pCheck);
        break;
      }
      cbList = cbList->pNext;
    }
  }
  if(member->callbackList == NULL)
  {
    char removeMatch[4096] = "\0";
    if(member->member != NULL)
    {
      sprintf(removeMatch, "member='%s',",member->member);
    }
    if(iFace->memberList == member)
    {
      iFace->memberList = member->pNext;
      FreeMember(member);
    }
    else
    {
      tMemberList * pMemAct = iFace->memberList;
      while(pMemAct->pNext != NULL)
      {
        tMemberList * pCheck = pMemAct->pNext;
        if(pCheck  == member)
        {
          pMemAct->pNext = pCheck->pNext;
          FreeMember(pCheck);
        }
        pMemAct = pMemAct->pNext;
      }
    }
    sprintf(removeMatch, "%stype='signal',interface='%s'", removeMatch, iFace->interface);
    if(iFace->memberList == NULL)
    {

      if(iFace == rootSignalHandler)
      {
        rootSignalHandler = iFace->pNext;
        FreeInterface(iFace);
      }
      else
      {
        tInterfaceList * pIfAct = rootSignalHandler;
        while(pIfAct->pNext != NULL)
        {
          tInterfaceList * pCheck = pIfAct->pNext;
          if(pCheck  == iFace)
          {
            pIfAct->pNext = pCheck->pNext;
            FreeInterface(pCheck);
          }
          pIfAct = pIfAct->pNext;
        }
      }
    }
    dbus_bus_remove_match(con->bus,removeMatch,&con->error);
    if(dbus_error_is_set  ( &con->error ))
    {
      return -1;
    }
  }
  SERV_RecallServerThread();
  return 0;
}
#else
int com_MSG_DeregisterSignal(com_tConnection     * con,
                             int               handle,
                             void(*MemoryFreeFunction)(void *user_data))
{
  tInterfaceList * iFace;
  tInterfaceList * iFacePrev;
  dbus_error_free(&con->error);
  iFace = rootSignalHandler;
  iFacePrev = NULL;
  while(iFace != NULL)
  {
    tMemberList    * member = iFace->memberList;
    tMemberList    * memberPrev = NULL;
    while(member != NULL)
    {
      tCallbackList  * cbList = member->callbackList;
      tCallbackList  * cbListPrev = NULL;

      while(cbList != NULL)
      {
        if(cbList->handle == handle)
        {
          char removeMatch[4096] = "\0";
          const char * pRemoveMatch = removeMatch;
          if(MemoryFreeFunction != NULL)
          {
            MemoryFreeFunction(cbList->user_data);
          }
          if(member->member != NULL)
          {
            sprintf(removeMatch, "member='%s',",member->member);
          }
          sprintf(removeMatch, "%stype='signal',interface='%s'", removeMatch, iFace->interface);
          DeleteCallbackFromList(&member->callbackList, cbListPrev, cbList);
          if(member->callbackList == NULL)
          {
            DeleteMemberFromList(&iFace->memberList, memberPrev, member);
            if(iFace->memberList == NULL)
            {
              DeleteInterfaceFromList(&rootSignalHandler, iFacePrev, iFace);
              //dbus_bus_remove_match(con->bus,removeMatch,&con->error);
              com_MSG_MethodCall(con, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, "RemoveMatch",
                                 DBUS_TYPE_STRING, &pRemoveMatch,
                                 COM_TYPE_INVALID,COM_TYPE_INVALID);
            }
          }
          SERV_RecallServerThread();

          if(dbus_error_is_set  ( &con->error ))
          {
            return -1;
          }
          //return or goto ... both is bad
          goto loopOut;
        }
        cbListPrev = cbList;
        cbList = cbList->pNext;
      }
      memberPrev = member;
      member = member->pNext;
    }
    iFacePrev = iFace;
    iFace = iFace->pNext;
  }
loopOut:
  return 0;
}
#endif

//-- Function: com_MSG_DeregisterObject ---------------------------------------------------
///
///  Removes an Object-Handler for a given path
///
///  \param path         the path
///  \param callback     the callback-function
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_DeregisterObject(com_tConnection     * con,
                             const char          * path)
{
  DIRECT_RemoveObject(path);
  if(   con->type != COM_CONNECTION_CONTEXT
     && TRUE ==dbus_connection_unregister_object_path  (con->bus,path))
  {
    if(objectReferenceCounter != 0)
    {
      objectReferenceCounter--;
    }
    SERV_RecallServerThread();
    return 0;
  }
  else
  {
    return -1;
  }
}

void com_MSG_SetMethodCallTimeout(com_tConnection     * con,
                                  int timeout_milliseconds)
{
  con->method_call_timeout = timeout_milliseconds;
}
