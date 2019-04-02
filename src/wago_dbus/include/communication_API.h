//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license!
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file wago_dbus_API.h
///
/// \version 0.1
///
/// \brief This API Provides funtions for using DBUS on IPC an PAC
///
/// \author Hans Florian Scholz : WAGO
//------------------------------------------------------------------------------

#ifndef _WAGO_DBUS_API_H
#define _WAGO_DBUS_API_H

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <dbus/dbus.h>

//#include <LedCtl_API.h>



//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define WAGO_DBUS_NAME_LEDSERVER    "wago.ledserver"
#define WAGO_DBUS_PATH_LED          "/wago/errorserver/LED/"
#define WAGO_DBUS_PATH_LOG          "/wago/errorserver/log"
#define WAGO_DBUS_INTERFACE_LED     "wago.errorserver.LED"
#define WAGO_DBUS_INTERFACE_EVENTS  "wago.errorserver.events"
#define WAGO_DBUS_INTERFACE_LOG     "wago.errorserver.log"

#define WAGO_DBUS_MSG_EVENT     "log"
#define WAGO_DBUS_MSG_OVERFLOW  "overflow"
#define WAGO_DBUS_MSG_GET_LED   "get"
#define WAGO_DBUS_MSG_RLD_CFG   "reloadConfig"
#define WAGO_DBUS_MSG_CLR_LOG   "clearLog"
#define WAGO_DBUS_MSG_GET_LOG   "get"
#define WAGO_DBUS_LOGLIST_EL    "ListElement"
#define WAGO_DBUS_LOGLIST_END   "ListEnd"


#define  COM_TYPE_INVALID            DBUS_TYPE_INVALID
#define  COM_TYPE_BYTE               DBUS_TYPE_BYTE
#define  COM_TYPE_BOOLEAN            DBUS_TYPE_BOOLEAN
#define  COM_TYPE_INT16              DBUS_TYPE_INT16
#define  COM_TYPE_UINT16             DBUS_TYPE_UINT16
#define  COM_TYPE_INT32              DBUS_TYPE_INT32
#define  COM_TYPE_UINT32             DBUS_TYPE_UINT32
#define  COM_TYPE_INT64              DBUS_TYPE_INT64
#define  COM_TYPE_UINT64             DBUS_TYPE_UINT64
#define  COM_TYPE_DOUBLE             DBUS_TYPE_DOUBLE
#define  COM_TYPE_STRING             DBUS_TYPE_STRING
#define  COM_TYPE_OBJECT_PATH        DBUS_TYPE_OBJECT_PATH
#define  COM_TYPE_SIGNATURE          DBUS_TYPE_SIGNATURE
#define  COM_TYPE_UNIX_FD            DBUS_TYPE_UNIX_FD
#define  COM_TYPE_DBUS_TYPE_ARRAY    DBUS_TYPE_ARRAY
#define  COM_TYPE_DBUS_TYPE_VARIANT  DBUS_TYPE_VARIANT
#define  COM_TYPE_ARRAY              DBUS_TYPE_ARRAY
//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

typedef enum eComConnectionType{
  COM_CONNECTION_PROXY,
  COM_CONNECTION_DIRECT,
  COM_CONNECTION_CONTEXT
}tComConType;

typedef struct stConnection{
    DBusConnection * bus;
    DBusMessage    * lastMessage;
    DBusMessage    * lastReply;
    DBusMessage    * directReply;
    DBusError        error;
    int              method_call_timeout;
    tComConType      type;
}com_tConnection;

typedef struct stComMessage{
    DBusMessage *msg;
} com_tComMessage;

typedef dbus_bool_t         com_tComBool;
typedef int                 com_tSignalHandle;

typedef enum{
  DBUSVAR_CONNECTION,
  DBUSVAR_LAST_MESSAGE,
  DBUSVAR_LAST_REPLY,
  DBUSVAR_ERROR
}tDBusVar;

typedef struct stDbusWorker tDbusWorker;

