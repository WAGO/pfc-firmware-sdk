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
///  \file     communication_direct.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "communication_API.h"
#include "communication_internal.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> /* needed to define getpid() */
#include <glib.h>
#include <dbus/dbus-glib-lowlevel.h>

/*typedef struct stDiectServers {
  DBusServer * server;
  char       * name;
  struct stDirectServers * pNext;
}tDirectServers;

static tDirectServers *pDiectServersRoot = NULL;*/

typedef struct{
  DBusObjectPathVTable *  vtable;
  tObject              * object;
  const char           * path;
}tDirectObject;

GHashTable * serverTable = NULL;
GList      * directConnections = NULL;
GHashTable * objectTable = NULL;
GList      * nameList = NULL;


static const char * serverPathPrefix = "/tmp/";
static const char * serverSocketTypePrefix = "unix:path=";

static const char * directObjectPath = "/wago/dbus/directCommnuication";
static const char * startDirectCommunication = "start";


static void StopDirectComServer(DBusServer * server)
{
  dbus_server_disconnect(server);
  dbus_server_unref(server);
}

void RegisterObjectForNewConnection(char * path,
                                    tDirectObject * dObj,
                                    DBusConnection *new_connection)
{
  dbus_connection_try_register_object_path  (new_connection, path, dObj->vtable, (void*)dObj->object, NULL);
}

static DBusHandlerResult LocalObject(DBusConnection *connection,
                 DBusMessage    *message,void *blah)
{
  (void)blah;
  (void)message;
  directConnections = g_list_remove(directConnections,connection);
  dbus_connection_unref(connection);
  return DBUS_HANDLER_RESULT_HANDLED;
}

void NewDirectConnectionInit(DBusServer *server, DBusConnection *new_connection, void *data)
{
  (void)server; //unused
  (void)data;   //unused

  static DBusObjectPathVTable vtable;
  dbus_connection_setup_with_g_main (new_connection, com_GEN_GetServerContext());
  //dbus_connection_set_route_peer_messages(new_connection, TRUE);
  dbus_connection_set_allow_anonymous   (new_connection,TRUE) ;
  //dbus_connection_add_filter (new_connection, (DBusHandleMessageFunction) MySignalHandler, NULL, NULL);
  vtable.message_function = (DBusObjectPathMessageFunction)LocalObject;
  vtable.unregister_function = (DBusObjectPathUnregisterFunction)NULL;
  dbus_connection_try_register_object_path  (new_connection,DBUS_PATH_LOCAL,&vtable,NULL,NULL);
  dbus_connection_ref   (new_connection);
  g_hash_table_foreach(objectTable,(GHFunc)RegisterObjectForNewConnection,new_connection);
  directConnections = g_list_append(directConnections, new_connection);
}

static void EnableDirectCommunication(com_tConnection* con, com_tComMessage* msg, void* nothing)
{
  (void)nothing; //unused
  const char * serverName = NULL;
  const char * method = NULL;
  if(serverTable == NULL)
  {
    serverTable = g_hash_table_new_full(g_str_hash,g_str_equal,free,
                                        (GDestroyNotify)StopDirectComServer);
  }
  method = com_MSG_GetMethodName(msg);
  serverName =  dbus_message_get_destination  (msg->msg);

  if(!strcmp(startDirectCommunication,method))
  {
    char   pathContainer[1024];
    char * namePath = pathContainer;
    DBusServer * server;
    con->type = COM_CONNECTION_DIRECT;
    if(TRUE != g_hash_table_lookup_extended(serverTable, serverName, NULL, (gpointer *)&server))
    {
      DBusError error;

      sprintf(namePath, "%s%s%d.%s",serverSocketTypePrefix, serverPathPrefix,getpid(),serverName);

      dbus_error_init(&error);
      server = dbus_server_listen (pathContainer, &error);
      if (!server)
      {
        com_MSG_ReturnError(con,msg,
            error.name,error.message);
        return;
      }
      dbus_server_set_new_connection_function(server, NewDirectConnectionInit,&objectTable,NULL);
      dbus_server_setup_with_g_main (server,com_GEN_GetServerContext() );
      g_hash_table_insert(serverTable,strdup(serverName),server);
    }
    namePath=dbus_server_get_address  ( server ) ;
    com_MSG_Return(con,msg,COM_TYPE_STRING, &namePath,COM_TYPE_INVALID );
    dbus_free(namePath);
  }
}

void AddObjectToPrivateConnection( DBusConnection *con,tDirectObject * dObj)
{
  dbus_connection_try_register_object_path  (con, dObj->path, dObj->vtable, (void*)dObj->object, NULL);
}

void DIRECT_AddObject(DBusObjectPathVTable * vtable, const char *  path, tObject * newObject)
{
  static DBusObjectPathVTable localVTable;
  tDirectObject * dObj = malloc(sizeof(tDirectObject));
  if(objectTable == NULL)
  {
    objectTable = g_hash_table_new_full(g_str_hash,g_str_equal,free,free);
  }
  dObj->object = newObject;
  localVTable.message_function = vtable->message_function;
  localVTable.unregister_function = NULL;
  dObj->vtable = &localVTable;
  dObj->path   = path;
  g_hash_table_insert(objectTable,strdup(path),dObj);
  if(directConnections != NULL)
  {
    g_list_foreach(directConnections,(GFunc)AddObjectToPrivateConnection,dObj);
  }
}

