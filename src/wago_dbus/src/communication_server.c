//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
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
#include <config.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <errno.h>
#include <pthread.h>
#include <mqueue.h>
#include <poll.h>

#include "communication_API.h"
#include "communication_internal.h"

#define WORKER_PREFIX "wdbw_"
#define WORKER_NAME_LEN(name) (strlen(WORKER_PREFIX)+strlen(name)+strlen("_XXXXXX")+1)


struct stDbusWorker{
	pthread_t id;
	char * name;
	unsigned int priority;
	mqd_t  fdq;
};

typedef struct stWorkerList tWorkerList;

struct stWorkerList {
	tDbusWorker * worker;
	tWorkerList * pNext;
};



typedef struct stWorkerMsgQ {
	tObject * object;
	DBusConnection * DBusCon;
	DBusMessage     * DBusMsg;
}tWorkerMsgQ;

tReceiverState  receiverState = REC_STATE_STOP;
tWorkerList * pWorkerRoot=NULL;

bool                     signalFilterRegistered = false;
bool                     receiverUp = false;
int                      objectReferenceCounter = 0;
static pthread_t         threadReceiver;
static int               receiverThreadPriority = 49;
static GMainLoop       * loop =  NULL;
static pthread_mutex_t   mutexReceiver = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t   mutexStartReceiver = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t    condStartReceiver = PTHREAD_MUTEX_INITIALIZER;
static const char      * receiverThreadName = "com_DBUS_worker";
//static GMainContext *SERV_Context=NULL;

guint
my_g_idle_add ( GSourceFunc    function,
      		 GMainContext *Context)
{
  GSource *source;
  guint id;

  if (function == NULL)
  {
	return -1;
  }

  source = g_idle_source_new ();

  g_source_set_priority (source,G_PRIORITY_DEFAULT_IDLE );

  g_source_set_callback (source, function, NULL, NULL);
  id = g_source_attach (source, Context);
  g_source_unref (source);

  return id;
}

guint
my_g_timeout_add ( guint          interval,
                    GSourceFunc    function,
  		    GMainContext *Context)
{
  GSource *source;
  guint id;


  if (function == NULL)
  {
	return -1;
  }

  source = g_timeout_source_new (interval);

  g_source_set_priority (source,G_PRIORITY_DEFAULT_IDLE );

  g_source_set_callback (source, function, NULL, NULL);
  id = g_source_attach (source, Context);
  g_source_unref (source);

  return id;
}


gboolean  loopReady(gpointer data)
{
  (void) data;
  if(receiverUp == false)
  {
    receiverUp = true;
    //pthread_mutex_unlock(&mutexReceiver);
    if (pthread_mutex_lock(&mutexStartReceiver) == -EOWNERDEAD)
    {
       pthread_mutex_consistent(&mutexStartReceiver);
    }
    pthread_cond_broadcast(&condStartReceiver);
    pthread_mutex_unlock(&mutexStartReceiver);
  }

  return FALSE;
}

gboolean  timeHandler(gpointer data)
{
  (void) data;
  if(receiverUp == false)
  {
    g_idle_remove_by_data(NULL);
    receiverUp = true;
    //pthread_mutex_unlock(&mutexReceiver);
    if (pthread_mutex_lock(&mutexStartReceiver) == -EOWNERDEAD)
    {
       pthread_mutex_consistent(&mutexStartReceiver);
    }
    pthread_cond_broadcast(&condStartReceiver);
    pthread_mutex_unlock(&mutexStartReceiver);
  }

  return FALSE;
}

//-- Function: ReceiveServer ---------------------------------------------------
///
///  DBus Thread-Function for receiving messages
///
///
///  \return only NULL
//------------------------------------------------------------------------------
static void * ReceiveServer(void * user_data)
{
  DBusError error;

  DIRECT_EnableDirectCommunication();

  //we use GLib because it is much easier to use the preimplemented main loop
  loop = g_main_loop_new (com_GEN_GetServerContext(), FALSE);

  //add idle function to be sure that main-loop is ready when mutexReceiver is free
  my_g_idle_add (loopReady,com_GEN_GetServerContext() );
  //if main loop will not become idle it may have already insalled DBUS dispatcher.
  //so we can unlock the mutex after a timeout
  my_g_timeout_add(50,timeHandler, com_GEN_GetServerContext() );
  g_main_loop_run (loop);
  //receiverUp = false;
  pthread_exit(NULL);

}