typedef void(*com_tHandlerFunction)(com_tConnection*, com_tComMessage*, void*);

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//-- Function: com_GEN_Init -------------------------------------------------------
///
/// Initiates the dbus connection
///
/// \param com_tConnection pointer to a connection variable
///
/// \return 0 if ok; -1 if fail
//------------------------------------------------------------------------------
int com_GEN_Init(com_tConnection*);


//-- Function: com_GEN_Close ------------------------------------------------------
///
/// Closes the DBus connection
///
//------------------------------------------------------------------------------
void com_GEN_Close(com_tConnection*);

//-- Function: com_GEN_ErrorIsSet ---------------------------------------------
///
///  Ask is error is set
///
/// \param com_tConnection pointer to a connection variable
///
///  \param 0 no error; 1 an error is set
///
//------------------------------------------------------------------------------
int com_GEN_ErrorIsSet(com_tConnection*);

//-- Function: com_GEN_ErrorGetName ---------------------------------------------
///
///  Get the name of an error
///
/// \param com_tConnection pointer to a connection variable
///
///  \return the name as string
//------------------------------------------------------------------------------
char * com_GEN_ErrorGetName(com_tConnection*);

//-- Function: com_GEN_ErrorGetMessage ---------------------------------------------
///
///  Get the Error Message
///
/// \param com_tConnection pointer to a connection variable
///
///  \return the Message as string
//------------------------------------------------------------------------------
char * com_GEN_ErrorGetMessage(com_tConnection*);

//-- Function: com_GEN_GetDBusVar ---------------------------------------------
///
///  Returns the value of an given DBus Variable
///
/// \param tDBusVar enumaration for possible-dbus-variables
///
///  \return a pointer to the DBus Variable
//------------------------------------------------------------------------------
void * com_GEN_GetDBusVar(com_tConnection*, tDBusVar);//Not Implemented

//-- Function: com_GEN_SetDBusVar ---------------------------------------------
///
///  Set the value of an given DBus Variable
///
/// \param tDBusVar enumaration for possible-dbus-variables
/// \param void* pointer to the associated variable
///
//------------------------------------------------------------------------------
void   com_GEN_SetDBusVar(com_tConnection*, tDBusVar, void*);//Not Implemented

//-- Function: com_GEN_SetThreadCancelBlockFunktion ---------------------------------------------
///
///  Set handler for blocking and unblocking unsave thread cancelling
///
/// \param blocker funktion to block
/// \param unblocker function to unblock
///
///  \return 0 if ok -1 if not
//------------------------------------------------------------------------------
int   com_GEN_SetThreadCancelBlockHandling(int(*blocker)(void),int(*unblocker)(void));

//-- Function: com_GEN_BlockThreadCancelling ---------------------------------------------
///
///   Block unsave thread cancelling
///
///
///  \return 0 if ok -1 if not
//------------------------------------------------------------------------------
int   com_GEN_BlockThreadCancelling(void);

//-- Function: com_GEN_UnblockThreadCancelling ---------------------------------------------
///
///  Unblocking unsave thread cancelling
///
///
///  \return 0 if ok -1 if not
//------------------------------------------------------------------------------
int   com_GEN_UnblockThreadCancelling(void);

//-- Function: com_SERV_Stop ---------------------------------------------------
///
///  Stops the DBus server which is running as additional thread.
///
///
///  \return 0 server is stopped; -1 server was not stopped
//------------------------------------------------------------------------------
int com_SERV_Stop(void);

//-- Function: com_SERV_Start ---------------------------------------------------
///
///  Start the DBus Server-Thread manually.
///
///
///  \return 0 server is running; -1 server is not running
//------------------------------------------------------------------------------
int com_SERV_Start(void);

//-- Function: com_SERV_join ---------------------------------------------------
///
///  wait for the DBus Server Thread till it ends up
///
///
///  \return return-value of the thread
//------------------------------------------------------------------------------
void* com_SERV_Join(void);

//-- Function: com_SERV_Surrender ---------------------------------------------------
///
///  Gives back the control of the server thread to the automatic
///
///
//------------------------------------------------------------------------------
void com_SERV_Surrender(void);

