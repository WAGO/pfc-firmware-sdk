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
///  \file     user2.c
///
///  \version  $Revision: 65684 $
///
///  \brief    this program regisers an Object an wait for incoming messages
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>

#include <communication_API.h>

void ping(com_tConnection * con, com_tComMessage * msg, void * data)
{
  static int c = 0;
  c++;
  printf("Ping:%d\n", c);
  sleep(5);
  printf("ping_feddisch\n");
  com_MSG_Return(con,msg,COM_TYPE_INVALID);
}


void pong(com_tConnection * con, com_tComMessage * msg, void * data)
{
  static int c = 0;
  char * name;
  c++;
  printf("Pong:%d\n", c);
  name = com_MSG_GetMethodName(msg);
  printf("name %s\n",name);
  com_MSG_Return(con,msg,COM_TYPE_INVALID);
}

int main()
{
  com_tConnection con;
  tDbusWorker * worker1;
  tDbusWorker * worker2;
  int i;
  com_GEN_Init(&con);
  com_MSG_RegisterName(&con, "test.B");

  (void)com_SERV_CreateNewWorker(0,"myFairWorker");
  (void)com_SERV_CreateNewWorker(10,"myFifoWorker");
  worker1 = com_SERV_GetWorker(0,NULL);
  worker2 = com_SERV_GetWorker(10,"myFifoWorker");

  com_MSG_RegisterObjectForWorker(&con,"/test/B/ping",ping,NULL,worker1);
  com_MSG_RegisterObjectForWorker(&con,"/test/B/pong",pong,NULL,worker2);
  while(1)
  {
    sleep(1);
  }
  puts("now close");
  com_MSG_DeregisterObject(&con,"/test/B/ping");
  com_MSG_DeregisterObject(&con,"/test/B/pong");
  com_GEN_Close(&con);
  sleep(1);

  return 0;
}

//---- End of source file ------------------------------------------------------