//-- Function: CallSignalHandler -----------------------------------------------
///
///  Call a user-defined Signal-handler
///
///
///  \return DBUS_HANDLER_RESULT_NOT_YET_HANDLED if no handler was called
///          DBUS_HANDLER_RESULT_HANDLED if a handler was called
//------------------------------------------------------------------------------
static DBusHandlerResult CallSignalHandler(tMemberList * mbr, DBusMessage * message)
{
  if(   (mbr == NULL)
     || (mbr->callbackList == NULL))
  {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }
  else
  {
    tCallbackList * cbl = mbr->callbackList;
    com_tConnection con;
    con.bus = busConnection.bus;
    dbus_error_init(&con.error);
    while(cbl != NULL)
    {
      com_tComMessage msg = {
                         .msg = message
      };
      cbl->callback(&con, &msg, cbl->user_data);
      dbus_error_free(&con.error);
      cbl = cbl->pNext;
    }
    return DBUS_HANDLER_RESULT_HANDLED;
  }
}

//-- Function: SignalHandler -----------------------------------------------
///
///  Dbus-Signal handler for calling the user defined handler
///
///
///  \return DBUS_HANDLER_RESULT_NOT_YET_HANDLED if no handler was called
///          DBUS_HANDLER_RESULT_HANDLED if a handler was called
//-----------------------------------------------------------------------------
static DBusHandlerResult com_SERV_SignalHandler (DBusConnection *connection,
                                 DBusMessage    *message)
{
  DBusHandlerResult ret = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  //check if message is a signal
  if(DBUS_MESSAGE_TYPE_SIGNAL == dbus_message_get_type(message))
  {
    if(rootSignalHandler != NULL)
    {
      tInterfaceList * iFace = MSG_GetInterface(dbus_message_get_interface(message));

      if(iFace != NULL)
      {
        tMemberList * mbr = MSG_GetMember(iFace, NULL);;
        ret =  CallSignalHandler(mbr, message);
        mbr = MSG_GetMember(iFace, dbus_message_get_member(message));
        if(   (CallSignalHandler(mbr, message) == DBUS_HANDLER_RESULT_HANDLED)
           || (ret                             == DBUS_HANDLER_RESULT_HANDLED))
        {
          ret = DBUS_HANDLER_RESULT_HANDLED;
        }
      }
    }
  }

  return ret;
}

GMainContext* SERV_GetContext(void)
{
  return g_main_loop_get_context(loop);
}

//-- Function: SERV_BlockServer -----------------------------------------------
///
///  Lock the Server-Mutex
///
//-----------------------------------------------------------------------------
void SERV_BlockServer()
{
  if (pthread_mutex_lock(&mutexReceiver) == -EOWNERDEAD)
  {
     pthread_mutex_consistent(&mutexReceiver);
  }
}

void             SERV_ServerInit(        void)
{
#if 1
  pthread_mutexattr_t mutexAttr;
  pthread_condattr_t condAttr;
  pthread_mutexattr_init(&mutexAttr);
  pthread_mutexattr_setprotocol(&mutexAttr, PTHREAD_PRIO_INHERIT);
  pthread_mutexattr_setrobust(&mutexAttr, PTHREAD_MUTEX_ROBUST);
  pthread_mutex_init(&mutexReceiver, &mutexAttr);
  pthread_mutexattr_destroy(&mutexAttr);

  pthread_condattr_init(&condAttr);
  pthread_condattr_setclock(&condAttr, CLOCK_MONOTONIC);
  pthread_cond_init(&condStartReceiver,&condAttr);

  pthread_mutexattr_init(&mutexAttr);
  pthread_mutexattr_setprotocol(&mutexAttr, PTHREAD_PRIO_INHERIT);
  pthread_mutexattr_setrobust(&mutexAttr, PTHREAD_MUTEX_ROBUST);
  pthread_mutex_init(&mutexStartReceiver, &mutexAttr);
  pthread_mutexattr_destroy(&mutexAttr);
  pthread_condattr_destroy(&condAttr);
#endif
}

//-- Function: SERV_TryBlockServer -----------------------------------------------
///
///  Try to Lock the Server-Mutex an return te errorcode of the trylock op
///
//-----------------------------------------------------------------------------
int SERV_TryBlockServer()
{
  SERV_BlockServer();

  if(receiverUp == true)
  {
    return -1;
  }
  else
  {
    return 0;
  }

}