//-- Function: com_SERV_CreateNewWorker ---------------------------------------------
///
///  Create a new worker for asyncrounus communication
///
/// \param priority set priority for worker (0 SCHED_OTHER; 1-99 FIFO)
/// \param name     name of the worker will be prefixed with wdbw_ and postfixed with ranom chars
///
///  \return a new tDBus worker instance or NULL if error
//------------------------------------------------------------------------------
tDbusWorker * com_SERV_CreateNewWorker(unsigned char priority,char * name);

//-- Function: com_SERV_GetWorker ---------------------------------------------
///
///  get a existing worker with priority and maybe this name
///
/// \param priority priority for worker (0 SCHED_OTHER; 1-99 FIFO)
/// \param name     NULL or name of the worker without prefix an postfix (see com_SERV_CreateNewWorker)
///
///  \return the  tDBus worker instance or NULL if nothing found
tDbusWorker * com_SERV_GetWorker(unsigned char priority,char * name);

//-- Function: com_SERV_SetListenerPriority ---------------------------------------------
///
///  set the priority of the DBUS listener / main-Worker
///
/// \param priority priority for the listener / main-worker (0 SCHED_OTHER; 1-99 FIFO)
///
///  \return old prio if OK; -1 on error
int com_SERV_SetListenerPriority(unsigned char priority);

//-- Function: com_MSG_Signal ---------------------------------------------------
///
///  Send a Signal to a given interface
///
///  \param com_tConnection* pointer to a connection variable
///  \param path         object path
///  \param interface    the interface
///  \param name         the member-name
///  \param additional parameters
///
//------------------------------------------------------------------------------
void com_MSG_Signal(com_tConnection*,
                     const char * path,
                     const char * interface,
                     const char * name,
                     int          first_arg_type,
                     ...);

//-- Function: com_MSG_SignalVaArg ---------------------------------------------------
///
///  Send a Signal to a given interface with variable parameterlist pointer
///
///  \param com_tConnection* pointer to a connection variable
///  \param path         object path
///  \param interface    the interface
///  \param name         the member-name
///  \param additional parameters
///
//------------------------------------------------------------------------------
void com_MSG_SignalVaArg(com_tConnection* con,
                         const char * path,
                         const char * interface,
                         const char * name,
                         int          first_arg_type,
                         va_list      var_args);

//-- Function: com_MSG_MethodCall ---------------------------------------------------
///
///  Send a MethodCall to a given object
///
///  \param com_tConnection* pointer to a connection variable
///  \param dest         the destination name
///  \param path         object path
///  \param name         the member-name
///  \param additional parameters
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_MethodCall(com_tConnection*,
                       const char * destination,
                       const char * path,
                       const char * name,
                       int          first_arg_type,
                       ...);

int com_MSG_MethodCallVaArgs(com_tConnection* con,
                              const char * destination,
                              const char * path,
                              const char * name,
                              int          first_arg_type,
                              va_list      var_args);

//-- Function: com_MSG_RegisterSignal ---------------------------------------------------
///
///  Registers a Signal-Handler for a given interface
///
///  \param interface    the interface
///  \param name         the signal member
///  \param callback     the callback-function
///
///  \return A handle usable for unregister this signal handler or -1
//------------------------------------------------------------------------------
com_tSignalHandle com_MSG_RegisterSignal(com_tConnection* con,
                                     const char * interface,
                                     const char * name,
                                     com_tHandlerFunction,
                                     void * user_data );

//-- Function: com_MSG_RegisterName ---------------------------------------------------
///
///  Registers a unique name on the bus
///
///  \param name    the name
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_RegisterName(com_tConnection * con, const char * name);

//-- Function: com_MSG_RegisterObject ---------------------------------------------------
///
///  Registers an Object-Handler for a given path
///
///  \param con          the dbus-connection
///  \param path         the path
///  \param callback     the callback-function
///  \param user_data    the additonal user data
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_RegisterObject(com_tConnection     * con,
                           const char      * path,
                           com_tHandlerFunction   callback,
                           void            * user_data);


