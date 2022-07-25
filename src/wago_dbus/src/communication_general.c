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
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "communication_API.h"
#include "communication_internal.h"



com_tConnection busConnection = {
     .bus = NULL,
     .lastMessage = NULL,
     .lastReply  = NULL
    };

static GMainContext* com_GEN_ServerContext=NULL;

static int(*threadCancelBlocker)(void)=NULL;
static int(*threadCancelUnblocker)(void)=NULL;

//-- Function: com_GEN_GetContext   --------------------------------------------
///
/// Returns the glib context of the server 
///
/// \param none 
///
/// \return Pointer, which references the context
//------------------------------------------------------------------------------
GMainContext* com_GEN_GetServerContext(void)
{
  static int once = 1;
  if(once)
  {
    com_GEN_ServerContext = g_main_context_new ();
    once=0;
  }
  return com_GEN_ServerContext;
}
//-- Function: com_GEN_Init -------------------------------------------------------
///
/// Initiates the dbus connection
///
/// \param tConnection pointer to a connection variable
///
/// \return 0 if ok; -1 if fail
//------------------------------------------------------------------------------
int com_GEN_Init(com_tConnection* con)
{
  int ret = 0;
  static int once = 0;

  //connect to the bus... if connection already exist, copy connection pointer
  dbus_error_init (&busConnection.error);
  busConnection.bus = dbus_bus_get (DBUS_BUS_SYSTEM, &busConnection.error);
  dbus_threads_init_default();

// g_thread_init() has been deprecated and is no longer needed starting with
// glib v2.32
#if !GLIB_CHECK_VERSION(2,32,0)
  if ( !g_thread_get_initialized())
  {
    g_thread_init(NULL);
  }
#endif

  dbus_connection_setup_with_g_main (busConnection.bus, com_GEN_GetServerContext());
  if (!busConnection.bus)
  {
    ret = -1;
  }
  if(con != NULL)
  {
    con->bus = busConnection.bus;
    dbus_error_init(&con->error);
    dbus_move_error(&busConnection.error, &con->error);
    con->lastMessage = NULL;
    con->lastReply = NULL;
    con->directReply = NULL;
    con->method_call_timeout = -1;
    con->type = COM_CONNECTION_PROXY;
  }
  if(once == 0)
  {
    once = 1;
    SERV_ServerInit();
  }
  return ret;
}


//-- Function: com_GEN_Close ------------------------------------------------------
///
/// Closes the DBus connection
///
///  \return 0 ok; -1 error
//------------------------------------------------------------------------------
void com_GEN_Close(com_tConnection* con)
{
  switch(con->type)
  {
    case COM_CONNECTION_PROXY:
      dbus_connection_unref(con->bus);
      if(FALSE == dbus_connection_get_is_connected(con->bus))
      {
        SERV_StopServerThread();
      }
      dbus_error_free(&con->error);
      break;
    case COM_CONNECTION_DIRECT:
      dbus_connection_unref(con->bus);
      break;
    case COM_CONNECTION_CONTEXT:
    default:
      break;
  }

}


//-- Function: com_GEN_ErrorIsSet ---------------------------------------------
///
///  Ask is error is set
///
/// \param tConnection pointer to a connection variable
///
///  \param 0 no error; 1 an error is set
///
//------------------------------------------------------------------------------
int com_GEN_ErrorIsSet(com_tConnection* con)
{
  if(con == NULL)
  {
    return 0;
  }
  else
  {
    return dbus_error_is_set  ( &con->error );
  }
}

//-- Function: com_GEN_ErrorGetName ---------------------------------------------
///
///  Get the name of an error
///
/// \param tConnection pointer to a connection variable
///
///  \return the name as string
//------------------------------------------------------------------------------
const char * com_GEN_ErrorGetName(com_tConnection* con)
{
  if(con == NULL)
  {
    return 0;
  }
  else
  {
    return con->error.name;
  }
}

//-- Function: com_GEN_ErrorGetMessage ---------------------------------------------
///
///  Get the Error Message
///
/// \param tConnection pointer to a connection variable
///
///  \return the Message as string
//------------------------------------------------------------------------------
const char * com_GEN_ErrorGetMessage(com_tConnection* con)
{
  if(con == NULL)
  {
    return 0;
  }
  else
  {
    return con->error.message;
  }
}

//-- Function: com_GEN_GetDBusVar ---------------------------------------------
///
///  Returns the value of an given DBus Variable
///
/// \param tDBusVar enumaration for possible-dbus-variables
///
///  \return a pointer to the DBus Variable
//------------------------------------------------------------------------------
void * com_GEN_GetDBusVar(com_tConnection* con, tDBusVar variant)
{
  switch(variant)
  {
    case  DBUSVAR_CONNECTION:
      return con->bus;
      break;
    case  DBUSVAR_ERROR:
      return &con->error;
      break;
    case  DBUSVAR_LAST_MESSAGE:
      return con->lastMessage;
      break;
    case  DBUSVAR_LAST_REPLY:
      return con->lastReply;
      break;
    default:
      return NULL;
      break;
  }
}

//-- Function: com_GEN_SetDBusVar ---------------------------------------------
///
///  Set the value of an given DBus Variable
///
/// \param tDBusVar enumaration for possible-dbus-variables
/// \param void* pointer to the associated variable
///
//------------------------------------------------------------------------------
void   com_GEN_SetDBusVar(com_tConnection* con, tDBusVar variant, void* value)
{
  switch(variant)
  {
    case  DBUSVAR_CONNECTION:
      con->bus = (DBusConnection*) value;
      break;

    case  DBUSVAR_ERROR:
      dbus_error_free(&con->error);
      dbus_move_error((DBusError*) value, &con->error);
      break;
    default:
      break;
  }
}

//-- Function: com_GEN_SetThreadCancelBlockFunktion ---------------------------------------------
///
///  Set handler for blocking and unblocking unsave thread cancelling
///
/// \param blocker funktion to block
/// \param unblocker function to unblock
///
///  \return 0 if ok -1 if not
//------------------------------------------------------------------------------
int   com_GEN_SetThreadCancelBlockHandling(int(*blocker)(void),int(*unblocker)(void))
{
  if(   (blocker != NULL && unblocker == NULL)
     || (blocker == NULL && unblocker != NULL))
  {
    return -1;
  }

  threadCancelBlocker = blocker;
  threadCancelUnblocker = unblocker;

  return 0;
}

//-- Function: com_GEN_BlockThreadCancelling ---------------------------------------------
///
///   Block unsave thread cancelling
///
///
///  \return 0 if ok -1 if not
//------------------------------------------------------------------------------
int   com_GEN_BlockThreadCancelling(void)
{
  if(threadCancelBlocker == NULL && threadCancelBlocker == NULL)
  {
    return 0;
  }
  if(   (threadCancelBlocker != NULL && threadCancelBlocker == NULL)
     || (threadCancelBlocker == NULL && threadCancelUnblocker != NULL))
  {
    return -1;
  }

  return threadCancelBlocker();
}

//-- Function: com_GEN_UnblockThreadCancelling ---------------------------------------------
///
///  Unblocking unsave thread cancelling
///
///
///  \return 0 if ok -1 if not
//------------------------------------------------------------------------------
int   com_GEN_UnblockThreadCancelling(void)
{
  if(threadCancelBlocker == NULL && threadCancelBlocker == NULL)
  {
    return 0;
  }
  if(   (threadCancelBlocker != NULL && threadCancelBlocker == NULL)
     || (threadCancelBlocker == NULL && threadCancelUnblocker != NULL))
  {
    return -1;
  }

  return threadCancelUnblocker();
}
