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
///  \version  $Revision: 1 $
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
  int i;
  com_GEN_Init(&con);
  com_MSG_RegisterName(&con, "test.B");
  com_MSG_RegisterObject(&con,"/test/B/ping",ping,NULL);
  com_MSG_RegisterObject(&con,"/test/B/pong",pong,NULL);
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
