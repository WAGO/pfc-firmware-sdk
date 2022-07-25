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
///  \version  $Revision: 65684 $
///
///  \brief    this program initiate a direct connection
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>

#include <communication_API.h>

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

    ret =com_MSG_MethodCall(&con, "test.B","/test/B/pong","PING",COM_TYPE_INVALID,COM_TYPE_INVALID);
    if(ret != 0)
    {
      printf("Fehler: %s\n",com_GEN_ErrorGetMessage(&con));
      count ++;
    }
    usleep(100000);
  }
  return 0;
}

//---- End of source file ------------------------------------------------------
