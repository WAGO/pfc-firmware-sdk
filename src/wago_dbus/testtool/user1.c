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
///  \file     user1.c
///
///  \version  $Revision: 1 $
///
///  \brief    this program registers an Object and send messages to iself and to another object
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>

#include <communication_API.h>

void pingpong(com_tConnection * con, com_tComMessage * msg, void * data)
{
  static int c = 0;
  c++;
  printf("ping:%d\n", c);
  com_MSG_Return(con,msg,COM_TYPE_INVALID);
}

int test1(com_tConnection * con)
{
  struct timespec start,end;
  int ret;
  clock_gettime(CLOCK_MONOTONIC,&start);
  ret = com_MSG_MethodCall(con, "test.B","/test/B/ping","PONG",COM_TYPE_INVALID,COM_TYPE_INVALID);
  clock_gettime(CLOCK_MONOTONIC,&end);
  printf("start: %ld.%.9ld\n",start.tv_sec, start.tv_nsec);
  printf("end  : %ld.%.9ld\n",end.tv_sec, end.tv_nsec);
  printf("diff : %ld.%.9ld\n\n,",end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec);
  return ret;
}

int test2(com_tConnection * con)
{
  struct timespec start,end;
  int ret;
  clock_gettime(CLOCK_MONOTONIC,&start);
  ret = com_MSG_MethodCall(con, "test.A","/test/A/ping","PONG",COM_TYPE_INVALID,COM_TYPE_INVALID);
  clock_gettime(CLOCK_MONOTONIC,&end);
  printf("start: %ld.%.9ld\n",start.tv_sec, start.tv_nsec);
  printf("end  : %ld.%.9ld\n",end.tv_sec, end.tv_nsec);
  printf("diff : %ld.%.9ld\n\n,",end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec);
  return ret;
}


int main()
{
  com_tConnection con;

  com_tConnection direct;
  int i;
  int count = 0;
  com_GEN_Init(&con);
  com_MSG_RegisterName(&con, "test.A");
  com_MSG_RegisterObject(&con,"/test/A/ping",pingpong,NULL);
  //com_MSG_SetMethodCallTimeout(&con,3000);
  //sleep(5);

  i = com_DIRECT_GetDirectConnection(&con,&direct,"test.A");
  if(i != 0)
  {
    printf("fehler: %s\n", com_GEN_ErrorGetMessage(&con));
    return 0;
  }

  for(i=0;i<1000000;i++)
  {

    int ret;
    /*ret = test1(&con);
    if(ret != 0)
    {
      printf("Fehler: %s\n",com_GEN_ErrorGetMessage(&con));
      count ++;
    }*/
    ret = test2(&direct);
    if(ret != 0)
    {
      printf("Fehler: %s\n",com_GEN_ErrorGetMessage(&direct));
      count ++;
    }

  }
  printf("Fehler: %d\n",count);
  return 0;
}

//---- End of source file ------------------------------------------------------