//-- Function: SERV_UnblockServer ----------------------------------------------
///
///  Unock the Server-Mutex
///
//-----------------------------------------------------------------------------
void SERV_UnblockServer()
{
  pthread_mutex_unlock(&mutexReceiver);
}

//-- Function: SERV_IsServerRunning ----------------------------------------------
///
///  Check if the DBus-Sever is running
///
///  \return 0 if it is running; -1 if not
///
//----------------------------------------------------------------------------
int SERV_IsServerRunning()
{
  if(   (loop != NULL)
     && (g_main_loop_is_running(loop)))
  {
    return 0;
  }
  return -1;
}

//-- Function: SERV_StopServerThread -------------------------------------------
///
///  Stop the Server-Thread. Works from inside and outside the thread, so it
//   can be used from inside and outside a handler
///
//------------------------------------------------------------------------------
void SERV_StopServerThread(void)
{
  if(SERV_TryBlockServer())
  {
    //DBusMessage * msg;
    GMainContext* context;
    g_main_loop_quit (loop);
    context = g_main_loop_get_context       (loop);
    g_main_context_wakeup           (context);
    pthread_join (threadReceiver, NULL);
    receiverUp = false;
    g_main_loop_unref(loop);
  }
  SERV_UnblockServer();
}

//-- Function: SERV_RecallServerThread -------------------------------------------
///
///  Test if server state of server (running or not) and also check if it must
///  be started or stopped
///
//------------------------------------------------------------------------------
void SERV_RecallServerThread(void)
{
  if(rootSignalHandler != NULL)
  {
    if(signalFilterRegistered == false)
    {
      dbus_connection_add_filter (busConnection.bus, (DBusHandleMessageFunction) com_SERV_SignalHandler, NULL, NULL);
      signalFilterRegistered = true;
    }
  }
  else if(signalFilterRegistered == true)
  {
    dbus_connection_remove_filter   (busConnection.bus,(DBusHandleMessageFunction) com_SERV_SignalHandler, NULL );
    signalFilterRegistered = false;
  }
  SERV_BlockServer();
  if(receiverState == REC_STATE_STOP)
  {
    pthread_attr_t attr;
    int rc = 0;
    struct sched_param scheduling_parameter;
    int stackSize = 16*1024 <= 2 * PTHREAD_STACK_MIN ? 4 * PTHREAD_STACK_MIN : 16*1024;
    //prepare for starting the thread
    pthread_attr_init(&attr);   //attr Initialisieren

    if(pthread_attr_setstacksize(&attr, stackSize))
    {
      pthread_attr_setstacksize(&attr, 2 * PTHREAD_STACK_MIN);
    }
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    /*pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    scheduling_parameter.sched_priority = 1;
    pthread_attr_setschedparam(&attr, &scheduling_parameter);*/

	if(receiverThreadPriority == 0)
	{
		pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

		scheduling_parameter.sched_priority = 0;
		pthread_attr_setschedparam(&attr, &scheduling_parameter);
	}
	else
	{
		pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

		scheduling_parameter.sched_priority = receiverThreadPriority;
		pthread_attr_setschedparam(&attr, &scheduling_parameter);
	}
    //start thread
    //pthread_mutex_lock(&mutexReceiver);

    if (pthread_mutex_lock(&mutexStartReceiver) == -EOWNERDEAD)
    {
       pthread_mutex_consistent(&mutexStartReceiver);
    }
    if(pthread_create(&threadReceiver, &attr,(void * )ReceiveServer,busConnection.bus))
    {
      perror("pthread create");
    }
#ifdef HAVE_PTHREAD_SETNAME_NP
    pthread_setname_np(threadReceiver, receiverThreadName);
#endif
    pthread_attr_destroy(&attr);
    pthread_cond_wait(&condStartReceiver,&mutexStartReceiver);
    pthread_mutex_unlock(&mutexStartReceiver);
    receiverState = REC_STATE_RUNNING;
    SERV_UnblockServer();
  }
  else if(receiverState == REC_STATE_RUNNING)
  {
    SERV_UnblockServer();
    if(   (signalFilterRegistered == FALSE)
       && (objectReferenceCounter == 0))
    {
      SERV_StopServerThread();
      receiverState = REC_STATE_STOP;
    }
  }

}

