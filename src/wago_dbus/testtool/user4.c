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
///  \file     user4.c
///
///  \version  $Revision: 1 $
///
///  \brief    this program initiate a direct connection
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>

#include <communication_API.h>

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
  ret = com_MSG_MethodCall(con, "test.B","/test/B/pong","PONG",COM_TYPE_INVALID,COM_TYPE_INVALID);
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
  int count;
  com_GEN_Init(&con);

  for(i=0;i<10000;i++)
  {
    int ret;
    i = com_DIRECT_GetDirectConnection(&con,&direct,"test.B");

    if(i != 0)
    {
      printf("fehler: %s\n", com_GEN_ErrorGetMessage(&con));
      return 0;
    }
    //com_MSG_MethodCall(&direct, "test.B","/test/B/ping","PING",COM_TYPE_INVALID);
    ret = test1(&con);
    if(ret != 0)
    {
      printf("Fehler: %s\n",com_GEN_ErrorGetMessage(&direct));
      count ++;
    }
    usleep(100000);
    ret = test2(&con);
    if(ret != 0)
    {
      printf("Fehler: %s\n",com_GEN_ErrorGetMessage(&direct));
      count ++;
    }
    usleep(100000);
    //com_GEN_Close(&direct);
  }
  return 0;
}

//---- End of source file ------------------------------------------------------
