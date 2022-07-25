//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     config_tool_msg_com.h
///
///  \version  $Revision: 65689 $1
///
///  \brief    
///
///  \author   Stefanie Meihöfer : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef _config_tool_msg_com_h_

#define _config_tool_msg_com_h_

#define ipcMsgCom_GET_RESPONSE              1
#define ipcMsgCom_NO_RESPONSE               2


int ipcMsgCom_Send(int    messageType,
                   void*  pMessageContent,
                   int    sizeofContent,
                   int    responseRequested,
                   int    timeout);

#endif