//-- Function: com_SERV_Stop ---------------------------------------------------
///
///  Stops the DBus server which is running as additional thread.
///
///
///  \return 0 server is stopped; -1 server was not stopped
//------------------------------------------------------------------------------
int com_SERV_Stop(void)
{
  SERV_StopServerThread();
  receiverState = REC_STATE_USER_STOP;
  return 0;
}

//-- Function: com_SERV_Start ---------------------------------------------------
///
///  Start the DBus Server-Thread manually.
///
///
///  \return 0 server is running; -1 server is not running
//------------------------------------------------------------------------------
int com_SERV_Start(void)
{
  if(   (loop == NULL)
     || (!g_main_loop_is_running(loop)))
  {
    pthread_attr_t attr;
    //prepare for starting the thread
    pthread_attr_init(&attr);   //attr Initialisieren
    if(pthread_attr_setstacksize(&attr, 16 * 1024))
    {
      pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    }
    //start thread
    //pthread_mutex_lock(&mutexReceiver);
    SERV_BlockServer();
    pthread_create(&threadReceiver, &attr,(void * )ReceiveServer,busConnection.bus);
#ifdef HAVE_PTHREAD_SETNAME_NP
    pthread_setname_np(threadReceiver, receiverThreadName);
#endif

    pthread_attr_destroy(&attr);
  }
  receiverState = REC_STATE_USER_RUNNING;
  return 0;
}

//-- Function: com_SERV_join ---------------------------------------------------
///
///  wait for the DBus Server Thread till it ends up
///
///
///  \return return-value of the thread
//------------------------------------------------------------------------------
void* com_SERV_Join(void)
{
  pthread_join(threadReceiver, NULL);
  return NULL;
}

//-- Function: com_SERV_Surrender ---------------------------------------------------
///
///  Gives back the control of the server thread to the automatic
///
///
//------------------------------------------------------------------------------
void com_SERV_Surrender(void)
{
  if(   (loop != NULL)
     && (g_main_loop_is_running(loop)))
  {
    receiverState = REC_STATE_RUNNING;
  }
  else
  {
    receiverState = REC_STATE_STOP;
  }
  SERV_RecallServerThread();
}

void SERV_AppentToWorkingQueue(tObject * object,
								DBusConnection * con,
								DBusMessage * msg)
{
	tWorkerMsgQ stMessage;

	stMessage.DBusCon = con;
	stMessage.DBusMsg = msg;
	stMessage.object = object;

    mq_send(object->worker->fdq, (const char *)&stMessage,
                    sizeof(tWorkerMsgQ), 0);
}

static void * ObjectWorker(void * user_data)
{
	tDbusWorker * pThis = (tDbusWorker*) user_data;
	while(1)
	{
		struct pollfd fdrec;
		fdrec.fd = pThis->fdq;
		fdrec.events = POLLIN;
		fdrec.revents = 0;
		// wait for message (forever) used because we opened fd in O_NONBLOCK mode
		if(0 < poll(&fdrec, 1, -1))
		{
		  tWorkerMsgQ stMessage;
		  if(0 < mq_receive(pThis->fdq, (char *) &stMessage,
							sizeof(tWorkerMsgQ),NULL))
		  {
		    com_tComMessage msg;
		    memset(&msg, 0, sizeof msg);
		    com_tConnection con;
		    memset(&con, 0, sizeof con);

		    con.type = COM_CONNECTION_PROXY;
		    con.method_call_timeout = -1;
			  con.bus = stMessage.DBusCon;
			  dbus_error_init(&con.error);
			  msg.msg = stMessage.DBusMsg;
			  stMessage.object->callback(&con,&msg,stMessage.object->user_data);
			  dbus_message_unref(stMessage.DBusMsg);
		  }
		}
	}
	pthread_exit(NULL);
	return NULL;
}

static void SERV_OpenQueue(tDbusWorker*  worker, char * workername, long int msgsz, long int maxmsg)
{
  //mqd_t ret = -1;
  struct mq_attr mqAttr;
  char * queueName = malloc(strlen("/")+strlen(workername)+1);

  sprintf(queueName,"/%s",workername);

  mqAttr.mq_flags = 0;
  mqAttr.mq_maxmsg = maxmsg;//create a good size for the message buffer
  mqAttr.mq_msgsize = msgsz;
  worker->fdq = mq_open(queueName, (O_RDWR|O_CREAT|O_EXCL|O_NONBLOCK), (S_IWUSR|S_IRUSR), &mqAttr );
  if(worker->fdq < 0)
  {
    mq_unlink(queueName);
    worker->fdq = mq_open(queueName, (O_RDWR|O_CREAT|O_EXCL|O_NONBLOCK), (S_IWUSR|S_IRUSR), &mqAttr );
  }

}

