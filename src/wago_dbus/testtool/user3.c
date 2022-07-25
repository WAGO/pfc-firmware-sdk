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
///  \file     user3.c
///
///  \version  $Revision: 1 $
///
///  \brief    this program does only send messages to user 1
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>

#include <communication_API.h>


int main()
{
  com_tConnection con;
  int i;
  com_GEN_Init(&con);
  for(i=0;i<10000;i++)
  {
    if(0 != com_MSG_MethodCall(&con, "test.A","/test/A/peng","PING",COM_TYPE_INVALID))
    {
      printf("fehler:\n%s\n%s\n",com_GEN_ErrorGetName(&con), com_GEN_ErrorGetMessage(&con));
    }
    usleep(100000);
  }
  return 0;
}

//---- End of source file ------------------------------------------------------