void RemoveObjectFromPrivateConnection( DBusConnection *con,const char * path)
{
  dbus_connection_unregister_object_path  (con,path);
}

void DIRECT_RemoveObject( const char *  path)
{
  if(directConnections != NULL)
  {
    g_list_foreach(directConnections,(GFunc)RemoveObjectFromPrivateConnection, (gpointer)path);
  }
  g_hash_table_remove(objectTable,path);
}

void DIRECT_EnableDirectCommunication(void)
{
  static gboolean once = FALSE;

  if(once == FALSE)
  {
    /*com_MSG_RegisterObject( &busConnection,
                            directObjectPath,
                            EnableDirectCommunication,
                            NULL);*/
    tObject * newObject = MSG_NewObject(EnableDirectCommunication, NULL);
    static DBusObjectPathVTable vtable;
    vtable.message_function = (DBusObjectPathMessageFunction)MSG_CallObject;
    vtable.unregister_function = (DBusObjectPathUnregisterFunction)MSG_UnregisterObject;
    if(FALSE != dbus_connection_try_register_object_path  (busConnection.bus,
                                                           directObjectPath,
                                                           &vtable,
                                                           (void*)newObject,
                                                           &busConnection.error))
     {
      once = TRUE;
     }
  }
}

void DIRECT_RequestName(const char * name)
{
  nameList = g_list_append(nameList, strdup(name));
}

static int _GetDirectSocket(com_tConnection * globalConnection,
                                   com_tConnection * directConnection,
                                   const char * name)
{
  int ret = -1;
  char * pathToDirectSocket = NULL;


  ret =  com_MSG_MethodCall(globalConnection,
                            name,
                            directObjectPath,
                            startDirectCommunication,
                            COM_TYPE_INVALID,
                            COM_TYPE_STRING, &pathToDirectSocket,
                            COM_TYPE_INVALID);
  if(ret == 0 && directConnection != NULL)
  {
    directConnection->bus = dbus_connection_open  (pathToDirectSocket, &globalConnection->error);
    if(directConnection->bus != NULL)
    {
      dbus_error_init(&directConnection->error);
      dbus_move_error(&busConnection.error, &directConnection->error);
      directConnection->lastMessage = NULL;
      directConnection->lastReply = NULL;
      directConnection->directReply = NULL;
      directConnection->method_call_timeout = -1;
      directConnection->type = COM_CONNECTION_DIRECT;
    }
    else
    {
      ret = -1;
    }
  }

  return ret;
}

static int _GetDirectContext(com_tConnection * globalConnection,
                             com_tConnection * directConnection,
                             const char * name)
{
  (void)name; //unused
  (void)globalConnection; //unused

  int ret = -1;

  if(directConnection != NULL)
  {
    directConnection->bus = NULL;
    dbus_error_init(&directConnection->error);
    directConnection->lastMessage = NULL;
    directConnection->lastReply = NULL;
    directConnection->directReply = NULL;
    directConnection->method_call_timeout = -1;
    directConnection->type = COM_CONNECTION_CONTEXT;
    ret = 0;
  }

  return ret;
}

//-- Function: com_DIRECT_GetDirectConnection ----------------------------------
///
/// Initiates a direct Connection to a name-holding program
///
/// \param tConnection  pointer to the proxified connection variable
/// \param tConnection  pointer to a new connection variable used for direct communication
/// \param const char * destination of the direct connection
///
/// \return 0 if ok; -1 if fail
//------------------------------------------------------------------------------
int com_DIRECT_GetDirectConnection(com_tConnection * globalConnection,
                                   com_tConnection * directConnection,
                                   const char * name)
{
  int ret = -1;
  if(NULL == g_list_find_custom (nameList, name, (GCompareFunc)strcmp))
  {
    ret = _GetDirectSocket(globalConnection,directConnection,name);
  }
  else
  {
    ret = _GetDirectContext(globalConnection,directConnection,name);
  }
  return ret;
}


DBusMessage * DIRECT_HandleContextMessage(com_tConnection * con, DBusMessage * message)
{
  tDirectObject * dObj = NULL;
  com_tComMessage msg;

  dbus_error_init(&con->error);
  msg.msg = message;

  dObj = g_hash_table_lookup(objectTable,dbus_message_get_path(message));
  if(dObj == NULL)
  {
    char eMsg[255];
    sprintf(eMsg,"Method \"%s\" with signature \"%s\" on interface \"%s\" doesn't exist",
            dbus_message_get_member(message),
            dbus_message_get_signature (message),
            dbus_message_get_interface(message));
    con->directReply = dbus_message_new_error(message,DBUS_ERROR_UNKNOWN_METHOD,eMsg);
  }
  else
  {
    dObj->object->callback(con, &msg, dObj->object->user_data);
  }
  return con->directReply;
}

//---- End of source file ------------------------------------------------------
