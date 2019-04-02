//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2006 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
#ifndef _IPC_MSG_COM_H
#define _IPC_MSG_COM_H

// queue key for message queue to thread; must be unique in programm
#define IPC_MSG_COM_QKEY              ((key_t)82)

// address of "ipc message communication" for the first element message structure
#define ADR_IPC_MSG_COM_THREAD          1

// maximum size of message content
#define ipcMsgCom_MAX_CONTENT_SIZE      2560

enum
{
  ipcMsgCom_TIMEOUT       = -1,
  ipcMsgCom_INVALID       = 0,
  ipcMsgCom_STOP          = 1,
  ipcMsgCom_PLC_STATE,
  ipcMsgCom_PROJECT_INFO,
  ipcMsgCom_TASK_INFO,

  // last entry - don't remove!
  ipcMsgCom_MAX_MSG_TYPE
}
ipcMsgCom_messageTypes;


// Pointer to funktion to process a message
typedef int (*tpFktProcessMessage)(void*    pstMsgContent,
                                   uint32_t sizeofContent);


// structure for the message-queue to access process values
typedef struct 
{
  // first element of message structure, normaly "message type" - used for addressing
  uint32_t  receiveAddress;

  // addresser which has sended the message and wants to get the answer (if one is necessairy)
  uint32_t  returnAddress;

  // message tpye
  uint32_t  type;

  uint32_t  responseRequested;

  int       status;

  uint32_t  sizeofContent;

  // message itself
  uint8_t   content[ipcMsgCom_MAX_CONTENT_SIZE];

} ipcMsgCom_tstMessage;


#endif