//-- Function: com_MSG_RegisterObjectForWorker ---------------------------------------------------
///
///  Registers an Object-Handler for a given path and to behandled in the given worker instance
///
///  \param con          the dbus-connection
///  \param path         the path
///  \param callback     the callback-function
///  \param user_data    the additonal user data
///  \param handle       handle of the worker-instance
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_RegisterObjectForWorker(com_tConnection * con,
                           const char * path,
                           com_tHandlerFunction callback,
                           void * user_data,
                           tDbusWorker * handle);

//-- Function: com_MSG_GetMethodName ---------------------------------------------------
///
///  Returnes the name of a given message
///
///  \param com_tComMessage*   the message
///
///  \return the method name
//------------------------------------------------------------------------------
const char * com_MSG_GetMethodName(com_tComMessage * msg);

//-- Function: com_MSG_GetParams ---------------------------------------------------
///
///  Get Parameter of a Message
///
///  \param com_tConnection*        pointer to a connection variable
///  \param com_tComMessage*        the message
///  \param additional parameter
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_GetParams(com_tConnection* con,
                      com_tComMessage * msg,
                      int first_arg_type, ...);

//-- Function: com_MSG_Return ---------------------------------------------------
///
///  Create Return to a given Message
///
///  \param com_tConnection*        pointer to a connection variable
///  \param com_tComMessage*        the message
///  \param additional parameter
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_Return(com_tConnection*,
                   com_tComMessage *,
                   int first_arg_type, ...);

//-- Function: com_MSG_ReturnError ---------------------------------------------------
///
///  Create error-message as Return to a given Message
///  The Error name must be a Valis Error name like:
///    org.freedesktop.DBus.Error.ServiceUnknown
///
///  \param com_tConnection*        pointer to a connection variable
///  \param com_tComMessage*        the message
///  \param name                error name
///  \param message             error message string
///
///  \return 0 on success; -1 on error
//------------------------------------------------------------------------------
int com_MSG_ReturnError(com_tConnection * con,
                        com_tComMessage * msg,
                        const char  * name,
                        const char  * format,
                        ...);

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
int com_MSG_DeregisterSignal(com_tConnection     * con,
                             com_tSignalHandle     handle,
                             void(*MemoryFreeFunction)(void *user_data));

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
                             const char      * path);

//-- Function: com_MSG_SetMethodCallTimeout ---------------------------------------------------
///
///  Set the value for the timeout for method_calls.
///  Default is -1 (see: DBus-Default-Timeou-Value for dtails)
///
///  \param com_tConnection*        pointer to a connection variable
///  \param timeout_milliseconds the newTimeout in Milliseconds
///
//------------------------------------------------------------------------------
void com_MSG_SetMethodCallTimeout(com_tConnection * con,
                                  int               timeout_milliseconds);

//-- Function: com_DIRECT_GetDirectConnection ----------------------------------
///
///  Open a Direct Connection to a given DBus-Name. This new Connection will not
///  use the D-Bus-daemon to send messages.
///  This routine send a Direct Connection Request (DCR) via the global shared Connection wich
///  is using the D-Bus-Daemon. On the other side the D-Bus server will become
///  something like a D-Bus server wich is listening at a private Unix-Domain socket.
///  It will send the path to tis socket to the requester who will open this socket and establish
///  the new connection.
///  Please do not forget to close this connection after using it.
///  You can use the new connection like a normal D-Bus connection. But do not forget that
///  signal on this object will only reach the oher side an not other recipients.
///  also registering objects is Pivate for this connection.
///  At the other side is fully transparent throw this library so the user may not
///  register that is answers direct messages.
///
///  \param com_tConnection *    pointer to a global D-Bus-Daemon Connection
///  \param com_tConnection *    pointer to a new Connection object wich will newly initialized
///  \param const char *         dbus-Name of the requestet connection partner.
///
//------------------------------------------------------------------------------
int com_DIRECT_GetDirectConnection(com_tConnection  * globalConnection,
                                   com_tConnection  * directConnection,
                                   const char       * name);

#ifdef __cplusplus
}
#endif
#endif
