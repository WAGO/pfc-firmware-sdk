//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2006 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     config_tool_msg_com.c
///
///  \version  $Revision: 59635 $1
///
///  \brief    Communicate with PLC via message queue.
///
///  \author   Stefanie Meihöfer : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>

#include "RtsInfo.h"

#include "wago_ct_ipc_msg_com.h"
#include "config_tool_lib.h"


//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

// replaced by a stub in according unit test
#ifndef __CPPUTEST__

static void signalIgnore( int signal)
{
  (void)signal;
}

int ipcMsgCom_Send(int    messageType,
                   void*  pMessageContent,
                   int    sizeofContent,
                   int    responseRequested,
                   int    timeout)
{
  UNUSED_PARAMETER(timeout);

  int                   status      = SUCCESS;
  ipcMsgCom_tstMessage  stMessage;

  if(sizeofContent > sizeof(stMessage.content))
  {
    printf("error: message to large\n");
    return INVALID_PARAMETER;
  }

  // open queue; stop with error, if queue is not existing
  int ipcMsgCom_qid = msgget(IPC_MSG_COM_QKEY, 0666);

  if(ipcMsgCom_qid < 0)
  {
    status = SYSTEM_CALL_ERROR;
    //printf("msgget error\n");
  }
  else
  {
    int ownMsgAddress  = getpid();

    // fill values in message-structure for queue-send
    stMessage.receiveAddress        = ADR_IPC_MSG_COM_THREAD;
    stMessage.returnAddress         = ownMsgAddress;
    stMessage.type                  = messageType;
    stMessage.responseRequested     = responseRequested;
    stMessage.status                = SUCCESS;
    stMessage.sizeofContent         = sizeofContent;
    memcpy(&stMessage.content,pMessageContent, sizeofContent);

    // send message to PLC
    if(msgsnd(ipcMsgCom_qid, (void*)&stMessage, sizeof(stMessage), IPC_NOWAIT) < 0)
    {
      status = SYSTEM_CALL_ERROR;
      //printf("msgsnd error\n"); 
    }
    else if(responseRequested == TRUE)
    {
      ipcMsgCom_tstMessage stResponseMessage;
      int ret;
      __sighandler_t oldVal;

      oldVal =signal(SIGALRM,signalIgnore);
      alarm(2);
      ret = msgrcv(ipcMsgCom_qid, (void*)&stResponseMessage, sizeof(stResponseMessage), ownMsgAddress, 0);
      alarm(0);
      signal(SIGALRM,oldVal);
      if(ret > 0)
      {
        // message came from timout watchdog
        if(stResponseMessage.type == ipcMsgCom_TIMEOUT)
        {
          status = TIMEOUT;
          //printf("Timeout\n");
        }

        // message came from PLC - kill unneeded timeout watchdog first and copy data
        else
        {
          memcpy(pMessageContent, &stResponseMessage.content, sizeofContent);

          // set return error, if error occured while message processing
          if(stResponseMessage.status != SUCCESS)
          {
            status = ERROR;
          }
        }
      }else if( ret < 0)
      {
        //timout
        if(errno == EINTR)
        {
          //remove message from queue nonblocking
          msgrcv(ipcMsgCom_qid, (void*)&stMessage, sizeof(stMessage), stMessage.receiveAddress, IPC_NOWAIT);
          status = TIMEOUT;
        }
        else
        {
          status = ERROR;
        }
      }
    }
  }

  return status;
}
#endif