tDbusWorker * com_SERV_CreateNewWorker(unsigned char priority,char * name)
{
	pthread_attr_t attr;
	struct sched_param scheduling_parameter;
	tDbusWorker * newWorker = NULL;
	tWorkerList * list = NULL;
	char threadName[16];
	int stackSize = 16*1024 <= 2 * PTHREAD_STACK_MIN ? 4 * PTHREAD_STACK_MIN : 16*1024;

	if(priority > 99 || name == NULL)
	{
		return NULL;
	}

	//prepare for starting the thread
	pthread_attr_init(&attr);   //attr Initialisieren

	if(pthread_attr_setstacksize(&attr, stackSize))
	{
	  pthread_attr_setstacksize(&attr, 2 * PTHREAD_STACK_MIN);
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(priority == 0)
	{
		pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

		scheduling_parameter.sched_priority = 0;
		pthread_attr_setschedparam(&attr, &scheduling_parameter);
	}
	else
	{
		pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

		scheduling_parameter.sched_priority = priority;
		pthread_attr_setschedparam(&attr, &scheduling_parameter);
	}
	newWorker = malloc(sizeof(struct stDbusWorker));
	newWorker->priority = priority;
	//newWorker->name = strdup(name);
	newWorker->name = malloc(WORKER_NAME_LEN(name));
    sprintf(newWorker->name,"%s%s_XXXXXX",WORKER_PREFIX,name);
    mktemp(newWorker->name);

	SERV_OpenQueue(newWorker, newWorker->name, sizeof(tWorkerMsgQ), 10);

	if(pthread_create(&newWorker->id, &attr,(void * )ObjectWorker,newWorker))
	{
	  perror("pthread create");
	}
	memcpy(threadName,newWorker->name,15);
	threadName[15]=0;
#ifdef HAVE_PTHREAD_SETNAME_NP
	pthread_setname_np(newWorker->id, threadName);
#endif
	pthread_attr_destroy(&attr);
	list = malloc(sizeof(tWorkerList));
	list->pNext=pWorkerRoot;
	list->worker = newWorker;
	pWorkerRoot=list;
	return newWorker;
}

tDbusWorker * com_SERV_GetWorker(unsigned char priority,char * name)
{
	tWorkerList * pAct = pWorkerRoot;
	char * extName = NULL;
	size_t szExtName;
	if(name != NULL)
	{
		extName = malloc(WORKER_NAME_LEN(name));
		sprintf(extName,"%s%s",WORKER_PREFIX,name);
		szExtName = strlen(extName);
	}
	while(pAct != NULL)
	{
		if(priority == pAct->worker->priority)
		{
			if(extName != NULL)
			{
				if(!strncmp(extName,pAct->worker->name,szExtName))
				{
					break;
				}
				else
				{
					continue;
				}
			}
			break;
		}
		pAct = pAct->pNext;
	}
	if(extName != NULL)
	{
		free(extName);
	}
	return pAct->worker;
}

int com_SERV_SetListenerPriority(unsigned char priority)
{
	int oldPriority = -1;
	int policy;
	struct sched_param param;


    if(!pthread_getschedparam(threadReceiver, &policy,&param))
    {
    	switch(policy)
    	{
    	  case SCHED_FIFO:
    		  oldPriority = param.sched_priority;
    		  break;
    	  case SCHED_OTHER:
    		  oldPriority = 0;
    		  break;
    	  default:
    		  //fuck
    		  break;
    	}

    	if(oldPriority >=0)
    	{

    		if(priority == 0)
			{
    			param.sched_priority = 0;
        		pthread_setschedparam(threadReceiver, SCHED_OTHER,&param);
			}
			else
			{
				param.sched_priority = priority;
				pthread_setschedparam(threadReceiver, SCHED_FIFO,&param);
			}
    	}
    }
    receiverThreadPriority = priority;

    return oldPriority;
}

