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
///  \file     wago_dbus_internal.h
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef WAGO_DBUS_INTERNAL_H_
#define WAGO_DBUS_INTERNAL_H_

#include <stdbool.h>
#include <pthread.h>
#include <glib.h>


typedef struct stCallbackList {
    com_tHandlerFunction        callback;
    com_tSignalHandle           handle;
    void                  * user_data;
    struct stCallbackList * pNext;
}tCallbackList;

typedef struct stMemberList {
    char                * member;
    tCallbackList       * callbackList;
    bool                  registered;
    struct stMemberList * pNext;
}tMemberList;

typedef struct stInterfaceList {
    char                   * interface;
    tMemberList            * memberList;
    bool                     registered;
    struct stInterfaceList * pNext;
}tInterfaceList;

typedef struct stObject {
    com_tHandlerFunction callback;
    void *          user_data;
    tDbusWorker	*   worker;
}tObject;

typedef struct {
    pthread_cond_t  condition;
    pthread_mutex_t mutex;
}tPendingConditions;

typedef enum {
  REC_STATE_STOP,
  REC_STATE_RUNNING,
  REC_STATE_USER_STOP,
  REC_STATE_USER_RUNNING
}tReceiverState;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
extern com_tConnection      busConnection;
extern tInterfaceList     * rootSignalHandler;
extern tReceiverState       receiverState;
extern bool                 signalFilterRegistered;
extern int                  objectReferenceCounter;


tMemberList    * MSG_GetMember(          tInterfaceList * pInterface,
                                         const char     * member);
tInterfaceList * MSG_GetInterface(       const char     * interface);
void MSG_SignalSendNoFlush(              com_tConnection* con,
                                         const char * path,
                                         const char * interface,
                                         const char * name,
                                         int          first_arg_type,
                                         ...);
tObject * MSG_NewObject(com_tHandlerFunction callback, void * user_data);
DBusHandlerResult MSG_CallObject(DBusConnection *connection,
                             DBusMessage *message,
                             tObject * object);
DBusHandlerResult MSG_UnregisterObject(DBusConnection *con,tObject * object);
void             SERV_BlockServer(       void);
int              SERV_TryBlockServer(    void);
void             SERV_ServerInit(        void);
void             SERV_UnblockServer(     void);
int              SERV_IsServerRunning(   void);
void             SERV_StopServerThread(  void);
void             SERV_RecallServerThread(void);
GMainContext*    SERV_GetContext(void);
void SERV_AppentToWorkingQueue(tObject * object,
								DBusConnection * con,
								DBusMessage * msg);

void DIRECT_RequestName(const char * name);
void DIRECT_AddObject(DBusObjectPathVTable  * vtable,
                      const char            *  path,
                      tObject               * newObject);
void DIRECT_RemoveObject( const char *  path);

GMainContext* com_GEN_GetServerContext(void);

void DIRECT_EnableDirectCommunication(void);
DBusMessage * DIRECT_HandleContextMessage(com_tConnection * con, DBusMessage * message);

#endif /* WAGO_DBUS_INTERNAL_H_ */
//---- End of source file ------------------------------------------------------
